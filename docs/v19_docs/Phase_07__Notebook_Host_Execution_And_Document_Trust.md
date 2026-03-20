# Phase 07: Notebook Host Execution And Document Trust

## Phase Purpose
Turn notebook support into a real workbench workflow with proper host state, execution lifecycle, persistence, and trust semantics.

## Measurable Outcome
- Notebook open/create actions surface a real notebook host.
- Execution queue, output, checkpoint, dirty-state, and trust are visible and reliable.
- Notebook navigation and restore integrate with the broader shell.

## Tasks

### P07-T01
- Phase ID: P07
- Task ID: P07-T01
- Task Class: Foundational
- Task Title: Replace Notification-Only Notebook Host Behavior With A Real Shell Host
- Priority: Critical
- Objective: Make `NotebookShellHost` drive visible notebook host state instead of only storing entries and emitting notifications.
- Why This Matters Now: Notebook commands and host abstractions already exist, but user-facing notebook behavior is still too thin to trust.
- Problem Statement: `NotebookShellHost::open_notebook()` and `create_notebook()` currently stop at state mutation and `NotificationEvent`.
- Scope: notebook host creation, open notebook routing, active notebook state, and shell presentation.
- Out of Scope: Rich cell editing polish.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookCommandProvider.cpp`
- Related Systems / Components: notebook shell host, notebook open/create commands, shell controller, tabs, restore.
- Current Behavior: Notebook create/open is not yet promoted into a real workbench host lifecycle.
- Intended Behavior: Notebook commands open a real notebook surface, set active notebook state, and integrate with shell tabs and restore.
- Technical Approach: Treat notebook as a first-class workbench surface with controller-owned host state.
- Implementation Steps:
  1. Add notebook host creation and activation to the shell controller.
  2. Make `NotebookShellHost` own active notebook metadata and host state transitions.
  3. Route notebook commands into that host.
  4. Integrate notebook entries with tab or workbench navigation surfaces.
- Validation Steps:
  1. Create notebook, open notebook, switch away, and return.
  2. Relaunch with notebook active and verify restore.
- Acceptance Criteria: Notebook flows produce a real notebook shell, not only notifications.
- Dependencies: P01-T01, P03-T02.
- Risks / Failure Modes: Notebook host may initially compete with editor tab assumptions.
- UX Notes: Notebook must feel like part of the same application, not a side tool.
- Observability / Diagnostics Notes: Trace notebook host state changes and notebook open source.
- Rollback / Safety Notes: Keep notebook commands gated if host construction fails.
- References / Context:
  - `NotebookShellHost.cpp` currently logs and notifies more than it drives shell UI.
- Example Scenarios Where Useful:
  - `New Notebook` from palette should create a real notebook workbench surface that can be revisited and restored.

### P07-T02
- Phase ID: P07
- Task ID: P07-T02
- Task Class: Workflow
- Task Title: Complete Notebook Execution Queue Output And Interrupt Semantics
- Priority: High
- Objective: Expose notebook execution as a visible, cancellable lifecycle with clear queue and output state.
- Why This Matters Now: Execution core exists, but without real host integration it cannot deliver notebook trust.
- Problem Statement: `NotebookExecutionPipeline` manages queue state, but execution visibility, output surfacing, and kernel/session lifecycle are incomplete.
- Scope: execute cell, run all, interrupt, restart-and-run-all, output projection, queue display, and status feedback.
- Out of Scope: Full remote kernel management.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookOutputIntegration.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookCellController.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`
- Related Systems / Components: execution queue, output integration, toolbar state, notebook session, kernel manager.
- Current Behavior: Queue bookkeeping exists, but execution lifecycle is not fully surfaced to a notebook host UX.
- Intended Behavior: Users can see what is executing, what finished, what failed, and what can be interrupted or restarted.
- Technical Approach: Bind pipeline state to notebook host widgets and status surfaces.
- Implementation Steps:
  1. Add notebook execution state projections for host and toolbar surfaces.
  2. Route completion and failure into visible output cells and status.
  3. Implement interrupt and restart actions through the same pipeline owner.
  4. Ensure queue state survives view changes without being lost.
- Validation Steps:
  1. Execute single cell, above, below, and run-all scopes.
  2. Interrupt during execution.
  3. Verify output, status, and queue count remain correct.
- Acceptance Criteria: Notebook execution is visibly stateful and controllable.
- Dependencies: P07-T01.
- Risks / Failure Modes: Long-running execution may expose missing async ownership boundaries.
- UX Notes: Users need trustworthy visible status for notebook execution, not silent background changes.
- Observability / Diagnostics Notes: Add execution trace entries per cell with queue position, start, finish, and outcome.
- Rollback / Safety Notes: If host output projection fails, keep underlying execution engine intact and report degraded UI state.
- References / Context:
  - `NotebookExecutionPipeline.cpp` already tracks queue and current cell but does not complete the full UX loop.
- Example Scenarios Where Useful:
  - Run-all should clearly indicate progress, allow interruption, and preserve outputs after switching side panels.

### P07-T03
- Phase ID: P07
- Task ID: P07-T03
- Task Class: Workflow
- Task Title: Finish Notebook Dirty State Checkpoint And Trust Integration
- Priority: High
- Objective: Make notebook persistence, checkpoints, autosave, and trust visible and consistent with shell save/restore policy.
- Why This Matters Now: Without persistence and trust semantics, notebook UX feels premium only on the surface.
- Problem Statement: `NotebookSessionManager` and `NotebookDocumentLifecycle` track useful state in memory, but they are not shell-authoritative and lack robust persistence.
- Scope: notebook dirty indicators, checkpoints, autosave intervals, trust banners, reopen behavior, and save prompts.
- Out of Scope: Signed notebook interchange across cloud providers.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSerializer.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: dirty state coordinator, notebook serializer, trust management, restore, shell prompts.
- Current Behavior: Checkpoints and trust levels exist as bookkeeping but do not yet drive complete UX decisions.
- Intended Behavior: Notebook trust and persistence are explicit and integrate cleanly with close, reopen, and restore.
- Technical Approach: Promote notebook lifecycle state into the shared save/restore pipeline and visible notebook host UI.
- Implementation Steps:
  1. Persist notebook session and checkpoint metadata.
  2. Connect notebook dirty-state changes to shell save/close policy.
  3. Surface trust state and trust changes in the notebook host.
  4. Restore notebook sessions during shell restore when appropriate.
- Validation Steps:
  1. Modify notebook, checkpoint, close, relaunch, and restore.
  2. Toggle trust and verify status surfaces update.
  3. Verify autosave interval behavior.
- Acceptance Criteria: Notebook persistence and trust behavior are visible, durable, and shell-consistent.
- Dependencies: P03-T03, P07-T01.
- Risks / Failure Modes: Trust state may be lost if notebook identity is unstable across saves.
- UX Notes: Users must understand whether notebook content is trusted and whether work is safely persisted.
- Observability / Diagnostics Notes: Add notebook session logs for checkpoint creation, autosave tick, trust changes, and restore decisions.
- Rollback / Safety Notes: Default to untrusted and dirty-safe behavior when state recovery is ambiguous.
- References / Context:
  - `NotebookSessionManager.cpp` already provides the state vocabulary needed for this work.
- Example Scenarios Where Useful:
  - Relaunching after notebook edits should restore the notebook with correct dirty, checkpoint, and trust status.
