#!/bin/bash
# Admin: Ray Rock 92610
# Logic: Kessel Flow / Pillar 05 Standard
# Target: Repos 13-54 (No Overwrite)

# Configurable Paths - Set these before running locally on Cauldron
REPOS_DIR="${REPOS_DIR:-/sdcard/path/to/your/repos}"
LOG_FILE="${LOG_FILE:-/sdcard/cauldron/logs/7-day-sweep.log}"
LICENSE_TEMPLATE="${LICENSE_TEMPLATE:-/sdcard/templates/MIT_LICENSE}"

# Ensure log directory exists
mkdir -p "$(dirname "$LOG_FILE")"
touch "$LOG_FILE"

echo "[$(date '+%Y-%m-%d %H:%M:%S')] Starting Kessel Sweep V2 - Discriminator Protocol" | tee -a "$LOG_FILE"

# Iterate over repos 13 to 54
for repo in $(ls "$REPOS_DIR" | sed -n '13,54p'); do
    echo "----------------------------------------" | tee -a "$LOG_FILE"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] Processing $repo..." | tee -a "$LOG_FILE"

    TARGET_DIR="$REPOS_DIR/$repo"
    if [ ! -d "$TARGET_DIR" ]; then
        echo "Error: Directory $TARGET_DIR does not exist. Skipping." | tee -a "$LOG_FILE"
        continue
    fi

    cd "$TARGET_DIR" || continue

    # 1. Standardize README Header
    if [ -f "README.md" ]; then
        if ! grep -q "# Organization: Rockberrypie / Cauldron" README.md; then
            sed -i '1i # Organization: Rockberrypie / Cauldron\n' README.md
            echo "Prepended Org Header to README.md" | tee -a "$LOG_FILE"
        else
            echo "Org Header already exists in README.md" | tee -a "$LOG_FILE"
        fi
    else
        echo "# Organization: Rockberrypie / Cauldron" > README.md
        echo "Created new README.md with Org Header" | tee -a "$LOG_FILE"
    fi

    # 2. Ensure Standard LICENSE exists
    if [ ! -f "LICENSE" ]; then
        if [ -f "$LICENSE_TEMPLATE" ]; then
            cp "$LICENSE_TEMPLATE" LICENSE
            echo "Copied standard MIT LICENSE" | tee -a "$LOG_FILE"
        else
            echo "Warning: LICENSE_TEMPLATE not found at $LICENSE_TEMPLATE. Skipping license creation." | tee -a "$LOG_FILE"
        fi
    else
        echo "LICENSE already exists" | tee -a "$LOG_FILE"
    fi

    # 3. Handle Forks
    # If the repo has a remote named 'upstream' or a URL containing 'fork', assume it's a fork
    if git remote -v 2>/dev/null | grep -iqE "upstream|fork"; then
        echo "Identified as a FORK. Integrating Shadow Vault for $repo" | tee -a "$LOG_FILE"

        if [ ! -f "SHADOW_VAULT_INTEGRATION.md" ]; then
            echo "# SHADOW_VAULT_INTEGRATION" > SHADOW_VAULT_INTEGRATION.md
            echo "" >> SHADOW_VAULT_INTEGRATION.md
            echo "This repository is integrated with the Shadow Vault per the Pillar 05 Standard under the Kessel Flow logic." >> SHADOW_VAULT_INTEGRATION.md
            echo "Created SHADOW_VAULT_INTEGRATION.md" | tee -a "$LOG_FILE"
        else
            echo "SHADOW_VAULT_INTEGRATION.md already exists" | tee -a "$LOG_FILE"
        fi
    else
        echo "Identified as Personal Source." | tee -a "$LOG_FILE"
    fi

    # 4. Commit using Pillar 05 Standard
    if ! git diff-index --quiet HEAD -- 2>/dev/null; then
        git add README.md LICENSE SHADOW_VAULT_INTEGRATION.md 2>/dev/null || true
        git commit -m "Apply Discriminator Protocol (Pillar 05 Standard) - Ref a1db165"
        echo "Committed changes for $repo" | tee -a "$LOG_FILE"
    else
        echo "No changes to commit for $repo" | tee -a "$LOG_FILE"
    fi

    echo "[$(date '+%Y-%m-%d %H:%M:%S')] Finished $repo" | tee -a "$LOG_FILE"
done

echo "----------------------------------------" | tee -a "$LOG_FILE"
echo "[$(date '+%Y-%m-%d %H:%M:%S')] Kessel Sweep V2 Complete." | tee -a "$LOG_FILE"
