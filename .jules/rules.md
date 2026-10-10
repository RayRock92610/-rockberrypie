# Autonomous Git Merge & Conflict Resolution Protocol

## 1. Core Operating Principles
* **Zero Blind Overwrites:** Never resolve conflicts by running wholesale `git checkout --ours` or `git checkout --theirs`. Do not discard feature branch work by adopting upstream files in bulk.
* **Invariant Preservation:** When reconciling conflicting hunks, all active repository invariants must be preserved:
  * **Sentinel Security Invariants:**
    * Zero information leakage: Return generic, sanitized error responses to clients (e.g., `{"error": "Internal processing error", "status": "failed"}`). Never expose `str(e)`, file paths, or raw stack traces.
    * Server-side diagnostics: Retain `logger.error("...: %s", type(err).__name__, exc_info=True)`.
    * AST Exception Chaining: When re-raising in an `except` block, enforce explicit chaining (`raise ... from None` or `raise ... from err`). Never leave bare `raise` or unchained exception conversions.
    * Defensive Execution: Retain command execution allowlists with strict timeouts (`timeout=30`), network client timeouts, HMAC authentication gates, and DB connection pooling.
  * **Database & Schema Invariants:**
    * Preserve existing index names and definitions while introducing composite or covering indexes.
    * Never index unbounded text/payload columns that degrade write throughput.
  * **Performance & Fast-Paths:**
    * Retain canonicalizer fast-paths (e.g., key sorting checks `if (keys.length > 1)`) and safe configuration defaults (e.g., `Variable.get(..., default_var="")`).

---

## 2. Standard Conflict Resolution Workflow

When a task requires rebasing, merging, or resolving conflicts against upstream (`main`/`master`):

### Step 1: Branch Isolation & Upstream Fetch
```bash
git fetch origin main
BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)
git merge origin/main --no-commit --no-ff || true
# List all conflicted files
git status --porcelain | grep "^UU" | awk "{print \$2}"

# Locate all active conflict markers
git grep -n "<<<<<<<"
! git grep -n "<<<<<<<" || { echo "ERROR: Unresolved conflict markers found."; exit 1; }
# Python repositories
python3 -m compileall .

# TypeScript / Node repositories
pnpm build || npm run build || tsc --noEmit
# Python: Check syntax errors and undefined names
flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics

# TypeScript: Lint checks
pnpm lint || npm run lint
# Python
if [ -f "pytest.ini" ] || [ -d "tests" ]; then
  pytest -v
else
  python3 -m unittest discover -s tests -p "test_*.py" -v
fi

# TypeScript / Vitest
pnpm test || npm test
# Confirm the diff is not empty and contains the feature payload
git diff origin/main --stat
git add -u
git commit -m "fix(merge): reconcile upstream conflicts preserving security and schema invariants"
git push origin "$BRANCH_NAME"
mkdir -p ./patches
git format-patch -1 HEAD -o ./patches/
```

### How to Deploy Permanently

1. **Commit to Default Branches:**
   Add this file to `.jules/rules.md` in:
   * `RayRock92610/Candyland` (`main`)
   * `RayRock92610/Rockflow` (`main`)
   * `RayRock92610/-rockberrypie` (`master`)
   * `RayRock92610/skills` (`main`)

2. **Jules Global Settings (Optional Backup):**
   In the Jules Web UI, navigate to [jules.google.com/settings](https://jules.google.com/settings) and append the contents of **Section 1 (Core Operating Principles)** to your global profile instructions so new tasks in any repository adhere to this protocol by default.
