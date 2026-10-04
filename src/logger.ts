import crypto from 'node:crypto';
import Database from 'better-sqlite3';
import {
  AgentExecutionEvent,
  AgentExecutionEventSchema
} from './types/logger.js';

const GENESIS_HASH = '0'.repeat(64);

export interface LogEventParams {
  event_id: string;
  trace_id: string;
  pipeline_id: string;
  agent: AgentExecutionEvent['agent'];
  model_config: AgentExecutionEvent['model_config'];
  input: AgentExecutionEvent['input'];
  reasoning_trace: AgentExecutionEvent['reasoning_trace'];
  tool_calls: AgentExecutionEvent['tool_calls'];
  state_delta: AgentExecutionEvent['state_delta'];
}

export class HashChainedLogger {
  private db: Database.Database;
  private insertEventStmt: Database.Statement;
  private latestEventHashStmt: Database.Statement;
  private selectAllEventsStmt: Database.Statement;
  private cachedLatestHash: string | null = null;

  constructor(dbPath: string = 'agent_audit.db') {
    this.db = new Database(dbPath);
    this.initDatabase();

    // ⚡ Bolt: Cache prepared statements to avoid query recompilation overhead
    // on every event logged or queried.
    // ⚡ Bolt: Use .pluck(true) to return a string primitive and .raw(true) to return flat arrays to bypass JS object allocation.
    this.latestEventHashStmt = this.db.prepare('SELECT event_hash FROM audit_events ORDER BY sequence DESC LIMIT 1').pluck(true);
    this.insertEventStmt = this.db.prepare(`
      INSERT INTO audit_events (
        event_id, trace_id, pipeline_id, timestamp, prev_event_hash, event_hash, payload, payload_hash
      ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    `);
    this.selectAllEventsStmt = this.db.prepare('SELECT sequence, prev_event_hash, event_hash, payload, payload_hash FROM audit_events ORDER BY sequence ASC').raw(true);
  }

  private initDatabase(): void {
    this.db.exec(`
      PRAGMA journal_mode = WAL;
      PRAGMA synchronous = NORMAL;

      CREATE TABLE IF NOT EXISTS audit_events (
        sequence INTEGER PRIMARY KEY AUTOINCREMENT,
        event_id TEXT UNIQUE NOT NULL,
        trace_id TEXT NOT NULL,
        pipeline_id TEXT NOT NULL,
        timestamp TEXT NOT NULL,
        prev_event_hash TEXT NOT NULL,
        event_hash TEXT NOT NULL,
        payload TEXT NOT NULL
      );
      CREATE INDEX IF NOT EXISTS idx_trace_id ON audit_events(trace_id);
    `);
    try {
      this.db.exec('ALTER TABLE audit_events ADD COLUMN payload_hash TEXT');
    } catch (e) {
      // Ignore if it already exists
    }
  }

  /**
   * Recursively canonicalize objects by sorting keys alphabetically
   * ⚡ Bolt: Optimized by replacing intermediate array allocations (like .map().join())
   * with traditional for loops and ordering type-checking branches.
   */
  public canonicalize(obj: unknown): string {
    if (obj === null) return 'null';
    const type = typeof obj;
    if (type === 'object') {
      if (Array.isArray(obj)) {
        let result = '[';
        for (let i = 0; i < obj.length; i++) {
          if (i > 0) result += ',';
          const val = this.canonicalize(obj[i]);
          // JSON.stringify can return undefined for functions/symbols, which is
          // passed through via the cast. We need to check it dynamically.
          result += (val as unknown) === undefined ? 'null' : val;
        }
        return result + ']';
      }

      const rec = obj as Record<string, unknown>;
      const sortedKeys = Object.keys(rec).sort();
      let result = '{';
      let first = true;
      for (let i = 0; i < sortedKeys.length; i++) {
        const key = sortedKeys[i];
        const val = this.canonicalize(rec[key]);
        if ((val as unknown) === undefined) continue;
        if (!first) result += ',';
        result += JSON.stringify(key) + ':' + val;
        first = false;
      }
      return result + '}';
    }

    if (type === 'string') {
      return JSON.stringify(obj);
    }
    if (type === 'boolean') return obj ? 'true' : 'false';
    if (type === 'number') return Number.isFinite(obj) ? String(obj) : 'null';
    return JSON.stringify(obj) as unknown as string;
  }

  public computeHash(content: string): string {
    // ⚡ Bolt: Using native crypto.hash() for ~2x performance over createHash()
    return crypto.hash('sha256', content, 'hex');
  }

  public getLatestEventHash(): string {
    if (this.cachedLatestHash !== null) {
      return this.cachedLatestHash;
    }
    const hash = this.latestEventHashStmt.get() as string | undefined;
    this.cachedLatestHash = hash ? hash : GENESIS_HASH;
    return this.cachedLatestHash;
  }

  public logEvent(params: LogEventParams): AgentExecutionEvent {
    const prev_event_hash = this.getLatestEventHash();
    const timestamp = new Date().toISOString();

    const partialPayload = {
      event_id: params.event_id,
      timestamp,
      trace_id: params.trace_id,
      pipeline_id: params.pipeline_id,
      agent: params.agent,
      model_config: params.model_config,
      input: params.input,
      reasoning_trace: params.reasoning_trace,
      tool_calls: params.tool_calls,
      state_delta: params.state_delta,
      integrity: {
        prev_event_hash,
      },
    };

    const canonicalString = this.canonicalize(partialPayload);
    const event_hash = this.computeHash(canonicalString);

    const fullEvent: AgentExecutionEvent = {
      ...partialPayload,
      integrity: {
        prev_event_hash,
        event_hash,
      },
    };

    // Validate payload against Zod schema prior to persistence
    const validatedEvent = AgentExecutionEventSchema.parse(fullEvent);

    const payloadStr = JSON.stringify(validatedEvent);
    this.insertEventStmt.run(
      validatedEvent.event_id,
      validatedEvent.trace_id,
      validatedEvent.pipeline_id,
      validatedEvent.timestamp,
      validatedEvent.integrity.prev_event_hash,
      validatedEvent.integrity.event_hash,
      payloadStr,
      this.computeHash(payloadStr)
    );

    // ⚡ Bolt: Cache the new hash to avoid querying it on the next logEvent call
    this.cachedLatestHash = validatedEvent.integrity.event_hash;

    return validatedEvent;
  }

  /**
   * Verify total integrity of the local hash chain
   */
  public verifyChainIntegrity(): { valid: boolean; brokenSequence?: number; totalEvents?: number } {
    // ⚡ Bolt: Use .iterate() instead of .all() to stream rows iteratively
    // This significantly reduces memory spikes and CPU overhead when querying large datasets.
    const rows = this.selectAllEventsStmt.iterate() as IterableIterator<[number, string, string, string, string | null]>;

    let expectedPrevHash = GENESIS_HASH;

    for (const row of rows) {
      const [sequence, prev_event_hash, event_hash, payload, payload_hash] = row;
      if (prev_event_hash !== expectedPrevHash) {
        return { valid: false, brokenSequence: sequence };
      }

      if (payload_hash) {
        if (this.computeHash(payload) !== payload_hash) {
          return { valid: false, brokenSequence: sequence };
        }
      } else {
        const parsed = JSON.parse(payload);
        if (parsed?.integrity) {
          parsed.integrity.event_hash = undefined;
        }

        const recomputedHash = this.computeHash(this.canonicalize(parsed));
        if (recomputedHash !== event_hash) {
          return { valid: false, brokenSequence: sequence };
        }
      }

      expectedPrevHash = event_hash;
    }

    return { valid: true };
  }

  public close(): void {
    this.db.close();
  }
}
