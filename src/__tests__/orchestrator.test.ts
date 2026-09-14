import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import fs from 'node:fs';
import path from 'node:path';
import { HashChainedLogger } from '../logger.js';
import { PipelineOrchestrator } from '../orchestrator.js';

const TEST_DB = path.join(__dirname, 'test_orchestrator_audit.db');

describe('PipelineOrchestrator Engine', () => {
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

  it('should run a multi-step pipeline cleanly to completion', async () => {
    const result = await orchestrator.runPipeline('kessel-flow-deploy', [
      {
        agent: { id: 'agent-owl', role: 'architect', persona: 'owl' },
        modelConfig: { provider: 'google', model: 'gemini-3.1-pro', temperature: 0, seed: 100 },
        prompt: 'Analyze state schema for migration compatibility',
        executor: async () => ({
          reasoningTrace: [{ step_number: 1, thought: 'Schema matches v1 spec', decision: 'APPROVE' }],
          toolCalls: [],
          stateDelta: { target_subsystem: 'db', mutations: [] },
        }),
      },
    ]);

    expect(result.status).toBe('COMPLETED');
    expect(result.events).toHaveLength(1);

    const verification = logger.verifyChainIntegrity();
    expect(verification.valid).toBe(true);
  });

  it('should generate trace IDs using cryptographically secure UUID v4 format', async () => {
    const result = await orchestrator.runPipeline('trace-id-check', []);

    expect(result.traceId).toMatch(
      /^trace-[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i
    );
  });
});
