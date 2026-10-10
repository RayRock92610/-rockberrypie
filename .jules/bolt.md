## 2023-10-06 - Empty file hashing performance
**Learning:** Checking for 0-byte files using `os.fstat(f.fileno()).st_size == 0` after opening the file descriptor avoids an extra `stat` system call and safely short-circuits empty file hashes, although the real-world performance gain may be negligible unless the directory is overwhelmingly populated by empty files.
**Action:** When calculating cryptographic hashes of files (e.g., in a traversal script), use `os.fstat(f.fileno()).st_size == 0` after opening the file to check if it is empty and return the known empty string hash (e.g., for SHA-256) directly.

## 2026-10-10 - Key Sorting Fast-Path in Canonicalizer
**Learning:** Calling Array.prototype.sort on single-key or empty objects introduces avoidable sorting overhead in hot hashing paths.
**Action:** Fast-path `keys.length <= 1` before invoking `.sort()` in canonicalization helpers.
