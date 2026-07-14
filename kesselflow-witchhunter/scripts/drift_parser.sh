#!/bin/bash
# Pulse Report: Drift Event Parser for Kessel Flow
LOG_FILE="events.jsonl"

echo "--- Witch Hunter: Pulse Report ---"
# Logic to identify if drift is Additive or Overwrite
# ⚡ Bolt: Replaced grep | while read with a single-pass awk to avoid subshell process spawning overhead
awk '/"event_type":"drift"/ { print "Alert: System Drift Detected - " $0 }' "$LOG_FILE"