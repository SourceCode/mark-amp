# Phase 03: Commands Shortcuts And Action Surface Removal

## Task Count

- 2 tasks

## Task V29-P03-T01

- Phase ID: V29-P03
- Task ID: V29-P03-T01
- Task Title: Remove command-palette, shortcut, and hidden action registrations for retired features
- Priority: Critical
- Category: Menu / Command Removal
- Objective: Delete all command palette registrations, keyboard routes, and hidden action IDs that invoke Canvas, Notebook, Flashcards, or Tasks.
- Why This Matters Now: After shell entry points are removed, command and shortcut backdoors become the main risk for incomplete retirement.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` still registers notebook and canvas commands in the command palette, and prior plans introduced wide command-surface parity expectations.
- User / Product Impact: Prevents power users from invoking deleted code paths and reduces command-surface clutter.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp:3698`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardCommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskCommandProvider.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_02__Command_Context_And_Interaction_Contracts.md`; `/Users/ryanrentfro/code/markamp/docs/v21_docs/Phase_02__Main_Menu_Command_Palette_And_Shortcut_Parity.md`
- Scope: Command palette entries, keybindings, action IDs, command providers, shortcut overlay content, and hidden experimental actions.
- Out of Scope: Event-family removal handled in Phase 10.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardCommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TaskCommandProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlay.cpp`
- Related Features / Systems / Components: Command palette, shortcut manager, command providers.
- Current Behavior / Presence: Notebook and canvas commands are explicitly registered; flashcards and tasks have provider classes and event routes.
- Intended Post-Removal Behavior: Searchable command surfaces only expose retained workflows.
- Removal / Simplification Direction: Delete providers and registrations rather than marking commands disabled.
- Technical Approach: Remove registration blocks, eliminate provider classes where feature-owned, prune keybinding docs and shortcut overlay content.
- Implementation Steps: Delete notebook/canvas palette commands; remove flashcard and task providers; remove command IDs and overlay text; update tests and snapshots.
- Validation Steps: Open command palette and shortcut overlay; search `canvas`, `notebook`, `flashcard`, `task`; expect no hits except explicit migration notices if any.
- Acceptance Criteria: No command palette or shortcut route remains for retired features.
- Dependencies: Phase 02.
- Parallelization Notes: Can run alongside context-menu cleanup.
- Risks / Failure Modes: Orphaned command IDs may remain referenced by menus or tests.
- Migration / Compatibility Notes: None.
- UX / Layout Cleanup Notes: Command categories should be re-balanced after deletions.
- Cleanup / Consolidation Notes: Remove dead command-provider abstractions if they become single-use.
- Rollback / Safety Notes: Validate retained commands still register cleanly.
- References / Context: v21 command parity work should now be reinterpreted as removal parity for these features.
- Example Scenarios: Typing `Notebook` or `Canvas` in the palette returns nothing.

## Task V29-P03-T02

- Phase ID: V29-P03
- Task ID: V29-P03-T02
- Task Title: Remove toolbar, panel-local, and context-menu actions that target retired features
- Priority: High
- Category: Menu / Command Removal
- Objective: Eliminate secondary action surfaces such as toolbars, panel-local buttons, and context menus that still route into retired features.
- Why This Matters Now: Even after top-level menus are removed, panel-local buttons and context actions can preserve hidden feature reachability.
- Removal Gap Statement: Layout-managed panels still expose task and feature-specific actions, and prior plans pushed more action parity into local surfaces.
- User / Product Impact: Reduces accidental dead-end clicks and makes retained surfaces feel intentionally smaller.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:3053`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2693`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2710`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2893`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v21_docs/Phase_04__Context_Menus_Inline_Actions_And_Target_Aware_Enablement.md`; `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_04__Menus_Command_Palette_Context_Menus_And_Toolbars.md`
- Scope: Toolbar buttons, inline panel headers, empty-state buttons, context menus, panel actions, and right-click affordances.
- Out of Scope: Activity bar and panel registration removal handled in Phase 04.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Related Features / Systems / Components: Panel headers, inline action rows, toolbar authority.
- Current Behavior / Presence: Removed features still advertise local actions like `New Task`, notebook actions, and board creation/export affordances.
- Intended Post-Removal Behavior: No local surface suggests those workflows are still available.
- Removal / Simplification Direction: Delete the local actions and simplify the hosting panels or remove the panels entirely.
- Technical Approach: Remove buttons and action groups before deleting their backing services; keep local surfaces visually coherent after action removal.
- Implementation Steps: Audit panel headers and empty states; delete retired actions; remove associated callbacks; refresh retained toolbar groups.
- Validation Steps: Inspect retained toolbars and side panels for absence of retired actions.
- Acceptance Criteria: No toolbar or local context action remains for the retired feature families.
- Dependencies: Phase 02.
- Parallelization Notes: Can run before full panel deletion but must be reconciled with Phase 04.
- Risks / Failure Modes: Empty toolbars or awkward spacing may remain after button removal.
- Migration / Compatibility Notes: None.
- UX / Layout Cleanup Notes: Replace action-heavy empty states with retained-scope guidance where necessary.
- Cleanup / Consolidation Notes: Remove now-unused helper functions that only built retired action clusters.
- Rollback / Safety Notes: Keep retained toolbar shortcuts and spacing intact.
- References / Context: Earlier panel-local action parity work becomes a removal checklist here.
- Example Scenarios: Task panel no longer shows `+ New Task`; no empty notebook panel offers `Run All`.
