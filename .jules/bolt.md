## $(date +%Y-%m-%d) - Optimize `RaspiCamControl.c` and `rtsp_reader.c`
**Learning:** Naively updating a string length tracker using `current_len += strlen(tmp)` after `strncat(dest, tmp, limit)` can lead to incorrect lengths if `tmp` is truncated, potentially causing buffer overflow on subsequent operations.
**Action:** Always account for string truncation when updating cached lengths. For example: `int space_left = MAX - current_len - 1; int tmp_len = strlen(tmp); current_len += (tmp_len < space_left) ? tmp_len : space_left;`.

## 2026-10-10 - Bounded Guardrail Sanitization
**Learning:** In C applications, check existing call sites carefully before hoisting strlen to prevent unconditional executions. In TS/JS guardrails, slicing input before regex scanning eliminates redundant allocations and mitigates regex traversal overhead on large payloads.
**Action:** Always verify candidate patterns exist verbatim in target source trees before applying hoisting refactors.
