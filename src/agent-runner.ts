import crypto from 'node:crypto';
import { HashChainedLogger } from './logger.js';
import { AgentExecutionEvent } from './types/logger.js';

export interface AgentRunOptions {
  agent: AgentExecutionEvent['agent'];
  modelConfig: AgentExecutionEvent['model_config'];
  rawPrompt: string;
  rawPromptHash: string;
  sanitizedSummary: string;
  pipelineId: string;
  traceId?: string;
}

export class AgentRunner {
  private logger: HashChainedLogger;

  constructor(logger: HashChainedLogger) {
    this.logger = logger;
  }

  /**
   * Executes a managed agent invocation step and records the complete trace
   */
  public async executeStep(
    options: AgentRunOptions,
    executor: () => Promise<{
      reasoningTrace: AgentExecutionEvent['reasoning_trace'];
      toolCalls: AgentExecutionEvent['tool_calls'];
      stateDelta: AgentExecutionEvent['state_delta'];
    }>
  ): Promise<AgentExecutionEvent> {
    const eventId = crypto.randomUUID();
    const traceId = options.traceId || `trace-${crypto.randomUUID()}`;
    // ⚡ Bolt Optimization: Reuse pre-computed hash from earlier in pipeline to avoid redundant CPU overhead.
    const promptHash = options.rawPromptHash;

    const startTime = Date.now();
    
    // Execute the model logic/tool calls via the injected executor function
    const executionResult = await executor();

    const loggedEvent = this.logger.logEvent({
      event_id: eventId,
      trace_id: traceId,
      pipeline_id: options.pipelineId,
      agent: options.agent,
      model_config: options.modelConfig,
      input: {
        raw_prompt_hash: promptHash,
        sanitized_input_summary: options.sanitizedSummary,
      },
      reasoning_trace: executionResult.reasoningTrace,
      tool_calls: executionResult.toolCalls,
      state_delta: executionResult.stateDelta,
    });

    return loggedEvent;
  }
}
