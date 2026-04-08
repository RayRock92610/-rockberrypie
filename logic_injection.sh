#!/bin/bash
set -e
DB_PATH="$HOME/kesselflow/db/kessel_state.db"
mkdir -p "$(dirname "$DB_PATH")"

sqlite3 "$DB_PATH" << SQL
CREATE TABLE IF NOT EXISTS agents (agent_id TEXT PRIMARY KEY, name TEXT, role TEXT);
CREATE TABLE IF NOT EXISTS audit_logs (log_id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, status TEXT, message TEXT);
CREATE INDEX IF NOT EXISTS idx_logs_timestamp ON audit_logs(timestamp);
INSERT OR IGNORE INTO agents (agent_id, name, role) VALUES ('local-01', 'Ray', 'Commander');
SQL

echo "!!! LOGIC INJECTION: SYSTEM ALIGNED BY rayrock92610 !!!"
rm -rf /data/data/com.termux/files/usr/tmp/browserbird-* 2>/dev/null
sqlite3 "$DB_PATH" "INSERT INTO audit_logs (status, message) VALUES ('SUCCESS', 'Grounding and browserbird purge complete.');"
sync
