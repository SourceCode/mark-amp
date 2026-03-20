# Phase 03: Workspace Session Dirty And Save Flows

## Phase Purpose
Finish workspace continuity so relaunch, restore, autosave, reopen, and dirty-state behavior are trustworthy.

## Measurable Outcome
- Workspace open and restore use one deterministic pipeline.
- Session snapshots are real persisted artifacts, not memory-only helpers.
- Save and close behavior is consistent across editor, notebook, and canvas surfaces.

## Tasks

### P03-T01
- Phase ID: P03
- Task ID: P03-T01
- Task Class: Foundational
- Task Title: Make Workspace Open And Workspace Load State Real Runtime Contracts
- Priority: Critical
- Objective: Replace direct workspace scanning paths with a single orchestrated open pipeline that updates load state and shell visibility.
- Why This Matters Now: Workspace entry flows still diverge and some new services are not in the real path.
- Problem Statement: `MainFrame` handles `WorkspaceOpenRequestEvent` directly, while `WorkspaceOpenOrchestrator` exists separately and currently publishes only a refresh request.
- Scope: startup panel open, recent workspace open, drag/drop folder, menu open folder, command-line workspace open, load-state transitions.
- Out of Scope: Multi-root redesign.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceLoadStateModel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/RecentWorkspaces.cpp`
- Related Systems / Components: startup panel, recent workspaces, workspace load state, file tree refresh.
- Current Behavior: Different open paths scan directories and mutate UI directly.
- Intended Behavior: Every workspace-open entry path uses one orchestrator that owns validation, loading feedback, recents updates, and restore handoff.
- Technical Approach: Promote `WorkspaceOpenOrchestrator` to the live path and give it explicit hooks into shell controller and load-state model.
- Implementation Steps:
  1. Route all workspace-open requests into the orchestrator.
  2. Transition load state through `idle -> loading -> ready/failed/empty`.
  3. Remove direct directory scan handling from `MainFrame`.
  4. Ensure load-state changes are visible in explorer/startup/search surfaces.
- Validation Steps:
  1. Open workspace from every supported entry path.
  2. Simulate invalid path and empty workspace.
  3. Verify recents, notifications, and shell state are identical across entry points.
- Acceptance Criteria: One workspace-open pipeline exists and it drives visible load state.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Hidden assumptions in explorer population or file tree refresh may break.
- UX Notes: Users should always know whether the workspace is loading, empty, ready, or failed.
- Observability / Diagnostics Notes: Add an open-workspace trace containing source, target path, duration, result, and restore outcome.
- Rollback / Safety Notes: Keep old direct scan logic behind a temporary fallback path only during rollout.
- References / Context:
  - `WorkspaceLoadStateModel` exists but is not yet clearly driving the visible shell.
- Example Scenarios Where Useful:
  - Opening a folder from the welcome screen and opening it from File > Open Folder should produce the same progress and ready state.

### P03-T02
- Phase ID: P03
- Task ID: P03-T02
- Task Class: Foundational
- Task Title: Replace Memory-Only Workspace Session Restore With Persisted Snapshots
- Priority: Critical
- Objective: Turn `WorkspaceSessionRestore` into a durable persisted restore system that actually reopens the shell state it records.
- Why This Matters Now: The current session restore object gives a false sense of completion and blocks real continuity.
- Problem Statement: `WorkspaceSessionRestore::restore_snapshot()` returns true without applying state, and snapshot data is not managed as a real persisted runtime artifact.
- Scope: open files, active file, shell visibility, panel state, workbench mode, window state handoff, and restore policy.
- Out of Scope: Cross-machine sync of restore state.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ShellLayoutState.cpp`
- Related Systems / Components: restore policy, file tabs, shell layout, active file, last workspace, window restore.
- Current Behavior: Snapshot structures exist but restore is effectively a stub.
- Intended Behavior: Session snapshots persist to disk and restore actual shell state after workspace readiness.
- Technical Approach: Persist snapshots per workspace and restore only after workspace open completes.
- Implementation Steps:
  1. Choose and implement on-disk snapshot storage under config/workspace state.
  2. Capture shell snapshot on safe checkpoints and shutdown.
  3. Restore open files and active surface only after workspace open completes.
  4. Handle missing files, missing panels, and outdated surfaces gracefully.
- Validation Steps:
  1. Open several tabs, switch panels, relaunch, and restore.
  2. Delete one restored file and verify graceful degradation.
  3. Verify restore policy toggle behavior.
- Acceptance Criteria: Session restore reopens the actual previous shell state instead of just replaying placeholder events.
- Dependencies: P03-T01.
- Risks / Failure Modes: Restoring too early can race explorer population or workbench surface creation.
- UX Notes: Restore should feel reliable but never trap the user in a broken state.
- Observability / Diagnostics Notes: Record snapshot save, snapshot load, restore duration, skipped files, and degraded restore reasons.
- Rollback / Safety Notes: Keep restore opt-out and snapshot reset commands available during rollout.
- References / Context:
  - `restoreWindowState()` in `MainFrame` explicitly notes that session/file restore is not handled there.
- Example Scenarios Where Useful:
  - A user relaunches after working in both editor and canvas and gets the expected surface and open documents back.

### P03-T03
- Phase ID: P03
- Task ID: P03-T03
- Task Class: Workflow
- Task Title: Normalize Dirty State Autosave And Close Semantics Across Surfaces
- Priority: High
- Objective: Use shared dirty-state and save policy rules for editor, notebook, and canvas workflows.
- Why This Matters Now: The shell currently presents save prompts and modified indicators inconsistently and mostly editor-first.
- Problem Statement: `MainFrame::onClose()` prompts only around unsaved files in `LayoutManager`, while notebook and canvas continuity services remain separate and incomplete.
- Scope: dirty tracking, autosave, close prompts, external reload handling, crash-safe checkpoints.
- Out of Scope: Collaborative merge conflict UX.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasWorkbenchMode.cpp`
- Related Systems / Components: save prompts, autosave timer, dirty indicators, notebook checkpoints, canvas board persistence.
- Current Behavior: Dirty-state logic is surface-specific and incomplete outside the text editor path.
- Intended Behavior: The shell knows whether the current session is safely persisted regardless of surface type.
- Technical Approach: Add a shared save/dirty coordinator that aggregates surface state and close behavior.
- Implementation Steps:
  1. Define a surface-agnostic dirty-state contract.
  2. Register editor, notebook, and canvas contributors.
  3. Replace close-time editor-only logic with aggregated session checks.
  4. Align autosave and checkpoint timing with the same policy source.
- Validation Steps:
  1. Modify editor text, notebook cells, and canvas board state separately.
  2. Close the app and verify prompt correctness.
  3. Verify autosave/checkpoint clears dirty indicators when successful.
- Acceptance Criteria: Close, autosave, and restore semantics are coherent across major workbench surfaces.
- Dependencies: P03-T01, P03-T02.
- Risks / Failure Modes: Dirty-state over-reporting can make the app feel noisy; under-reporting can cause data loss.
- UX Notes: Users should not need to understand internal surface differences to trust save behavior.
- Observability / Diagnostics Notes: Add per-surface dirty-state diagnostics plus an aggregated close-decision trace.
- Rollback / Safety Notes: Default to conservative prompting if any surface reports an uncertain dirty state.
- References / Context:
  - `NotebookSessionManager` and `NotebookDocumentLifecycle` already track pieces of this state but are not shell-authoritative.
- Example Scenarios Where Useful:
  - Closing the window after editing a notebook but not a text file should still produce the right save checkpoint behavior.
