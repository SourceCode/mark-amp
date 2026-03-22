# Phase 10 - Explorer, Project Navigation, And Workspace Flows

## Phase Goal

Finish the workspace and project shell so explorer, file operations, startup flows, navigation, and workspace-state continuity feel like one intentional IDE.

## Measurable Outcome

- Workspace open/refresh/registration flows are deterministic.
- Explorer and navigation flows are artifact-aware and restore-safe.
- Startup and workspace continuity stop masking incomplete product behavior.

### Task P10-T01

- Phase ID: `P10`
- Task ID: `P10-T01`
- Task Title: Complete workspace open and registration orchestration end to end
- Priority: `P0`
- Category: `Core Workflow Completion`
- Objective: Ensure opening a workspace fully initializes tree, artifacts, panels, and shell context.
- Why This Matters Now: Workspace context underpins explorer, search, MRU, restore, and project actions.
- Execution Gap Statement: Workspace orchestration still relies on partial shell and restore assumptions.
- User / Product Impact: Project entry can feel fragile or incomplete.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v19` Phase 03 and 05, `v20` Phase 05
- Scope: Open workspace, refresh, root registration, sample/default fallback handling
- Out of Scope: Multi-root workspace feature expansion
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.h`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Related Features / Systems / Components: Workspace root, shell initialization, explorer
- Current Behavior: Workspace initialization is still partly shell-local and partly service-driven.
- Intended Completed Behavior: Workspace open produces one deterministic shell initialization chain.
- Missing Pieces: Full orchestration boundaries, root propagation, restore coordination
- Technical Approach: Centralize workspace-open sequencing and make UI hosts consume its results.
- Implementation Steps: Normalize open event flow; propagate root to explorer/search/settings; align restore ordering.
- Validation Steps: Open empty, normal, and sample workspaces and verify consistent shell initialization.
- Acceptance Criteria: Opening a workspace deterministically initializes the shell without placeholder fallbacks.
- Dependencies: `P03-T03`, `P05-T02`
- Parallelization Notes: Can proceed with explorer and startup work.
- Risks / Failure Modes: Mixed workspace-root assumptions can break downstream services like search.
- Cleanup / Migration Notes: Remove shell-local workspace initialization shortcuts.
- Observability / Diagnostics Notes: Emit workspace-open stages and root propagation diagnostics.
- Rollback / Safety Notes: Fallback to safe sample workspace only when explicitly configured.
- References / Context: Workspace open orchestrator and shell host files
- Example scenarios where useful: Opening a workspace rehydrates explorer, search panel, active artifacts, and recent history coherently.

### Task P10-T02

- Phase ID: `P10`
- Task ID: `P10-T02`
- Task Title: Finish file operations through canonical services and navigation updates
- Priority: `P0`
- Category: `File / Workspace Completion`
- Objective: Ensure create, rename, move, duplicate, delete, and reveal update the full shell correctly.
- Why This Matters Now: File operations are still partially direct and not fully reflected across navigation surfaces.
- Execution Gap Statement: File tree and related flows can still perform local file-system behavior without full artifact/nav updates.
- User / Product Impact: Workspace state and open artifacts can go stale after file operations.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/core/FileOperationService.h`
- Prior Plan References: `v20` Phase 02, `v23` Phase 03
- Scope: Rename/move/delete/duplicate/reveal/new folder/new file propagation
- Out of Scope: External VCS-specific file operation semantics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ActiveDocumentTracker.h`, `/Users/ryanrentfro/code/markamp/src/core/NavigationService.h`
- Related Features / Systems / Components: File operations, explorer, open artifacts, navigation
- Current Behavior: File operations may not fully update tabs, recents, breadcrumbs, and navigation history.
- Intended Completed Behavior: File operations atomically update file system, artifact model, explorer, and navigation surfaces.
- Missing Pieces: Operation result propagation and shell-wide reconciliation
- Technical Approach: Route file operations through canonical services that publish structured update events to all shell models.
- Implementation Steps: Implement service-backed operations; reconcile open artifacts; update explorer/history/breadcrumbs.
- Validation Steps: Rename/move/delete open and closed files and verify shell coherence.
- Acceptance Criteria: File operations leave no stale tabs, broken breadcrumbs, or orphaned navigation entries.
- Dependencies: `P05-T02`, `P02-T04`
- Parallelization Notes: Can proceed with source-control integration later.
- Risks / Failure Modes: Cross-surface reconciliation bugs can create broken references or dangling history entries.
- Cleanup / Migration Notes: Remove local file-op code from explorer and other widgets.
- Observability / Diagnostics Notes: Track operation intent, affected artifacts, and reconciliation outcomes.
- Rollback / Safety Notes: Keep confirmation and undo-friendly behavior for destructive actions.
- References / Context: File operation service and navigation infrastructure
- Example scenarios where useful: Renaming an open file updates the tab, breadcrumbs, explorer entry, and MRU item without closing the document.

### Task P10-T03

- Phase ID: `P10`
- Task ID: `P10-T03`
- Task Title: Complete startup, welcome, and recent-workspace continuity on top of real state
- Priority: `P1`
- Category: `Core Workflow Completion`
- Objective: Make startup and welcome surfaces reflect actual workspace and artifact continuity instead of thin shell assumptions.
- Why This Matters Now: Startup is still one of the first places users see product incompleteness.
- Execution Gap Statement: Startup and recent surfaces still sit on top of partial MRU and workspace state.
- User / Product Impact: First-run and reopen experience feels utility-grade rather than product-grade.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Prior Plan References: `v22` Phase 16, `v24` Phase 03
- Scope: Startup panel, recent workspaces, create-new affordances, continuity messaging
- Out of Scope: Tutorial-content authoring
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/OnboardingService.h`
- Related Features / Systems / Components: Startup panel, recent items, onboarding
- Current Behavior: Startup surface still relies on partial state and pending styling/behavior assumptions.
- Intended Completed Behavior: Startup reflects real recents, restore options, and create/open entry points accurately.
- Missing Pieces: MRU integration, workspace restore summaries, action routing
- Technical Approach: Bind startup UI to the canonical recent/workspace/artifact services.
- Implementation Steps: Replace local assumptions; wire action entry points; surface restore state accurately.
- Validation Steps: Launch into first-run, recent-workspace, and crash-recovery scenarios.
- Acceptance Criteria: Startup options accurately reflect what can really be restored or created.
- Dependencies: `P03-T04`, `P10-T01`
- Parallelization Notes: Can progress with UI-system polish after state is real.
- Risks / Failure Modes: Startup can still mislead if restore metadata is stale.
- Cleanup / Migration Notes: Remove temporary styling and placeholder messaging once real state is wired.
- Observability / Diagnostics Notes: Log startup mode, recent-item load, and restore availability.
- Rollback / Safety Notes: Fall back to minimal startup safely when state is unavailable.
- References / Context: Startup panel and onboarding service
- Example scenarios where useful: Startup shows the last workspace, last active artifacts, and actionable recovery choices after a crash.

### Task P10-T04

- Phase ID: `P10`
- Task ID: `P10-T04`
- Task Title: Normalize navigation history, breadcrumbs, and back/forward semantics
- Priority: `P1`
- Category: `Search / Navigation Completion`
- Objective: Make workspace-scale navigation feel coherent across explorer, editor, notebook, and canvas-linked flows.
- Why This Matters Now: Navigation is one of the strongest signals of whether the product feels like one system.
- Execution Gap Statement: Navigation surfaces still have artifact and shell-context gaps.
- User / Product Impact: Back/forward and breadcrumb behavior feel inconsistent or shallow.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp`
- Prior Plan References: `v19` Phase 05, `v22` Phase 05
- Scope: History entries, breadcrumb models, cross-surface navigation semantics
- Out of Scope: Advanced workspace graph navigation beyond current scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.h`
- Related Features / Systems / Components: Navigation history, breadcrumbs, shell context
- Current Behavior: Navigation support exists but is not uniformly robust across surfaces.
- Intended Completed Behavior: Navigation semantics are predictable across project, editor, notebook, and canvas-linked paths.
- Missing Pieces: Shared artifact-aware location model and restore-safe history policy
- Technical Approach: Normalize location records to artifact plus surface context and use them everywhere.
- Implementation Steps: Update history schema; update breadcrumb generation; verify back/forward rules across surface types.
- Validation Steps: Move through mixed surface workflows and verify history accuracy.
- Acceptance Criteria: Navigation history and breadcrumbs reflect real shell context, not only file-path assumptions.
- Dependencies: `P06-T05`, `P09-T01`
- Parallelization Notes: Can progress with project and source-control work.
- Risks / Failure Modes: Mixed old/new history records can break traversal.
- Cleanup / Migration Notes: Migrate existing history serialization safely.
- Observability / Diagnostics Notes: Emit navigation-entry type and resolution success/failure.
- Rollback / Safety Notes: Provide safe fallback for unknown legacy history records.
- References / Context: Navigation service and breadcrumb surfaces
- Example scenarios where useful: Navigating from a search hit to a notebook cell and then back returns to the exact prior context.

### Task P10-T05

- Phase ID: `P10`
- Task ID: `P10-T05`
- Task Title: Add workspace-scale smoke paths for project entry, tree ops, and navigation
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Protect the project-shell workflows most likely to regress during ongoing migration work.
- Why This Matters Now: Workspace and explorer behavior touch many subsystems at once.
- Execution Gap Statement: Core project-shell flows lack enough explicit end-to-end regression protection.
- User / Product Impact: Regressions in workspace entry or navigation can make the app feel broken immediately.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.h`
- Prior Plan References: `v19` Phase 09, `v20` Phase 10, `v23` Phase 19
- Scope: Workspace open, explorer operations, recent-workspace reopen, back/forward, startup entry
- Out of Scope: Pixel-perfect UI checks
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Features / Systems / Components: Smoke tests, workflow runner, workspace shell
- Current Behavior: Regression coverage exists but does not yet fully gate these cross-surface paths.
- Intended Completed Behavior: Workspace-scale smoke tests fail quickly on broken project-shell flows.
- Missing Pieces: Scenario definitions, fixtures, gate integration
- Technical Approach: Add deterministic workspace fixtures and scripted smoke pathways.
- Implementation Steps: Define workspace fixtures; add smoke scenarios; wire into CI and pre-release runs.
- Validation Steps: Intentionally break explorer/nav behavior and confirm failures are caught.
- Acceptance Criteria: Core project-shell flows are covered by repeatable smoke tests.
- Dependencies: `P10-T01` through `P10-T04`
- Parallelization Notes: Can be developed incrementally while the phase lands.
- Risks / Failure Modes: Non-deterministic fixtures can reduce trust in the smoke suite.
- Cleanup / Migration Notes: Retire manual-only verification checklists where smoke coverage becomes reliable.
- Observability / Diagnostics Notes: Publish workflow IDs, failure stages, and shell snapshots on smoke failures.
- Rollback / Safety Notes: Keep smoke fixtures isolated from user data.
- References / Context: Workflow smoke infrastructure
- Example scenarios where useful: Opening a workspace, creating a file, renaming it, reopening after restart, and navigating back/forward is one smoke pathway.
