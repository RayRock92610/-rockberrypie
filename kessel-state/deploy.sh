#!/usr/bin/env bash
# Exit immediately if a command exits with a non-zero status
set -eo pipefail

BASHRC_PATH="${HOME}/.bashrc"
STATE_FILE="${HOME}/.kessel_state.json"
STATE_DIR="${HOME}/kesselflow/state"
LOG_DIR="${HOME}/kesselflow/logs"

echo "====================================================="
echo "🧹🌙 Initializing Kessel-State Deployment Core..."
echo "====================================================="
# 1. Enforce directory workspace layout
echo "[*] Creating required state and log storage pools..."
mkdir -p "$STATE_DIR" "$LOG_DIR"
# 2. Check for dependency tooling
echo "[*] Validating core tool dependencies..."
for cmd in git jq bash; do
    if ! command -v "$cmd" &> /dev/null; then
        echo "[-] Missing system dependency: '$cmd'. Attempting automated package install..."
        if command -v pkg &> /dev/null; then
            pkg install -y "$cmd"
        else
            echo "[-] Error: Package manager 'pkg' not found. Please install '$cmd' manually."
            exit 1
        fi
    fi
done
# 3. Create placeholder configuration structures if absent
if [ ! -f "${STATE_DIR}/master_state.json" ]; then
    echo '{"status": "INITIALIZED", "role": "node"}' > "${STATE_DIR}/master_state.json"
fi
if [ ! -f "${STATE_DIR}/runtime_flags.json" ]; then
    echo '{"fido_status": "LIVE", "empire_workers": 4}' > "${STATE_DIR}/runtime_flags.json"
fi
# 4. Clean up old hooks from target shell profile
if [ -f "$BASHRC_PATH" ]; then
    echo "[*] Cleaning old telemetry targets from .bashrc profile..."
    # Safely strip prior script inject blocks
    sed -i '/# === KESSEL STATE ENGINE BLOCK ===/,/# === END KESSEL STATE ENGINE BLOCK ===/d' "$BASHRC_PATH"
else
    touch "$BASHRC_PATH"
fi
# 5. Inject the live prompt command hook
echo "[*] Injecting clean prompt tracking hook layer..."
cat << 'HOOK_EOF' >> "$BASHRC_PATH"

# === KESSEL STATE ENGINE BLOCK ===
_kessel_pulse() {
  local exit_code=$?
  # Prevent failure inside prompt command if components are missing
  echo '{
    "timestamp": "'$(date -Iseconds)'",
    "pwd": "'"$PWD"'",
    "git_branch": "'$(git branch --show-current 2>/dev/null || echo "no-git")'",
    "last_exit_code": '$exit_code',
    "last_command": "'"${BASH_COMMAND:-$0}"'",
    "files": "'$(ls -F 2>/dev/null | tr '\n' ' ' | sed 's/"/\\"/g')'",
    "processes": "'$(ps 2>/dev/null | wc -l)'"
  }' > ~/.kessel_state.json 2>/dev/null
}

# Safely append to existing PROMPT_COMMAND arrays without breaking other tools
if [[ -z "$PROMPT_COMMAND" ]]; then
  PROMPT_COMMAND="_kessel_pulse"
elif [[ "$PROMPT_COMMAND" != *"_kessel_pulse"* ]]; then
  PROMPT_COMMAND="_kessel_pulse; $PROMPT_COMMAND"
fi
# === END KESSEL STATE ENGINE BLOCK ===
HOOK_EOF
# 6. Seed initial execution runtime pulse
echo "[*] Bootstrapping fresh local memory metrics..."
export PWD
_kessel_pulse || true

echo "====================================================="
echo "[+] SUCCESS: Kessel-State integration deployed."
echo "    Run: source ~/.bashrc to wake up the system."
echo "====================================================="
