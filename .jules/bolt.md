## $(date +%Y-%m-%d) - Optimize `RaspiCamControl.c` and `rtsp_reader.c`
**Learning:** Naively updating a string length tracker using `current_len += strlen(tmp)` after `strncat(dest, tmp, limit)` can lead to incorrect lengths if `tmp` is truncated, potentially causing buffer overflow on subsequent operations.
**Action:** Always account for string truncation when updating cached lengths. For example: `int space_left = MAX - current_len - 1; int tmp_len = strlen(tmp); current_len += (tmp_len < space_left) ? tmp_len : space_left;`.
## 2026-10-10 - Precompile Static Regex in `InputGuardrail`
**Learning:** Re-compiling a regex literal inside a hot path `.replace(/.../g, '')` adds overhead, particularly on high-volume evaluation functions like `InputGuardrail.evaluate`.
**Action:** Always extract and hoist static `RegExp` literals into class statics or module-scope constants when they are heavily reused across loop iterations or high-frequency invocations, and use index-based `for` loops rather than `for..of` iterators.
