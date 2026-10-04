import crypto from 'node:crypto';

export interface GuardrailResult {
  passed: boolean;
  riskScore: number; // 0.0 (clean) to 1.0 (high risk)
  sanitizedSummary: string;
  violations: string[];
  rawPromptHash: string;
}

export class InputGuardrail {
  private static INJECTION_PATTERNS = [
    /ignore\s+(all\s+)?previous\s+instructions/i,
    /system\s+prompt\s+override/i,
    /disregard\s+(above|prior)\s+rules/i,
    /<script[\s\S]*?>[\s\S]*?<\/script>/i,
    /rm\s+-rf\s+\//i,
    /drop\s+database/i,
  ];

  /**
   * Computes SHA-256 digest of raw input
   */
  public static hashInput(input: string): string {
    return crypto.createHash('sha256').update(input, 'utf8').digest('hex');
  }

  /**
   * Evaluates input string against injection vectors and produces a sanitized summary
   */
  public static evaluate(input: string): GuardrailResult {
    const violations: string[] = [];
    let riskScore = 0.0;

    for (const pattern of this.INJECTION_PATTERNS) {
      if (pattern.test(input)) {
        violations.push(`Pattern match: ${pattern.source}`);
        riskScore += 0.35;
      }
    }

    // Cap risk score at 1.0
    riskScore = Math.min(riskScore, 1.0);
    const passed = riskScore < 0.5;

    // Generate sanitized summary (stripping unsafe control characters)
    const sanitizedSummary = input
      .replace(/[\u0000-\u001F\u007F-\u009F]/g, '')
      .trim()
      .slice(0, 256);

    return {
      passed,
      riskScore,
      sanitizedSummary,
      violations,
      rawPromptHash: this.hashInput(input),
    };
  }
}
