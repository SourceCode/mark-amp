# Phase 03: Shell Layout Panels And Session Continuity

## Phase Goal
Stabilize shell composition, panel lifecycle, and restore behavior so the product feels dependable across launches and mode changes.

## Tasks
### Task 1
- Phase ID: P03
- Task ID: P03-T01
- Task Title: Re-enable And Correct Window Restore
- Objective: Make persisted window state actually restore and behave safely across displays.
- Problem Statement: `restoreWindowState()` exists but is commented out, and its file restore behavior is incomplete.
- Scope: Window position, size, maximize state, display validation, initial show timing.
- Out of Scope: Full workspace/session restore.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.h`
  - `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Related Systems / Components: Config, window manager/platform abstraction, shell startup.
- Current Behavior: State is saved on close but not restored on startup.
- Intended Behavior: Relaunch restores safe window bounds and maximize state.
- Technical Approach: Re-enable restore at the correct point in shell initialization and remove fake last-file restore behavior from this path.
- Implementation Steps:
  1. Restore window bounds before first user-visible layout.
  2. Keep off-screen validation.
  3. Separate window restore from file/session restore.
  4. Add tests or reproducible checks for multi-display cases.
- Edge Cases / Failure Modes: Previous display missing; invalid saved bounds; maximize toggling before shell is ready.
- UX Considerations: Avoid launch flicker and jumping windows.
- Dependencies: P01-T04.
- Validation Steps: Save state, relaunch, verify bounds across normal and maximized modes.
- Acceptance Criteria: Window restore works consistently and safely.
- Rollback / Safety Notes: Fall back to center if restore data is invalid.
- References / Context: `MainFrame.cpp` line area around `restoreWindowState()`.
- Example interactions or usage scenarios where helpful: Close app on secondary monitor, disconnect monitor, relaunch, app recenters instead of opening off-screen.

### Task 2
- Phase ID: P03
- Task ID: P03-T02
- Task Title: Implement Real Session Restore For Open Files And Active Surface
- Objective: Restore meaningful workbench state instead of only raw window geometry.
- Problem Statement: Current restore path only republishes an active-file event; it does not reopen tabs or reconstruct the active surface.
- Scope: Open tabs, active tab, active editor group, active sidebar mode, bottom panel state, workbench mode, last canvas board when applicable.
- Out of Scope: Notebook checkpoint/compute restore, handled later.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EditorStateSerializer.h`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Related Systems / Components: TabBar, EditorGroupManager, WorkbenchShell, Config, session restore service.
- Current Behavior: Layout state is partially saved; document/session state is not restored cohesively.
- Intended Behavior: Relaunch returns the user to the same working context.
- Technical Approach: Persist explicit shell/session snapshot data and restore after workspace load.
- Implementation Steps:
  1. Define snapshot schema.
  2. Persist open files, active file, and panel state.
  3. Restore after workspace scan completes.
  4. Guard against missing files and stale paths.
- Edge Cases / Failure Modes: Deleted files; restore to empty workspace; conflicts with command-line open path.
- UX Considerations: Fail soft with clear missing-file feedback.
- Dependencies: P03-T01.
- Validation Steps: Restore with multiple tabs, split groups, and active bottom/secondary panels.
- Acceptance Criteria: Session continuity feels intentional and complete.
- Rollback / Safety Notes: Prefer partial restore over blocking startup.
- References / Context: `SaveLayoutState()` and `RestoreLayoutState()` currently store only part of the needed state.
- Example interactions or usage scenarios where helpful: Relaunch after editing three files in split view restores those tabs and the active split.

### Task 3
- Phase ID: P03
- Task ID: P03-T03
- Task Title: Replace Placeholder Sidebar Panels With Typed Shell Hosts
- Objective: Stop presenting placeholder panels as if workflows are complete.
- Problem Statement: Several sidebar modes in `LayoutManager` are generic “feature panels” rather than real integrated workbench surfaces.
- Scope: Settings, themes, notebooks, graph, AI, canvas sidebar mode, presentation, database, tasks where placeholder hosts still exist.
- Out of Scope: Full domain feature depth for each panel.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Systems / Components: Sidebar panel registry, activity bar, secondary sidebar, typed panel implementations.
- Current Behavior: Some modes show generic buttons and static empty-state text unrelated to the real subsystem state.
- Intended Behavior: Each registered mode is backed by a typed host or is intentionally hidden until ready.
- Technical Approach: Replace generic factory use with typed adapters that can consume real data and commands.
- Implementation Steps:
  1. Identify placeholder-only registrations.
  2. Create typed shell hosts or hide unavailable modes.
  3. Ensure each host supports focus, empty state, and command routing.
  4. Connect activity bar and settings deep links to real hosts.
- Edge Cases / Failure Modes: Surfacing unfinished domains too early; mismatched sidebar and workbench mode.
- UX Considerations: Prefer honest empty state over fake functionality.
- Dependencies: P01-T04, P02-T02.
- Validation Steps: Click every activity bar item and verify each opens a real, intentional surface.
- Acceptance Criteria: No major sidebar item routes to a generic placeholder masquerading as product behavior.
- Rollback / Safety Notes: Temporary feature hiding is preferable to shipping dead controls.
- References / Context: `LayoutManager.cpp` panel registration block around notebooks/settings/themes/canvas/graph.
- Example interactions or usage scenarios where helpful: Clicking Notebooks opens an actual notebook entry surface, not a static placeholder panel.

### Task 4
- Phase ID: P03
- Task ID: P03-T04
- Task Title: Normalize Panel Visibility Docking And Restore Rules
- Objective: Make sidebars, secondary sidebars, and bottom panels obey shared visibility rules.
- Problem Statement: Multiple toggles and panel zones exist, but visibility and restore semantics are not clearly unified.
- Scope: Primary sidebar, secondary sidebar, bottom panel, status bar, zen mode interactions, mode switch interactions.
- Out of Scope: New docking paradigms.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: WorkbenchShell, panel area model, shell layout persistence, zen mode.
- Current Behavior: Different toggles directly show/hide widgets with local assumptions.
- Intended Behavior: Visibility changes update one shared shell state model and restore predictably.
- Technical Approach: Centralize panel visibility mutations and persistence through layout state services.
- Implementation Steps:
  1. Consolidate panel-toggle entry points.
  2. Normalize saved visibility state.
  3. Resolve conflicts between zen mode and user-restored panel state.
  4. Update shell refresh and focus behavior after visibility changes.
- Edge Cases / Failure Modes: Hidden panel receiving focus; secondary sidebar left open with unsupported mode.
- UX Considerations: Panels should not unexpectedly reappear or vanish after unrelated actions.
- Dependencies: P03-T02.
- Validation Steps: Toggle panels in different orders, relaunch, and verify restore matches expectation.
- Acceptance Criteria: Panel visibility is predictable and restorable.
- Rollback / Safety Notes: Keep state schema backward compatible where possible.
- References / Context: Sidebar and bottom panel controls are currently spread across `MainFrame`, `LayoutManager`, `Toolbar`, and status bar.
- Example interactions or usage scenarios where helpful: Entering zen mode hides shell chrome but exiting restores the exact prior panel layout.

### Task 5
- Phase ID: P03
- Task ID: P03-T05
- Task Title: Complete Startup To Workspace Open Flow
- Objective: Make the startup screen, recent workspaces, drag/drop, open folder, and argument-open flows resolve through one workspace-opening path.
- Problem Statement: Startup and workspace open behavior is partially duplicated and can diverge.
- Scope: Startup panel actions, command-line path open, drag/drop, open recent, menu open folder, workspace scan, recents update.
- Out of Scope: Multi-root workspace redesign.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/RecentWorkspaces.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.cpp`
- Related Systems / Components: StartupPanel, recent workspaces, workspace events, file tree population.
- Current Behavior: Several entry paths perform similar workspace scanning and view switching logic.
- Intended Behavior: Every workspace-open path uses one orchestrated flow with consistent feedback, recents behavior, and restore hooks.
- Technical Approach: Extract a shell-level workspace open orchestrator and route all entry points through it.
- Implementation Steps:
  1. Inventory all workspace-open entry points.
  2. Consolidate scanning, recents update, and view switching.
  3. Add loading/error feedback for slow or failed workspace opens.
  4. Integrate session restore after successful open.
- Edge Cases / Failure Modes: Nonexistent argument path; unreadable folder; repeated open requests while one is in progress.
- UX Considerations: Users should always know whether a workspace is loading, loaded, or failed.
- Dependencies: P02-T05, P03-T02.
- Validation Steps: Open workspace from every entry point and compare resulting state.
- Acceptance Criteria: One deterministic workspace-open pipeline exists.
- Rollback / Safety Notes: Preserve existing scanning logic until orchestrator is proven.
- References / Context: `MainFrame` currently duplicates workspace open handling in multiple subscriptions and handlers.
- Example interactions or usage scenarios where helpful: Double-clicking a recent workspace and using File > Open Folder should produce identical shell state.

