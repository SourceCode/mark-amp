# Phase 05: Explorer Search And Navigation Completion

## Phase Purpose
Make explorer, workspace search, quick open, and result navigation behave like one coherent IDE workflow.

## Measurable Outcome
- Search uses the actual workspace root and canonical result activation.
- Explorer, open editors, search results, and navigation history share the same active-document truth.
- Quick open is no longer a sidecar behavior.

## Tasks

### P05-T01
- Phase ID: P05
- Task ID: P05-T01
- Task Class: Foundational
- Task Title: Make Active Document And Workspace Root Shared Services
- Priority: Critical
- Objective: Establish one authoritative active-document contract and one authoritative workspace-root contract for explorer, search, and navigation surfaces.
- Why This Matters Now: Search, quick open, explorer, and result activation still infer or publish active-file state independently.
- Problem Statement: `SearchSidebarPanel`, `QuickOpenIntegration`, `SearchNavigationContract`, and `MainFrame` each participate in open/reveal flows differently.
- Scope: active file tracking, workspace root access, reveal in explorer, and document activation events.
- Out of Scope: Symbol index quality improvements.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/ActiveDocumentTracker.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SidebarStateSynchronizer.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: active document, explorer selection, file tree reveal, workspace service, sidebars.
- Current Behavior: Active-file state is published broadly, but ownership and canonical consumers are still diffuse.
- Intended Behavior: All workspace surfaces read the same active-document and workspace-root services.
- Technical Approach: Strengthen `ActiveDocumentTracker` and workspace-root accessors, then make surfaces depend on them rather than local state.
- Implementation Steps:
  1. Define the canonical active-document source and workspace-root provider.
  2. Replace local assumptions in explorer, search, and quick open.
  3. Ensure activation and reveal behaviors happen in the right order.
  4. Remove redundant local state where possible.
- Validation Steps:
  1. Open documents from explorer, search, and quick open.
  2. Verify explorer highlight and open-editors list stay synchronized.
- Acceptance Criteria: Explorer, search, and quick open share the same active-document and workspace-root truth.
- Dependencies: P01-T01, P03-T01.
- Risks / Failure Modes: Surfaces that assumed direct tab ownership may need adapter logic.
- UX Notes: Active-document drift is one of the fastest ways to make an IDE feel unreliable.
- Observability / Diagnostics Notes: Add active-document change tracing with source and previous value.
- Rollback / Safety Notes: Keep legacy local state only as read-only mirrors during migration.
- References / Context:
  - `SearchSidebarPanel` currently uses `"."` as workspace root.
- Example Scenarios Where Useful:
  - Selecting a search result should update the open editors list and explorer reveal without extra clicks.

### P05-T02
- Phase ID: P05
- Task ID: P05-T02
- Task Class: Workflow
- Task Title: Replace Search Placeholder Execution With Workspace-Aware Search Navigation
- Priority: High
- Objective: Finish workspace search so it uses real workspace roots, proper cancellation, canonical result activation, and panel-state feedback.
- Why This Matters Now: Search UI exists and feels substantial, but key execution details are still placeholder-grade.
- Problem Statement: `SearchSidebarPanel` executes against `"."`, owns its own async management, and activates results through local callbacks instead of a canonical navigation path.
- Scope: search execution root, cancellation, result activation, no-workspace behavior, and result-state rendering.
- Out of Scope: Distributed or remote search.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSearchEngine.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/SearchNavigationContract.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceLoadStateModel.cpp`
- Related Systems / Components: search panel, workspace service, navigation contract, empty/loading/error states.
- Current Behavior: Search works locally but still uses placeholder root and local task control.
- Intended Behavior: Search is workspace-aware, cancellable, and activates results through the same document-navigation path used elsewhere.
- Technical Approach: Move execution coordination into a search controller that consumes workspace state and navigation contracts.
- Implementation Steps:
  1. Inject workspace-root access into search.
  2. Replace local async handling with a controller or service-managed task path.
  3. Route result activation through `SearchNavigationContract`.
  4. Show no-workspace, loading, empty, and error states through shared panel-state patterns.
- Validation Steps:
  1. Search with and without workspace open.
  2. Start a long search, then cancel or replace the query.
  3. Activate results and verify exact line/column navigation.
- Acceptance Criteria: Search behaves like a first-class workspace feature instead of a local panel experiment.
- Dependencies: P05-T01, P09-T02.
- Risks / Failure Modes: Search panel responsiveness may regress if task handoff is poorly staged.
- UX Notes: Keyboard-first search and result activation must feel immediate and exact.
- Observability / Diagnostics Notes: Log query, root, file count, duration, and activation path.
- Rollback / Safety Notes: Maintain current search engine implementation behind the controller while execution ownership shifts.
- References / Context:
  - `SearchSidebarPanel.cpp` explicitly notes placeholder root and temporary async handling.
- Example Scenarios Where Useful:
  - `Cmd+Shift+F`, type query, arrow through results, press Enter, and land on the exact match in the active editor.

### P05-T03
- Phase ID: P05
- Task ID: P05-T03
- Task Class: Workflow
- Task Title: Integrate Quick Open Navigation History And Result Activation
- Priority: High
- Objective: Make quick open, recent items, and symbol/file activation use the same navigation and MRU semantics as explorer and search.
- Why This Matters Now: Quick open exists conceptually but still sits outside the main command and navigation spine.
- Problem Statement: `QuickOpenIntegration` mostly publishes `ActiveFileChangedEvent` and logs intent, while command execution for command-mode items is still only a comment.
- Scope: quick open file activation, symbol activation, command-mode dispatch, recent-item MRU, and navigation history.
- Out of Scope: Full fuzzy ranking redesign beyond basic canonicalization.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/QuickOpenIntegration.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CrossSurfaceNavigator.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp`
- Related Systems / Components: quick open, MRU, symbol navigation, command palette, navigation history.
- Current Behavior: Quick open logic is partially isolated and command-mode activation is not finished.
- Intended Behavior: Quick open is a navigation mode layered on top of the same command and document activation services used elsewhere.
- Technical Approach: Extend the command/prompt system with quick-open modes backed by canonical navigation and command execution.
- Implementation Steps:
  1. Route quick-open file and symbol activation through the canonical navigator.
  2. Implement command-mode dispatch through `CommandRegistry`.
  3. Share MRU bookkeeping with navigation history.
  4. Align palette UI so quick open and command mode feel like one system.
- Validation Steps:
  1. Open files, symbols, and commands from quick open.
  2. Verify recent ordering and back/forward navigation history.
- Acceptance Criteria: Quick open does not rely on sidecar activation behavior.
- Dependencies: P02-T01, P05-T01.
- Risks / Failure Modes: Mixing quick open and command palette modes without clear context can confuse ranking.
- UX Notes: Quick open should feel like a discoverable power tool, not a separate subsystem.
- Observability / Diagnostics Notes: Track quick-open mode, chosen result type, and navigation outcome.
- Rollback / Safety Notes: Keep the old activation helper behind adapters until history integration is stable.
- References / Context:
  - `QuickOpenIntegration.cpp` explicitly leaves command execution to the registry without implementation.
- Example Scenarios Where Useful:
  - Typing a symbol in quick open should open the file, move the caret, update MRU, and show the right navigation history entry.
