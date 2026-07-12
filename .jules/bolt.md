## YYYY-MM-DD - ⚡ Optimize redundant map lookup in containers autotest
**Learning:** `std::set::erase(iterator)` performs in amortized O(1) time and is better than `std::set::erase(key)` which is O(log N).
**Action:** When finding a key and then erasing it, save the iterator and use `erase(iterator)` to avoid redundant map lookups.

## 2024-07-12 - Optimize bash grep with while loop using awk
**Learning:** In bash scripts, filtering and acting on lines using a `grep ... | while read ...` loop is an anti-pattern that introduces high subshell and execution overhead compared to a single-pass tool.
**Action:** Replaced `grep | while read` loop with single-pass `awk` to eliminate subshell and execution overhead in `kesselflow-witchhunter/scripts/drift_parser.sh`.
