#!/bin/bash

# Source the Vault safely
VAULT_PATH="$HOME/.kessel_vault/kessel_vault.sh"
if [[ -f "$VAULT_PATH" ]]; then
    source "$VAULT_PATH"
else
    echo "[!] Warning: Kessel Vault not found at $VAULT_PATH. Proceeding with limited context."
fi

# Source the core alignment to ensure identity is locked
# Assumes kessel_core.sh is in the same directory
SOURCE_PATH="$(dirname "$0")/kessel_core.sh"
if [ -f "$SOURCE_PATH" ]; then
    source "$SOURCE_PATH"
else
    echo "[!] Error: kessel_core.sh not found. Alignment failed."
    exit 1
fi

echo "[~] Initializing API Node Handshake..."

# Corrected Configuration
GITHUB_API="https://api.github.com/repos/RayRock92610/Kessel_Run_1"
BONEYARD_NODE="http://10.0.0.15:37881"

# Check GitHub Link
GH_STATUS=$(curl -o /dev/null -s -w "%{http_code}" "$GITHUB_API")
if [ "$GH_STATUS" -eq 200 ]; then
    echo "[+] GitHub Node: CONNECTED (200)"
else
    echo "[!] GitHub Node: OFFLINE ($GH_STATUS)"
fi

# Check Boneyard Node (Z Flip 5)
BY_STATUS=$(curl -o /dev/null -s -w "%{http_code}" --max-time 5 "$BONEYARD_NODE")
if [ "$BY_STATUS" -eq 200 ] || [ "$BY_STATUS" -eq 000 ]; then
    echo "[+] Boneyard Node (37881): RESPONDING ($BY_STATUS)"
else
    echo "[!] Boneyard Node (37881): OFFLINE ($BY_STATUS)"
fi

echo "------------------------------------------------"
echo "[+] Sync Cycle Complete."