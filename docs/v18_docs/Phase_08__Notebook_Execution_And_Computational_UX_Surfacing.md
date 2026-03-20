# Phase 08: Notebook Execution And Computational UX Surfacing

## Phase Goal
Surface the existing notebook core into a coherent, user-facing notebook workflow that belongs inside the workbench.

## Tasks
### Task 1
- Phase ID: P08
- Task ID: P08-T01
- Task Title: Create A Real Notebook Shell Host
- Objective: Replace notebook placeholder/sidebar affordances with a typed notebook workbench host.
- Problem Statement: Notebook core services exist, but sidebar/workbench UI presence is not integrated at the same maturity level.
- Scope: Notebook list/open/create shell host, notebook surface container, activity bar integration, empty state, commands.
- Out of Scope: Rich cell UI in this task.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/FileTemplateEngine.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: NotebookManager, activity bar, workbench modes, file templates.
- Current Behavior: Notebooks appear in the activity bar and menus, but the registered sidebar surface is currently a placeholder.
- Intended Behavior: Users can create, open, browse, and focus notebooks from a real shell surface.
- Technical Approach: Introduce a notebook workbench host and register it in the shell like other typed panels/modes.
- Implementation Steps:
  1. Build typed notebook host.
  2. Connect notebook list/open/create actions.
  3. Route activity bar selection to the typed host.
  4. Add empty/loading/error states.
- Edge Cases / Failure Modes: No notebooks available; stale notebook files; invalid notebook metadata.
- UX Considerations: Notebook entry should feel as direct as Explorer or Canvas.
- Dependencies: P03-T03 and P03-T05.
- Validation Steps: Use activity bar and commands to create/open notebooks.
- Acceptance Criteria: Notebooks are surfaced through a real host, not a placeholder.
- Rollback / Safety Notes: Hide unfinished notebook modes rather than ship dead affordances.
- References / Context: Notebook lifecycle core exists in `src/core/Notebook.cpp`.
- Example interactions or usage scenarios where helpful: Clicking Notebooks in the activity bar should show actual notebook resources and actions.

### Task 2
- Phase ID: P08
- Task ID: P08-T02
- Task Title: Wire Notebook Document Lifecycle To Persistence And Tab State
- Objective: Make notebook open, close, rename, remove, and save participate in normal workbench resource flows.
- Problem Statement: Notebook lifecycle events exist in core, but the workbench needs a visible document lifecycle around them.
- Scope: Notebook open/close, tab integration if applicable, rename/remove propagation, persistence, recent/open state.
- Out of Scope: Execution semantics.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSerializer.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: NotebookManager, notebook serializer, shell tabs, session restore.
- Current Behavior: Notebook lifecycle is present in core but not fully reflected in workbench navigation.
- Intended Behavior: Notebook resources open and persist with the same confidence as files or boards.
- Technical Approach: Add notebook document adapters into shell resource management.
- Implementation Steps:
  1. Define notebook resource identity and open path.
  2. Wire lifecycle events to shell state.
  3. Persist active/open notebook state where appropriate.
  4. Handle rename/remove across all visible surfaces.
- Edge Cases / Failure Modes: Notebook deleted while open; serializer validation failure; duplicate names.
- UX Considerations: Notebook resources need clear names, dirty state, and close flows.
- Dependencies: P08-T01 and P03-T02.
- Validation Steps: Create, open, rename, close, and reopen notebook resources.
- Acceptance Criteria: Notebook lifecycle is visible and reliable in the shell.
- Rollback / Safety Notes: Keep notebook data safe on partial failures.
- References / Context: Notebook events exist in `Events.h` and core manager implementation.
- Example interactions or usage scenarios where helpful: Renaming a notebook updates its visible title everywhere immediately.

### Task 3
- Phase ID: P08
- Task ID: P08-T03
- Task Title: Surface Cell Editing And Execution Controls End To End
- Objective: Connect cell management and execution pipeline to visible notebook controls.
- Problem Statement: `NotebookCellManager`, `KernelManager`, and `NotebookExecutionPipeline` exist, but their UI surface is not fully integrated.
- Scope: Add/remove/move cells, edit cell source, run cell, run all, execution state, keyboard shortcuts, toolbar or inline controls.
- Out of Scope: New kernel types.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookCellManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/KernelManager.h`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: notebook UI host, execution pipeline, cell manager, kernel manager.
- Current Behavior: Core execution and cell lifecycle layers are substantial, but visible notebook interaction paths need shell/UI realization.
- Intended Behavior: Notebook users can edit and run cells through obvious, reliable controls.
- Technical Approach: Build a notebook surface that binds cell UI actions to existing core APIs and events.
- Implementation Steps:
  1. Design notebook cell host interactions.
  2. Wire add/remove/move/edit to cell manager.
  3. Wire run actions to execution pipeline.
  4. Reflect running/success/failure state in UI.
- Edge Cases / Failure Modes: No kernel, kernel start failure, running multiple cells, cancelled execution.
- UX Considerations: Execution controls must be clear without overwhelming prose-first notebook use.
- Dependencies: P02-T02 and P08-T01.
- Validation Steps: Edit cells, reorder them, run one cell, run all cells, and observe output/state.
- Acceptance Criteria: Notebook execution is a real product workflow.
- Rollback / Safety Notes: Fail clearly when kernels are unavailable.
- References / Context: Existing notebook core is strong enough to support a real UI surfacing effort.
- Example interactions or usage scenarios where helpful: Clicking “Run Cell” in an active notebook executes the focused code cell and marks its execution state.

### Task 4
- Phase ID: P08
- Task ID: P08-T04
- Task Title: Integrate Notebook Outputs Search And Export
- Objective: Complete the user workflow from computation to persisted output and discoverability.
- Problem Statement: Notebook search, export, diff, and serializer systems exist but are not yet integrated as one coherent UX.
- Scope: Output rendering, output persistence/trust, notebook search index, export actions, basic diff/compare entry points where applicable.
- Out of Scope: Rich visualization rendering beyond existing support.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSearchIndex.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookExportEngine.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookDiffEngine.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: outputs, trust/session manager, export engine, notebook search.
- Current Behavior: Subsystems exist independently, but users need integrated entry points and shell feedback.
- Intended Behavior: Notebook outputs can be searched, exported, and restored consistently.
- Technical Approach: Expose output/search/export actions through notebook UI and shared workbench feedback channels.
- Implementation Steps:
  1. Surface output state and trust level.
  2. Add notebook-specific search path and result navigation.
  3. Hook export actions into shared progress/notification UX.
  4. Add session restore for outputs where supported.
- Edge Cases / Failure Modes: Untrusted output; failed export; large output search.
- UX Considerations: Users must understand what is persisted and what is transient.
- Dependencies: P08-T03 and P09-T01.
- Validation Steps: Execute notebook, search outputs, export notebook, relaunch, inspect restored state.
- Acceptance Criteria: Notebook workflows extend beyond editing into output and export completion.
- Rollback / Safety Notes: Respect trust restrictions when restoring rich outputs.
- References / Context: Notebook export, search, diff, and session services already exist.
- Example interactions or usage scenarios where helpful: Search within notebook output should open the notebook and focus the correct cell/output region.

### Task 5
- Phase ID: P08
- Task ID: P08-T05
- Task Title: Align Notebook Commands Menus And Activity Bar Entry Points
- Objective: Make notebook actions discoverable from standard workbench surfaces.
- Problem Statement: Notebooks have menu/activity affordances but not a fully harmonized command story.
- Scope: Menu items, command palette entries, toolbar actions, sidebar selection, keyboard shortcuts.
- Out of Scope: New notebook-specific command ideas beyond current capability set.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NavigationCommandProvider.h`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Related Systems / Components: command system, activity bar, notebook host, command palette.
- Current Behavior: Discoverability is incomplete relative to the amount of notebook functionality already present.
- Intended Behavior: Notebook workflows are reachable through the same workbench interaction model as editor and canvas workflows.
- Technical Approach: Register notebook actions in the shared command model and bind them to the new notebook host.
- Implementation Steps:
  1. Audit existing notebook commands and menu items.
  2. Add missing palette/menu/activity bindings.
  3. Ensure enable/disabled state respects notebook context.
  4. Add feedback for commands when no notebook is active.
- Edge Cases / Failure Modes: Commands triggered with no notebook open; mixed editor/notebook focus.
- UX Considerations: Users should not need to discover notebooks accidentally.
- Dependencies: P02-T02 and P08-T01.
- Validation Steps: Trigger notebook actions from each standard entry surface.
- Acceptance Criteria: Notebook functionality is discoverable and consistent.
- Rollback / Safety Notes: Hide commands until backing context is valid.
- References / Context: There is already a Notebooks menu entry in `MainFrame`.
- Example interactions or usage scenarios where helpful: “Open in Notebook” from navigation or command palette should reach the notebook host directly.

