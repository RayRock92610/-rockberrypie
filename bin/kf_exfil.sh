#!/bin/bash
# Kessel Flow: Data Exfiltration Protocol (Pillar 05)
# Pulls logs from the Vault (Z Flip 5) to the Orchestrator (S25)

VAULT_IP="10.0.0.15:37881"
REMOTE_DIR="/data/data/com.termux/files/home/bug_bounty_workspace/logs"
LOCAL_DIR="./research_vault/$(date +%Y-%m-%d)"

echo "[EXFIL] Initiating data pull from $VAULT_IP..."

# 1. Create local timestamped directory
mkdir -p "$LOCAL_DIR"

# 2. Pull the logs via the verified ADB bridge
# We use the -p flag to preserve timestamps
adb -s $VAULT_IP pull "$REMOTE_DIR/." "$LOCAL_DIR/"

if [ $? -eq 0 ]; then
    echo "[SUCCESS] Data secured in $LOCAL_DIR."
    # 3. Optional: Clear remote logs to save space on the Flip
    # adb -s $VAULT_IP shell "rm -rf $REMOTE_DIR/*"
else
    echo "[ERROR] Exfiltration failed. Is the node link active?"
fi
