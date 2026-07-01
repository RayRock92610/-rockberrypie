## 2026-07-01 - Linking local OpenMAX IL core

**Learning:** When linking to local OpenMAX IL core using CMake, do not blindly add the `PRIVATE` keyword when using `target_link_libraries` if the preceding `target_link_libraries` in the same target does not use it. Mixing plain and keyword signatures on the same target will cause CMake errors.

**Action:** Always check how `target_link_libraries` is already used for the target in the CMake file, and follow the existing style (i.e., if it uses plain signature, use plain signature; if it uses `PRIVATE/PUBLIC`, use `PRIVATE/PUBLIC`).
