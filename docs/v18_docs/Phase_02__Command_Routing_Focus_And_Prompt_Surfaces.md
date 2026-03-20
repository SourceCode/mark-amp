# Phase 02: Command Routing Focus And Prompt Surfaces

## Phase Goal
Make every action entry point behave consistently and complete the missing prompt/UI service chains.

## Tasks
### Task 1
- Phase ID: P02
- Task ID: P02-T01
- Task Title: Wire Quick Pick And Input Box Services End To End
- Objective: Connect bootstrap, event bus, UI host, and callback completion for extension-facing prompts.
- Problem Statement: Prompt services currently stop at published request events and even that path is not bootstrapped.
- Scope: `MarkAmpApp` bootstrap, prompt service event bus assignment, shell-level consumer, result/cancel handling.
- Out of Scope: Advanced multi-step wizard flows.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/InputBoxService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: EventBus, overlay/dialog host, focus manager, accessibility.
- Current Behavior: `set_event_bus()` is defined but unused; no visible consumer handles the request events.
- Intended Behavior: Prompts are visible, keyboard-driven, cancellable, and resolve callbacks safely.
- Technical Approach: Add a shell-owned prompt host and explicit service-to-host result plumbing.
- Implementation Steps:
  1. Set both services’ event bus pointers during app bootstrap.
  2. Add shell subscription/host component for request events.
  3. Route selection/submit/cancel results back into service callbacks.
  4. Add shutdown-safe cancellation.
- Edge Cases / Failure Modes: Prompt reopened while one is active; host destroyed before callback completion.
- UX Considerations: Prompt must restore focus to invoking control.
- Dependencies: Phase 01 prompt contract.
- Validation Steps: Exercise single-select, multi-select, cancel, input accept, and shutdown cases.
- Acceptance Criteria: Prompt services are usable by first-party and plugin code.
- Rollback / Safety Notes: Guard against dangling callbacks during teardown.
- References / Context: Current gap evidenced in `MarkAmpApp.cpp`, `QuickPickService.cpp`, and `InputBoxService.cpp`.
- Example interactions or usage scenarios where helpful: `Open in Secondary Sidebar...` should no longer rely on a nonfunctional quick-pick path.

### Task 2
- Phase ID: P02
- Task ID: P02-T02
- Task Title: Consolidate First Party Command Registration
- Objective: Move core workbench actions behind one canonical command definition path.
- Problem Statement: `MainFrame`, `LayoutManager`, `Toolbar`, and widget-local bindings currently duplicate action logic.
- Scope: Save, open, find, replace, sidebar toggles, view mode changes, tab actions, settings open, canvas/workbench mode.
- Out of Scope: Plugin-defined commands beyond adapter support.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.cpp`
- Related Systems / Components: Menu bar, toolbar, command palette, shortcut overlay, context menus.
- Current Behavior: Same user intent is implemented multiple times with different side effects.
- Intended Behavior: UI surfaces dispatch through one command definition and one execution path.
- Technical Approach: Introduce or extend central command registry/provider usage and replace direct inline lambdas incrementally.
- Implementation Steps:
  1. Extract a core action set.
  2. Bind UI surfaces to command IDs instead of custom lambdas.
  3. Centralize enable/visible state.
  4. Keep adapter shims during migration.
- Edge Cases / Failure Modes: Reentrancy; context-specific commands without active editor or workspace.
- UX Considerations: Consistent labels, shortcuts, and disabled-state messaging.
- Dependencies: P01-T02.
- Validation Steps: Compare behavior from menu, toolbar, palette, and shortcuts for the same actions.
- Acceptance Criteria: No first-party command has materially different behavior by trigger source.
- Rollback / Safety Notes: Migrate in slices to avoid broad breakage.
- References / Context: Current duplication is visible in frame and toolbar sources.
- Example interactions or usage scenarios where helpful: Save from toolbar and `Cmd+S` must both produce identical persistence and feedback behavior.

### Task 3
- Phase ID: P02
- Task ID: P02-T03
- Task Title: Unify Focus And Keyboard Routing Across Shell Surfaces
- Objective: Make focus movement, keyboard mode, and action routing predictable across panels and overlays.
- Problem Statement: Focus behavior is spread across custom controls and local event bindings, which risks inconsistent keyboard access.
- Scope: Activity bar, sidebars, tab bar, status bar, command palette, settings dialog, prompt host, editor/canvas/workbench mode changes.
- Out of Scope: Screen-reader copy improvements outside practical routing changes.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FocusManager.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: FocusManager, focus ring renderer, accessibility controller, accelerator table.
- Current Behavior: Focus state is locally managed and varies by control.
- Intended Behavior: The shell has a consistent focus order, focus restore rules, and keyboard-mode semantics.
- Technical Approach: Define shell-level focus targets and route key handling through centralized policies where possible.
- Implementation Steps:
  1. Map focusable surfaces.
  2. Standardize focus entry/exit and restore.
  3. Normalize accelerator versus widget-local key precedence.
  4. Add focus regression cases for overlays and prompts.
- Edge Cases / Failure Modes: Focus trapped in closed/hidden panels; shortcuts firing while a prompt is active.
- UX Considerations: Keyboard-only use must be coherent and discoverable.
- Dependencies: P01-T02 and P01-T03.
- Validation Steps: Navigate the shell without mouse input and confirm focus returns after modal/overlay dismissal.
- Acceptance Criteria: Focus order and action routing are deterministic across major surfaces.
- Rollback / Safety Notes: Preserve platform-native text-entry behavior.
- References / Context: `MarkAmpApp::FilterEvent` already toggles keyboard mode globally.
- Example interactions or usage scenarios where helpful: Open command palette, cancel, and return focus to the prior editor or sidebar control.

### Task 4
- Phase ID: P02
- Task ID: P02-T04
- Task Title: Standardize Context Menu Generation And Command Invocation
- Objective: Replace bespoke context menu behavior with shared models and command execution.
- Problem Statement: Context menus are scattered, and their actions often bypass the same pathways used elsewhere.
- Scope: Explorer/file tree, editor, tab bar, output panel, problems panel, debug console, canvas, search results.
- Out of Scope: Radical visual redesign.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- Related Systems / Components: ContextMenuModel, command registry, selection model.
- Current Behavior: Many menus directly bind inline lambdas with surface-specific assumptions.
- Intended Behavior: Context menu actions use the same command IDs and state rules as primary entry points.
- Technical Approach: Build per-surface context factories that emit shared command descriptors.
- Implementation Steps:
  1. Catalog context menu surfaces.
  2. Replace inline command bodies with command IDs.
  3. Unify enable/disable and shortcut display.
  4. Standardize empty-area menus and selection-dependent menus.
- Edge Cases / Failure Modes: Right-click on stale selection; selection changes caused by context click.
- UX Considerations: Context menus should reinforce, not contradict, the rest of the workbench.
- Dependencies: P02-T02.
- Validation Steps: Compare context menu actions with menu/palette/shortcut equivalents.
- Acceptance Criteria: Context menus are command-driven and consistent.
- Rollback / Safety Notes: Keep surface-specific selection context as parameters, not duplicated implementations.
- References / Context: File tree and tab bar contain especially rich local menus.
- Example interactions or usage scenarios where helpful: Rename in file tree context menu should share validation and feedback with keyboard rename.

### Task 5
- Phase ID: P02
- Task ID: P02-T05
- Task Title: Standardize Command Feedback For Success Failure And Disabled States
- Objective: Ensure every command has predictable user feedback semantics.
- Problem Statement: Some actions silently do nothing when prerequisites are missing, while others use notifications or status changes inconsistently.
- Scope: Command execution feedback, disabled-state reasons, no-op messaging, command completion toasts/status updates.
- Out of Scope: Domain-specific long-form diagnostics.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Systems / Components: NotificationEvent, status bar, command palette, accessibility announcements.
- Current Behavior: Feedback varies widely by surface and command.
- Intended Behavior: Each command has a clear success, loading, disabled, empty, and error story.
- Technical Approach: Add command-result conventions and central helpers.
- Implementation Steps:
  1. Define feedback categories by command type.
  2. Wire common success/error helpers.
  3. Expose disabled reasons to palette/tooltips where possible.
  4. Ensure feedback is also accessible.
- Edge Cases / Failure Modes: Repeated actions spamming notifications; noisy success toasts for routine commands.
- UX Considerations: Favor calm, contextual feedback over modal interruption.
- Dependencies: P02-T02.
- Validation Steps: Review representative actions across editor, workspace, canvas, and settings.
- Acceptance Criteria: Silent failure paths are eliminated for top-level commands.
- Rollback / Safety Notes: Throttle or coalesce repeated notifications.
- References / Context: Needed before broad workflow polish in later phases.
- Example interactions or usage scenarios where helpful: Running “Save” with no active file should show a clear but lightweight explanation.

