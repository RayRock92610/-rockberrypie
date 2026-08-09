#!/bin/bash
# KesselFlow SOP Sync - Truth-First Veracity Check
# Path: /app/kesselflow/scripts/sop_sync.sh

set -euo pipefail

BY_PATH="/app/boneyard/KesselFlow/SOPs"
LOG_PATH="/app/boneyard/logs/sop_delta.log"
CURRENT="/app/kesselflow/current_state.json"
MASTER="$BY_PATH/master_state.json"
HISTORY_DIR="$BY_PATH/history"
TIMESTAMP=$(date +%Y-%m-%d_%H%M)

# 1. State Check: Verify BoneYard mount and expected files
if [ ! -d "$BY_PATH" ]; then
    echo "ERROR: BoneYard not mounted at $BY_PATH" | tee -a "$LOG_PATH"
    return 1 2>/dev/null || return 1
fi

if [ ! -f "$CURRENT" ]; then
    echo "[$TIMESTAMP] ERROR: local current_state.json missing: $CURRENT" | tee -a "$LOG_PATH"
    return 1 2>/dev/null || return 1
fi

if [ ! -d "$HISTORY_DIR" ]; then
    mkdir -p "$HISTORY_DIR"
    echo "[$TIMESTAMP] Created missing history dir: $HISTORY_DIR" | tee -a "$LOG_PATH"
fi

# Ensure a canonical master_state exists to diff against
if [ ! -f "$MASTER" ]; then
    cp "$CURRENT" "$MASTER"
    echo "[$TIMESTAMP] Initialized master_state.json from local copy" >> "$LOG_PATH"
fi

# 2. Delta Generation: lightweight “changed?” check
TMP_DIFF="/tmp/sop_delta_${TIMESTAMP}.diff"

# diff -q returns 0 if same, 1 if different, 2 if error
if diff -q "$CURRENT" "$MASTER" > "$TMP_DIFF" 2>&1; then
    # Files are identical
    echo "[$TIMESTAMP] No drift detected. Veracity 100%." >> "$LOG_PATH"
    rm -f "$TMP_DIFF"
else
    # Files differ (or diff failed; check the TMP_DIFF for error text)
    if [ $? -eq 1 ]; then
        echo "[$TIMESTAMP] Delta detected. Updating BoneYard SOPs." >> "$LOG_PATH"
        cp "$CURRENT" "$MASTER"
        cat "$TMP_DIFF" >> "${HISTORY_DIR}/delta_${TIMESTAMP}.log"
        rm -f "$TMP_DIFF"
    else
        # diff itself failed (e.g., I/O issue)
        echo "[$TIMESTAMP] ERROR: diff failed (I/O or permission issue)" | tee -a "$LOG_PATH"
        cat "$TMP_DIFF" >> "$LOG_PATH" 2>/dev/null || return 1
        rm -f "$TMP_DIFF"
        return 1 2>/dev/null || return 1
    fi
fi
