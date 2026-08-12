#!/usr/bin/env bash
set -euo pipefail

# 1. Define paths
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$SCRIPT_DIR"

VENV_PATH="${HOME}/.venv/bin"
ENTRYPOINT="${VENV_PATH}/comet"

# 2. Guard check entrypoint existence and permissions
if [[ ! -x "${ENTRYPOINT}" ]]; then
    # Fallback to python module execution if binary launcher isn't symlinked
    if [[ -x "${VENV_PATH}/python3" ]]; then
        ENTRYPOINT="${VENV_PATH}/python3 -m comet"
    else
        echo "[ERROR] Comet entrypoint not found at ${ENTRYPOINT}" >&2
        python3 -c 'import sys; sys.exit(127)'
    fi
fi

# 3. Environment Export
export OLLAMA_HOST="${OLLAMA_HOST:-127.0.0.1:11434}"

# 4. Execute
exec ${ENTRYPOINT} "$@"
