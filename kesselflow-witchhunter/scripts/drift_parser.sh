#!/bin/bash
# Pulse Report: Drift Event Parser for Kessel Flow
LOG_FILE="events.jsonl"

echo "--- Witch Hunter: Pulse Report ---"
# ⚡ Bolt: Replaced grep | while read with awk to eliminate subshell and loop overhead for large log files
# Logic to identify if drift is Additive or Overwrite
awk '/"event_type":"drift"/ { print "Alert: System Drift Detected - " $0 }' "$LOG_FILE"