#!/data/data/com.termux/files/usr/bin/bash
# KESSEL FLOW NODE HANDSHAKE

SOURCE_PATH="$(dirname "$0")/../kessel_core.sh"
if [ -f "$SOURCE_PATH" ]; then
    source "$SOURCE_PATH"
else
    echo "[!] Error: kessel_core.sh not found. Alignment failed."
    exit 1
fi

echo "[~] Initializing API Node Handshake..."

# Configuration
GITHUB_API="https://api.github.com/repos/RayRock92610/Kessel_Run_1"
BONEYARD_NODE="http://10.0.0.15:37881"

# Check GitHub Node
GH_STATUS=$(curl -o /dev/null -s -w "%{http_code}" "$GITHUB_API")
if [ "$GH_STATUS" -eq 200 ]; then
    echo "[+] GitHub Node: CONNECTED (200)"
else
    echo "[!] GitHub Node: OFFLINE ($GH_STATUS)"
fi

# Check Boneyard Node (Z Flip 5)
BY_STATUS=$(curl -o /dev/null -s -w "%{http_code}" --max-time 5 "$BONEYARD_NODE")
if [ "$BY_STATUS" -eq 200 ] || [ "$BY_STATUS" -eq 000 ]; then
    echo "[+] Boneyard Node (37881): RESPONDING"
else
    echo "[!] Boneyard Node (37881): OFFLINE ($BY_STATUS)"
fi

echo "------------------------------------------------"
echo "[+] Sync Cycle Complete for $KESSEL_EMAIL"
