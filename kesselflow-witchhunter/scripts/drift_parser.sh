#!/bin/bash
# Pulse Report: Drift Event Parser for Kessel Flow
LOG_FILE="events.jsonl"

echo "--- Witch Hunter: Pulse Report ---"
# ⚡ Bolt: Use a single-pass awk instead of grep | while read
# to eliminate subshell execution overhead.
# Logic to identify if drift is Additive or Overwrite
awk '/"event_type":"drift"/ { print "Alert: System Drift Detected - " $0 }' "$LOG_FILE"