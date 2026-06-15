## 2024-05-19 - Code Health Improvement: Added Getters to RaspiCamControl
**Learning:** Legacy C libraries often lack explicit get/set pairs for all properties, especially opaque structures managed via property dictionaries or function endpoints. In this specific layer (MMAL camera configuration), parameters are retrieved generically by port ID (`mmal_port_parameter_get*`), which causes friction for higher-level consumers needing direct property access on structured states like `RASPICAM_CAMERA_PARAMETERS`. Implementing explicit getters encapsulating these MMAL generic parameter retrievals simplifies the client interface significantly.
**Action:** When working with similar driver interfaces built on message queues or dynamic parameter lists, abstract generic gets/sets into struct-specific getters/setters systematically to avoid fragmented manual struct population in higher-level application logic.
## 2024-05-29 - Avoid strlen() in Loops
**Learning:** Calling `strlen()` inside loops (especially tight or frequently called loops like those processing URIs or network schemes) introduces redundant O(N) overhead per iteration.
**Action:** When working with static arrays of strings or structures containing strings, pre-calculate and store the string lengths in the structure initialization. This allows O(1) length lookups during loop execution.
## 2024-06-02 - Optimize string concatenation loop in gencmd
**Learning:** In C, concatenating strings in a loop using `strncat` and `strlen` to find the end of the string results in O(N^2) time complexity. This is an anti-pattern for performance when building large strings from parts.
**Action:** Maintain an explicit `offset` counter and use functions like `vcos_safe_strcpy` (or `memcpy` with size checks) to append data at the known offset in O(1) time per concatenation.
## 2024-06-02 - Optimize file filtering in drift_detection.py
**Learning:** When optimizing file filtering in deep directory traversals (like `os.walk`) in Python, avoid calling `fnmatch.fnmatch()` iteratively inside loops, which causes O(N*M) overhead. Instead, pre-compile multiple glob patterns into a single regular expression. Additionally, when caching pre-compiled regex patterns for functions that accept varying filter lists (like exclusion dicts in `drift_detection.py`), avoid using a single global variable which causes stale matches.
**Action:** Use a dictionary cache keyed by the pattern tuple (e.g., `_CACHE[tuple(patterns)]`) to store the compiled regex, avoiding O(N*M) overhead for multiple patterns while ensuring correctness for dynamic exclusion lists.
## 2026-06-14 - Redundant Variable Initialization in Inner Python Loops
**Learning:** In tight inner loops, specifically those repeatedly invoking a function (like O(N) traversals), re-evaluating expressions (e.g. `list_a + list_b` followed by casting to a `tuple`) at the start of each iteration incurs huge overhead in Python.
**Action:** When a loop involves calling a filtering/matching function with static rules, evaluate those static rules outside the loop and pass the finalized configuration object (e.g., a pre-computed `tuple`) downward. Here it resulted in a ~3.8x speedup.
## 2024-06-14 - Test vcos_generic_blockpool_alloc
**Learning:** Adding unit tests for VCOS components requires checking `CMakeLists.txt` for `VCOS_EXCLUDE_TESTS` blocks and modifying them to ensure test directories are processed. When allocating and testing VCOS blockpools, `vcos_blockpool_create_on_heap` handles the setup perfectly. Ensure to call `vcos_init()` before testing VCOS library functions.

**Action:** When adding tests for VCOS abstractions, ensure you do not use hallucinated underlying properties (e.g., `num_free_blocks`). Instead, test via the public API (like verifying allocations return `NULL` when exhausted). Ensure the new test executable is properly linked and wrapped via `add_test` in `CMakeLists.txt`.
## 2024-05-24 - Optimize drift detection file hashing
**Learning:** Checking against existing sets/dicts before executing expensive I/O operations (like file hashing) drastically reduces runtime in verification scripts. In drift detection, new files inherently differ from the baseline, meaning their hash is not required to flag them as "new".
**Action:** When comparing file states against a baseline, verify existence in the baseline dictionary before calculating hashes to prevent unnecessary disk reads for newly added files.

## 2024-05-18 - [Fix ISO range bounds]
**Learning:** Modern camera components use hardware/driver specific bounds; when encountering ambiguous `TODO` boundaries in configuration, it is beneficial to look closely at man pages (`raspicam.7`) or associated documentation, which frequently explicitly sets out the valid domain intervals intended by the original authors (e.g., ISO ranges of 100-800) instead of relying heavily on default magic numbers alone.
**Action:** When adding clamps for user parameters, review existing `.7` and `.1` pages within the directory prior to relying entirely on external hardware specs to ensure fidelity to the explicit project documentation.
