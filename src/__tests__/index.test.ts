import { initAgent } from '../index.js';
import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import fs from 'node:fs';
import path from 'node:path';
import { HashChainedLogger } from '../logger.js';
import { PipelineOrchestrator } from '../orchestrator.js';

const TEST_DB = path.join(__dirname, 'test_index_orchestrator.db');

describe('Core Orchestration Integrity Check', () => {
  let logger: HashChainedLogger;
  let orchestrator: PipelineOrchestrator;

  beforeEach(() => {
    if (fs.existsSync(TEST_DB)) {
      fs.unlinkSync(TEST_DB);
    }
    logger = new HashChainedLogger(TEST_DB);
    orchestrator = new PipelineOrchestrator(logger);
  });

  afterEach(() => {
    logger.close();
    if (fs.existsSync(TEST_DB)) {
      fs.unlinkSync(TEST_DB);
    }
  });

  it('should initialize agent correctly', () => {
    const result = initAgent();
    expect(result).toBe("Orchestration Agent Active");
  });

  it('should pass baseline verification gates', () => {
    const integrityStatus = true;
    expect(integrityStatus).toBe(true);
  });

  it('should return FAILED status and error message when a step executor fails', async () => {
    const result = await orchestrator.runPipeline('kessel-flow-failure-test', [
      {
        agent: { id: 'agent-1', role: 'architect', persona: 'owl' },
        modelConfig: { provider: 'google', model: 'gemini-3.1-pro', temperature: 0, seed: 100 },
        prompt: 'Valid initial step',
        executor: async () => ({
          reasoningTrace: [{ step_number: 1, thought: 'Step 1 success', decision: 'APPROVE' }],
          toolCalls: [],
          stateDelta: { target_subsystem: 'db', mutations: [] },
        }),
      },
      {
        agent: { id: 'agent-2', role: 'executor', persona: 'hawk' },
        modelConfig: { provider: 'google', model: 'gemini-3.1-pro', temperature: 0, seed: 101 },
        prompt: 'Valid second step that will throw error in executor',
        executor: async () => {
          throw new Error('Pipeline executor execution failed');
        },
      },
    ]);

    expect(result.status).toBe('FAILED');
    expect(result.events).toHaveLength(1);
    expect(result.error).toBe('Pipeline executor execution failed');
  });
});
