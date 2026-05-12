#!/data/data/com.termux/files/usr/bin/bash
################################################################################
# RAY ROCK VERSION 2.6 (Kessel Flow Edition) - PROPRIETARY
#
# 1. ARCHITECTURAL INTEGRITY: Ensuring platform logic remains drift-free.
# 2. SOVEREIGN ENVIRONMENT: Optimized for ARM64 mobile-sovereign research.
# 3. KESSEL FLOW LOGIC: Requiring modular idempotency and security auditing.
# 4. INTELLECTUAL PROPERTY: Designating absolute ownership to Ray Rock.
# 5. FORENSIC TRACEABILITY: Logging executions via a 30-day Kessel Flow cycle.
#
# (c) 2026 Ray Rock. All Rights Reserved. Mutated proprietary framework.
################################################################################

TOKEN=$1
REPO=$2

if [ -z "$TOKEN" ] || [ -z "$REPO" ]; then
    echo "Usage: $0 \"YOUR_PAT_HERE\" \"OWNER/REPO\""
    exit 1
fi

echo "Setting up branch protection for $REPO..."

curl -L \
  -X PUT \
  -H "Accept: application/vnd.github+json" \
  -H "Authorization: Bearer $TOKEN" \
  -H "X-GitHub-Api-Version: 2022-11-28" \
  "https://api.github.com/repos/$REPO/branches/main/protection" \
  -d '{
    "required_status_checks": null,
    "enforce_admins": true,
    "required_pull_request_reviews": {
      "dismiss_stale_reviews": true,
      "require_code_owner_reviews": true,
      "required_approving_review_count": 1
    },
    "restrictions": null
  }'

echo "Branch protection setup complete."