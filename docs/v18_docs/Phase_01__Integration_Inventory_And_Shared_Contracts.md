# Phase 01: Integration Inventory And Shared Contracts

## Phase Goal
Create the system map and shared contracts that let follow-on agent work be deterministic instead of speculative.

## Tasks
### Task 1
- Phase ID: P01
- Task ID: P01-T01
- Task Title: Build UI To Capability Integration Ledger
- Objective: Produce a concrete inventory of visible controls and the backing services, commands, and events they should drive.
- Problem Statement: MarkAmp contains many partially wired surfaces; later implementation work will drift unless every control is mapped to its actual chain.
- Scope: Main shell, startup, activity bar, sidebars, tab strip, toolbar, menus, context menus, status bar, dialogs, overlays, editor, canvas, notebook, search, settings.
- Out of Scope: Fixing behavior in this task.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: EventBus, ShortcutManager, CommandPalette, panel registry, status bar, prompt services.
- Current Behavior: Control ownership is spread across frame, layout manager, and widget-local bindings.
- Intended Behavior: A maintained ledger exists for every visible action with trigger path, handler path, state path, and persistence/feedback path.
- Technical Approach: Generate a repository-backed matrix keyed by surface area and command/event identifiers.
- Implementation Steps:
  1. Enumerate visible controls by source file.
  2. Trace each control to event, command, and service.
  3. Mark missing or duplicate chains.
  4. Store ownership and acceptance notes for follow-on phases.
- Edge Cases / Failure Modes: Hidden controls; platform-only controls; duplicated bindings with non-identical semantics.
- UX Considerations: Inventory should distinguish user-facing intent from implementation detail.
- Dependencies: None.
- Validation Steps: Cross-check ledger entries against menu creation, accelerator binding, and panel factories.
- Acceptance Criteria: No major workbench control remains unclassified.
- Rollback / Safety Notes: Documentation only.
- References / Context: Existing gaps in prompt services, settings deep links, and status bar extensions.
- Example interactions or usage scenarios where helpful: Clicking a toolbar save button, pressing `Cmd+S`, and choosing Save from menu should resolve to the same command row in the ledger.

### Task 2
- Phase ID: P01
- Task ID: P01-T02
- Task Title: Define Shared Command And Action Contract
- Objective: Specify a canonical action model used by menu, toolbar, shortcuts, palette, and context menus.
- Problem Statement: Actions are currently registered in multiple places with drift risk.
- Scope: Command metadata, enabled/disabled state, visibility, context, icon, title, shortcut text, telemetry key.
- Out of Scope: Full implementation of every action.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h`
  - `/Users/ryanrentfro/code/markamp/src/core/WindowCommandProvider.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SidebarCommandProvider.cpp`
- Related Systems / Components: ShortcutManager, command palette, menu bar, toolbar, context menu model.
- Current Behavior: Labels, shortcuts, and invocation sites are duplicated.
- Intended Behavior: One command definition can hydrate all action surfaces.
- Technical Approach: Standardize a command descriptor and route providers through it.
- Implementation Steps:
  1. Audit existing command provider types.
  2. Define canonical fields and resolution rules.
  3. Identify compatibility adapters needed for legacy code.
  4. Document consumer migration order.
- Edge Cases / Failure Modes: Context-sensitive commands; commands with parameter prompts; commands owned by plugins.
- UX Considerations: Command labels and shortcut text must stay consistent everywhere.
- Dependencies: P01-T01.
- Validation Steps: Compare a sample set of commands across menu, toolbar, palette, and shortcut overlay.
- Acceptance Criteria: A written contract exists and covers all first-party action surfaces.
- Rollback / Safety Notes: Documentation only.
- References / Context: Duplication in `MainFrame`, `LayoutManager`, and `Toolbar`.
- Example interactions or usage scenarios where helpful: “Toggle Sidebar” resolves identically from chrome button, menu item, command palette, and shortcut.

### Task 3
- Phase ID: P01
- Task ID: P01-T03
- Task Title: Define Prompt Surface Contract For Quick Pick And Input Box
- Objective: Turn extension-facing prompt services into a concrete workbench contract.
- Problem Statement: `QuickPickService` and `InputBoxService` emit request events but are not wired end to end.
- Scope: Request event schema, hosting surface, focus behavior, result/cancel plumbing, async lifecycle, keyboard semantics.
- Out of Scope: Implementing all prompt UIs.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/InputBoxService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: EventBus, modal/overlay system, focus manager, accessibility announcer.
- Current Behavior: Services can hold callbacks and publish events, but app bootstrap does not set the event bus and UI consumers are absent.
- Intended Behavior: Prompts are first-class shell overlays with deterministic callback completion.
- Technical Approach: Define prompt host ownership and event/result lifecycle contract.
- Implementation Steps:
  1. Specify host window and stacking rules.
  2. Define result event or direct callback completion path.
  3. Define escape, enter, focus return, and reentrancy rules.
  4. Define extension safety constraints.
- Edge Cases / Failure Modes: Nested prompts; prompt shown during modal dialog; workbench shutdown while prompt is active.
- UX Considerations: Must behave like IDE quick open, not a generic modal dialog.
- Dependencies: P01-T02.
- Validation Steps: Trace one single-select, one multi-select, and one input prompt scenario.
- Acceptance Criteria: Prompt contract explicitly closes the current orphaned-service gap.
- Rollback / Safety Notes: Documentation only.
- References / Context: `MainFrame` currently uses `QuickPickService` for at least one palette command.
- Example interactions or usage scenarios where helpful: “Open in Secondary Sidebar...” should show a real quick pick and return selection into sidebar state.

### Task 4
- Phase ID: P01
- Task ID: P01-T04
- Task Title: Define Workbench Mode And Panel Lifecycle Contract
- Objective: Establish how startup, editor, canvas, notebook, and sidebar/bottom-panel states coexist and restore.
- Problem Statement: Multiple modes and panel systems exist, but lifecycle ownership is fragmented.
- Scope: Startup screen, editor workspace, canvas workspace, sidebars, bottom panels, overlays, session restore, panel creation/destruction.
- Out of Scope: Surface-specific features.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceLayout.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelLifecycleManager.cpp`
- Related Systems / Components: WorkbenchShell, PanelAreaModel, panel registry, session restore, config persistence.
- Current Behavior: Window restore is partially disabled and some mode transitions are ad hoc.
- Intended Behavior: Each workbench mode has explicit ownership, state, restore, and transition rules.
- Technical Approach: Define lifecycle states and persistence responsibilities per mode and panel zone.
- Implementation Steps:
  1. Map all mode transitions.
  2. Define persistent versus transient UI state.
  3. Define lazy versus eager creation rules.
  4. Define teardown and restore sequencing.
- Edge Cases / Failure Modes: Mode switching during unsaved edits; restoring into missing workspace; plugin-contributed panels unavailable at restore time.
- UX Considerations: No surprise panel loss, focus loss, or restore mismatch.
- Dependencies: P01-T01.
- Validation Steps: Review startup-to-editor, editor-to-canvas, relaunch-restore flows.
- Acceptance Criteria: Follow-on work can implement lifecycle without inventing new rules.
- Rollback / Safety Notes: Documentation only.
- References / Context: `restoreWindowState()` exists but is not called; `RestoreLayoutState()` only restores partial shell state.
- Example interactions or usage scenarios where helpful: Relaunch should restore window size, active workspace, active panel, and active surface coherently.

### Task 5
- Phase ID: P01
- Task ID: P01-T05
- Task Title: Define MUI Icon Architecture And Legacy Isolation Plan
- Objective: Freeze the canonical icon architecture before migration.
- Problem Statement: Icon code is split across legacy registry/library paths, pipeline/manifest paths, Lucide assets, and ad hoc fallbacks.
- Scope: Canonical source, naming, asset packaging, runtime lookup, theme tinting, DPI, fallback, extension compatibility.
- Out of Scope: Replacing every icon in this phase.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTypeIconResolver.cpp`
  - `/Users/ryanrentfro/code/markamp/resources/icons/lucide/folder.svg`
- Related Systems / Components: Icon manifest, file tree, toolbar, activity bar, status bar, command icons.
- Current Behavior: Multiple runtime paths can render icons, and some surfaces still use placeholders or emoji.
- Intended Behavior: MUI is the canonical system; legacy icon paths are adapter-only and removable.
- Technical Approach: Define canonical ID scheme and a compatibility layer for old names during migration.
- Implementation Steps:
  1. Inventory all icon call sites and names.
  2. Define MUI asset mapping.
  3. Define one rendering path.
  4. Define deprecation path for Lucide, emoji, and placeholder IDs.
- Edge Cases / Failure Modes: Missing asset parity; extension icon contributions; command/file/folder icon differences.
- UX Considerations: Preserve semantic meaning, target size, and contrast.
- Dependencies: P01-T01.
- Validation Steps: Verify every icon category has a target architecture.
- Acceptance Criteria: MUI migration in Phase 10 has a stable architectural target.
- Rollback / Safety Notes: Documentation only.
- References / Context: Prior v16 icon program exists but current code still contains legacy and placeholder usage.
- Example interactions or usage scenarios where helpful: File tree chevrons and activity bar icons should not depend on fallback glyphs or unrelated icon IDs.

