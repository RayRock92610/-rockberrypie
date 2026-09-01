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
    const traceId = `trace-${Date.now()}-${Math.random().toString(36).slice(2, 7)}`;
    const events: AgentExecutionEvent[] = [];

    for (const step of steps) {
      // 1. Evaluate Guardrails
      const guardrailResult = InputGuardrail.evaluate(step.prompt);
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
        const event = await this.runner.executeStep(
          {
            agent: step.agent,
            modelConfig: step.modelConfig,
            rawPrompt: step.prompt,
            sanitizedSummary: guardrailResult.sanitizedSummary,
            pipelineId,
            traceId,
          },
          step.executor
        );
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
