# Phase 03: Persistence, Autosave, Restore, Recovery, And Session Trust

## Phase Intent

Turn the artifact spine into dependable save, restart, crash-recovery, and reopen behavior.

## Release-Ready Exit Criteria

- save, save as, autosave, recovery, and reopen all obey one persistence contract,
- external change handling and recovery prompts are trustworthy,
- restart continuity works across text, notebook, and canvas artifacts.

## Task Count

3

## Task P03-T01

- Phase ID: P03
- Task ID: P03-T01
- Task Title: Move save and save-as authority out of `LayoutManager`-local file handling
- Priority: P0
- Category: Persistence / Restore
- Objective: reduce `LayoutManager` from persistence owner to shell coordinator and route save behavior through canonical persistence services.
- Why This Matters Now: current save logic is too centralized in UI-local code to be trustworthy at release scale.
- Release Gap Statement: save behavior remains coupled to surface-local implementation details rather than artifact-owned persistence.
- User / Product Impact: save prompts, save targets, close behavior, and non-text artifact persistence remain inconsistent.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`
- Prior Plan References: `v20 Phase 05`; `v23 Phase 04`; `v24 Phase 03`
- Scope: save; save as; close with dirty artifacts; active artifact persistence coordination.
- Out of Scope: non-release-path export formats.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Features / Systems / Components: save orchestration; close prompts; artifact state; persistence
- Current Behavior: much of save and save-as policy still lives in `LayoutManager`.
- Intended Release-Ready Behavior: shell requests saves, but artifact-aware persistence services execute and report them.
- Missing Pieces: canonical orchestration path; artifact-kind delegation; save result propagation.
- Technical Approach: introduce a shell-owned persistence coordinator that dispatches by artifact kind and updates lifecycle state centrally.
- Implementation Steps:
1. Extract save logic from `LayoutManager`.
2. Route text, notebook, and canvas save requests through a shared orchestrator.
3. Convert UI prompts and notifications into thin delegates over persistence results.
- Validation Steps:
1. Save and save-as from menu, tab context, close prompt, and autosave path.
2. Verify lifecycle state, paths, and notifications stay consistent.
- Acceptance Criteria: `LayoutManager` no longer owns artifact file I/O semantics beyond shell coordination.
- Dependencies: P02-T01; P02-T02
- Parallelization Notes: can proceed with recovery work after interfaces are defined.
- Risks / Failure Modes: regressions in close prompts; event duplication; path update bugs.
- Observability / Diagnostics Notes: log save requests, result status, artifact ID, and target path.
- Rollback / Safety Notes: keep existing UI prompts while backend ownership changes.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

## Task P03-T02

- Phase ID: P03
- Task ID: P03-T02
- Task Title: Harden autosave, recovery drafts, external-change detection, and restore prompts
- Priority: P0
- Category: Diagnostics / Recovery
- Objective: make autosave and recovery behavior dependable and user-visible across restart and invalid-state scenarios.
- Why This Matters Now: release-grade trust requires more than successful manual saves.
- Release Gap Statement: recovery behavior still depends too heavily on UI-local and draft-path assumptions.
- User / Product Impact: crash recovery and external change handling are major trust signals in an IDE.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/RecoveryUXService.h`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Prior Plan References: `v18 Phase 09`; `v20 Phase 05`; `v24 Phase 03`
- Scope: autosave timer; draft generation; draft cleanup; recovery prompts; external file change detection.
- Out of Scope: cloud-sync conflict resolution.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/RecoveryUXService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: autosave; recovery UX; session restore; file watching
- Current Behavior: autosave and recovery exist, but confidence depends on `LayoutManager` internals and partial shell ownership.
- Intended Release-Ready Behavior: autosave produces reliable recoverable state, restart presents correct recovery choices, and external-change behavior is explicit.
- Missing Pieces: artifact-aware draft ownership; recovery UX service adoption; invalid-state tests.
- Technical Approach: centralize draft metadata and restore decisions around artifact IDs and lifecycle state, not view-local assumptions.
- Implementation Steps:
1. Persist autosave and recovery metadata by artifact ID.
2. Integrate `RecoveryUXService` with restart and conflict flows.
3. Add explicit external-change resolution prompts.
- Validation Steps:
1. Simulate crash/restart, external edit, and failed save scenarios.
2. Verify recovered content, user messaging, and cleanup.
- Acceptance Criteria: restart and external-change scenarios behave consistently across artifact kinds and pass smoke coverage.
- Dependencies: P03-T01
- Parallelization Notes: can proceed in parallel with session continuity work.
- Risks / Failure Modes: orphaned drafts; stale recovery metadata; user-confusing prompts.
- Observability / Diagnostics Notes: emit recovery event logs and draft inventory summaries.
- Rollback / Safety Notes: never delete prior recovery data until successful adoption is confirmed.
- References / Context: `docs/v24_docs/Phase_03__Persistence_Autosave_Restore_Recovery_And_Recent_Items.md`

## Task P03-T03

- Phase ID: P03
- Task ID: P03-T03
- Task Title: Make session restore and recent-item reopen continuity artifact-aware and verifiable
- Priority: P0
- Category: Persistence / Restore
- Objective: ensure restart continuity restores the right artifacts, shell layout, and active context without reviving dead or duplicate paths.
- Why This Matters Now: release readiness requires restart confidence, not only save confidence.
- Release Gap Statement: session restore still appears partially applied and memory-first in design.
- User / Product Impact: incorrect reopen state undermines confidence in the entire workbench.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NavigationHistory.h`
- Prior Plan References: `v18 Phase 03`; `v19 Phase 03`; `v24 Phase 03`; `v24 Phase 10`
- Scope: recent files; recent workspaces; reopen active tabs/artifacts; panel restore; active artifact restore.
- Out of Scope: cloud and multi-device state restore.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NavigationHistory.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: session restore; navigation history; workspace continuity
- Current Behavior: session state exists, but artifact ownership and restore correctness remain too implicit.
- Intended Release-Ready Behavior: after restart, the workspace restores only valid artifacts and surfaces, preserving user continuity without invalid-state leakage.
- Missing Pieces: artifact-aware restore records; dead-path cleanup; restore smoke coverage.
- Technical Approach: persist restore records as canonical artifact/session descriptors and validate them on load before rehydrating the shell.
- Implementation Steps:
1. Define a canonical restore record for active artifacts and panels.
2. Validate restore entries against current workspace state.
3. Add recent-item and reopen smoke suites.
- Validation Steps:
1. Reopen mixed text/notebook/canvas sessions after clean exit and crash exit.
2. Verify active artifact, ordering, and panel continuity.
- Acceptance Criteria: restart restore is deterministic, artifact-aware, and free of duplicate or invalid reopen behavior.
- Dependencies: P03-T01; P03-T02
- Parallelization Notes: ties directly into workspace-navigation hardening later.
- Risks / Failure Modes: duplicate reopen; invalid missing-path restore; panel-order drift.
- Observability / Diagnostics Notes: emit session restore summaries with restored, skipped, and invalid entries.
- Rollback / Safety Notes: keep migration readers for older session snapshots.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
