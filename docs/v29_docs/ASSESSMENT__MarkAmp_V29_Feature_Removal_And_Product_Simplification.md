# MarkAmp V29 Assessment: Feature Removal And Product Simplification

## Scope

This assessment re-inspects the repository for the dedicated retirement of Canvas, Notebook, Flashcards, and Tasks. It is based on current source layout plus prior planning artifacts found in `/Users/ryanrentfro/code/markamp/docs/v18_docs` through `/Users/ryanrentfro/code/markamp/docs/v27_docs`.

## Confirmed Repository Findings

### A. Shell And Navigation Exposure

- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` still exposes:
  - `Canvas Mode` in the `View` menu
  - `Flashcard Browser` and `Flashcard Review Session` in the `Tools` menu
  - top-level `Notebooks` and `Canvas` menus
  - command-palette registrations for `New Notebook`, `Open Notebook`, `Canvas: New Board`, `Canvas: Toggle Canvas Mode`, and `Canvas: Select Tool`
- `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp` still registers `Notebooks`, `Canvas`, `Flashcards`, and `Tasks` as first-class activity bar items.
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still registers sidebar panels for `NOTEBOOKS`, `CANVAS`, `FLASHCARDS`, and `TASKS`.
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` still has sidebar-mode labeling for notebooks, canvas, flashcards, and tasks.

### B. Shared Infrastructure Still Models Removed Features As First-Class

- `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h` still defines `ArtifactKind::kNotebook` and `ArtifactKind::kCanvas`.
- `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp` still exposes `create_notebook()` and `create_canvas()` and tracks per-kind counters.
- `/Users/ryanrentfro/code/markamp/src/core/Events.h` still contains large dedicated event families for notebooks, canvas, FSRS, flashcards, tasks, kanban, and calendar.
- `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp` still packages canvas-derived features as built-in plugins, including canvas collaboration, canvas apps, kanban board, and mind map.

### C. Canvas Is A Major Product Surface, Not A Small Leaf Feature

- `/Users/ryanrentfro/code/markamp/src/canvas` contains 330 files.
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` contains full canvas workbench switching through `ShowCanvasWorkspace()`.
- `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp` and `/Users/ryanrentfro/code/markamp/src/core/CanvasShellIntegration.cpp` plug canvas into creation, open, save, restore, tabs, and tree nodes.
- `/Users/ryanrentfro/code/markamp/src/core/CanvasSessionContext.cpp`, `/Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp`, and `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp` show collaboration, plugin, and session depth.

### D. Notebook Is Deeply Wired Into Lifecycle And Runtime Flows

- `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp` creates default markdown and code cells, opens notebook artifacts, and handles save/save-as/restore.
- `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp` still exists as a host abstraction.
- `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookVariableInspector.cpp`, and `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp` show runtime and UI surface spread.
- Notebook-specific core files total 42.

### E. Flashcards And FSRS Have Real Persistence And Command Footprints

- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h` persists decks, cards, and review logs in workspace storage.
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSReviewSession.cpp` manages active review sessions.
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardCommandProvider.cpp` exposes a feature command surface.
- `/Users/ryanrentfro/code/markamp/src/core/AIFlashcardGenerator.cpp` and `/Users/ryanrentfro/code/markamp/src/core/AICommandProvider.cpp` tie flashcards into AI flows.
- `/Users/ryanrentfro/code/markamp/src/ui/FlashcardBrowserPanel.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/FlashcardReviewPanel.cpp` keep dedicated UI alive.

### F. Tasks Are Split Between Markdown Extraction, Panels, And Scheduling

- `/Users/ryanrentfro/code/markamp/src/core/TaskService.h` scans markdown content for task syntax and metadata.
- `/Users/ryanrentfro/code/markamp/src/core/TaskAggregator.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskBoardEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskGanttEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskReminder.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TaskScheduler.cpp`, and `/Users/ryanrentfro/code/markamp/src/core/TaskRecurrence.cpp` show backend spread.
- `/Users/ryanrentfro/code/markamp/src/ui/TaskListPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/KanbanEngine.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/CalendarEngine.cpp` show user-facing surfaces.
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still builds a tasks panel with filter tabs and a `+ New Task` affordance.

### G. Tests And Build Graph Still Treat These Features As Shipping Product

- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` and `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` explicitly compile notebook, canvas, flashcard/FSRS, and task files.
- Identified test coverage includes:
  - at least 107 canvas-referencing tests
  - at least 48 notebook-referencing tests
  - at least 7 flashcard-referencing tests
  - at least 5 task-referencing tests
- Many broader integration and release tests also implicitly depend on these features.

## Highest-Leverage Simplification Opportunities

1. Collapse workbench composition by deleting canvas mode entirely.
2. Collapse artifact infrastructure from three artifact families to one retained text-document family.
3. Shrink the event catalog by removing large dedicated families that no longer need to be routed or tested.
4. Remove placeholder panels and simplify sidebar information architecture around retained editor-centric workflows.
5. Shrink build time and regression surface by deleting large feature-specific test families.

## Highest-Risk Removal Blockers

1. Shared artifact infrastructure still assumes notebooks and canvases are peers of text files.
2. Session restore and recent-item handling may reopen removed artifacts or panels.
3. Generic shell code still branches on removed sidebar modes, workbench modes, and menu IDs.
4. Plugin and feature-flag infrastructure still carries canvas-derived contributions.
5. Help text, AI commands, and docs still teach removed workflows.

## Product Direction After Removal

MarkAmp should become a simpler Markdown-first IDE/editor focused on retained text editing, search, graph, AV/data views, export, AI writing support, and source-control workflows. The remaining shell should feel intentionally narrower:

- no second creative-workbench mode
- no computational notebook mode
- no study-system side product
- no task-management subsystem competing with editing

## Recommended Removal Posture

- Prefer true deletion over feature flags or hidden toggles.
- Keep only minimal compatibility code required to ignore, migrate, or safely discard old persisted state.
- Remove shell affordances first so users cannot reach dead code during staged implementation.
- Collapse shared abstractions early where removed features distort retained architecture.
- Add explicit “removed-feature is ignored” migration tests before deleting old persistence readers.
