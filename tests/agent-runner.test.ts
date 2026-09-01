import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import fs from 'node:fs';
import path from 'node:path';
import { HashChainedLogger } from '../src/logger.js';
import { AgentRunner } from '../src/agent-runner.js';

const TEST_DB = path.join(__dirname, 'test_runner_audit.db');

describe('AgentRunner Integration', () => {
  let logger: HashChainedLogger;
  let runner: AgentRunner;

  beforeEach(() => {
    if (fs.existsSync(TEST_DB)) {
      fs.unlinkSync(TEST_DB);
    }
    logger = new HashChainedLogger(TEST_DB);
    runner = new AgentRunner(logger);
  });

  afterEach(() => {
    logger.close();
    if (fs.existsSync(TEST_DB)) {
      fs.unlinkSync(TEST_DB);
    }
  });

  it('should execute an agent step and append a valid hash-chained event record', async () => {
    const event = await runner.executeStep(
      {
        agent: { id: 'agent-jules', role: 'maintenance', persona: 'jules' },
        modelConfig: { provider: 'google', model: 'gemini-3.1-pro', temperature: 0, seed: 42 },
        rawPrompt: 'Check system health and clear stale cache',
        sanitizedSummary: 'Check system health',
        pipelineId: 'kessel-flow-main',
      },
      async () => ({
        reasoningTrace: [
          { step_number: 1, thought: 'Inspecting cache directory', decision: 'EXECUTE' },
        ],
        toolCalls: [
          { tool_name: 'fs.readdir', parameters: { path: '/tmp/cache' }, execution_status: 'SUCCESS', duration_ms: 15 },
        ],
        stateDelta: { target_subsystem: 'cache', mutations: [{ table: 'files', action: 'DELETE' }] },
      })
    );

    expect(event.agent.persona).toBe('jules');
    expect(event.integrity.event_hash).toBeDefined();

    const chainIntegrity = logger.verifyChainIntegrity();
    expect(chainIntegrity.valid).toBe(true);
  });
});
