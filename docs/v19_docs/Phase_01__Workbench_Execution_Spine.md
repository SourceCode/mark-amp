# Phase 01: Workbench Execution Spine

## Phase Purpose
Replace fragmented shell orchestration with one durable workbench execution path that owns startup, workspace open, shell mode transitions, and runtime service wiring.

## Measurable Outcome
- `MainFrame` stops owning major workflow orchestration directly.
- `WorkspaceOpenOrchestrator`, `WorkspaceLoadStateModel`, and shell mode control are instantiated into the live app path.
- `MainFrame` and `LayoutManager` become UI surfaces instead of competing workflow controllers.

## Tasks

### P01-T01
- Phase ID: P01
- Task ID: P01-T01
- Task Class: Foundational
- Task Title: Introduce A Canonical Workbench Shell Controller
- Priority: Critical
- Objective: Create one controller that owns startup screen, editor shell, canvas shell, notebook shell, workspace open, and restore entry sequencing.
- Why This Matters Now: `MainFrame` and `LayoutManager` currently split shell control, which keeps every downstream workflow fragile.
- Problem Statement: `MainFrame` directly calls `showEditor()`, `showStartupScreen()`, `layout_->OpenFileInTab()`, and workspace scan logic while `LayoutManager` also controls workbench mode and canvas mode.
- Scope: Consolidate shell transitions, app-ready initialization hooks, and mode switching.
- Out of Scope: Full multi-window support.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.h`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasWorkbenchMode.h`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasWorkbenchMode.cpp`
- Related Systems / Components: `MainFrame`, `LayoutManager`, startup panel, workspace open, canvas mode, notebook mode, shell layout state.
- Current Behavior: Multiple code paths can switch shell state independently, and some new controller-like classes are not part of the real path.
- Intended Behavior: One controller decides which workbench surface is visible, when restore is allowed to run, and how shell state changes are sequenced.
- Technical Approach: Add a dedicated shell controller in `src/core/` and move high-level flow decisions out of `MainFrame` and `LayoutManager`.
- Implementation Steps:
  1. Add a controller that receives `EventBus`, `Config`, `WorkspaceOpenOrchestrator`, `WorkspaceLoadStateModel`, and references to the frame/layout shell.
  2. Move `showEditor()` and `showStartupScreen()` decision points behind the controller.
  3. Route command-line open, drag/drop open, recent workspace open, and explicit workspace open through the controller.
  4. Make `LayoutManager` expose shell capabilities without deciding shell policy.
- Validation Steps:
  1. Launch with no args, with file arg, and with folder arg.
  2. Switch between editor and canvas from the same controller path.
  3. Close the last tab and verify controller returns to the correct shell state.
- Acceptance Criteria: Shell-state transitions are not implemented in parallel across `MainFrame` and `LayoutManager`.
- Dependencies: None.
- Risks / Failure Modes: Hidden assumptions in existing event subscriptions may still mutate UI directly.
- UX Notes: Startup/editor/canvas/notebook transitions must feel deterministic and non-jumpy.
- Observability / Diagnostics Notes: Add structured logs for shell transition start, shell transition complete, and rejected transitions.
- Rollback / Safety Notes: Keep old entry points behind temporary adapter methods until the new controller is fully wired.
- References / Context:
  - `MainFrame` currently opens files and workspaces directly.
  - `LayoutManager` currently owns workbench and canvas switching.
- Example Scenarios Where Useful:
  - Opening a recent workspace from the startup screen and opening a file from the command line should converge through the same shell controller.

### P01-T02
- Phase ID: P01
- Task ID: P01-T02
- Task Class: Foundational
- Task Title: Wire Runtime Shell Services Into The Actual App Bootstrap
- Priority: Critical
- Objective: Instantiate and route the shell services that currently exist only as dormant or partial abstractions.
- Why This Matters Now: Several v18-era services exist but are not yet first-class runtime owners, which makes the architecture look more integrated than it is.
- Problem Statement: `WorkspaceLoadStateModel`, `WorkspaceOpenOrchestrator`, `StatusBarAdapter`, `SettingsDeepLinkRouter`, and related shell services are either absent from bootstrap or only partially connected.
- Scope: MarkAmpApp bootstrap wiring and shell-service ownership.
- Out of Scope: Service feature expansion beyond wiring and basic lifecycle correctness.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceLoadStateModel.h`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceLoadStateModel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/StatusBarAdapter.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ShellLayoutState.cpp`
- Related Systems / Components: app bootstrap, shell controller, status bar, settings routing, workspace load state.
- Current Behavior: `MarkAmpApp` initializes many services, but important shell integration services are still comments, TODO-level notes, or disconnected helpers.
- Intended Behavior: Bootstrap creates and owns all shell-critical services, then hands them to the shell controller and visible UI surfaces.
- Technical Approach: Convert shell services into real app members and inject them through composition rather than ad hoc lookups.
- Implementation Steps:
  1. Add app-owned members for shell-critical services.
  2. Instantiate them before `MainFrame` creation.
  3. Pass them into the shell controller and UI bridges.
  4. Remove comments that imply future wiring once the runtime path is real.
- Validation Steps:
  1. Verify each service is constructed and used during launch.
  2. Verify no `MainFrame` handler silently reimplements the same responsibility.
  3. Add a startup diagnostic summary listing enabled shell services.
- Acceptance Criteria: The app bootstrap owns the shell integration services and no longer leaves them effectively dead.
- Dependencies: P01-T01.
- Risks / Failure Modes: Lifetime issues if UI surfaces outlive service owners.
- UX Notes: No visible UX target here except increased reliability of downstream flows.
- Observability / Diagnostics Notes: Emit a startup report naming each shell service and whether it is actively bound.
- Rollback / Safety Notes: Maintain default-safe behavior if any optional service is absent during incremental rollout.
- References / Context:
  - `MarkAmpApp.cpp` currently notes that `WorkspaceOpenOrchestrator` is not yet fully wired.
  - `StatusBarAdapter` and `SettingsDeepLinkRouter` exist but are not obvious runtime owners.
- Example Scenarios Where Useful:
  - Debugging “settings opens but does not navigate” should have a real service chain to inspect instead of scattered subscriptions.

### P01-T03
- Phase ID: P01
- Task ID: P01-T03
- Task Class: Cleanup
- Task Title: Retire Duplicate Shell Entry Paths And Dead Controller Residue
- Priority: High
- Objective: Remove stale direct paths once the new shell controller is authoritative.
- Why This Matters Now: Leaving duplicate paths in place will cause drift immediately after the new shell wiring lands.
- Problem Statement: `MainFrame` still subscribes to workspace and settings events with direct UI actions, while other new abstractions exist for the same jobs.
- Scope: Shell-level duplicate handlers, transitional wrappers, obsolete comments, and dead orchestration code.
- Out of Scope: Domain-specific cleanup inside notebook or canvas modules.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
- Related Systems / Components: shell event routing, settings routing, workspace open, startup transitions.
- Current Behavior: Old direct handlers and newer abstractions coexist.
- Intended Behavior: One authoritative path remains for each shell responsibility.
- Technical Approach: After P01-T01 and P01-T02 land, delete or narrow duplicate handlers instead of preserving them indefinitely.
- Implementation Steps:
  1. Audit shell event subscriptions in `MainFrame` and `LayoutManager`.
  2. Remove subscriptions whose only purpose was temporary bridging.
  3. Leave thin UI delegate methods where needed, but not policy decisions.
  4. Update documentation and comments to reflect final ownership.
- Validation Steps:
  1. Re-run startup, open file, open workspace, switch mode, and settings-open smoke paths.
  2. Confirm no user action fires duplicate notifications or duplicate state mutations.
- Acceptance Criteria: There is one live owner per shell concern and duplicate handlers are removed.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Hidden UI behavior may have depended on duplicate publishes.
- UX Notes: This task should remove subtle double-renders and inconsistent restores.
- Observability / Diagnostics Notes: Temporarily log event-source ownership during this cleanup to catch missing consumers.
- Rollback / Safety Notes: Delete duplicated paths only after smoke validation passes for each flow.
- References / Context:
  - `MainFrame` still handles `WorkspaceOpenRequestEvent` and `SettingsOpenRequestEvent` directly.
- Example Scenarios Where Useful:
  - Opening settings from palette, menu, and button should no longer rely on parallel handlers with different behavior.
