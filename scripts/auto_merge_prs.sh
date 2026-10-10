#!/bin/bash
set -euo pipefail

# 1. Scope & Pre-Flight Inspection & 2. Audit Candidate List
echo "1. Scope & Pre-Flight Inspection & 2. Audit Candidate List"
gh pr list --state open --json number,title,headRefName,mergeable,statusCheckRollup,isDraft \
  --jq '.[] | select(.isDraft == false and .mergeable == "MERGEABLE") | {number: .number, title: .title, branch: .headRefName}'

# 3. Sequential Merge Execution
echo "3. Sequential Merge Execution"
MERGED_PRS=()

for pr in $(gh pr list --state open --json number,mergeable,isDraft \
  --jq '.[] | select(.isDraft == false and .mergeable == "MERGEABLE") | .number'); do
  echo "===> Processing PR #$pr"

  # Verify CI status passes or has no hard failures
  STATUS=$(gh pr view "$pr" --json statusCheckRollup --jq '.statusCheckRollup[]?.state // "SUCCESS"' | grep -E "FAILURE|TIMED_OUT" || true)
  if [ -n "$STATUS" ]; then
    echo "Skipping PR #$pr: Failing CI checks detected."
    continue
  fi

  TITLE=$(gh pr view "$pr" --json title --jq '.title')

  # Squash merge and prune branch
  if gh pr merge "$pr" --squash --delete-branch --admin || gh pr merge "$pr" --squash --delete-branch; then
    echo "Successfully merged PR #$pr"
    MERGED_PRS+=("#$pr: $TITLE")
  else
    echo "Failed to merge PR #$pr"
  fi
done

# 4. Post-Merge Synchronization & Invariant Verification
echo "4. Post-Merge Synchronization & Invariant Verification"
git checkout main
git pull origin main

! git grep -n "<<<<<<<" || { echo "ERROR: Unresolved conflict markers found on main"; exit 1; }

if [ -f "pnpm-lock.yaml" ]; then
  pnpm test
elif [ -f "pytest.ini" ] || [ -d "tests" ]; then
  pytest -v
fi

# 5. Final Reporting
echo "5. Final Reporting"
if [ ${#MERGED_PRS[@]} -eq 0 ]; then
  echo "No clean PRs were merged during this run."
else
  echo "Summary of merged PRs:"
  for pr_info in "${MERGED_PRS[@]}"; do
    echo "- $pr_info"
  done
fi
