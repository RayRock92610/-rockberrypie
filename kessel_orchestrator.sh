#!/usr/bin/env bash
set -euo pipefail

WORKSPACE_DIR="$(pwd)"
LOG_FILE="$WORKSPACE_DIR/workspace/logs/op_log.txt"
mkdir -p "$WORKSPACE_DIR/workspace/logs"

echo "=== KESSEL FLOW ORCHESTRATOR INITIALIZED ===" | tee -a "$LOG_FILE"
echo "Timestamp: $(date -u)" | tee -a "$LOG_FILE"

# Phase 1: Environment & Config Check
check_env() {
    echo "[*] Phase 1: Checking environment configurations..." | tee -a "$LOG_FILE"
    if [[ -f "config/model_config.json" ]]; then
        echo "  [OK] Model configuration found." | tee -a "$LOG_FILE"
    else
        echo "  [WARN] Missing config/model_config.json." | tee -a "$LOG_FILE"
    fi
    if [[ -f "config/clive_roster.json" ]]; then
        echo "  [OK] Clive agent roster found." | tee -a "$LOG_FILE"
    else
        echo "  [WARN] Missing config/clive_roster.json." | tee -a "$LOG_FILE"
    fi
}

# Phase 2: Witch Hunter Security Audit
run_security_scan() {
    echo "[*] Phase 2: Executing Witch Hunter Static Analysis..." | tee -a "$LOG_FILE"
    if [[ -f "scripts/witch_hunter/rules_engine.sh" ]]; then
        bash scripts/witch_hunter/rules_engine.sh
    else
        echo "  [FAIL] Witch Hunter rules engine script missing at scripts/witch_hunter/rules_engine.sh" | tee -a "$LOG_FILE"
        exit 1
    fi
}

# Pipeline Execution
if [[ "${1:-}" == "--check-env" ]]; then
    check_env
elif [[ "${1:-}" == "--security-only" ]]; then
    run_security_scan
else
    check_env
    run_security_scan
    echo "=== KESSEL FLOW ORCHESTRATION SUCCESSFUL ===" | tee -a "$LOG_FILE"
fi
