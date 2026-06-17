## 2026-06-16 - Safe deprecation of function parameters exposed externally
**Learning:** When cleaning up unused functionality or hacks in a public header file (`khrn_prod_4.h`), deleting function prototypes or changing their signatures directly can break Application Binary Interface (ABI) compatibility for closed-source blobs or external consumers that link against it.
**Action:** Always run an `nm -g` or `find . -type f \( -name "*.a" -o -name "*.so" -o -name "*.o" \) -exec nm -g {} \;` search to see if the symbol exists in a binary implementation before fully removing the prototype. If the implementation is missing from source but may be provided externally, preserve the function signature (number/types of arguments) but rename the deprecated arguments to `unused_X` and add a `/* deprecated */` comment.

## 2026-06-17 - Optimize FDT node lookups to single-pass O(N)
**Learning:** Flattened Device Tree operations using `fdt_next_node()` combined with property accessors like `fdt_getprop()` resulted in inefficient O(N^2) scaling because `fdt_next_node()` scans properties to reach the next node, and `fdt_getprop()` re-scans properties locally.
**Action:** When implementing custom parsing loops in FDT or similar serial formats, use low-level tag access (`fdt_next_tag`) to read streams in a strict single-pass O(N) traversal.
