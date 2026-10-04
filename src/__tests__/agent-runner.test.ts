import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import fs from 'node:fs';
import path from 'node:path';
import { AgentRunner } from '../agent-runner.js';
import { HashChainedLogger } from '../logger.js';

const TEST_DB = path.join(__dirname, 'test_agent_runner.db');

describe('AgentRunner', () => {
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

  it('should propagate errors thrown by the executor', async () => {
    const options = {
      agent: { id: 'test-agent', role: 'test', persona: 'test' },
      modelConfig: { provider: 'test', model: 'test', temperature: 0, seed: 1 },
      rawPrompt: 'test prompt',
      sanitizedSummary: 'test summary',
      pipelineId: 'test-pipeline',
    };

    const errorMessage = 'Executor failed unexpectedly';
    const failingExecutor = async () => {
      throw new Error(errorMessage);
    };

    await expect(runner.executeStep(options, failingExecutor)).rejects.toThrow(errorMessage);
  });
});
