## 2026-10-04 - Fast-path Drift Detection
**Learning:** Checking for file drift by calculating hashes on every file is extremely expensive due to I/O and CPU requirements. By caching `stat()` results (`size` and `mtime`) in the baseline, we can fast-path unmodified files without hashing them.
**Action:** Always include fast paths for I/O operations by utilizing cheap filesystem metadata checks.
