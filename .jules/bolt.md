## 2026-10-04 - mmal: implement clock port connection functionality
**Learning:** In MMAL graph, clock port connection functionality was disabled via assigning the function pointer to `NULL`. The solution involves checking if the underlying graph clock port defines a `pf_connect` function. If it does, we assign the `graph_port_connect` function to the component's clock port `pf_connect` pointer, utilizing the existing port proxying pattern.
**Action:** Next time when implementing a "disabled for now" port function in the MMAL framework (e.g., `pf_connect`), verify whether a proxy mechanism (like `graph_port_connect`) exists, check if the underlying port supports it via a presence check (e.g., `graph->clock[i]->priv->pf_connect`), and assign the proxy function accordingly. Be sure to strip out "disabled" conditions (e.g., `&& 0`) to actually enable the feature.
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

## 2026-09-24 - Optimize Guardrail prompt summarization for large inputs
**Learning:** Performing regex replacements over unbounded inputs just to extract a small bounded prefix introduces severe O(N) performance penalties on large strings.
**Action:** Slice the input string down to a safe upper bound (e.g., 1024 characters) before applying expensive regex operations to achieve O(1) performance for prefix summarization.

## 2026-09-25 - Optimize Guardrail prompt evaluation with combined regex
**Learning:** Testing a string against an array of regular expressions iteratively is O(N) over the number of patterns for non-matching strings.
**Action:** Combine the regular expressions into a single `COMBINED_PATTERN` using `.map(p => p.source).join('|')`. If the combined pattern doesn't match, we can skip the loop entirely for a massive performance gain on clean inputs.

## 2026-09-26 - Handling undefined in custom JSON canonicalization
**Learning:** When replacing array `.map().join()` with manual `for` loops and string concatenation in custom JSON serialization, failing to explicitly handle `undefined` values (which `JSON.stringify()` drops for objects and serializes to `null` for arrays) causes implicit string coercion to 'undefined' or breaks deterministic formatting.

## 2026-09-27 - Optimize SQLite Last Insert Read
**Learning:** Querying the database for the latest record state (like a previous hash in an audit log) on every insert introduces unnecessary read I/O and synchronous blocking in an append-only system.
**Action:** Cache the latest deterministic state in memory to eliminate redundant SELECT queries during sequential inserts.

## 2026-10-01 - Optimize cryptographic hashing performance
**Learning:** Node.js v21+ introduces `crypto.hash()` which computes hashes in a single call. This is significantly faster (around 2x) than the stream-based `crypto.createHash().update().digest()` approach, because it avoids creating the intermediate Hash object and stream overhead.
**Action:** When computing a hash for a single string or buffer in Node.js v21+, replace `crypto.createHash(algo).update(data).digest(encoding)` with `crypto.hash(algo, data, encoding)`.

## 2026-10-03 - Optimize native C-Level File Hashing via hashlib.file_digest
**Learning:** Python 3.11+ introduces `hashlib.file_digest(fileobj, digest)` which executes the read-and-update loop entirely in C/OpenSSL, bypassing Python buffer allocations and bytecode loop evaluation for a 15–30% faster traversal hashing.
**Action:** When hashing file objects in Python 3.11+, use `hashlib.file_digest(f, "sha256")` to avoid the overhead of manual buffered reading loops.

## 2026-10-03 - Optimize SQLite Query Row Mapping
**Learning:** When querying single columns or a few fields with `better-sqlite3`, the default row mapping allocates JavaScript objects. This causes heap allocation overhead in hot paths.
**Action:** Chain `.pluck(true)` to return primitives or `.raw(true)` to return flat arrays, bypassing JavaScript object key creation and shape allocation.

## 2026-10-04 - Optimize JSON Canonicalization Verification
**Learning:** Destructuring and rebuilding a large parsed JSON object (e.g., 10+ keys) to omit a single property creates severe GC pressure and object allocation overhead inside tight iterative loops (like streaming database row validation). Assigning `undefined` to the property mutates the object in place without deoptimizing V8's hidden classes, and is efficiently ignored by our custom JSON canonicalizer.
**Action:** When performing verification hashing over large payloads from a database, mutate the `JSON.parse` output directly (e.g., `parsed.integrity.event_hash = undefined`) instead of constructing an entirely new verification object mapping all fields manually.

## $(date +%Y-%m-%d) - Overlap synchronous CPU processing with asynchronous I/O wait times in sequential loops
**Learning:** When you have a strict sequential requirement for a series of steps (so you cannot parallelize the steps using `Promise.all`), you can still often improve performance by interleaving operations. If a step involves both a CPU-bound phase (e.g. guardrail evaluation/hashing) and an async I/O-bound phase (e.g. a database write), you can overlap the CPU work of step *N+1* with the I/O await of step *N*.
**Action:** When inspecting loops iterating over sequential `await` promises, look for pure, stateless CPU work at the beginning of the loop iteration. Pre-compute the CPU work for the first element outside the loop, then inside the loop, while the current step's async I/O promise is pending (un-awaited), eagerly compute the CPU work for the next iteration. Then `await` the current step's promise.

## $(date +%Y-%m-%d) - Optimize JSON.parse in logger.ts
**Learning:** Parsing JSON and canonicalizing it during a high-volume iterative verification loop causes significant CPU overhead. Storing a pre-computed hash of the payload in the database and verifying that hash instead drastically reduces this overhead.
**Action:** When validating data integrity in a sequential pipeline where large payloads are involved, compute the payload hash at insertion time and store it as a separate column. Use this pre-computed hash for fast verification, falling back to parsing and canonicalizing only for backward compatibility with older records.

## 2026-10-04 - HashChainedLogger Canonicalize Determinism Verification
**Learning:** \`JSON.stringify\` semantics diverge between objects and arrays: object properties with \`undefined\` values are completely omitted, whereas array items with \`undefined\` values serialize as \`null\` (\`[1, null, 3]\`). Cryptographic hash chaining breaks if the custom canonicalizer deviates from this behavior.
**Action:** Enforce automated edge-case assertions covering undefined array mapping, object key lexicographical sorting, and sparse structures before verifying audit log integrity.
