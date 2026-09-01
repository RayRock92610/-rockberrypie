import { z } from 'zod';

export const AgentSchema = z.object({
  id: z.string().min(1),
  role: z.string().min(1),
  persona: z.string().min(1),
});

export const ModelConfigSchema = z.object({
  provider: z.string().min(1),
  model: z.string().min(1),
  temperature: z.number().min(0).max(2),
  top_p: z.number().min(0).max(1).optional(),
  seed: z.number().int().nullable(),
  system_instruction_hash: z.string().length(64).optional(),
});

export const InputSchema = z.object({
  raw_prompt_hash: z.string().length(64),
  sanitized_input_summary: z.string(),
});

export const ReasoningStepSchema = z.object({
  step_number: z.number().int().positive(),
  thought: z.string(),
  decision: z.string(),
  confidence_score: z.number().min(0).max(1).optional(),
});

export const ToolCallSchema = z.object({
  tool_name: z.string().min(1),
  parameters: z.record(z.string(), z.unknown()),
  execution_status: z.enum(['SUCCESS', 'FAILED', 'BLOCKED_BY_GUARDRAIL']),
  output_hash: z.string().length(64).optional(),
  duration_ms: z.number().int().nonnegative(),
});

export const StateDeltaSchema = z.object({
  target_subsystem: z.string().min(1),
  mutations: z.array(z.record(z.string(), z.unknown())),
});

export const IntegritySchema = z.object({
  prev_event_hash: z.string().length(64),
  event_hash: z.string().length(64),
});

export const AgentExecutionEventSchema = z.object({
  event_id: z.string().uuid(),
  timestamp: z.string().datetime(),
  trace_id: z.string().min(1),
  pipeline_id: z.string().min(1),
  agent: AgentSchema,
  model_config: ModelConfigSchema,
  input: InputSchema,
  reasoning_trace: z.array(ReasoningStepSchema),
  tool_calls: z.array(ToolCallSchema),
  state_delta: StateDeltaSchema,
  integrity: IntegritySchema,
});

export type Agent = z.infer<typeof AgentSchema>;
export type ModelConfig = z.infer<typeof ModelConfigSchema>;
export type Input = z.infer<typeof InputSchema>;
export type ReasoningStep = z.infer<typeof ReasoningStepSchema>;
export type ToolCall = z.infer<typeof ToolCallSchema>;
export type StateDelta = z.infer<typeof StateDeltaSchema>;
export type Integrity = z.infer<typeof IntegritySchema>;
export type AgentExecutionEvent = z.infer<typeof AgentExecutionEventSchema>;
