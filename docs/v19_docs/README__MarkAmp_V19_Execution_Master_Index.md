# MarkAmp V19 Execution Master Index

## Purpose Of The V19 Pass
The `v19` pass is the next execution package after the broad `v18` integration plan. It is intentionally narrower, more dependency-aware, and more implementation-oriented. It focuses on the remaining seams that still prevent MarkAmp from behaving like one elite, trustworthy workbench.

This package is based on another repository inspection, including `v18_docs` and the current implementations in:

- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/StatusBarAdapter.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp`

## Fresh Assessment
The repository has real progress since the earlier planning pass, but many of the right-looking abstractions are still disconnected from the actual shell.

Key current findings:

- `MainFrame` still owns major workflow logic directly, including file open flows, workspace open handling, palette population, accelerators, and settings-open behavior. The shell still depends on local handlers instead of one execution spine.
- `WorkspaceOpenOrchestrator`, `WorkspaceLoadStateModel`, `ShellLayoutState`, `SettingsDeepLinkRouter`, `StatusBarAdapter`, `CanvasWorkbenchMode`, and `NotebookShellHost` exist, but several are not actually instantiated into the main runtime path or only log state.
- `WorkspaceSessionRestore` is still a memory-only stub. It serializes snapshots but does not apply them meaningfully.
- `CommandRegistry` and `register_workbench_commands()` exist, but `MainFrame::RegisterPaletteCommands()` still manually constructs a separate command universe. Shortcut registration is also still duplicated in `MainFrame`.
- `CommandPaletteModel` and `CommandRegistry` both implement search/ranking separately, so palette behavior cannot yet be canonical.
- `SearchSidebarPanel` still searches `"."` instead of the active workspace root and spins a local async task with placeholder coordination.
- `CanvasWorkspacePanel` creates a new `EventBus` for `CanvasPanel`, which breaks durable integration with the main shell event graph.
- `NotebookShellHost`, `NotebookDocumentLifecycle`, and `NotebookSessionManager` currently stop at in-memory bookkeeping and notifications rather than a true notebook workbench experience.
- `StatusBarItemService` exists and is wired into plugins, but the visible status bar is still mostly self-rendered internal state. The adapter exists but is not yet the integration bridge.
- Icon migration support exists in several places, but the system is still fragmented between canonical IDs, manifest lookup, emoji fallbacks, unicode placeholders, and separate MUI alias logic.

## What Changed From V18
The `v18` plan correctly identified the integration classes of work. The `v19` package changes the plan in these ways:

- It no longer treats contract definition as the main work. The next wave is implementation consolidation.
- It re-sequences work around the true blocker chain: shell execution spine first, then command/context routing, then session and panel lifecycle, then workflow-specific completion.
- It explicitly calls out partial implementations that now need to be either completed or removed.
- It narrows broad initiatives into smaller, testable tasks that AI coding agents can execute directly.
- It adds stronger requirements for diagnostics, smoke-path validation, and regression protection so integration quality can be measured instead of assumed.

## Phase Summary
1. [Phase 01: Workbench Execution Spine](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_01__Workbench_Execution_Spine.md)
   Consolidate startup, workspace open, shell mode switching, and core runtime service ownership under one workbench controller path.
2. [Phase 02: Command Context And Interaction Contracts](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_02__Command_Context_And_Interaction_Contracts.md)
   Make commands, shortcuts, menus, toolbars, palette, prompts, and context menus resolve through one command and context system.
3. [Phase 03: Workspace Session Dirty And Save Flows](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_03__Workspace_Session_Dirty_And_Save_Flows.md)
   Finish workspace open, restore, autosave, dirty-state, reopen, and crash-safe continuity.
4. [Phase 04: Docking Panels Focus And Visibility Lifecycle](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_04__Docking_Panels_Focus_And_Visibility_Lifecycle.md)
   Remove panel lifecycle drift across sidebars, bottom panels, focus restore, and placeholder surfaces.
5. [Phase 05: Explorer Search And Navigation Completion](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_05__Explorer_Search_And_Navigation_Completion.md)
   Unify explorer, search, quick open, result activation, and active-document synchronization.
6. [Phase 06: Settings Menus Toolbars And Surface Discoverability](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_06__Settings_Menus_Toolbars_And_Surface_Discoverability.md)
   Finish settings deep linking and make top-level interaction surfaces consistent and trustworthy.
7. [Phase 07: Notebook Host Execution And Document Trust](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_07__Notebook_Host_Execution_And_Document_Trust.md)
   Promote notebook code from bookkeeping objects to a usable workbench surface with proper execution and persistence.
8. [Phase 08: Canvas Workbench And Cross Surface Integration](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_08__Canvas_Workbench_And_Cross_Surface_Integration.md)
   Finish canvas mode as a real workbench surface and connect board, panel, and cross-surface flows.
9. [Phase 09: Feedback Observability And Regression Harnesses](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_09__Feedback_Observability_And_Regression_Harnesses.md)
   Standardize feedback and add enough instrumentation and tests to trust integration changes.
10. [Phase 10: MUI Icon Completion And Visual Normalization](/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_10__MUI_Icon_Completion_And_Visual_Normalization.md)
   Finish the MUI migration and eliminate mixed icon semantics, layout drift, and accessibility inconsistencies.

## Highest Priority Next Moves
- Land Phase 01 before broad UI polish work. The current shell still has too many direct mutations in `MainFrame` and `LayoutManager`.
- Land Phase 02 immediately after Phase 01 so new work stops deepening command duplication.
- Start Phase 03 and Phase 04 in parallel after Phase 01 foundations are in place.
- Start Phase 05 once workspace-root ownership and active-document ownership are centralized.
- Do not push more notebook or canvas UI on top of the current shell until Phases 01 through 04 remove the major lifecycle drift.

## Critical Path Dependencies
- Phase 01 blocks almost everything. Without a real workbench execution spine, later fixes keep scattering logic across `MainFrame`, `LayoutManager`, and ad hoc event handlers.
- Phase 02 depends on Phase 01 because canonical command execution needs a stable shell owner and stable active-context source.
- Phase 03 depends on Phase 01 and partially on Phase 02 because restore and autosave rules must use canonical shell and command paths.
- Phase 04 depends on Phase 01 because panel state has to persist through the same shell controller that owns session transitions.
- Phase 05 depends on Phases 01 through 03 for canonical workspace root, active file, and open/reveal behavior.
- Phase 07 depends on Phases 01 through 04 so notebook tabs, execution status, and restore behavior plug into the real shell instead of another island.
- Phase 08 depends on Phases 01 through 04 and must explicitly consume the same event bus, command, focus, and persistence systems.
- Phase 09 can begin earlier for instrumentation, but its full validation harness is only useful once Phases 01 through 08 are landing.
- Phase 10 should inventory in parallel, but final normalization should happen after command, panel, and canvas/notebook hosts stop moving structurally.

## Parallelizable Work
- After Phase 01, Phase 03 and Phase 04 can run in parallel if they share the same shell controller contracts.
- In Phase 02, prompt-host work can run in parallel with palette/menu/shortcut consolidation if file ownership is separated.
- In Phase 05, quick open and workspace search can run in parallel once active-document and workspace-root contracts are stable.
- In Phase 07, notebook host wiring and execution/output lifecycle can run in parallel after notebook document ownership is established.
- In Phase 08, canvas event-bus consolidation and cross-surface navigation can run in parallel after canvas shell ownership is fixed.
- In Phase 10, icon inventory cleanup and accessibility labeling can run in parallel after the canonical icon resolution layer is chosen.

## Top UX Integration Risks
- The user sees multiple surfaces that appear functional but are still backed by placeholder behavior or notification-only flows.
- Palette, menu, toolbar, shortcut, and context menu actions can still drift because they are not all using the same command registry.
- Search and navigation behavior is present but not yet trustworthy because result activation, workspace root selection, and active-file propagation remain fragmented.
- Canvas and notebook workflows look first-class in places but still do not share the same shell lifecycle, restore semantics, or focus model as the editor.
- Mixed iconography and unicode placeholders still make surfaces feel inconsistent even where functionality is present.

## Top Technical Architecture Risks
- `MainFrame` and `LayoutManager` remain overloaded with orchestration, direct event subscriptions, and duplicated state ownership.
- New abstractions risk becoming dead weight if they are not actually promoted into the main runtime path.
- The shell still relies on raw event fan-out instead of durable controller boundaries in several critical flows.
- Memory-only models for session, notebook, panel layout, and icon migration can create false confidence without delivering real continuity.
- Local background tasks and local event buses inside individual surfaces will keep breaking shared state if not removed.

## Acceptance Criteria For The V19 Wave
- There is one authoritative shell/workbench execution path for startup, workspace open, mode switching, and restore.
- Palette, shortcuts, menus, toolbars, and context menus all execute through the same command registry and context-evaluation path.
- Workspace restore, dirty state, autosave, reopen, and crash recovery behave deterministically for editor, notebook, and canvas surfaces.
- Panel visibility, activation, focus restore, and docking persistence are consistent across primary sidebar, secondary sidebar, and bottom panel.
- Explorer, search, quick open, and result activation all use the same workspace-root and active-document contracts.
- Notebook and canvas surfaces no longer depend on isolated event paths or notification-only shell adapters.
- Status, notifications, loading, failure, and recovery surfaces are observable and regression-tested.
- MUI icons are canonical across major surfaces, with no mixed legacy residue in visible primary workflows.
