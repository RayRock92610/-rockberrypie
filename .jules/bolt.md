## 2024-05-12 - Loop unswitching in hello_teapot deindex
**Learning:** Legacy graphics applications like hello_teapot and raspicam often contain tight loops evaluating branch conditions (like `size >= 3`) on every iteration. This loop unswitching pattern is common technical debt in this codebase.
**Action:** Always check internal tight loops for branch conditions based on variables that remain constant throughout the loop's execution.
## 2026-05-16 - Optimize duplicate strlen calls in models.c
**Learning:** Legacy parsing loops in this codebase (like `load_wavefront_obj`) often contain duplicate calls to `strlen` in condition checks and statements (e.g., `if (s[strlen(s)-1] == 10) s[strlen(s)-1]=0;`). Since `strlen` is O(N), this results in unnecessary O(2N) operations.
**Action:** Always check string parsing loops for repeated evaluations of `strlen` on the same variable, and cache the result in a local variable like `size_t slen = strlen(s);` to reduce overhead to O(N).
## 2026-05-18 - Optimize Duplicate strlen Calls

**Learning:** `strlen` calls inside initialization operations or loops can perform unnecessarily redundant O(N) string traversal calculations.
**Action:** Extract `strlen` calls out of loops and inner assignments into cached local variables, especially for string allocations (`malloc`), optimizing O(N) evaluations to O(1) reads.
## 2026-05-18 - Optimize redundant strlen calls in glShaderSource
**Learning:** Legacy GL API wrappers often iterate over string arrays (like shader sources) multiple times, calling `strlen()` on the same strings repeatedly. Since shader strings can be large, this redundant O(N) evaluation across multiple loops adds up.
**Action:** Extract the lengths into a temporary array (using a small stack buffer like `GLint cached_len_buf[32]` with a fallback to `khrn_platform_malloc` for larger arrays) in the first loop, and re-use these cached lengths in subsequent loops.
