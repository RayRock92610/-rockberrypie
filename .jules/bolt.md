## 2023-10-06 - Empty file hashing performance
**Learning:** Checking for 0-byte files using `os.fstat(f.fileno()).st_size == 0` after opening the file descriptor avoids an extra `stat` system call and safely short-circuits empty file hashes, although the real-world performance gain may be negligible unless the directory is overwhelmingly populated by empty files.
**Action:** When calculating cryptographic hashes of files (e.g., in a traversal script), use `os.fstat(f.fileno()).st_size == 0` after opening the file to check if it is empty and return the known empty string hash (e.g., for SHA-256) directly.

## $(date +%Y-%m-%d) - [Cache redundant strlen calls]
**Learning:** Bounds checking logic relying on `strncat` and a running `current_len` must carefully handle string truncation. Unconditionally incrementing `current_len` by the source string length (e.g., `strlen(tmp)`) rather than the number of actually appended characters breaks boundary checks and leads to stack buffer overflow.
**Action:** Always compute the safe copy length explicitly via `(tmp_len < space) ? tmp_len : space` when updating buffer position offsets.
