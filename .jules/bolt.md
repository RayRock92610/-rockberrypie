## YYYY-MM-DD - ⚡ Optimize redundant map lookup in containers autotest
**Learning:** `std::set::erase(iterator)` performs in amortized O(1) time and is better than `std::set::erase(key)` which is O(log N).
**Action:** When finding a key and then erasing it, save the iterator and use `erase(iterator)` to avoid redundant map lookups.

## 2025-07-08 - ⚡ Optimize bash script by replacing subshell pipe with awk
**Learning:** `grep | while read` is an anti-pattern in bash scripting due to the high subshell and execution overhead. Using tools like `awk` can execute the task considerably faster.
**Action:** When filtering and acting on lines in a file, prefer using a single `awk` or `sed` command over a `grep | while read` loop to minimize process spawning overhead.
