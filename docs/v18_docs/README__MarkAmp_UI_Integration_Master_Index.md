# MarkAmp UI Integration Master Index

## Initiative Summary
This package is a repository-grounded execution plan for turning MarkAmp into a cohesive IDE-grade product by closing UI-to-functionality gaps, standardizing interaction architecture, and completing end-to-end workflows across shell, editor, canvas, notebook, search, settings, feedback, and icon systems.

## Deep Assessment
The current codebase already contains substantial architecture and UI, but several high-impact seams are only partially integrated:

- `QuickPickService` and `InputBoxService` publish UI request events but are not wired to the app event bus or a concrete workbench UI consumer. Evidence: [src/core/QuickPickService.cpp](/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp), [src/core/InputBoxService.cpp](/Users/ryanrentfro/code/markamp/src/core/InputBoxService.cpp), [src/app/MarkAmpApp.cpp](/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp).
- Extension status bar contributions are created through `StatusBarItemService`, but the visible `StatusBarPanel` appears to render only its own internal state and not service-backed items. Evidence: [src/core/PluginManager.cpp](/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp), [src/core/StatusBarItemService.h](/Users/ryanrentfro/code/markamp/src/core/StatusBarItemService.h), [src/ui/StatusBarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp).
- Settings deep links stop short of delivery: `MainFrame` logs `SettingsOpenRequestEvent` details but does not forward them into a concrete settings navigation/update path. Evidence: [src/ui/MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp).
- Window/session continuity is incomplete. `restoreWindowState()` exists but is not called, and its “restore last open file” behavior only republishes an active-file event rather than reopening the document. Evidence: [src/ui/MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp).
- Major sidebar modes still use placeholder “feature panels” instead of real integrated surfaces, especially notebooks, settings/themes, graph, AI, and parts of canvas. Evidence: [src/ui/LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp).
- Notebook business logic is broad, but user-facing notebook shell integration is weak relative to the existing core capability set. Evidence: [src/core/Notebook.cpp](/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp), [src/core/NotebookCellManager.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookCellManager.cpp), [src/core/NotebookExecutionPipeline.h](/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.h).
- The icon stack is fragmented across `IconLibrary`, `IconRegistry`, `IconProvider`, `IconPipeline`, file icon resolvers, legacy names, emoji, Lucide assets, and prior “material-style” planning, which blocks a clean MUI migration. Evidence: [src/ui/IconLibrary.cpp](/Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp), [src/ui/IconManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp), [src/ui/IconPipeline.cpp](/Users/ryanrentfro/code/markamp/src/ui/IconPipeline.cpp), [src/ui/FileTreeCtrl.cpp](/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp).
- Command definition, menu behavior, accelerators, toolbar actions, palette commands, and event subscriptions are duplicated across `MainFrame`, `LayoutManager`, and per-surface widgets, which creates drift risk and inconsistent UX. Evidence: [src/ui/MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp), [src/ui/LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp), [src/ui/Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), [src/core/ShortcutManager.h](/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h).

## Phase Order
1. [Phase 01: Integration Inventory And Shared Contracts](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_01__Integration_Inventory_And_Shared_Contracts.md)
2. [Phase 02: Command Routing Focus And Prompt Surfaces](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_02__Command_Routing_Focus_And_Prompt_Surfaces.md)
3. [Phase 03: Shell Layout Panels And Session Continuity](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_03__Shell_Layout_Panels_And_Session_Continuity.md)
4. [Phase 04: Editor Workflow Completion And Cross Surface State](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_04__Editor_Workflow_Completion_And_Cross_Surface_State.md)
5. [Phase 05: Explorer Search Navigation And Workspace Flows](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_05__Explorer_Search_Navigation_And_Workspace_Flows.md)
6. [Phase 06: Settings Preferences And Configuration UX](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_06__Settings_Preferences_And_Configuration_UX.md)
7. [Phase 07: Canvas Workbench And Cross Surface Integration](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_07__Canvas_Workbench_And_Cross_Surface_Integration.md)
8. [Phase 08: Notebook Execution And Computational UX Surfacing](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_08__Notebook_Execution_And_Computational_UX_Surfacing.md)
9. [Phase 09: Feedback Status Recovery And Observability UX](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_09__Feedback_Status_Recovery_And_Observability_UX.md)
10. [Phase 10: MUI Icon Migration Consistency And Validation](/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_10__MUI_Icon_Migration_Consistency_And_Validation.md)

## Phase Purpose Summary
- Phase 01 establishes a source-of-truth integration map, removes ambiguity in intent, and defines cross-cutting contracts.
- Phase 02 fixes the action layer so menu, toolbar, shortcut, palette, context menu, and prompt surfaces all hit the same commands.
- Phase 03 stabilizes the shell: layout restore, window state, docking, startup/editor/canvas mode transitions, and panel lifecycle.
- Phase 04 completes editor-centric workflows including save/revert/undo/find/view/state propagation.
- Phase 05 connects workspace navigation, file operations, search, results, and explorer-driven actions.
- Phase 06 makes settings discoverable, deep-linkable, persistable, and consistent with `Config`, `ScopedConfig`, and workspace overrides.
- Phase 07 turns canvas into a first-class workbench mode rather than an isolated panel set.
- Phase 08 surfaces notebook functionality that already exists in core systems through integrated UI and command flows.
- Phase 09 standardizes loading, success, error, notification, status, and recovery patterns across the workbench.
- Phase 10 completes global MUI icon replacement and removes legacy icon path fragmentation.

## Dependencies Between Phases
- Phase 01 is prerequisite for all other phases because it defines the integration ledger and ownership map.
- Phase 02 should precede Phases 04 through 09 because command routing and prompt surfaces are shared infrastructure.
- Phase 03 should land before broad workflow polish because session state and panel lifecycle affect every major surface.
- Phase 06 depends on Phase 02 for deep-link routing and on Phase 03 for shell presentation.
- Phase 07 and Phase 08 depend on Phase 02 and Phase 03 for mode switching, panel hosting, and command execution.
- Phase 09 should start after initial Phase 02 through 08 flows exist so feedback patterns can be normalized against real workflows.
- Phase 10 can inventory in parallel with Phase 01, but large-scale replacement should wait until shell/workflow touchpoints are stabilized by Phases 02 through 09.

## Parallel Workstreams
- Phase 01 task execution can split by shell, editor, canvas, notebook, settings, and icon subsystems.
- During Phase 02, prompt-surface wiring can run in parallel with command-registry consolidation if ownership is split by file set.
- In Phase 03, session restore and docking/panel lifecycle work can run independently until integration testing.
- In Phase 05, explorer/file operations and search/results/navigation can run in parallel.
- In Phase 07, canvas shell integration can run in parallel with inspector/comments/facilitation integration.
- In Phase 08, notebook document lifecycle work can run in parallel with execution/output work if both use shared notebook contracts from Phase 01.
- In Phase 10, icon inventory, asset migration, and usage cleanup can run in parallel once the canonical MUI mapping exists.

## High Risk Integration Areas
- `MainFrame` and `LayoutManager` event duplication.
- `EventBus`-driven UI requests with missing consumers.
- Multiple icon systems and asset naming conventions.
- Session restore across startup, editor, canvas, and workspace state.
- Placeholder sidebar panels masking missing end-to-end workflows.
- Notebook UI surfacing versus already-existing notebook core APIs.
- Status, notifications, and progress being implemented separately by panel.
- Focus behavior across activity bar, sidebars, tab bars, editors, overlays, and dialogs.

## Full Initiative Acceptance Criteria
- Every visible shell action has a deterministic end-to-end path from UI trigger to business logic to state update to user feedback.
- Toolbar, menu, shortcut, context menu, palette, and programmatic command entry points resolve through a shared command model with no behavioral drift.
- Startup, workspace open, session restore, window restore, and panel restore behave consistently across relaunches.
- Editor, canvas, and notebook workflows are first-class workbench modes rather than isolated feature islands.
- Settings and workspace preferences are deep-linkable, searchable, scope-aware, and reliably persisted.
- Notifications, status, progress, empty states, and errors follow one consistent UX language.
- Legacy icon sources are removed or adapter-isolated; MUI icons are canonical across the application.
- The resulting architecture reduces one-off event handlers, placeholder panels, and duplicate control logic.

