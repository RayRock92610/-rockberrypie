## YYYY-MM-DD - ⚡ Optimize redundant map lookup in containers autotest
**Learning:** `std::set::erase(iterator)` performs in amortized O(1) time and is better than `std::set::erase(key)` which is O(log N).
**Action:** When finding a key and then erasing it, save the iterator and use `erase(iterator)` to avoid redundant map lookups.

## 2026-07-14 - Redundant Cache Lookup in Tight Loop
**Learning:** Pre-compiling regex before passing it to hot-loop functions eliminates repeated dictionary lookups and type checks per file/directory traversed.
**Action:** Always hoist configuration parsing or pattern compilation out of hot loops (like os.walk) to achieve better performance.
## $(date +%Y-%m-%d) - Edge Case Testing for File Reading Permissions
**Learning:** Checking for IOError when attempting to read a file isn't just about missing files, it also covers permission denied cases. Unittest mock wasn't enough to properly cover the physical file permission behavior, so actual filesystem tests using os.chmod provide higher fidelity.
**Action:** When testing file I/O operations, use `os.chmod` to construct real unreadable file scenarios instead of purely mocking the open function.

## 2026-07-18 - Optimize bash while read loop to awk
**Learning:** When filtering and acting on lines in a file within bash scripts, avoid using `grep ... | while read ...` loops as they introduce high subshell and execution overhead. Use single-pass tools like `awk` or `sed` to maximize performance by minimizing process spawning.
**Action:** Replace grep | while read with awk for single-pass file processing in bash scripts.

## 2026-07-18 - Missing Error Path Test for verify_kessel_context.py (Missing 2 vars)
**Learning:** Adding a test for specifically two missing environment variables increases test coverage and explicitly asserts the logic for aggregating the exact variables missing. Utilizing `@patch.dict(os.environ, {...}, clear=True)` effectively handles clearing out unnecessary environment variables to isolate the tested edge case.
**Action:** When adding missing path error tests that require environment isolation, default to using `@patch.dict` with `clear=True` instead of manually removing keys inside the test.
