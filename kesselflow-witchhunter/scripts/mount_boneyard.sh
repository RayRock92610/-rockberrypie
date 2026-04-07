#!/data/data/com.termux/files/usr/bin/bash
PEACH='\033[38;5;210m'
RESET='\033[0m'

# 1. Setup Termux Storage (If not already done)
termux-setup-storage

# 2. Create the Anchor Point
mkdir -p ~/Boneyard_Active

# 3. Mount/Link the 2TB Muscle
# Android usually mounts external drives under /storage/UUID/
ln -sfn /storage/57C4-E7FF ~/Boneyard_Active

# 4. Verify the connection
if [ -d "$HOME/Boneyard_Active" ]; then
    echo -e "${PEACH}[Jules] Muscle Mounted. 2TB Boneyard is online.${RESET}"
    ls -lh ~/Boneyard_Active
else
    echo "[!] Mount failed. Ensure OTG is enabled and the drive is plugged in."
fi