# Phase 04: Persistence Save Restore Recovery And Repository Completion

## Outcome

Complete the repository’s save, autosave, recovery, rollback, import/export, and snapshot subsystems so persistence no longer stops at event publication, in-memory models, or stubbed storage helpers.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P04-T01

- Phase ID: P04
- Task ID: P04-T01
- Task Title: Finish authoritative save and autosave execution across all primary artifact types
- Priority: P0
- Category: Persistence Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make save/autosave perform real durable work instead of optimistic state updates and split service behavior.
- Why This Matters Now: Lifecycle completion is meaningless if save is not authoritative.
- Completion Gap Statement: Save orchestration still contains optimistic and event-thin assumptions rather than one authoritative persistence contract.
- User / Product Impact: Dirty state, autosave, and shutdown trust remain weak.
- Repository Evidence: [DocumentPersistenceOrchestrator.cpp](/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp), [AtomicWriteService.h](/Users/ryanrentfro/code/markamp/src/core/AtomicWriteService.h), [AtomicWriter.cpp](/Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp)
- Scope: Save, save-all, autosave, save-as, dirty clearing, disk writes, file replacement, and persistence errors.
- Out of Scope: Transport-based cloud sync handled in Phase 12.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/AtomicWriteService.h`; `/Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: Save flows, autosave, shutdown, dirty state.
- Current Behavior: Persistence ownership is distributed and can be too optimistic.
- Intended Completed Behavior: Save and autosave are authoritative, transactional, and shared across files, notebooks, and canvases.
- Missing Pieces: One persistence contract, cross-artifact implementations, and consistent failure semantics.
- Technical Approach: Centralize save execution under an artifact-aware persistence service backed by atomic write and journal recovery.
- Implementation Steps: Audit save callers; centralize write execution; align dirty-state updates to write results; finish autosave scheduling and backoff behavior.
- Validation Steps: Save and autosave every artifact type under normal, slow, and failing write conditions.
- Acceptance Criteria: Dirty state clears only after durable successful writes.
- Dependencies: Phase 03.
- Risks / Failure Modes: Partial centralization can break one artifact type while fixing another.
- Cleanup / Migration Notes where relevant: Remove widget-local save fallbacks once the central path is authoritative.
- Observability / Diagnostics Notes where relevant: Emit save latency, failure, retry, and journal-use metrics.
- Rollback / Safety Notes: Keep old save paths only behind guarded fallbacks until parity is proven.
- References / Context: This phase closes the persistence gap behind the earlier lifecycle plans.
- Example scenarios where useful: Autosave succeeds for an unsaved notebook promoted to disk during a long editing session.

### P04-T02

- Phase ID: P04
- Task ID: P04-T02
- Task Title: Finish recovery journals, pending-recovery replay, and failed-write remediation
- Priority: P0
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Turn existing atomic-write and recovery-journal concepts into a real crash and write-failure safety net.
- Why This Matters Now: Atomic writing only matters if recovery is fully exercised and user-visible.
- Completion Gap Statement: Recovery primitives exist, but replay, conflict handling, and user-level remediation remain incomplete.
- User / Product Impact: Failed writes and crashes can still create confusing or lossy states.
- Repository Evidence: [AtomicWriteService.h](/Users/ryanrentfro/code/markamp/src/core/AtomicWriteService.h), [AtomicWriter.cpp](/Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp), [WorkspaceSessionRestore.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp)
- Scope: Recovery journals, replay policies, crash startup checks, user prompts, and conflicted recovery handling.
- Out of Scope: Network sync conflict resolution handled later.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/AtomicWriteService.h`; `/Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: Recovery, crash restart, startup, save safety.
- Current Behavior: Recovery support exists structurally but is not yet a finished user-facing resilience flow.
- Intended Completed Behavior: Pending recoveries are discovered, replayed, surfaced, and resolved deterministically.
- Missing Pieces: Startup integration, user workflow, conflict semantics, and test coverage.
- Technical Approach: Integrate recovery-journal scanning into startup and artifact restore flows with deterministic replay rules.
- Implementation Steps: Add startup scanning; map journals to artifacts; prompt or auto-recover based on policy; record outcomes; test crash recovery edges.
- Validation Steps: Simulate interrupted writes and confirm recovery behavior across artifact types.
- Acceptance Criteria: The product can explain and recover from interrupted writes instead of silently ignoring them.
- Dependencies: P04-T01.
- Risks / Failure Modes: Aggressive replay can overwrite user intent if journal ownership is ambiguous.
- Cleanup / Migration Notes where relevant: Retire any silent temporary-file cleanup paths that bypass journal logic.
- Observability / Diagnostics Notes where relevant: Log recovery replay decisions and unresolved entries.
- Rollback / Safety Notes: Allow manual discard or export of recovered content before applying it.
- References / Context: Recovery is one of the clearest distinctions between a broad app and a finished IDE.
- Example scenarios where useful: A crash during save triggers a clear recovery choice on next launch.

### P04-T03

- Phase ID: P04
- Task ID: P04-T03
- Task Title: Complete repository snapshot diffing, checkout safety, and structured workspace history
- Priority: P1
- Category: Persistence Completion
- Atomic Completion Tasks Covered: 60
- Objective: Replace repository placeholder behavior with real diff and rollback support.
- Why This Matters Now: Snapshot infrastructure exists but still stops at shallow compare behavior.
- Completion Gap Statement: Snapshot diffing and some repository flows are still stubbed even though the repository system is user-facing.
- User / Product Impact: History and rollback features can appear broader than they really are.
- Repository Evidence: [RepositoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp), [HistoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/HistoryService.cpp), [HistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp)
- Scope: Snapshot diff, restore safety snapshots, history browsing, compare views, rollback integrity.
- Out of Scope: Git-native history handled in Phase 09.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HistoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.h`
- Related Features / Systems / Components: Snapshot repository, history panel, rollback, compare views.
- Current Behavior: Snapshot diff is still stubbed and some history views remain thin shells.
- Intended Completed Behavior: Repository snapshots support real diffing, safe checkout, and browsable workspace history.
- Missing Pieces: Manifest compare logic, diff materialization, history UI hookup, rollback verification.
- Technical Approach: Materialize snapshot manifests and diff them structurally instead of returning shallow metadata.
- Implementation Steps: Build snapshot manifests; implement compare; feed history UI and diff UI; add rollback safety rules and tests.
- Validation Steps: Create sequential snapshots, compare them, and roll back with safety copies preserved.
- Acceptance Criteria: Repository history features no longer rely on stub compare behavior.
- Dependencies: P04-T01, P04-T02.
- Risks / Failure Modes: Snapshot extraction and compare can become expensive without manifest caching.
- Cleanup / Migration Notes where relevant: Remove placeholder compare summaries once real diffs exist.
- Observability / Diagnostics Notes where relevant: Track snapshot size, compare duration, and checkout failures.
- Rollback / Safety Notes: Always create and verify a safety snapshot before destructive checkout.
- References / Context: The product already has repository concepts; this task makes them honest.
- Example scenarios where useful: A user can diff two workspace snapshots and understand what changed before rolling back.

### P04-T04

- Phase ID: P04
- Task ID: P04-T04
- Task Title: Finish import export and build-log persistence paths that still stop at placeholder storage behavior
- Priority: P1
- Category: Persistence Completion
- Atomic Completion Tasks Covered: 60
- Objective: Replace incomplete file I/O and placeholder output generation in secondary persistence paths.
- Why This Matters Now: Completion debt is not limited to primary save flows.
- Completion Gap Statement: Export, import, and log-storage helpers still rely on visible placeholder or stub behavior in several areas.
- User / Product Impact: Secondary workflows feel unreliable and incomplete even when core editing works.
- Repository Evidence: [BuildLogManager.cpp](/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp), [DocumentImporter.cpp](/Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp), [ExportTemplateEngine.cpp](/Users/ryanrentfro/code/markamp/src/core/ExportTemplateEngine.cpp)
- Scope: Build log save/load, import queue persistence, export template fallback behavior, pending import/export status.
- Out of Scope: Cloud or marketplace transport behavior.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExportTemplateEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ImportDialog.cpp`
- Related Features / Systems / Components: Import/export, build output, persistence, diagnostics.
- Current Behavior: Some secondary persistence surfaces still use file-I/O stubs or placeholder substitutions.
- Intended Completed Behavior: Import/export and build-log persistence are durable, inspectable, and free of placeholder storage logic.
- Missing Pieces: Real file I/O, import queue persistence, fallback policy cleanup, and verification coverage.
- Technical Approach: Replace stub storage helpers with real implementations and explicit error handling.
- Implementation Steps: Implement build-log I/O; persist importer state where needed; tighten export fallback handling; add tests around file and template failures.
- Validation Steps: Save and reload build logs, import sessions, and export templates under success and failure cases.
- Acceptance Criteria: Secondary persistence paths no longer rely on placeholder file-I/O behavior.
- Dependencies: P04-T01.
- Risks / Failure Modes: Secondary paths can quietly diverge from primary persistence policies if not aligned.
- Cleanup / Migration Notes where relevant: Remove “visible commented placeholder” fallback content once proper validation exists.
- Observability / Diagnostics Notes where relevant: Add import/export/build-log persistence diagnostics.
- Rollback / Safety Notes: Prefer additive persistence alongside existing in-memory state during migration.
- References / Context: This closes completion debt outside the primary save button path.
- Example scenarios where useful: Build logs survive restart and can be compared later instead of disappearing with the session.

### P04-T05

- Phase ID: P04
- Task ID: P04-T05
- Task Title: Add persistence and recovery smoke paths that prove real durability rather than model correctness only
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Validate full durability chains end to end.
- Why This Matters Now: Model tests are not enough for persistence completion.
- Completion Gap Statement: Existing tests do not yet guarantee that finished save, restore, and recovery paths survive real restarts and failures.
- User / Product Impact: Users remain the first integration test for persistence bugs.
- Repository Evidence: [test_v20_session_restore.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_v20_session_restore.cpp), [scripts/smoke_test.sh](/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh), [tests/integration](/Users/ryanrentfro/code/markamp/tests/integration)
- Scope: Save/reopen, autosave, crash-restart recovery, snapshot rollback, import/export durability, build-log persistence.
- Out of Scope: UI-only visual checks.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_v20_session_restore.cpp`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp`
- Related Features / Systems / Components: Durability, restart safety, recovery, regression protection.
- Current Behavior: Durability coverage is still too fragmented and model-heavy.
- Intended Completed Behavior: Persistence changes are protected by restart-level and failure-level smoke coverage.
- Missing Pieces: Restart harnesses, fault injection around writes, and artifact-mixed integration coverage.
- Technical Approach: Add durable smoke scenarios that operate on real temp directories and simulated failure points.
- Implementation Steps: Define mixed-artifact scenarios; add interrupted-write tests; wire smoke scripts; publish pass/fail artifacts and logs.
- Validation Steps: Run the suite on clean temp workspaces and confirm recovered content and restored sessions match expectations.
- Acceptance Criteria: Persistence regressions trip automated coverage before release.
- Dependencies: P04-T01 through P04-T04.
- Risks / Failure Modes: Flaky temp-dir or crash-simulation tests can erode trust if not made deterministic.
- Cleanup / Migration Notes where relevant: Retire weaker placeholder persistence tests once stronger smoke coverage exists.
- Observability / Diagnostics Notes where relevant: Store recovery and restart traces with each smoke run.
- Rollback / Safety Notes: Keep tests hermetic and isolated from real user data.
- References / Context: Finished persistence is not credible without restart-aware validation.
- Example scenarios where useful: A simulated crash during autosave still restores the user’s notebook and unsaved file on relaunch.
