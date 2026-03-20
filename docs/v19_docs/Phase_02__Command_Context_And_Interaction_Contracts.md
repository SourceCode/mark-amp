# Phase 02: Command Context And Interaction Contracts

## Phase Purpose
Make every user-visible action resolve through one command registry, one active-context model, and one prompt-host policy.

## Measurable Outcome
- `CommandRegistry` becomes the canonical execution source.
- Palette, menus, shortcuts, toolbars, and context menus stop drifting.
- Prompt surfaces preserve focus and use workbench-native behavior.

## Tasks

### P02-T01
- Phase ID: P02
- Task ID: P02-T01
- Task Class: Foundational
- Task Title: Replace MainFrame Palette Registration With Registry-Backed Commands
- Priority: Critical
- Objective: Remove the separately maintained command palette command list in `MainFrame` and build palette contents from `CommandRegistry`.
- Why This Matters Now: The palette is still a second command system with different labels, coverage, and behavior.
- Problem Statement: `MainFrame::RegisterPaletteCommands()` manually registers commands while `CommandRegistry` and `register_workbench_commands()` already exist.
- Scope: Command palette population, MRU, preview, search scoring, and execution handoff.
- Out of Scope: Final command taxonomy expansion.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`
- Related Systems / Components: command palette, command registry, shortcut labels, MRU ranking, command feedback.
- Current Behavior: Palette registration is manual and diverges from the centralized registry already initialized in `MarkAmpApp`.
- Intended Behavior: Palette items are projected from `CommandRegistry` plus active context, and execute via `CommandRegistry::execute_command()`.
- Technical Approach: Make the palette a view over the registry instead of a separate command store.
- Implementation Steps:
  1. Inject `CommandRegistry` into palette-related surfaces.
  2. Replace `RegisterPaletteCommands()` with a registry projection layer.
  3. Remove duplicate fuzzy scoring between `CommandPaletteModel` and `CommandRegistry` by selecting one canonical scorer.
  4. Route execution, MRU updates, and destructive previews through registry metadata.
- Validation Steps:
  1. Compare a representative set of file, edit, view, notebook, and canvas commands between palette and menu.
  2. Confirm palette executes the same code path as menus and shortcuts.
  3. Verify missing-command regressions with a command inventory diff.
- Acceptance Criteria: Command palette no longer owns a manually curated command universe.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Some palette-only commands may reveal missing metadata in `CommandRegistry`.
- UX Notes: Palette labels, icons, shortcuts, disabled states, and previews must feel authoritative.
- Observability / Diagnostics Notes: Log command source as `palette` through the same execution log used by other surfaces.
- Rollback / Safety Notes: Keep a temporary registry-to-palette adapter until parity is proven.
- References / Context:
  - `MainFrame::RegisterPaletteCommands()` is currently massive and drift-prone.
  - `CommandPaletteModel` and `CommandRegistry` both implement search/ranking.
- Example Scenarios Where Useful:
  - `Save`, `Open Preferences`, and `Toggle Canvas Mode` should all execute from the same command registry metadata.

### P02-T02
- Phase ID: P02
- Task ID: P02-T02
- Task Class: Foundational
- Task Title: Unify Shortcut Menu Toolbar And Context Menu Dispatch
- Priority: Critical
- Objective: Make every visible action source dispatch the same command ID with the same enablement rules.
- Why This Matters Now: The current shell still encodes behavior in wx accelerators, menu handlers, toolbar buttons, and raw event subscriptions independently.
- Problem Statement: Commands are still duplicated in `MainFrame`, `Toolbar`, `LayoutManager`, and context-menu implementations.
- Scope: accelerators, menu bindings, toolbar actions, header actions, and context menu actions.
- Out of Scope: Full rework of every feature command definition beyond core workbench actions.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuBuilder.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CommandFeedback.cpp`
- Related Systems / Components: command registry, shortcuts, menu bar, toolbar, context menus, disabled-state reporting.
- Current Behavior: Different action surfaces can hit different code paths or expose different availability.
- Intended Behavior: Every action source resolves a command ID, asks whether it is available in the current context, and executes through the same dispatcher.
- Technical Approach: Introduce a workbench command dispatcher adapter that all UI surfaces call.
- Implementation Steps:
  1. Add a command dispatch adapter with source tagging.
  2. Convert high-value menu and toolbar actions first.
  3. Convert context menus to command IDs rather than surface-local lambdas where possible.
  4. Surface disabled feedback consistently through `CommandFeedbackHelper`.
- Validation Steps:
  1. Compare menu, shortcut, palette, and toolbar for `save`, `find`, `toggle sidebar`, and `settings`.
  2. Compare file-tree rename/delete actions with keyboard and palette equivalents.
- Acceptance Criteria: Shared commands no longer have separate business logic per interaction surface.
- Dependencies: P02-T01.
- Risks / Failure Modes: Some UI affordances may still need surface-specific parameter capture.
- UX Notes: Disabled actions should explain why they are unavailable instead of failing silently.
- Observability / Diagnostics Notes: Record command ID, source, context outcome, and duration in the execution log.
- Rollback / Safety Notes: Migrate high-traffic commands first and leave low-risk local handlers temporarily only if necessary.
- References / Context:
  - `Toolbar.cpp` and `MainFrame.cpp` both hold action behavior today.
- Example Scenarios Where Useful:
  - Right-click rename in explorer and keyboard rename should share the same validation, error, and success path.

### P02-T03
- Phase ID: P02
- Task ID: P02-T03
- Task Class: Workflow
- Task Title: Replace Native Dialog Prompt Flows With Workbench Prompt Hosts
- Priority: High
- Objective: Move quick pick and input box flows out of `wxGetSingleChoiceIndex` and `wxTextEntryDialog` into a consistent workbench-owned host.
- Why This Matters Now: The current prompt flows bypass focus rules, styling, keyboard policy, and accessibility consistency.
- Problem Statement: `QuickPickService` and `InputBoxService` publish request events, but `MainFrame` consumes them with platform-native blocking dialogs.
- Scope: quick pick, input box, focus restore, cancellation semantics, and accessibility announcements.
- Out of Scope: Full extension webview prompt system.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/InputBoxService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp`
- Related Systems / Components: prompt surfaces, extension services, focus manager, accessibility controller, keyboard routing.
- Current Behavior: Prompt requests are converted into blocking native dialogs by `MainFrame`.
- Intended Behavior: Prompt requests render inside a styled workbench surface, preserve invoking context, and restore focus cleanly on cancel or submit.
- Technical Approach: Add a reusable prompt host overlay that services quick pick and input box requests.
- Implementation Steps:
  1. Add a prompt host widget and controller in `src/ui/`.
  2. Route service request events into that host.
  3. Preserve invoking focus via `FocusManager` snapshot/restore.
  4. Remove direct native dialog fallback from normal runtime paths.
- Validation Steps:
  1. Open quick pick from command and extension paths.
  2. Cancel and submit prompts using keyboard only.
  3. Verify focus returns to the invoking editor or panel.
- Acceptance Criteria: Prompt flows are non-blocking, theme-consistent, and focus-safe.
- Dependencies: P02-T01, P02-T02.
- Risks / Failure Modes: Keyboard precedence bugs may appear if overlay and accelerators both consume keys.
- UX Notes: The prompt host should feel like part of the IDE, not a system dialog interruption.
- Observability / Diagnostics Notes: Log prompt request, host render, submit, cancel, and focus restore.
- Rollback / Safety Notes: Keep native dialog fallback behind a debug or failure-only path until prompt host stabilizes.
- References / Context:
  - `QuickPickService` and `InputBoxService` are ready for UI consumption but the current consumer is still native dialogs.
- Example Scenarios Where Useful:
  - An extension-created quick pick should look and behave like a first-party command palette surface.
