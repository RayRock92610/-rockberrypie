#!/bin/bash
# [PROJECT_KESSEL] 🦅🛠️
# Module: Boneyard Sync (Off-Device Archive)
# Authorized by: rayrock92610

set -e
DB_PATH="$HOME/kesselflow/db/kessel_state.db"
EXPORT_DIR="$HOME/kesselflow/export"
BONEYARD_PATH="/sdcard/Boneyard" # Verified USB mount point

# 1. MOUNT DETECTION
if [ ! -d "$BONEYARD_PATH" ]; then
    echo "ERROR: Boneyard (2TB USB) not detected at $BONEYARD_PATH."
    sqlite3 "$DB_PATH" "INSERT INTO audit_logs (status, message) VALUES ('FAIL', 'Boneyard sync failed: USB not mounted');"
    exit 1
fi

echo "[$(date)] KESSEL: Boneyard detected. Syncing staged archives..."

# 2. ATOMIC MOVE
# Moves all .sql and .log files from the export staging area to the USB
COUNT=$(ls -1 "$EXPORT_DIR"/*.{sql,log} 2>/dev/null | wc -l)

if [ "$COUNT" -gt 0 ]; then
    mv "$EXPORT_DIR"/*.{sql,log} "$BONEYARD_PATH/"
    sqlite3 "$DB_PATH" "INSERT INTO audit_logs (status, message) VALUES ('SUCCESS', 'Moved $COUNT files to Boneyard persistence.');"
    echo "[$(date)] KESSEL: $COUNT files archived successfully."
else
    echo "[$(date)] KESSEL: No files staged for export."
fi

sync
