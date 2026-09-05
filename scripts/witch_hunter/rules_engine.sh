#!/usr/bin/env bash
set -euo pipefail

LOG_FILE="workspace/logs/witch_hunter.log"
mkdir -p "$(dirname "$LOG_FILE")"

echo "=== [WITCH HUNTER] STATIC ANALYSIS & LOGIC DRIFT SCAN ===" | tee "$LOG_FILE"
echo "Timestamp: $(date -u)" | tee -a "$LOG_FILE"
echo "--------------------------------------------------------" | tee -a "$LOG_FILE"

VIOLATIONS=0
EXCLUDES=(
  "--exclude-dir=.git"
  "--exclude-dir=node_modules"
  "--exclude-dir=venv"
  "--exclude-dir=scripts"
)

# Rule WH-001: Check for potential unvalidated IDOR parameters in route definitions
echo "[*] Checking Rule WH-001: IDOR / Route Parameter Validation..." | tee -a "$LOG_FILE"
if grep -rnE "${EXCLUDES[@]}" "def .*\((.*id:.*|.*user_id.*)\):" . 2>/dev/null | grep -v "Depends(" > /dev/null; then
    echo "  [WARN] Potential unvalidated IDOR parameter found without FastAPI dependency injection." | tee -a "$LOG_FILE"
    ((VIOLATIONS++))
else
    echo "  [PASS] IDOR parameter checks satisfied." | tee -a "$LOG_FILE"
fi

# Rule WH-002: Check for missing timeout handling or dangerous clock references
echo "[*] Checking Rule WH-002: Logic Drift & Timeout Controls..." | tee -a "$LOG_FILE"
if grep -rnE "${EXCLUDES[@]}" "(time\.sleep\(0\)|timeout\s*=\s*None)" . 2>/dev/null; then
    echo "  [WARN] Unbounded timeout or zero-sleep logic drift risk identified." | tee -a "$LOG_FILE"
    ((VIOLATIONS++))
else
    echo "  [PASS] Logic drift timeout parameters strictly set." | tee -a "$LOG_FILE"
fi

# Rule WH-003: Check for unsafe header overrides
echo "[*] Checking Rule WH-003: Header Injection & Verb Tampering..." | tee -a "$LOG_FILE"
if grep -rnE "${EXCLUDES[@]}" "(X-Forwarded-For|X-Original-URL)" . 2>/dev/null; then
    echo "  [WARN] Unchecked proxy header references detected." | tee -a "$LOG_FILE"
    ((VIOLATIONS++))
else
    echo "  [PASS] No raw header override vulnerabilities found." | tee -a "$LOG_FILE"
fi

echo "--------------------------------------------------------" | tee -a "$LOG_FILE"
echo "[RESULT] Scan completed with $VIOLATIONS violation(s)." | tee -a "$LOG_FILE"
