## YYYY-MM-DD - ⚡ Optimize redundant map lookup in containers autotest
**Learning:** `std::set::erase(iterator)` performs in amortized O(1) time and is better than `std::set::erase(key)` which is O(log N).
**Action:** When finding a key and then erasing it, save the iterator and use `erase(iterator)` to avoid redundant map lookups.

## 2026-07-13 - Avoid subshell loops in bash
**Learning:** Filtering and acting on lines in a file using `grep ... | while read ...` is an anti-pattern that introduces high subshell and execution overhead.
**Action:** Use single-pass tools like `awk` or `sed` to maximize performance by minimizing process spawning.
