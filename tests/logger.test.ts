import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import fs from 'node:fs';
import path from 'node:path';
import Database from 'better-sqlite3';
import { HashChainedLogger, LogEventParams } from '../src/logger.js';

const TEST_DB = path.join(__dirname, 'test_audit.db');

describe('HashChainedLogger', () => {
  let logger: HashChainedLogger;

  const mockParams: LogEventParams = {
    event_id: '550e8400-e29b-41d4-a716-446655440000',
    trace_id: 'trace-1234',
    pipeline_id: 'test-pipeline',
    agent: {
      id: 'agent-1',
      role: 'maintenance',
      persona: 'jules',
    },
    model_config: {
      provider: 'google',
      model: 'gemini-3.1-pro',
      temperature: 0,
      seed: 42,
    },
    input: {
      raw_prompt_hash: 'a'.repeat(64),
      sanitized_input_summary: 'Run unit test suite',
    },
    reasoning_trace: [
      {
        step_number: 1,
        thought: 'Evaluating test files',
        decision: 'EXECUTE',
      },
    ],
    tool_calls: [
      {
        tool_name: 'vitest',
        parameters: { target: 'logger.test.ts' },
        execution_status: 'SUCCESS',
        duration_ms: 120,
      },
    ],
    state_delta: {
      target_subsystem: 'sqlite',
      mutations: [{ table: 'audit_events', action: 'INSERT' }],
    },
  };

  beforeEach(() => {
    if (fs.existsSync(TEST_DB)) {
      fs.unlinkSync(TEST_DB);
    }
    logger = new HashChainedLogger(TEST_DB);
  });

  afterEach(() => {
    logger.close();
    if (fs.existsSync(TEST_DB)) {
      fs.unlinkSync(TEST_DB);
    }
  });

  it('should canonicalize objects deterministically regardless of key order', () => {
    const objA = { z: 1, a: 2, m: { b: 3, a: 4 } };
    const objB = { a: 2, m: { a: 4, b: 3 }, z: 1 };

    const canonicalA = logger.canonicalize(objA);
    const canonicalB = logger.canonicalize(objB);

    expect(canonicalA).toBe(canonicalB);
  });

  it('should log an event and correctly set Genesis hash for the first event', () => {
    const event = logger.logEvent(mockParams);

    expect(event.integrity.prev_event_hash).toBe('0'.repeat(64));
    expect(event.integrity.event_hash).toHaveLength(64);

    const verification = logger.verifyChainIntegrity();
    expect(verification.valid).toBe(true);
  });

  it('should maintain a valid hash chain across multiple sequentially logged events', () => {
    const event1 = logger.logEvent(mockParams);

    const params2: LogEventParams = {
      ...mockParams,
      event_id: '550e8400-e29b-41d4-a716-446655440001',
      input: {
        raw_prompt_hash: 'b'.repeat(64),
        sanitized_input_summary: 'Run build step',
      },
    };

    const event2 = logger.logEvent(params2);

    expect(event2.integrity.prev_event_hash).toBe(event1.integrity.event_hash);

    const verification = logger.verifyChainIntegrity();
    expect(verification.valid).toBe(true);
  });

  it('should detect tampering if a payload in the database is modified', () => {
    logger.logEvent(mockParams);

    const db = new Database(TEST_DB);
    db.prepare("UPDATE audit_events SET payload = REPLACE(payload, 'Run unit test suite', 'Tampered payload') WHERE sequence = 1").run();
    db.close();

    const verification = logger.verifyChainIntegrity();
    expect(verification.valid).toBe(false);
    expect(verification.brokenSequence).toBe(1);
  });
});
