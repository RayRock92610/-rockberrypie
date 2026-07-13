#!/bin/bash
# Pulse Report: Drift Event Parser for Kessel Flow
LOG_FILE="events.jsonl"

echo "--- Witch Hunter: Pulse Report ---"
# ⚡ Bolt: Replaced grep and while read with single-pass awk to eliminate subshell overhead
awk '/"event_type":"drift"/ { print "Alert: System Drift Detected - " $0 }' "$LOG_FILE"
