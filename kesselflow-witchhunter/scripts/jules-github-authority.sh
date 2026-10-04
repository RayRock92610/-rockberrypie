#!/data/data/com.termux/files/usr/bin/bash
# TITLE: JULES-GITHUB-AUTHORITY-V1
# ---------------------------------------------------------

# 1. COLOR PROFILE (PEACH DRAGON)
PEACH='\033[38;5;210m'
RESET='\033[0m'

# 2. CREDENTIALS (Handed from Vault)
# Read JULES_KEY from environment to avoid hardcoding secrets
if [ -z "$JULES_KEY" ]; then
    echo -e "${PEACH}[!] JULES_KEY environment variable is not set. Aborting.${RESET}"
    exit 1
fi
GH_USER="RayRock92610"
REPO_NAME="bug_bounty_workspace"

echo -e "${PEACH}[Jules] Authority Key Engaged. Initiating Kessel Flow Push...${RESET}"

# 3. INITIALIZE LOCAL WORKSPACE (If not already a git repo)
cd ~/$REPO_NAME
if [ ! -d ".git" ]; then
    echo "[Jules] Initializing local repository..."
    git init -b main
fi

# 4. CONFIGURE GITHUB AUTHENTICATION
# This uses the key as a Personal Access Token (PAT) for the session
git remote remove origin 2>/dev/null
export GH_USER
export JULES_KEY
git remote add origin "https://github.com/${GH_USER}/${REPO_NAME}.git"
git config --local credential.helper '!f() { echo "username=${GH_USER}"; echo "password=${JULES_KEY}"; }; f'

# 5. THE DRAGON AUDIT (Final check before push)
echo -e "${PEACH}[Dragon] Running final audit... Burning Witches at the source.${RESET}"
WITCHES=$(grep -rE "([0-9a-zA-Z]{32,})" . --exclude-dir=".git" 2>/dev/null)
if [ ! -z "$WITCHES" ]; then
    echo -e "${PEACH}[!] WITCH DETECTED. Push aborted for security.${RESET}"
    exit 1
fi

# 6. COMMIT & PUSH (Pillar 05 Standard)
echo "[Jules] Committing Pillar 05 Standard updates..."
git add .
git commit -m "Kessel Flow: Pillar 05 Standardized Deployment (Automated by Jules)"
git push -u origin main

echo -e "${PEACH}[Jules] Kessel Flow successfully pushed to GitHub. Sky is clear.${RESET}"
