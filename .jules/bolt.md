## YYYY-MM-DD - ⚡ Optimize redundant map lookup in containers autotest
**Learning:** `std::set::erase(iterator)` performs in amortized O(1) time and is better than `std::set::erase(key)` which is O(log N).
**Action:** When finding a key and then erasing it, save the iterator and use `erase(iterator)` to avoid redundant map lookups.
## 2026-07-10 - ⚡ Remove grep | while read anti-pattern
**Learning:** The `grep | while read` pattern in Bash is an anti-pattern that creates high subshell and execution overhead because it spawns new processes per line read. Using a single pass text processing tool like `awk` maximizes performance by minimizing process spawning.
**Action:** When filtering and acting on lines in a file within bash scripts, avoid using `grep | while read` loops. Use single-pass tools like `awk` or `sed`.
