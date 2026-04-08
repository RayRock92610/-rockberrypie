#!/bin/bash
DB_PATH="$HOME/kesselflow/db/kessel_state.db"
TARGETS=("Void_Encoder" "browserbird")
for proc in "${TARGETS[@]}"; do
    PIDS=$(pgrep -f "$proc" | grep -v "$$")
    if [ ! -z "$PIDS" ]; then
        kill -9 $PIDS 2>/dev/null
        sqlite3 "$DB_PATH" "INSERT INTO audit_logs (status, message) VALUES ('THREAT_NEUTRALIZED', 'Killed $proc threads: $PIDS');"
    fi
done
sync
