# Phase 15: Cloud, Security, Vault, And Transport Release Hardening

## Phase Intent

Remove placeholder security and transport behavior from the release path.

## Release-Ready Exit Criteria

- release-path cloud sync does not rely on placeholder crypto,
- transport clients either work for supported scope or are clearly gated,
- user-visible sync and security messaging is honest and recoverable.

## Task Count

3

## Task P15-T01

- Phase ID: P15
- Task ID: P15-T01
- Task Title: Replace placeholder XOR encryption and stub KDF behavior in cloud sync
- Priority: P0
- Category: Release Blocker
- Objective: remove explicitly insecure placeholder encryption from the release path.
- Why This Matters Now: this is a direct release blocker for any cloud or security-adjacent workflow.
- Release Gap Statement: `CloudSyncService` still uses XOR placeholder encryption and placeholder key derivation comments.
- User / Product Impact: insecure or placeholder crypto invalidates trust in sync and vault-related promises.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/EncryptionService.h`
- Prior Plan References: `v23 Phase 12`; `v24 Phase 15`
- Scope: encryption, decryption, key derivation, configuration validation, failure reporting for release-path sync.
- Out of Scope: future provider breadth unrelated to supported release scope.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/KeyManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/VaultEncryptionManager.cpp`
- Related Features / Systems / Components: cloud sync; encryption; key management; vaults
- Current Behavior: release-path crypto is explicitly placeholder and insecure.
- Intended Release-Ready Behavior: supported sync and vault paths use real cryptography and fail safely.
- Missing Pieces: production crypto integration; migration strategy; error handling.
- Technical Approach: implement supported crypto primitives through the project’s chosen security stack and gate unsupported states.
- Implementation Steps:
1. Replace XOR placeholder paths with real authenticated encryption.
2. Replace placeholder KDF logic with production-safe derivation.
3. Add configuration and failure tests.
- Validation Steps:
1. Encrypt/decrypt valid data and fail on tampered data.
2. Verify sync workflows reject invalid keys or configs clearly.
- Acceptance Criteria: no release-path cloud or vault encryption path relies on placeholder crypto.
- Dependencies: none
- Parallelization Notes: transport client work can proceed in parallel once crypto interfaces stabilize.
- Risks / Failure Modes: migration incompatibility; silent data corruption; unsafe defaults.
- Observability / Diagnostics Notes: log crypto setup failures without leaking secrets.
- Rollback / Safety Notes: do not expose sync features if secure initialization fails.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`

## Task P15-T02

- Phase ID: P15
- Task ID: P15-T02
- Task Title: Complete or gate S3 and WebDAV transport behavior for supported release scope
- Priority: P1
- Category: Release Readiness
- Objective: ensure network transport clients either function for supported flows or are not exposed as complete.
- Why This Matters Now: release path must not lean on curl skeletons or partial list/head/get behavior.
- Release Gap Statement: transport implementations still include incomplete or stub-like behavior.
- User / Product Impact: broken sync transports create direct trust and data-integrity risk.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/S3Client.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp`
- Prior Plan References: `v23 Phase 12`; `v24 Phase 15`
- Scope: connection test, list, upload, download, error reporting for supported providers.
- Out of Scope: unsupported provider expansion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SyncEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`
- Related Features / Systems / Components: S3; WebDAV; sync engine; connection tests
- Current Behavior: transport clients still read as partial implementations.
- Intended Release-Ready Behavior: supported providers are real and recoverable; unsupported ones are gated.
- Missing Pieces: supported-provider matrix; real transport coverage; robust error handling.
- Technical Approach: narrow the release support matrix, finish that slice, and explicitly gate the rest.
- Implementation Steps:
1. Decide which providers are release-supported.
2. Finish required operations for those providers.
3. Hide or reject unsupported provider paths.
- Validation Steps:
1. Run connection, upload, and download scenarios.
2. Validate error handling for auth, missing resources, and transport failures.
- Acceptance Criteria: no release-supported transport path relies on incomplete stub behavior.
- Dependencies: P15-T01
- Parallelization Notes: can run with UI gating and sync-state work.
- Risks / Failure Modes: partial protocol handling; unsafe retries; weak error reporting.
- Observability / Diagnostics Notes: include provider, operation, and failure class in diagnostics.
- Rollback / Safety Notes: disable provider selection when support is incomplete.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp`

## Task P15-T03

- Phase ID: P15
- Task ID: P15-T03
- Task Title: Add sync, security, and vault recovery diagnostics and release-gate coverage
- Priority: P1
- Category: Diagnostics / Recovery
- Objective: make sync and security failures visible, actionable, and signoff-relevant.
- Why This Matters Now: secure systems must fail safely and observably, not opaquely.
- Release Gap Statement: even completed security paths remain risky without recovery and diagnostics proof.
- User / Product Impact: when sync fails, users need clear cause and safe fallback behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyncHistoryLogger.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyncConflictManager.cpp`
- Prior Plan References: `v23 Phase 12`; `v24 Phase 15`; `v24 Phase 19`
- Scope: sync failure notifications, history logging, conflict handling, vault error messaging, smoke coverage.
- Out of Scope: enterprise policy expansion beyond supported release scope.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SafeMode.h`
- Related Features / Systems / Components: sync history; conflict handling; privacy; safe mode
- Current Behavior: sync notifications exist, but failure coverage and signoff evidence still need tightening.
- Intended Release-Ready Behavior: sync and security failures are transparent, recoverable, and visible in release evidence.
- Missing Pieces: failure smoke suite; security signoff criteria; conflict-path validation.
- Technical Approach: add supported failure scenarios to the smoke and subsystem criteria systems.
- Implementation Steps:
1. Define sync/security failure scenarios that must pass or fail safely.
2. Add diagnostics and history outputs for those scenarios.
3. Bind them into subsystem done criteria.
- Validation Steps:
1. Trigger auth failure, network failure, and conflict scenarios.
2. Confirm diagnostics, notifications, and recovery behavior.
- Acceptance Criteria: security and sync failures are explicit, recoverable, and included in signoff evidence.
- Dependencies: P01-T03; P15-T01; P15-T02
- Parallelization Notes: largely validation-focused.
- Risks / Failure Modes: silent sync failure; ambiguous recovery state.
- Observability / Diagnostics Notes: preserve redact-safe logs and sync histories.
- Rollback / Safety Notes: default to safe failure and no data mutation on uncertain state.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/SyncConflictManager.cpp`
