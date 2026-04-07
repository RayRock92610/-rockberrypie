#!/data/data/com.termux/files/usr/bin/bash
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Witch Hunter: Boneyard Connection Check
if [ -d "/mnt/media/MuscleDrive" ]; then
    echo -e "${GREEN}[SUCCESS]${NC} Jules has established the Boneyard link."
    exit 0
else
    echo -e "${RED}[ERROR]${NC} Connection missing. Alerting Jules..."
    exit 1
fi