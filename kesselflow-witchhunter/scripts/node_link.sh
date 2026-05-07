#!/bin/bash
# Kessel Flow Pillar 06.02: Node Link & API Handler

# Load Authority Credentials (via Environment variable directly)
if [ -z "$JULES_KEY" ]; then
    echo -e "\033[0;31m[!] JULES_KEY environment variable is not set. Aborting.\033[0m"
    exit 1
fi

# API Endpoints
GITHUB_API="https://api.github.com/repos/RayRock92610/Kessel_Run_1"
BONEYARD_NODE="http://10.0.0.15:8000/v1/sync"

# Color definitions
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}[NODE LINK]${NC} Initializing API Handshake..."

# 1. Check GitHub API Status
GH_STATUS=$(curl -s -o /dev/null -w "%{http_code}" -H "Authorization: token $JULES_KEY" $GITHUB_API)

if [ "$GH_STATUS" -eq 200 ]; then
    echo -e "${GREEN}[SUCCESS]${NC} GitHub Node Link established."
else
    echo -e "${RED}[ERROR]${NC} GitHub API unreachable (Status: $GH_STATUS)"
fi

# 2. Check Z Flip 5 Boneyard Node
NODE_STATUS=$(curl -s --connect-timeout 5 -o /dev/null -w "%{http_code}" $BONEYARD_NODE)

if [ "$NODE_STATUS" -eq 200 ]; then
    echo -e "${GREEN}[SUCCESS]${NC} Boneyard API Node (Z Flip 5) is online."
else
    echo -e "${RED}[OFFLINE]${NC} Boneyard Node at 10.0.0.15 unreachable."
fi