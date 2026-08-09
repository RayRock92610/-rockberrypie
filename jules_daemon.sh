#!/bin/bash
# KESSEL FLOW - JULES DAEMON (v18.0)
# DYNAMIC VAULT / NO STATIC REPO MOUNTS

while true; do
    # SEARCH: Find the Boneyard without a static ID
    DYNAMIC_PATH=$(find /storage -maxdepth 3 -name "Boneyard" 2>/dev/null | head -n 1)

    if [ -n "$DYNAMIC_PATH" ]; then
        ln -sfn "$DYNAMIC_PATH" "$HOME/Kessel_Vault"
        # SYNC: Perform the 18-Entity Sweep (Pillar 05)
        ~/kessel_flow/health_check.sh --silent-sync
    fi

    # SLEEP: Standby for the next hourly pulse
    sleep 3600
done
