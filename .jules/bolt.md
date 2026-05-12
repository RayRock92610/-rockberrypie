## 2024-05-12 - Loop unswitching in hello_teapot deindex
**Learning:** Legacy graphics applications like hello_teapot and raspicam often contain tight loops evaluating branch conditions (like `size >= 3`) on every iteration. This loop unswitching pattern is common technical debt in this codebase.
**Action:** Always check internal tight loops for branch conditions based on variables that remain constant throughout the loop's execution.
