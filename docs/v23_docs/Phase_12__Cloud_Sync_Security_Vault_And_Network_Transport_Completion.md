# Phase 12: Cloud Sync Security Vault And Network Transport Completion

## Outcome

Finish the networked and security-sensitive subsystems so sync, storage, encryption, telemetry export, and related transports stop relying on placeholder cryptography, curl skeletons, or memory-only stubs.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P12-T01

- Phase ID: P12
- Task ID: P12-T01
- Task Title: Replace placeholder cloud-sync encryption and transport behavior with real secure implementations
- Priority: P0
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make sync and remote storage secure and real instead of structurally present but placeholder-backed.
- Why This Matters Now: Placeholder cryptography and transport skeletons are release blockers in any finished product.
- Completion Gap Statement: Cloud sync still uses XOR placeholder encryption and incomplete transport clients.
- User / Product Impact: Sync features cannot be considered trustworthy or production-ready.
- Repository Evidence: [CloudSyncService.cpp](/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp), [S3Client.cpp](/Users/ryanrentfro/code/markamp/src/core/S3Client.cpp), [WebDavClient.cpp](/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp)
- Scope: Encryption, upload/download, list/head/delete, connection tests, object metadata, retries, and error handling.
- Out of Scope: Cloud-provider-specific premium features beyond the existing contract.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/S3Client.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyncEngine.cpp`
- Related Features / Systems / Components: Cloud sync, encryption, S3, WebDAV, remote storage.
- Current Behavior: Sync transport and encryption are structurally present but incomplete and insecure for production.
- Intended Completed Behavior: Sync uses real secure crypto and real transport backends with explicit failure semantics.
- Missing Pieces: AES-GCM integration, curl/HTTP wiring, response parsing, metadata handling, and retry policy.
- Technical Approach: Finish secure crypto and transport clients beneath the existing sync orchestration layer.
- Implementation Steps: Replace XOR with real crypto; implement GET/PUT/HEAD/list flows; parse responses; add retry/backoff and object integrity checks.
- Validation Steps: Sync real fixture data against S3/WebDAV doubles and verify encryption, retry, and metadata behavior.
- Acceptance Criteria: Cloud sync no longer depends on placeholder encryption or skeleton transport paths.
- Dependencies: Phase 04.
- Risks / Failure Modes: Security-sensitive code can introduce subtle correctness issues if not backed by strong tests and libraries.
- Cleanup / Migration Notes where relevant: Remove placeholder crypto comments and stub-return branches.
- Observability / Diagnostics Notes where relevant: Emit sync transfer, retry, crypto-failure, and integrity-check metrics.
- Rollback / Safety Notes: Keep sync disabled by default until secure transport paths are validated.
- References / Context: This is one of the most severe unfinished clusters in the repository.
- Example scenarios where useful: A file sync encrypts, uploads, lists, downloads, and verifies content through a real transport path.

### P12-T02

- Phase ID: P12
- Task ID: P12-T02
- Task Title: Finish vault, file-watch, and platform-security seams that still rely on stubs
- Priority: P1
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Complete the security-adjacent infrastructure that still contains stubbed OS or file-watch behavior.
- Why This Matters Now: Security and vault features are only as complete as their platform hooks.
- Completion Gap Statement: Vault watching and adjacent platform services still contain explicit stub behavior.
- User / Product Impact: Secure-content workflows remain less robust than they appear.
- Repository Evidence: [VaultService.cpp](/Users/ryanrentfro/code/markamp/src/core/VaultService.cpp), [PlatformServices.cpp](/Users/ryanrentfro/code/markamp/src/core/PlatformServices.cpp), [PlatformServices.h](/Users/ryanrentfro/code/markamp/src/core/PlatformServices.h)
- Scope: Vault watching, update checks, secure file events, platform service persistence, and related OS hooks.
- Out of Scope: UI-only accessibility polish.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/VaultService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PlatformServices.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PlatformServices.h`; `/Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp`
- Related Features / Systems / Components: Vault, platform services, secure file handling, update checks.
- Current Behavior: Some security-adjacent hooks are still platform-light or stubbed.
- Intended Completed Behavior: Vault and platform-security workflows have real file-watch and platform-service support where intended.
- Missing Pieces: OS-specific watchers, service persistence, and hardened error handling.
- Technical Approach: Complete platform hooks beneath existing security and vault services rather than layering more abstractions above them.
- Implementation Steps: Implement real watchers; replace stub platform results; harden secure-content event handling; add platform-specific coverage.
- Validation Steps: Modify watched secure content, restart services, and verify update and watch behavior.
- Acceptance Criteria: Security-sensitive services no longer depend on stub platform hooks in production-supported modes.
- Dependencies: Phase 16.
- Risks / Failure Modes: Platform differences can create uneven security behavior if not tested per OS.
- Cleanup / Migration Notes where relevant: Remove comments that identify file watching as a stub in production paths.
- Observability / Diagnostics Notes where relevant: Log vault watch registration, failures, and platform-service fallback usage.
- Rollback / Safety Notes: Prefer disabling an unsupported watch capability over simulating it weakly.
- References / Context: This phase closes security-related completion debt, not just sync debt.
- Example scenarios where useful: Changes inside a vault trigger real secure-file updates rather than silently relying on stubs.

### P12-T03

- Phase ID: P12
- Task ID: P12-T03
- Task Title: Finish telemetry export, observability shipping, and remote diagnostics paths
- Priority: P2
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Replace “count as exported” and in-memory-only observability behavior with real transport-backed delivery where intended.
- Why This Matters Now: Observability code already distinguishes local versus exported behavior; the export side is still incomplete.
- Completion Gap Statement: Telemetry export and log shipping still contain explicit stub paths.
- User / Product Impact: Operational insight and remote diagnostics remain weaker than the subsystem API suggests.
- Repository Evidence: [OtlpExporter.cpp](/Users/ryanrentfro/code/markamp/src/core/OtlpExporter.cpp), [ObservabilityCommands.h](/Users/ryanrentfro/code/markamp/src/core/ObservabilityCommands.h), [TelemetryAbstraction.cpp](/Users/ryanrentfro/code/markamp/src/core/TelemetryAbstraction.cpp)
- Scope: OTLP export, crash export, buffered log shipping, flush behavior, enablement, and config-driven routing.
- Out of Scope: Product analytics policy decisions.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/OtlpExporter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ObservabilityCommands.h`; `/Users/ryanrentfro/code/markamp/src/core/TelemetryAbstraction.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExternalLogShipper.cpp`
- Related Features / Systems / Components: Telemetry, observability, crash export, remote diagnostics.
- Current Behavior: Export APIs exist but still behave like transport stubs or no-op abstractions.
- Intended Completed Behavior: Enabled export paths actually serialize and ship data through supported protocols.
- Missing Pieces: Real exporter transport, batching, flush semantics, and failure accounting.
- Technical Approach: Finish the transport-backed branch while keeping no-op behavior limited to intentionally disabled configurations.
- Implementation Steps: Implement OTLP serialization/transport; finish buffered shipper; add retry/backoff; align config and enablement behavior.
- Validation Steps: Enable export in test fixtures and verify metrics, crash events, and logs are actually shipped to a receiver double.
- Acceptance Criteria: Telemetry export no longer counts records as exported without transport.
- Dependencies: Phase 17.
- Risks / Failure Modes: Export paths can create startup or shutdown hangs if batching and flush are incomplete.
- Cleanup / Migration Notes where relevant: Remove tests that treat empty or count-only export behavior as final.
- Observability / Diagnostics Notes where relevant: Track dropped events, export queue depth, and transport failures.
- Rollback / Safety Notes: Keep no-op providers only for disabled configurations, not as silent fallback in enabled mode.
- References / Context: This phase turns observability infrastructure into a real runtime subsystem.
- Example scenarios where useful: Enabling OTLP actually sends metrics to a receiver instead of only incrementing a local counter.

### P12-T04

- Phase ID: P12
- Task ID: P12-T04
- Task Title: Complete remote-service error handling, retries, and offline-state recovery across sync and export systems
- Priority: P1
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Ensure networked systems fail and recover coherently once real transport exists.
- Why This Matters Now: Finishing transport without finishing recovery leaves product trust low.
- Completion Gap Statement: Existing network-related code is not yet uniformly explicit about retry, offline, and partial-failure behavior.
- User / Product Impact: Remote workflows can become brittle, confusing, or dangerous under transient failures.
- Repository Evidence: [CloudSyncService.cpp](/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp), [SyncScheduler.cpp](/Users/ryanrentfro/code/markamp/src/core/SyncScheduler.cpp), [OtlpExporter.cpp](/Users/ryanrentfro/code/markamp/src/core/OtlpExporter.cpp)
- Scope: Retry policy, backoff, offline queueing, partial success, sync conflict surfacing, exporter flush failure behavior.
- Out of Scope: Provider-specific SLA logic beyond current product contracts.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyncScheduler.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyncConflictManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/OtlpExporter.cpp`
- Related Features / Systems / Components: Sync, telemetry export, retries, offline handling, diagnostics.
- Current Behavior: Failure and retry semantics are not yet fully standardized because core transport is still incomplete.
- Intended Completed Behavior: Remote subsystems expose consistent retry, offline, and partial-failure behavior.
- Missing Pieces: Shared retry policies, offline queue handling, user feedback, and conflict integration.
- Technical Approach: Define a common remote-operation failure model and implement it across sync and export paths.
- Implementation Steps: Standardize retry and timeout rules; add offline state reporting; persist queued work where needed; connect sync conflicts and export drops to diagnostics.
- Validation Steps: Simulate network failures, credential failures, and flaky responses across sync and export flows.
- Acceptance Criteria: Remote operations no longer fail silently or inconsistently across subsystems.
- Dependencies: P12-T01, P12-T03.
- Risks / Failure Modes: Unbounded retries can hide failures and degrade performance.
- Cleanup / Migration Notes where relevant: Remove one-off error strings and ad hoc warning-only behavior once the shared model lands.
- Observability / Diagnostics Notes where relevant: Record retry counts, offline duration, queue depth, and final resolution state.
- Rollback / Safety Notes: Fail closed on destructive remote operations when conflict state is uncertain.
- References / Context: Completion includes failure and recovery, not only happy-path transport.
- Example scenarios where useful: A temporary WebDAV outage surfaces a retrying/offline state and recovers without data loss.

### P12-T05

- Phase ID: P12
- Task ID: P12-T05
- Task Title: Add security- and transport-focused completion harnesses
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Back secure and networked completion work with deterministic transport and crypto coverage.
- Why This Matters Now: Placeholder crypto and network stubs are too risky to replace without strong automated proof.
- Completion Gap Statement: Existing coverage does not yet fully prove secure transport behavior, offline recovery, or exporter correctness.
- User / Product Impact: Security regressions are high-severity and hard to detect manually.
- Repository Evidence: [test_phase27_cloud_sync.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase27_cloud_sync.cpp), [test_telemetry.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_telemetry.cpp), [test_safe_mode.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_safe_mode.cpp)
- Scope: Crypto round trips, S3/WebDAV doubles, retry behavior, conflict handling, exporter delivery, disabled/enabled paths.
- Out of Scope: External live-service acceptance tests.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_phase27_cloud_sync.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_telemetry.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_safe_mode.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`
- Related Features / Systems / Components: Sync, encryption, OTLP export, recovery, security.
- Current Behavior: Tests cover some model behavior but still reflect disabled or stub-heavy implementations.
- Intended Completed Behavior: Transport and security completion is protected by deterministic integration and failure-mode coverage.
- Missing Pieces: Transport doubles, crypto fixtures, remote failure simulations, and exporter verification endpoints.
- Technical Approach: Use hermetic doubles and fixture services to test secure/networked behavior without public network dependence.
- Implementation Steps: Create S3/WebDAV doubles; add crypto round-trip tests; verify exporter serialization and backoff; add offline/retry scenarios.
- Validation Steps: Reintroduce XOR encryption or count-only export behavior and verify suites fail.
- Acceptance Criteria: Security and transport regressions are blocking and reproducible.
- Dependencies: P12-T01 through P12-T04.
- Risks / Failure Modes: If doubles do not match real protocol semantics closely enough, false confidence can result.
- Cleanup / Migration Notes where relevant: Remove tests that only assert disabled/no-op branches for features intended to ship enabled.
- Observability / Diagnostics Notes where relevant: Store fixture request traces and crypto metadata for failed runs.
- Rollback / Safety Notes: Keep all security tests isolated from real secrets and external endpoints.
- References / Context: This phase is necessary to convert risky stub replacement into release-safe completion work.
- Example scenarios where useful: A sync test verifies encrypted upload/download integrity and conflict surfacing against a deterministic WebDAV fixture.
