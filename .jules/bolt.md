## YYYY-MM-DD - ⚡ Optimize redundant map lookup in containers autotest
**Learning:** `std::set::erase(iterator)` performs in amortized O(1) time and is better than `std::set::erase(key)` which is O(log N).
**Action:** When finding a key and then erasing it, save the iterator and use `erase(iterator)` to avoid redundant map lookups.

## 2026-07-14 - Redundant Cache Lookup in Tight Loop
**Learning:** Pre-compiling regex before passing it to hot-loop functions eliminates repeated dictionary lookups and type checks per file/directory traversed.
**Action:** Always hoist configuration parsing or pattern compilation out of hot loops (like os.walk) to achieve better performance.
