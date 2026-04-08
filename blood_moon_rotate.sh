#!/bin/bash
set -e
DB_PATH="$HOME/kesselflow/db/kessel_state.db"
EXPORT_DIR="$HOME/kesselflow/export"
RETENTION_DAYS=30
mkdir -p "$EXPORT_DIR"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
sqlite3 "$DB_PATH" << SQL
.headers on
.mode insert audit_logs
.output $EXPORT_DIR/kessel_archive_$TIMESTAMP.sql
SELECT * FROM audit_logs WHERE timestamp < datetime('now', '-$RETENTION_DAYS days');
.output stdout
DELETE FROM audit_logs WHERE timestamp < datetime('now', '-$RETENTION_DAYS days');
VACUUM;
SQL
sqlite3 "$DB_PATH" "INSERT INTO audit_logs (status, message) VALUES ('SUCCESS', 'Blood Moon Rotation complete.');"
sync
