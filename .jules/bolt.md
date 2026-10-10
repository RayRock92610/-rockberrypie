## $(date +%Y-%m-%d) - Optimize `RaspiCamControl.c` and `rtsp_reader.c`
**Learning:** Naively updating a string length tracker using `current_len += strlen(tmp)` after `strncat(dest, tmp, limit)` can lead to incorrect lengths if `tmp` is truncated, potentially causing buffer overflow on subsequent operations.
**Action:** Always account for string truncation when updating cached lengths. For example: `int space_left = MAX - current_len - 1; int tmp_len = strlen(tmp); current_len += (tmp_len < space_left) ? tmp_len : space_left;`.
## 2026-10-10 - Optimize Guardrail Evaluation
**Learning:** Applying regex replacements directly to unbounded string payloads can incur severe O(N) performance penalties.
**Action:** When generating a fixed-length summary from a potentially unbounded input string using regex replacements, slice the input down to a safe upper bound buffer (e.g., 1024 characters) prior to executing the regex to avoid severe O(N) performance penalties on large payloads.
