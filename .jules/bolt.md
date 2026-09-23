## YYYY-MM-DD - ⚡ Optimize redundant map lookup in containers autotest
**Learning:** `std::set::erase(iterator)` performs in amortized O(1) time and is better than `std::set::erase(key)` which is O(log N).
**Action:** When finding a key and then erasing it, save the iterator and use `erase(iterator)` to avoid redundant map lookups.

## 2026-07-14 - Redundant Cache Lookup in Tight Loop
**Learning:** Pre-compiling regex before passing it to hot-loop functions eliminates repeated dictionary lookups and type checks per file/directory traversed.
**Action:** Always hoist configuration parsing or pattern compilation out of hot loops (like os.walk) to achieve better performance.
## 2026-09-19 - Edge Case Testing for File Reading Permissions
**Learning:** Checking for IOError when attempting to read a file isn't just about missing files, it also covers permission denied cases. Unittest mock wasn't enough to properly cover the physical file permission behavior, so actual filesystem tests using os.chmod provide higher fidelity.
**Action:** When testing file I/O operations, use `os.chmod` to construct real unreadable file scenarios instead of purely mocking the open function.

## 2026-07-18 - Optimize bash while read loop to awk
**Learning:** When filtering and acting on lines in a file within bash scripts, avoid using `grep ... | while read ...` loops as they introduce high subshell and execution overhead. Use single-pass tools like `awk` or `sed` to maximize performance by minimizing process spawning.
**Action:** Replace grep | while read with awk for single-pass file processing in bash scripts.

## 2026-07-18 - Missing Error Path Test for verify_kessel_context.py (Missing 2 vars)
**Learning:** Adding a test for specifically two missing environment variables increases test coverage and explicitly asserts the logic for aggregating the exact variables missing. Utilizing `@patch.dict(os.environ, {...}, clear=True)` effectively handles clearing out unnecessary environment variables to isolate the tested edge case.
**Action:** When adding missing path error tests that require environment isolation, default to using `@patch.dict` with `clear=True` instead of manually removing keys inside the test.
## 2026-08-01 - Optimize C string copying loops
**Learning:** When copying strings in C, replacing open-coded character-by-character loops (e.g. `for(; *p!='\0'; ...)`) with standard library functions like `strlen()` and `memcpy()` significantly improves performance by utilizing optimized block memory operations.
**Action:** Replace open-coded loops for string copying with block memory operations like `memcpy`.

## 2026-08-04 - Optimize character string searches with strcspn
**Learning:** When searching for the first occurrence of any character from a set of delimiters in a C string, manually looping and calling `strchr` repeatedly introduces significant overhead. Replacing these loops with the standard library function `strcspn` utilizes highly optimized block memory operations for a massive performance gain.
**Action:** Replace open-coded `strchr` loops with `strcspn` for efficient character set matching.

## 2026-09-17 - Optimize SQLite Prepared Statements in Logger
**Learning:** Repeatedly calling `db.prepare()` inside frequently executed functions (like event loggers) causes severe CPU overhead due to query recompilation in `better-sqlite3`.
**Action:** Always cache prepared statements in class properties or variables for reuse, especially for database operations within hot paths or high-frequency event handlers.

## 2026-09-19 - Optimize SQLite Dataset Queries
**Learning:** When querying large datasets (e.g., log chains) using 'better-sqlite3', using the '.all()' method pulls the entire dataset into an array in memory, causing memory spikes and CPU overhead.
**Action:** Use the '.iterate()' method instead of '.all()'. This streams the rows iteratively, significantly reducing memory spikes and CPU overhead.

## 2026-09-19 - Enable SQLite WAL mode
**Learning:** SQLite's default rollback journal mode can cause significant I/O overhead on frequent inserts. Enabling Write-Ahead Logging (WAL) and setting synchronous=NORMAL drastically reduces this disk write latency.
**Action:** When initializing a SQLite database that expects a high volume of writes (like an event logger), execute `PRAGMA journal_mode = WAL;` and `PRAGMA synchronous = NORMAL;`.

## 2026-09-20 - Optimize string serialization by removing regex fast-path
**Learning:** Manually checking strings with a regex to conditionally bypass `JSON.stringify()` is an anti-pattern. Node's native `JSON.stringify()` is highly optimized and often faster than the combined overhead of executing a RegExp test in JavaScript and performing manual string concatenation, even for simple strings.
**Action:** Rely on standard library `JSON.stringify()` for string serialization rather than attempting manual regex-based fast paths.

## 2026-09-21 - Optimize Redundant Cryptographic Hashing
**Learning:** In sequential orchestration pipelines, recomputing cryptographic hashes (e.g., SHA-256) of large string payloads across different stages (like guardrails and agent execution) introduces redundant CPU overhead.
**Action:** Compute the hash once at the earliest stage and pass it in the options object to subsequent steps to eliminate redundant recomputation.

## 2026-09-23 - Optimize JSON Canonicalization
**Learning:** When optimizing deterministic JSON canonicalization for cryptographic hashing, custom recursive serialization can be heavily sped up by replacing intermediate array allocations (like `.map().join()`) with traditional `for` loops and optimizing type-checking branch order.
**Action:** Avoid intermediate array creations in hot recursive paths, and use direct string concatenation with `for` loops to eliminate memory allocation overhead.
