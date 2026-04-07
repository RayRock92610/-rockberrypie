#!/bin/bash
# Kessel Flow: Witch Hunter Guard Logic (Pillar 04)
# Validates script integrity before execution.

MANIFEST=".witch_hunter_manifest"
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}[WITCH HUNTER] Initiating Integrity Scan...${NC}"

# Check if manifest exists
if [ ! -f "$MANIFEST" ]; then
    echo -e "${RED}[ERROR] Manifest missing! Entering DEEP FREEZE.${NC}"
    exit 1
fi

# Function to verify a specific script
verify_script() {
    local script_path=$1
    echo -n "Checking $script_path... "

    # Extract the stored hash from the manifest
    # We use grep to find the specific file line and awk to get the hash
    STORED_HASH=$(grep "$script_path" "$MANIFEST" | awk '{print $1}' | tail -n 1)
    CURRENT_HASH=$(sha256sum "$script_path" | awk '{print $1}')

    if [ -z "$STORED_HASH" ]; then
        echo -e "${RED}NOT FOUND IN MANIFEST${NC}"
        return 1
    elif [ "$STORED_HASH" != "$CURRENT_HASH" ]; then
        echo -e "${RED}HASH MISMATCH! (Potential Tampering)${NC}"
        return 1
    else
        echo -e "${GREEN}VERIFIED${NC}"
        return 0
    fi
}

# Run checks on core Pillar 06 tools
FAILURES=0
verify_script "bin/kf_revive.sh" || ((FAILURES++))
verify_script "bin/kf_node_link.sh" || ((FAILURES++))

if [ $FAILURES -gt 0 ]; then
    echo -e "\n${RED}[CRITICAL] $FAILURES integrity failures detected.${NC}"
    echo -e "${RED}[STATUS] LOCKDOWN: Access to Vault denied.${NC}"
    exit 1
else
    echo -e "\n${GREEN}[SAFE] All systems verified. Kessel Flow is GO.${NC}"
    exit 0
fi
