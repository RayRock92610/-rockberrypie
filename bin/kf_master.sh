#!/bin/bash
# Kessel Flow: Master Execution Loop (Updated with Cloud Sync)

# 1. Integrity Gate
./bin/kf_guard.sh
if [ $? -ne 0 ]; then echo "[CRITICAL] Guard failed. Aborting."; exit 1; fi

# 2. Data Movement
./bin/kf_exfil.sh

# 3. Intelligence Generation
./bin/kf_sift.sh

# 4. Cloud Archival (Pillar 03)
echo "[SYNC] Pushing Intelligence Brief to GitHub..."
git add INTELLIGENCE_REPORT_$(date +%Y-%m-%d).md
git commit -m "Kessel Flow: Automated Intelligence Brief - $(date +%Y-%m-%d)"
git push origin main

echo "[SUCCESS] Kessel Run Complete. Data is local and mirrored to GitHub."
