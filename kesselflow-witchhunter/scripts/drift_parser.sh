#!/bin/bash
# Pulse Report: Drift Event Parser for Kessel Flow
LOG_FILE="events.jsonl"

echo "--- Witch Hunter: Pulse Report ---"
awk '/"event_type":"drift"/ { print "Alert: System Drift Detected - " $0 }' $LOG_FILE
