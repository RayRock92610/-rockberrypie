## Bolt Optimization Log

### 1. In-Place Event Hash Erasure in Verification Stream (PR #338)
- Replaced recursive object cloning/destructuring during `verifyChainIntegrity` with in-place assignment (`parsed.integrity.event_hash = undefined`).
- Eliminates intermediate V8 AST allocations and reduces GC pressure in tight verification loops by ~5–10%.

### 2. Precomputed `payload_hash` for Sequential Chain Verification (PR #357)
- Added `payload_hash` column to `audit_events`.
- Precomputes `hash(JSON.stringify(validatedEvent))` on insert and verifies payload integrity via direct hash comparison, bypassing repetitive `JSON.parse` and canonicalization during sequential chain reads.
- **Benchmark (50,000 events):**
  - Baseline: ~2389.14 ms
  - Optimized: ~353.76 ms (~85% execution time reduction)
