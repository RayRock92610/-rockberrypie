#!/bin/bash
# Pillar 06.03: ADB Revive Protocol
echo "Executing ADB Revive Protocol for Z Flip 5 Node..."
adb kill-server
adb start-server
adb reconnect
echo "Protocol complete."
