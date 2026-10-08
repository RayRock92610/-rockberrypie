## $(date +%Y-%m-%d) - Optimize `RaspiCamControl.c` and `rtsp_reader.c`
**Learning:** Naively updating a string length tracker using `current_len += strlen(tmp)` after `strncat(dest, tmp, limit)` can lead to incorrect lengths if `tmp` is truncated, potentially causing buffer overflow on subsequent operations.
**Action:** Always account for string truncation when updating cached lengths. For example: `int space_left = MAX - current_len - 1; int tmp_len = strlen(tmp); current_len += (tmp_len < space_left) ? tmp_len : space_left;`.
