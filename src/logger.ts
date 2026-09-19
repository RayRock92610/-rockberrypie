import crypto from 'node:crypto';
import Database from 'better-sqlite3';
import {
  AgentExecutionEvent,
  AgentExecutionEventSchema
} from './types/logger.js';

const GENESIS_HASH = '0'.repeat(64);
const SAFE_STRING_REGEX = /[\x00-\x1f"\\]/;

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

  constructor(dbPath: string = 'agent_audit.db') {
    this.db = new Database(dbPath);
    this.initDatabase();

    // ⚡ Bolt: Cache prepared statements to avoid query recompilation overhead
    // on every event logged or queried.
    this.latestEventHashStmt = this.db.prepare('SELECT event_hash FROM audit_events ORDER BY sequence DESC LIMIT 1');
    this.insertEventStmt = this.db.prepare(`
      INSERT INTO audit_events (
        event_id, trace_id, pipeline_id, timestamp, prev_event_hash, event_hash, payload
      ) VALUES (?, ?, ?, ?, ?, ?, ?)
    `);
    this.selectAllEventsStmt = this.db.prepare('SELECT sequence, prev_event_hash, event_hash, payload FROM audit_events ORDER BY sequence ASC');
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
  }

  /**
   * Recursively canonicalize objects by sorting keys alphabetically
   */
  public canonicalize(obj: unknown): string {
    if (obj === null) return 'null';
    const type = typeof obj;
    if (type === 'string') {
      return SAFE_STRING_REGEX.test(obj) ? JSON.stringify(obj) : '"' + obj + '"';
    }
    if (type === 'boolean') return obj ? 'true' : 'false';
    if (type === 'number') return Number.isFinite(obj) ? String(obj) : 'null';
    if (type !== 'object') return JSON.stringify(obj) as unknown as string;

    if (Array.isArray(obj)) {
      return '[' + obj.map((item) => this.canonicalize(item)).join(',') + ']';
    }

    const rec = obj as Record<string, unknown>;
    const sortedKeys = Object.keys(rec).sort();
    let result = '{';
    for (let i = 0; i < sortedKeys.length; i++) {
      if (i > 0) result += ',';
      const key = sortedKeys[i];
      const keyStr = SAFE_STRING_REGEX.test(key) ? JSON.stringify(key) : '"' + key + '"';
      result += keyStr + ':' + this.canonicalize(rec[key]);
    }
    return result + '}';
  }

  public computeHash(content: string): string {
    return crypto.createHash('sha256').update(content, 'utf8').digest('hex');
  }

  public getLatestEventHash(): string {
    const row = this.latestEventHashStmt.get() as { event_hash: string } | undefined;
    return row ? row.event_hash : GENESIS_HASH;
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

    this.insertEventStmt.run(
      validatedEvent.event_id,
      validatedEvent.trace_id,
      validatedEvent.pipeline_id,
      validatedEvent.timestamp,
      validatedEvent.integrity.prev_event_hash,
      validatedEvent.integrity.event_hash,
      JSON.stringify(validatedEvent)
    );

    return validatedEvent;
  }

  /**
   * Verify total integrity of the local hash chain
   */
  public verifyChainIntegrity(): { valid: boolean; brokenSequence?: number } {
    // ⚡ Bolt: Use .iterate() instead of .all() to stream rows iteratively
    // This significantly reduces memory spikes and CPU overhead when querying large datasets.
    const rows = this.selectAllEventsStmt.iterate() as IterableIterator<{ sequence: number; prev_event_hash: string; event_hash: string; payload: string }>;

    let expectedPrevHash = GENESIS_HASH;

    for (const row of rows) {
      if (row.prev_event_hash !== expectedPrevHash) {
        return { valid: false, brokenSequence: row.sequence };
      }

      const parsed = JSON.parse(row.payload);
      const partialPayload = {
        event_id: parsed.event_id,
        timestamp: parsed.timestamp,
        trace_id: parsed.trace_id,
        pipeline_id: parsed.pipeline_id,
        agent: parsed.agent,
        model_config: parsed.model_config,
        input: parsed.input,
        reasoning_trace: parsed.reasoning_trace,
        tool_calls: parsed.tool_calls,
        state_delta: parsed.state_delta,
        integrity: {
          prev_event_hash: parsed.integrity.prev_event_hash,
        },
      };

      const recomputedHash = this.computeHash(this.canonicalize(partialPayload));
      if (recomputedHash !== row.event_hash) {
        return { valid: false, brokenSequence: row.sequence };
      }

      expectedPrevHash = row.event_hash;
    }

    return { valid: true };
  }

  public close(): void {
    this.db.close();
  }
}
