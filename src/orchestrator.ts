import crypto from 'node:crypto';
import { HashChainedLogger } from './logger.js';
import { AgentRunner } from './agent-runner.js';
import { InputGuardrail } from './guardrails.js';
import { AgentExecutionEvent } from './types/logger.js';

export interface PipelineStep {
  agent: AgentExecutionEvent['agent'];
  modelConfig: AgentExecutionEvent['model_config'];
  prompt: string;
  executor: () => Promise<{
    reasoningTrace: AgentExecutionEvent['reasoning_trace'];
    toolCalls: AgentExecutionEvent['tool_calls'];
    stateDelta: AgentExecutionEvent['state_delta'];
  }>;
}

export interface OrchestrationResult {
  pipelineId: string;
  traceId: string;
  status: 'COMPLETED' | 'REJECTED_GUARDRAIL' | 'FAILED';
  events: AgentExecutionEvent[];
  error?: string;
}

export class PipelineOrchestrator {
  private runner: AgentRunner;

  constructor(logger: HashChainedLogger) {
    this.runner = new AgentRunner(logger);
  }

  /**
   * Executes a sequential multi-agent pipeline with guardrail enforcement
   */
  public async runPipeline(
    pipelineId: string,
    steps: PipelineStep[]
  ): Promise<OrchestrationResult> {
    const traceId = `trace-${crypto.randomUUID()}`;
    const events: AgentExecutionEvent[] = [];


    // ⚡ Bolt: Pre-evaluate the guardrail for all steps synchronously first
    // This allows throwing early if the end of a pipeline would fail, and replaces CPU interleaving overhead.

    // ⚡ Bolt: Evaluate the first guardrail immediately so we can overlap subsequent evaluations
    let nextGuardrail = steps.length > 0 ? InputGuardrail.evaluate(steps[0].prompt) : null;

    for (let i = 0; i < steps.length; i++) {
      const step = steps[i];
      // 1. Evaluate Guardrails (using the pre-computed result for this step)
      const guardrailResult = nextGuardrail!;
      if (!guardrailResult.passed) {
        return {
          pipelineId,
          traceId,
          status: 'REJECTED_GUARDRAIL',
          events,
          error: `Guardrail violation: ${guardrailResult.violations.join(', ')}`,
        };
      }

      // 2. Execute Step via AgentRunner
      try {
        const eventPromise = this.runner.executeStep(
          {
            agent: step.agent,
            modelConfig: step.modelConfig,
            rawPrompt: step.prompt,
            rawPromptHash: guardrailResult.rawPromptHash,
            sanitizedSummary: guardrailResult.sanitizedSummary,
            pipelineId,
            traceId,
          },
          step.executor
        );

        // ⚡ Bolt: While the current step is awaiting I/O (executor/db), compute the guardrail
        // for the *next* step. This overlaps synchronous CPU work with async I/O wait time.
        if (i + 1 < steps.length) {
          nextGuardrail = InputGuardrail.evaluate(steps[i + 1].prompt);
        }

        const event = await eventPromise;
        events.push(event);
      } catch (err: any) {
        return {
          pipelineId,
          traceId,
          status: 'FAILED',
          events,
          error: err?.message || 'Unknown execution failure',
        };
      }
    }

    return {
      pipelineId,
      traceId,
      status: 'COMPLETED',
      events,
    };
  }
}
