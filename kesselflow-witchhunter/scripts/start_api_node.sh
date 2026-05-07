#!/bin/bash
# Kessel Flow Pillar 06: API Node Server Start

echo "Killing any ghost processes on port 34281..."
# Kill any ghost processes first
fuser -k 34281/tcp

echo "Restarting with the Kessel Flow Node Link binding..."
# Restart with the Kessel Flow Node Link binding
uvicorn main:app --host 0.0.0.0 --port 34281 --reload