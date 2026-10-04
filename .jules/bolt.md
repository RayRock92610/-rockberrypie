## 2026-10-04 - Cache redundant strlen calculations
**Learning:** `strlen()` operations perform $O(N)$ string traversals. Evaluating them repeatedly in tight conditionals (like parameter validation or buffer manipulation) creates unnecessary CPU overhead.
**Action:** Always cache the result of `strlen` in a local `const size_t` variable at the nearest safe scope block and reuse it for condition checks and copying operations to avoid redundant scans.
