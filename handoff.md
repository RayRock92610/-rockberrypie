# KesselFlow / BoneYard Session Handoff

## Current State
KesselFlow is a living DevOps ecosystem running in an emulated Termux environment. It utilizes a daily 24hr SOP review cadence. Files are stored in the BoneYard (`/sdcard/boneyard/KesselFlow/SOPs/`) as markdown/JSON.

We just implemented the 24hr SOP synchronization via `sop_sync.sh` installed as a Termux cron job to verify state drift against the canonical `master_state.json`.

The workflow tree now includes:
- `~/kesselflow/scripts/sop_sync.sh`: The main veracity checking script.
- `~/kesselflow/current_state.json`: The local state file.
- `/sdcard/boneyard/KesselFlow/SOPs/master_state.json`: The authoritative state.

## Key Decisions
- **Daily 24hr SOP Cadence**: Enforces consistency across sessions.
- **Termux Cron Integration**: Automated state sync ensures the BoneYard remains updated without manual intervention.
- **Structured Handoffs**: Critical for continuous execution memory (implemented via `handoff.md` and `handoff.json`).

## Last Priorities (Completed/Active)
1. Solidify 24hr SOP automation (Implemented Termux cron).
2. Build handoff schema (`handoff.json` structured).
3. Extend workflow tree (`sop_sync.sh` added).
4. Test cross-session continuity.
5. Optimize drift detection (Implemented diff check in `sop_sync.sh`).

## Resume Prompt
"Resume from last session. I have reviewed the structured handoff in `handoff.md` and `handoff.json`. Continue extending the workflow tree by introducing the next agent pillar or service endpoint. Run a test drift via `sop_sync.sh` to ensure the delta is properly logged to BoneYard history."
