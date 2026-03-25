# MarkAmp V29 Feature-Removal And Product-Simplification Master Index

## Purpose

This v29 pass is the dedicated feature-removal and product-simplification wave for MarkAmp. It is focused entirely on safely and comprehensively removing Canvas, Notebook, Flashcards, and Tasks from the product, the UI, the command surface, the persistence model, the build graph, and the validation stack.

This is not a feature-expansion pass.
This is not a UI-polish pass except where UI cleanup is required by removal.
This is not a continuation of prior completion plans for these features.

## What Changed From Prior Planning

- V18 through V27 repeatedly treated Canvas and Notebook as first-class workbench surfaces that needed deeper shell integration, runtime hardening, and UI polish.
- V21 emphasized binding or removing large control surfaces in `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`.
- V20, V24, and V25 introduced and expanded shared artifact infrastructure that now still carries `ArtifactKind::kNotebook` and `ArtifactKind::kCanvas` in `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h` and `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`.
- V22, V26, and V27 dedicated major UI workstreams to Canvas and Notebook chrome that are now retired rather than implemented.
- V28 planning artifacts were not present in the repository during this pass, so v29 synthesizes from v18 through v27 plus current repository evidence.

## Work Retired, Merged, And Re-Sequenced

Removed from forward planning:
- Notebook host completion, runtime completion, chrome coordination, output polish, and notebook-specific visual redesign.
- Canvas workbench completion, canvas chrome coordination, collaboration polish, apps/widgets polish, and canvas-specific visual redesign.
- Flashcard deck, review, and FSRS expansion work.
- Task board, gantt, calendar, and task-panel expansion work.

Merged into v29 removal work:
- Menu and command cleanup from `/Users/ryanrentfro/code/markamp/docs/v21_docs/Phase_02__Main_Menu_Command_Palette_And_Shortcut_Parity.md`.
- Panel and shell cleanup from `/Users/ryanrentfro/code/markamp/docs/v21_docs/Phase_05__Panel_Registry_Primary_Sidebar_And_Controlled_Surface_Replacement.md`.
- Artifact lifecycle simplification from `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_02__Artifact_Lifecycle_And_Unsaved_Document_Spine.md`.
- Migration and dead-path retirement from `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_18__Migration_Retirement_Placeholder_Removal_And_Dead_Path_Cleanup.md` and `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_18__Migration_Stub_And_Dead_Path_Retirement.md`.

Retained after removal:
- Core Markdown text editing.
- Search, graph, AV/data views, source control, export paths that serve retained document flows.
- Theme system, plugin infrastructure, shell, editor, and settings infrastructure that still support the remaining product.

## Codebase Assessment Highlights

Removal blast radius confirmed in repository:
- Shell menus and command palette: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Activity bar, sidebar modes, and status indicators: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarMode.h`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Sidebar panel registry and canvas mode switching: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Shared artifact model still encodes notebook and canvas as first-class artifact families: `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`, `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`
- Notebook lifecycle and shell integration: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Canvas lifecycle and shell integration: `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`, `/Users/ryanrentfro/code/markamp/src/core/CanvasShellIntegration.cpp`
- Flashcards and FSRS persistence and commands: `/Users/ryanrentfro/code/markamp/src/core/FlashcardTypes.h`, `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`, `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardCommandProvider.cpp`
- Tasks, gantt, board, and calendar seams: `/Users/ryanrentfro/code/markamp/src/core/TaskService.h`, `/Users/ryanrentfro/code/markamp/src/core/TaskBoardEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskGanttEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`
- Feature plugins tied to removed capabilities: `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp`
- Event catalog still contains large dedicated families for notebook, canvas, flashcards, FSRS, tasks, kanban, and calendar: `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Build and test graph still compiles and runs extensive coverage for removed features: `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`, `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

Scale indicators observed during this pass:
- `/Users/ryanrentfro/code/markamp/src/canvas` contains 330 source files.
- Notebook-specific core files total 42 in `/Users/ryanrentfro/code/markamp/src/core`.
- `/Users/ryanrentfro/code/markamp/src/core/fsrs` contains 20 source files.
- Task-related core and UI files identified total 31 across `/Users/ryanrentfro/code/markamp/src/core` and `/Users/ryanrentfro/code/markamp/src/ui`.
- Repository test files referencing these families include at least 107 canvas-heavy, 48 notebook-heavy, 7 flashcard-heavy, and 5 task-heavy test files, plus shared integration suites.

## Definition Of Fully Removed

Canvas is fully removed when:
- no `Canvas` or board UI remains in menus, view modes, activity bar, sidebars, tabs, startup, or command palette
- no canvas workspace mode remains in layout orchestration
- no board artifact lifecycle, persistence, serializer, importer/exporter, collaboration, plugins, or tests remain
- no residual canvas-only events, tokens, docs, or onboarding help remain except explicit migration handling

Notebook is fully removed when:
- no notebook menu, command, host, tab, output, or runtime UI remains
- no notebook artifact kind, serializer, execution, cell controller, toolbar, or restore workflow remains
- no notebook-specific events, tests, docs, or help remain except explicit migration handling

Flashcards are fully removed when:
- no flashcard browser, review session, extraction, AI generation, import/export, or deck commands remain
- no FSRS runtime, store, review log persistence, or related events remain
- no flashcard help text, taskbar/sidebar labels, or tests remain except explicit migration handling

Tasks are fully removed when:
- no task list panel, task commands, board, gantt, calendar, recurrence, reminder, or aggregation UI remains
- no task indexing workflow, background scheduling, or task-specific persistence remains
- no task-specific events, docs, help text, or tests remain except explicit migration handling

## Highest-Priority Workstreams

1. Remove all user-facing entry points from the shell before touching deep deletion.
2. Collapse shared artifact infrastructure so text files become the only first-class artifact family.
3. Delete dedicated feature subsystems in descending blast radius order: Canvas, Notebook, Flashcards, Tasks.
4. Preserve startup, reopen, recent-item, and session stability by adding compatibility handling before deleting persistence readers.
5. Prune build graph and tests only after migration seams and retained behavior are stable.

## Critical Path Dependencies

- Phase 01 must finish before direct removals to lock the removal ledger and accepted compatibility posture.
- Phases 02 through 04 must remove entry points before Phases 06 through 09 delete implementation, otherwise users can still reach dead paths.
- Phase 05 must simplify artifact infrastructure before full notebook and canvas lifecycle deletion.
- Phase 10 must clean commands, events, plugins, and feature flags before Phase 18 dead-code retirement.
- Phase 11 must land migration and restore handling before old persisted workspaces become unreadable.
- Phase 15 and Phase 19 must happen after the code deletions are complete enough to re-baseline build and regression expectations.

## Parallelizable Workstreams

Can run in parallel after Phase 01:
- Phase 02 and Phase 03
- Phase 06 and Phase 07 once Phase 05 is in progress and shell entry points are removed
- Phase 08 and Phase 09 after Phase 03
- Phase 12, Phase 13, and Phase 14 after direct removals start
- Phase 16 and Phase 17 after shell removal direction is fixed

## Highest-Risk Dependencies

- `ArtifactKind::kNotebook` and `ArtifactKind::kCanvas` are embedded in shared artifact creation and registry code.
- `LayoutManager::ShowCanvasWorkspace()` and canvas mode switching currently change how the central content area behaves.
- `Events.h` contains large removed-feature event families that may still be referenced indirectly by generic listeners or tests.
- `BuiltInPlugins.cpp` exposes canvas-derived features as built-in plugins, which can leave orphaned feature flags and settings if removed incompletely.
- `tests/CMakeLists.txt` and `src/CMakeLists.txt` still explicitly compile removed-feature files; partial deletion will break the build immediately.

## Highest-Risk UX Breakage Risks

- Leaving empty activity bar slots or sidebar modes after deleting panels.
- Leaving menu headings such as `Notebooks` or `Canvas` with no valid actions.
- Leaving `Ctrl+4` canvas mode or notebook shortcuts bound after workbench modes are collapsed.
- Leaving startup/help text that still promises flashcards, notebooks, canvas, or tasks.
- Leaving status-bar labels or empty states that refer to removed views.

## Highest-Risk Persistence And Migration Risks

- Old workspaces or session-restore payloads may still reference notebook or canvas artifact kinds and reopened panels.
- Recent-item or tab-restore records may point at removed notebook or board files.
- Flashcard stores and task-derived caches may persist under workspace storage and require safe ignore or cleanup logic.
- Removal of serializers without compatibility readers can turn startup into a fatal error path instead of a graceful drop-and-notify path.

## Acceptance Criteria For The V29 Wave

- No user-facing shell, menu, command, shortcut, sidebar, or help entry point exists for Canvas, Notebook, Flashcards, or Tasks.
- Only retained product surfaces remain emphasized in navigation, startup, settings, and layout.
- Shared artifact, session, and restore systems no longer model notebook or canvas as active artifact families.
- Flashcard/FSRS and task/calendar engines, stores, schedulers, and panels are deleted or intentionally isolated only as migration readers pending final purge.
- Build configuration, test registration, and CI expectations are updated to the smaller product.
- Explicit migration behavior exists for old persisted state and does not crash startup, restore, or workspace open.
- The remaining app feels intentionally smaller and more coherent rather than partially amputated.

## Task Totals

- Total phases: 20
- Total tasks: 40
- Tasks per phase: 2 each

## Phase Summary

1. Phase 01 inventories all remaining code paths, fixes the definition of fully removed, and locks the compatibility stance.
2. Phase 02 removes main-menu, view-mode, and global shell entry points for the retired features.
3. Phase 03 removes command palette, shortcuts, toolbar, context, and hidden action routes.
4. Phase 04 simplifies activity bar, sidebars, panel registry, status bar, and workbench layout after feature retirement.
5. Phase 05 collapses shared artifact infrastructure so notebook and canvas are no longer first-class artifact kinds.
6. Phase 06 removes Canvas creation, board lifecycle, workbench mode, and shell integration.
7. Phase 07 removes Notebook creation, host, runtime, cells, execution, and output integration.
8. Phase 08 removes Flashcards, FSRS scheduling, deck workflows, review sessions, and AI flashcard generation.
9. Phase 09 removes Tasks, gantt, calendar, kanban task workflows, recurrence, reminders, and task panels.
10. Phase 10 strips removed-feature events, plugins, feature flags, and extension contribution seams.
11. Phase 11 cleans session restore, recent items, persisted layouts, storage, and migration handling.
12. Phase 12 removes file-type, import/export, search/help, and creation-flow affordances tied to retired features.
13. Phase 13 removes indexing, AI, telemetry, logging, and service branches that only existed for retired features.
14. Phase 14 removes icons, tokens, labels, onboarding copy, theme branches, and other UI residue.
15. Phase 15 prunes build targets, CMake registration, scripts, and obsolete tests.
16. Phase 16 rewrites startup, onboarding, empty states, and welcome flows around the retained product.
17. Phase 17 simplifies retained navigation, shell composition, information architecture, and product terminology.
18. Phase 18 retires dead code, comments, TODOs, docs, and obsolete planning references across the tree.
19. Phase 19 adds regression and migration coverage proving the features are gone and startup remains stable.
20. Phase 20 performs release-level validation, cleanup, and final signoff for the smaller product.

## Required Companion Documents

- `/Users/ryanrentfro/code/markamp/docs/v29_docs/ASSESSMENT__MarkAmp_V29_Feature_Removal_And_Product_Simplification.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_01__Removal_Inventory_And_Exit_Criteria.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_02__Global_UI_Entry_Point_Removal.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_03__Commands_Shortcuts_And_Action_Surface_Removal.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_04__Shell_Panels_And_Layout_Simplification.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_05__Artifact_Model_And_Shared_Infrastructure_Collapse.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_06__Canvas_Product_Removal.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_07__Notebook_Product_Removal.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_08__Flashcards_And_FSRS_Removal.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_09__Tasks_Calendar_And_Kanban_Removal.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_10__Events_Plugins_And_Feature_Flag_Cleanup.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_11__Persistence_Session_And_Migration_Cleanup.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_12__Creation_Open_Save_And_Import_Export_Cleanup.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_13__Service_Search_AI_And_Observability_Cleanup.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_14__Theme_Icon_Label_And_Help_Text_Cleanup.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_15__Build_Test_And_CI_Prune.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_16__Startup_Onboarding_And_Empty_State_Simplification.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_17__Retained_Navigation_And_Product_IA_Simplification.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_18__Dead_Code_Docs_And_Legacy_Reference_Retirement.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_19__Regression_Migration_And_Removal_Proof.md`
- `/Users/ryanrentfro/code/markamp/docs/v29_docs/Phase_20__Release_Closure_And_Smaller_Product_Signoff.md`
