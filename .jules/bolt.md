## 2026-05-27 - [Add channel-order support in RTP]
**Learning:** For adding SDP parameter parsing in RTP (like channel-order), parsing logic can map standard RFC parameters into Broadcom specific audio struct properties without needing a dedicated function pointer. Using generic string parsing routines (e.g. `rtp_get_parameter_string`) makes it flexible.
**Action:** When adding parameter parsers, always add a generic fetching function instead of casting integers.
