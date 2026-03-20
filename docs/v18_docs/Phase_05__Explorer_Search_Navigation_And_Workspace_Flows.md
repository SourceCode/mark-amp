# Phase 05: Explorer Search Navigation And Workspace Flows

## Phase Goal
Make workspace discovery, file operations, search, and navigation behave like one coherent IDE workflow.

## Tasks
### Task 1
- Phase ID: P05
- Task ID: P05-T01
- Task Title: Consolidate File And Workspace Operations Across Explorer And Menus
- Objective: Route rename, delete, new file/folder, reveal, terminal open, and workspace refresh through shared file-operation services.
- Problem Statement: Explorer interactions are rich but heavily widget-local.
- Scope: File tree actions, explorer toolbar, menu actions, command palette file actions, workspace refresh.
- Out of Scope: Full VCS-aware file operations.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.cpp`
- Related Systems / Components: File tree model, workspace service, notifications, context menus.
- Current Behavior: Explorer handles many operations inline with its own validation and refresh logic.
- Intended Behavior: All file operations use shared services and produce consistent UI updates.
- Technical Approach: Extract explorer actions into command/service adapters with tree refresh hooks.
- Implementation Steps:
  1. Inventory file operations implemented in the tree.
  2. Move core file mutations into services/commands.
  3. Rebind explorer menus and key handlers.
  4. Standardize confirmation and error handling.
- Edge Cases / Failure Modes: Permissions errors, rename collisions, deleting active/open files.
- UX Considerations: Explorer should feel dependable and low-friction.
- Dependencies: P02-T04 and P03-T05.
- Validation Steps: Run file operations from explorer and alternate entry points and compare outcomes.
- Acceptance Criteria: File operations are not trapped inside explorer-local logic.
- Rollback / Safety Notes: Keep file-system changes validated and reversible where possible.
- References / Context: `FileTreeCtrl.cpp` contains extensive direct operation handling.
- Example interactions or usage scenarios where helpful: Renaming an open file updates the tab strip and active editor path without manual refresh.

### Task 2
- Phase ID: P05
- Task ID: P05-T02
- Task Title: Finish Search Result Navigation And Selection Recovery
- Objective: Ensure search requests, result display, result activation, and editor reveal are end to end.
- Problem Statement: Search model, sidebar, and result tree exist, but the product needs deterministic navigation and state recovery.
- Scope: Search sidebar, search result tree, editor reveal, match highlighting, keyboard navigation, saved searches where surfaced.
- Out of Scope: Search engine algorithm redesign.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsTree.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SearchEngine.cpp`
- Related Systems / Components: SearchService, SearchPanelModel, editor reveal, navigation history.
- Current Behavior: Search UI exists with custom tree behavior, but full action/state feedback needs systematic wiring verification.
- Intended Behavior: Search behaves like a first-class IDE workflow with reliable open/reveal/focus.
- Technical Approach: Define explicit search request/result/open contracts and ensure editor navigation consumes them.
- Implementation Steps:
  1. Audit search request origins and result activation.
  2. Ensure search results open correct file and range.
  3. Synchronize search selection with editor focus and history.
  4. Add empty/loading/error states.
- Edge Cases / Failure Modes: File deleted after indexing; stale match locations; no workspace open.
- UX Considerations: Search should maintain orientation and highlight context.
- Dependencies: P02-T02 and P04-T04.
- Validation Steps: Search, navigate results by mouse and keyboard, and verify editor reveal behavior.
- Acceptance Criteria: Search results reliably take users to the intended content.
- Rollback / Safety Notes: Preserve incremental search performance while fixing UX chain gaps.
- References / Context: Search UI and service layers are present across `src/ui` and `src/core`.
- Example interactions or usage scenarios where helpful: Press `Cmd+Shift+F`, search, use arrow keys in results, press Enter, editor opens at the exact match and focus lands correctly.

### Task 3
- Phase ID: P05
- Task ID: P05-T03
- Task Title: Integrate Quick Open Command Palette And Navigation Indexes
- Objective: Make quick open, symbol navigation, and workspace navigation share search infrastructure and open behavior.
- Problem Statement: Palette search, workspace search, and navigation services have overlapping capability but fragmented UX.
- Scope: Command palette quick open modes, symbol/workspace/file navigation, recent items, MRU integration.
- Out of Scope: Semantic language intelligence beyond existing indexes.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSymbolIndex.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SearchCommands.h`
- Related Systems / Components: Command palette, fuzzy scoring, navigation history, symbol providers.
- Current Behavior: Command palette exists, but broader navigation integration needs standardization.
- Intended Behavior: Quick open, go to file, go to symbol, and command execution feel unified.
- Technical Approach: Add shared navigation source providers to the palette and unify open/reveal actions.
- Implementation Steps:
  1. Audit navigation-capable providers.
  2. Define palette modes and prefixes if applicable.
  3. Normalize selection/open behavior.
  4. Record navigation actions into history/MRU consistently.
- Edge Cases / Failure Modes: No workspace; empty indexes; stale symbol results.
- UX Considerations: Palette should be fast, predictable, and informative.
- Dependencies: P02-T01 and P02-T02.
- Validation Steps: Open files, symbols, and commands from the palette and compare focus/result behavior.
- Acceptance Criteria: Palette is a real top-level navigation surface, not just a command list.
- Rollback / Safety Notes: Keep palette responsive by using indexed sources.
- References / Context: `CommandPaletteModel`, `NavigationService`, and symbol indexes already exist.
- Example interactions or usage scenarios where helpful: Typing a filename or symbol in the palette should open the same editor state as explorer navigation.

### Task 4
- Phase ID: P05
- Task ID: P05-T04
- Task Title: Complete Explorer Search Sidebar And Open Editors Cohesion
- Objective: Ensure explorer, open editors, and search sidebars do not compete or drift in selected/open state.
- Problem Statement: Multiple sidebar surfaces expose overlapping document lists and navigation actions.
- Scope: Explorer panel, open editors section, search sidebar panel, file tree focus behavior, sidebar mode transitions.
- Out of Scope: New sidebars.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/OpenEditorsSection.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/FileTreeModel.cpp`
- Related Systems / Components: Open editors section, activity bar, active file state, file tree.
- Current Behavior: Each panel can track selection and visibility locally.
- Intended Behavior: Active/open document state is coherent no matter which sidebar surface initiated the action.
- Technical Approach: Normalize active/open state publication and consume it in all sidebar document views.
- Implementation Steps:
  1. Map active/open state publishers.
  2. Align sidebar views to shared state.
  3. Fix selection/focus handoff when switching modes.
  4. Add refresh rules for rename/close/remove scenarios.
- Edge Cases / Failure Modes: File removed externally; sidebar hidden during file change; duplicate filenames.
- UX Considerations: Users should not see conflicting active-file cues.
- Dependencies: P04-T04 and P05-T01.
- Validation Steps: Open, rename, close, and switch files from each sidebar surface.
- Acceptance Criteria: Explorer-related surfaces remain synchronized.
- Rollback / Safety Notes: Preserve independent filtering/search UI while sharing core state.
- References / Context: `OpenEditorsSection` and explorer tree both respond to tab/file events.
- Example interactions or usage scenarios where helpful: Closing a tab from Open Editors should update explorer highlighting and search result focus if relevant.

### Task 5
- Phase ID: P05
- Task ID: P05-T05
- Task Title: Complete Workspace Empty Loading And Error States
- Objective: Give users clear shell feedback for empty workspace, loading workspace, failed workspace, and missing file states.
- Problem Statement: Several flows jump directly from action to result without a unified intermediate state model.
- Scope: Startup panel, workspace load, explorer empty state, search without workspace, missing file on open, restore failures.
- Out of Scope: Full onboarding redesign.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Related Systems / Components: Startup shell, empty-state components, notifications, workspace loader.
- Current Behavior: Some empty states exist, but loading/failure states are inconsistent across flows.
- Intended Behavior: Every workspace-related flow has a user-visible state progression.
- Technical Approach: Add a shared workspace load state model and surface-specific render policies.
- Implementation Steps:
  1. Define workspace state transitions.
  2. Apply them to startup, explorer, and search.
  3. Add missing-file and failed-open messaging.
  4. Ensure state clears correctly on successful load.
- Edge Cases / Failure Modes: Partial scan results; transient file system errors; restore into missing workspace.
- UX Considerations: The app should never appear broken when it is merely waiting or missing context.
- Dependencies: P03-T05 and P09-T01.
- Validation Steps: Simulate empty, loading, failed, and partially restored workspace conditions.
- Acceptance Criteria: Workspace flows communicate state clearly and consistently.
- Rollback / Safety Notes: Use nonblocking feedback for recoverable errors.
- References / Context: `EmptyPanelState` exists and should be reused rather than duplicating placeholders.
- Example interactions or usage scenarios where helpful: Running Search with no workspace open should guide the user instead of silently showing nothing useful.

