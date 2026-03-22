# Phase 15 - Cloud Sync, Security, Vault, And Transport Completion

## Phase Goal

Finish or gate networked and security-sensitive product areas so placeholder transport and encryption logic no longer sits in production paths.

## Measurable Outcome

- Cloud sync uses real transport and encryption behavior or is explicitly gated.
- Security-sensitive pathways no longer rely on compilation placeholders.
- Transport failures are observable and recoverable.

### Task P15-T01

- Phase ID: `P15`
- Task ID: `P15-T01`
- Task Title: Replace placeholder encryption in `CloudSyncService`
- Priority: `P0`
- Category: `Migration Completion`
- Objective: Remove XOR placeholder encryption from production cloud-sync paths.
- Why This Matters Now: This is both a completion and product-safety issue.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp` still uses non-secure placeholder encryption.
- User / Product Impact: Cloud sync cannot be considered trustworthy.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`
- Prior Plan References: `v23` Phase 12 and 17
- Scope: Encryption/decryption, key derivation, nonce/tag handling, error paths
- Out of Scope: New provider types outside current S3/WebDAV scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/EncryptionService.h`
- Related Features / Systems / Components: Cloud sync, encryption, security
- Current Behavior: Placeholder cryptography exists in production code.
- Intended Completed Behavior: Cloud sync uses real authenticated encryption or is gated out of release.
- Missing Pieces: Real crypto integration and failure handling
- Technical Approach: Use the existing security stack or platform crypto backend behind clear service interfaces.
- Implementation Steps: Replace XOR placeholder; integrate real crypto; validate interoperability and error handling.
- Validation Steps: Encrypt/decrypt sync payloads, corrupt ciphertext intentionally, and verify safe failure.
- Acceptance Criteria: No production cloud-sync path uses placeholder encryption.
- Dependencies: None
- Parallelization Notes: Can proceed with transport work immediately.
- Risks / Failure Modes: Migration can create backward compatibility issues for previously synced data.
- Cleanup / Migration Notes: Add migration or incompatibility handling for old placeholder-encrypted payloads.
- Observability / Diagnostics Notes: Log encryption mode, payload sizes, and auth-failure diagnostics without leaking secrets.
- Rollback / Safety Notes: Fail closed on encryption errors.
- References / Context: Encryption and cloud-sync services
- Example scenarios where useful: Syncing a workspace uses real authenticated encryption and fails safely on tampered payloads.

### Task P15-T02

- Phase ID: `P15`
- Task ID: `P15-T02`
- Task Title: Finish S3 and WebDAV transport implementations or gate them from release
- Priority: `P0`
- Category: `Migration Completion`
- Objective: Replace curl skeleton behavior with real provider functionality or remove production claims.
- Why This Matters Now: Provider transport completeness determines whether cloud sync is real or decorative.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/S3Client.cpp` and `/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp` remain incomplete.
- User / Product Impact: Provider-specific sync can fail silently or behave inconsistently.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/S3Client.cpp`, `/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp`
- Prior Plan References: `v23` Phase 12 and 17
- Scope: Connect/test/list/get/put/head behaviors, auth, error mapping
- Out of Scope: Additional providers beyond current scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/HttpClient.h`
- Related Features / Systems / Components: Cloud providers, transport, sync
- Current Behavior: Provider clients are skeletal.
- Intended Completed Behavior: S3 and WebDAV clients support the operations required by sync flows with real error handling.
- Missing Pieces: Real request execution, result parsing, retries, auth validation
- Technical Approach: Complete provider clients behind shared transport contracts and fail-fast readiness checks.
- Implementation Steps: Implement provider calls; map errors; add retry/backoff and test-connection behavior; update sync orchestration.
- Validation Steps: Run provider-specific sync scenarios and forced network failures.
- Acceptance Criteria: Cloud provider clients no longer depend on skeletal placeholder operations in release paths.
- Dependencies: `P15-T01`
- Parallelization Notes: S3 and WebDAV can be implemented in parallel.
- Risks / Failure Modes: Provider auth and network variability can create unstable behavior without careful retries and timeouts.
- Cleanup / Migration Notes: Remove "curl skeleton" comments and placeholder branches once complete.
- Observability / Diagnostics Notes: Emit provider, operation, status code, retry count, and latency diagnostics.
- Rollback / Safety Notes: Gate incomplete providers rather than partially shipping them.
- References / Context: S3 and WebDAV client implementations
- Example scenarios where useful: Listing remote files on WebDAV returns real results and propagates auth failures cleanly to the UI.

### Task P15-T03

- Phase ID: `P15`
- Task ID: `P15-T03`
- Task Title: Finish sync conflict, retry, and recovery behavior on top of real transport
- Priority: `P1`
- Category: `Diagnostics / Recovery`
- Objective: Make sync failures and conflicts survivable rather than opaque.
- Why This Matters Now: Real transport makes conflict and retry behavior more important than placeholder "success" flows.
- Execution Gap Statement: Sync orchestration still needs stronger recovery and conflict behavior after transport completion.
- User / Product Impact: Sync can otherwise feel unsafe or random.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SyncConflictManager.h`, `/Users/ryanrentfro/code/markamp/src/core/SyncScheduler.h`
- Prior Plan References: `v23` Phase 12 and 19
- Scope: Conflict detection, pending conflict surfacing, retries, offline queue, status UX
- Out of Scope: Collaborative multi-user merge UX
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/OfflineSyncQueue.h`
- Related Features / Systems / Components: Sync engine, conflict manager, scheduler
- Current Behavior: Recovery behavior remains weaker than a production sync system needs.
- Intended Completed Behavior: Sync conflicts and transient failures are surfaced, retried appropriately, and recoverable.
- Missing Pieces: Real conflict policy integration and user-facing state handling
- Technical Approach: Integrate conflict manager, retry strategy, and offline queue with provider results and UI feedback.
- Implementation Steps: Define conflict states; wire retries/backoff; attach notifications and resolution entry points.
- Validation Steps: Simulate conflicts, offline operation, transient failures, and repeated retries.
- Acceptance Criteria: Cloud sync does not silently lose or overwrite data in common failure modes.
- Dependencies: `P15-T01`, `P15-T02`
- Parallelization Notes: Can proceed with vault and OTLP work.
- Risks / Failure Modes: Incorrect conflict policy can cause destructive data loss.
- Cleanup / Migration Notes: Remove simplistic "completed/failed" assumptions in sync UI flows.
- Observability / Diagnostics Notes: Track conflict counts, retry history, and recovery actions.
- Rollback / Safety Notes: Prefer explicit user intervention for destructive conflict resolution.
- References / Context: Sync scheduler, conflict manager, offline queue
- Example scenarios where useful: A network failure during upload creates a visible pending-sync state and retries safely when connectivity returns.

### Task P15-T04

- Phase ID: `P15`
- Task ID: `P15-T04`
- Task Title: Complete vault/security-sensitive settings and transport observability
- Priority: `P1`
- Category: `Diagnostics / Recovery`
- Objective: Ensure sensitive networked features are diagnosable without leaking secrets.
- Why This Matters Now: Completion requires production-readiness hygiene, not only happy-path logic.
- Execution Gap Statement: Sensitive transport and vault-related flows need stronger observability and safer error surfacing.
- User / Product Impact: Security features can fail opaquely, or diagnostics can become unsafe.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/VaultService.h`, `/Users/ryanrentfro/code/markamp/src/core/SecurityAuditLog.h`
- Prior Plan References: `v23` Phase 12 and 16
- Scope: Safe diagnostics, redaction, audit events, failure surfacing
- Out of Scope: Full enterprise policy expansion
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/OtlpExporter.cpp`
- Related Features / Systems / Components: Security audit, OTLP, redaction, vault
- Current Behavior: Diagnostics pathways exist but are not yet fully aligned with production-grade safety.
- Intended Completed Behavior: Sensitive subsystems emit useful but safe operational diagnostics.
- Missing Pieces: Redaction policy integration, exporter completion, secure error reporting
- Technical Approach: Add structured diagnostics with redaction and complete OTLP or gate it.
- Implementation Steps: Implement or gate OTLP exporter; apply redaction; route vault/sync errors through safe reporting paths.
- Validation Steps: Trigger failures and inspect logs/telemetry for usefulness without secret leakage.
- Acceptance Criteria: Security-sensitive completion work includes safe diagnostics and no placeholder exporters in release paths.
- Dependencies: `P15-T01`, `P19-T01`
- Parallelization Notes: Can progress with release-readiness work.
- Risks / Failure Modes: Over-redaction can make debugging impossible; under-redaction leaks sensitive data.
- Cleanup / Migration Notes: Remove placeholder telemetry exporters once real behavior is present.
- Observability / Diagnostics Notes: Emit redaction counts and exporter health state.
- Rollback / Safety Notes: Disable external export if safety or correctness is uncertain.
- References / Context: Security audit and OTLP infrastructure
- Example scenarios where useful: A failed vault unlock produces a clear audit event without logging sensitive material.

### Task P15-T05

- Phase ID: `P15`
- Task ID: `P15-T05`
- Task Title: Add provider-level sync and security readiness gates
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Prevent placeholder transports or insecure regressions from re-entering production paths.
- Why This Matters Now: Networked features regress easily when coverage is weak.
- Execution Gap Statement: Existing tests and audits do not yet fully gate provider completeness and safe crypto behavior.
- User / Product Impact: Cloud features can regress silently and damage trust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/tests/unit/test_telemetry.cpp`, `/Users/ryanrentfro/code/markamp/tests/unit/test_sync_conflict_manager.cpp`
- Prior Plan References: `v23` Phase 19
- Scope: Transport fixtures, crypto validation, sync conflict tests, exporter readiness tests
- Out of Scope: External live-service integration testing in CI
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh`
- Related Features / Systems / Components: Cloud sync, security, telemetry, regression coverage
- Current Behavior: Readiness is under-protected relative to the sensitivity of these features.
- Intended Completed Behavior: Provider and security completion is defended by deterministic tests and chaos/retry scenarios.
- Missing Pieces: Provider mocks, transport fixtures, security assertions
- Technical Approach: Build deterministic transport fixtures and negative-path tests around crypto and sync.
- Implementation Steps: Add provider fixtures; add corruption/retry/conflict cases; wire readiness suite into CI.
- Validation Steps: Seed placeholder transport or insecure crypto behavior and verify gate failure.
- Acceptance Criteria: Cloud and security-sensitive workflows have explicit regression gates.
- Dependencies: `P15-T01` through `P15-T04`
- Parallelization Notes: Can scaffold early and harden as transports complete.
- Risks / Failure Modes: Poorly isolated tests can become flaky or leak credentials if not designed carefully.
- Cleanup / Migration Notes: Retire tests that only assert placeholder exporter or placeholder crypto defaults.
- Observability / Diagnostics Notes: Capture provider fixture traces and redacted failure output.
- Rollback / Safety Notes: Keep all transport tests sandboxed and credential-free.
- References / Context: Existing sync and telemetry test coverage
- Example scenarios where useful: A CI run verifies that encrypted sync payloads cannot be decrypted after intentional corruption and that conflicts are preserved.
