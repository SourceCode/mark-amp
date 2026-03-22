# Phase 06 - Editor, Search, Preview, And Diagnostics Completion

## Phase Goal

Finish the editor-adjacent workflows that still stop at partial search, preview fallback paths, or weak diagnostic navigation.

## Measurable Outcome

- Search returns real results and navigates reliably.
- Preview and diagnostics surfaces are wired into the same artifact and command systems.
- Editor overlays and navigators stop depending on placeholder service behavior.

### Task P06-T01

- Phase ID: `P06`
- Task ID: `P06-T01`
- Task Title: Replace empty `SearchService` production paths with real search execution
- Priority: `P0`
- Category: `Editor Completion`
- Objective: Make keyword, phrase, and regex search return real data instead of empty results.
- Why This Matters Now: Search is a core IDE workflow and still contains explicit incomplete branches.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp` returns empty results for core search modes.
- User / Product Impact: Search UI cannot be trusted even when it appears present.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`
- Prior Plan References: `v19` Phase 05, `v20` Phase 10, `v23` Phase 05
- Scope: Keyword, phrase, regex execution and result shaping
- Out of Scope: SQL/query-mode feature expansion beyond current intent
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchEngine.h`, `/Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.h`
- Related Features / Systems / Components: Search engine, query parsing, result ranking
- Current Behavior: Search completion events can fire with no real results behind them.
- Intended Completed Behavior: Search modes execute against indexed content and return ranked, highlighted hits.
- Missing Pieces: Real execution backend, highlights, indexing hookup
- Technical Approach: Bind `SearchService` to index/query providers instead of placeholder return paths.
- Implementation Steps: Implement query execution; attach ranking and highlight building; surface errors for invalid regex.
- Validation Steps: Search across representative documents and compare hits to known fixtures.
- Acceptance Criteria: Keyword, phrase, and regex searches produce non-placeholder results with accurate counts and highlights.
- Dependencies: `P02-T02`, `P03-T01`
- Parallelization Notes: Can proceed with panel-side search host work once artifact state is stable.
- Risks / Failure Modes: Query correctness and large-result performance can regress together.
- Cleanup / Migration Notes: Remove comments and branches that intentionally return empty production results.
- Observability / Diagnostics Notes: Emit query type, index source, result count, and elapsed time.
- Rollback / Safety Notes: Keep feature flag for experimental ranking changes, not for empty-result fallback.
- References / Context: Search query parsing and indexing stack
- Example scenarios where useful: Regex search against open workspace returns real file/block hits instead of zero-result placeholders.

### Task P06-T02

- Phase ID: `P06`
- Task ID: `P06-T02`
- Task Title: Finish search panel and result-navigation integration
- Priority: `P0`
- Category: `Panel Completion`
- Objective: Make the search sidebar a trustworthy shell over real search services.
- Why This Matters Now: The panel currently reflects the service gap and also contains its own temporary behavior.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` still uses placeholder root and temporary async assumptions.
- User / Product Impact: Search feels partial, inconsistent, and workspace-agnostic.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- Prior Plan References: `v21` Phase 07, `v23` Phase 05 and 08
- Scope: Search panel query lifecycle, workspace root, result navigation, replace previews
- Out of Scope: Search UI visual polish beyond functional state clarity
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchResultNavigator.h`, `/Users/ryanrentfro/code/markamp/src/core/SearchReplacePreview.h`
- Related Features / Systems / Components: Search panel, editor navigation, replace preview
- Current Behavior: Panel behavior is partly synthetic and insufficiently attached to workspace state.
- Intended Completed Behavior: Search panel reflects active workspace, query state, navigation, replace preview, and errors accurately.
- Missing Pieces: Workspace-aware search root, async execution lifecycle, editor navigation handoff
- Technical Approach: Bind panel state to shared search session objects and artifact-aware navigation.
- Implementation Steps: Replace placeholder root; wire async states; attach navigation and replace-preview paths.
- Validation Steps: Search, navigate, replace preview, and cancel across multi-file workspace scenarios.
- Acceptance Criteria: Search panel is workspace-aware, result-accurate, and editor-navigation-complete.
- Dependencies: `P06-T01`, `P05-T02`
- Parallelization Notes: Can proceed with editor overlay completion.
- Risks / Failure Modes: Race conditions between async search and active artifact changes.
- Cleanup / Migration Notes: Remove temporary search-root and local async workaround logic.
- Observability / Diagnostics Notes: Log query lifecycle, cancel, replace preview generation, and navigation outcomes.
- Rollback / Safety Notes: Retain cancel-safe behavior if async implementation changes.
- References / Context: Search navigation contract
- Example scenarios where useful: Clicking a search result in the panel opens the correct file, group, and line even for newly restored sessions.

### Task P06-T03

- Phase ID: `P06`
- Task ID: `P06-T03`
- Task Title: Complete preview, math, and mermaid fallback seams with explicit readiness behavior
- Priority: `P1`
- Category: `Editor Completion`
- Objective: Replace silent or weak preview fallbacks with trustworthy rendering and readiness handling.
- Why This Matters Now: Preview fidelity still depends on placeholder branches.
- Execution Gap Statement: Preview and renderer surfaces still emit placeholders when backends are absent or partial.
- User / Product Impact: Users see incomplete previews without clear explanation or consistent fallback behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp`
- Prior Plan References: `v22` Phase 08 and 14, `v23` Phase 13
- Scope: Markdown preview, math rendering, mermaid rendering, fallback messaging
- Out of Scope: Entirely new renderer backends beyond the current product direction
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/IMathRenderer.h`
- Related Features / Systems / Components: Preview panel, rendering backends
- Current Behavior: Some preview modes still rely on placeholder output rather than explicit feature state.
- Intended Completed Behavior: Preview either renders correctly or communicates exact unavailable/degraded state without misleading content.
- Missing Pieces: Renderer availability contract, fallback UX, error propagation
- Technical Approach: Distinguish unavailable backend, invalid content, and degraded preview states in rendering outputs and UI.
- Implementation Steps: Replace placeholder branches; add readiness checks; surface renderer state in preview UI.
- Validation Steps: Render markdown with mermaid/math under available and unavailable backend scenarios.
- Acceptance Criteria: Preview behavior is explicit, correct, and no longer silently placeholder-driven.
- Dependencies: `P04-T03`
- Parallelization Notes: Can progress alongside import/export rendering work.
- Risks / Failure Modes: Renderer absence can still degrade preview unexpectedly if not surfaced clearly.
- Cleanup / Migration Notes: Remove temporary placeholder HTML paths once proper degraded-state UX exists.
- Observability / Diagnostics Notes: Emit renderer-availability and fallback reason diagnostics.
- Rollback / Safety Notes: Preserve safe plain-text/code fallback for renderer failures.
- References / Context: Preview panel and renderer services
- Example scenarios where useful: A missing mermaid backend shows a clear non-blocking unavailable state instead of a generic placeholder block.

### Task P06-T04

- Phase ID: `P06`
- Task ID: `P06-T04`
- Task Title: Complete diagnostics, problems navigation, and quick-fix handoff contracts
- Priority: `P1`
- Category: `Editor Completion`
- Objective: Make diagnostics surfaces navigate accurately and stop advertising incomplete quick-fix behavior.
- Why This Matters Now: Problems and diagnostic overlays exist, but downstream action chains are still weak.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` still defers quick-fix behavior and diagnostics navigation is not yet authoritative everywhere.
- User / Product Impact: Error reporting feels present but not fully actionable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v19` feedback/observability, `v22` Phase 08, `v23` Phase 05
- Scope: Problems panel, editor markers, navigation, quick-fix command contracts
- Out of Scope: Full language-server implementation beyond current diagnostics sources
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Features / Systems / Components: Diagnostics, problem navigation, editor markers
- Current Behavior: Diagnostics are visible, but quick-fix and navigation depth are incomplete.
- Intended Completed Behavior: Diagnostics surfaces navigate deterministically and either offer real fixes or clearly omit them.
- Missing Pieces: Navigation contract completion, quick-fix capability modeling
- Technical Approach: Normalize diagnostic entries and action capability into shared services consumed by panel and editor.
- Implementation Steps: Add stable navigation payloads; wire marker jumps; gate quick-fix UI on real providers.
- Validation Steps: Trigger diagnostics, navigate from panel to editor, invoke or hide quick-fix actions correctly.
- Acceptance Criteria: Diagnostics navigation works end to end, and quick-fix UI never misleads.
- Dependencies: `P04-T01`, `P05-T01`
- Parallelization Notes: Can proceed with editor and build-output work.
- Risks / Failure Modes: Diagnostics can desync if entries are keyed only by file path and not artifact state.
- Cleanup / Migration Notes: Remove placeholder quick-fix affordances where providers do not exist.
- Observability / Diagnostics Notes: Emit navigation target resolution and quick-fix provider availability.
- Rollback / Safety Notes: Hide quick-fix actions rather than leaving broken affordances visible.
- References / Context: Problems panel and diagnostics service
- Example scenarios where useful: Clicking a problem in an unsaved file opens the correct artifact and location, not only disk-backed files.

### Task P06-T05

- Phase ID: `P06`
- Task ID: `P06-T05`
- Task Title: Normalize editor navigation overlays to the active artifact model
- Priority: `P1`
- Category: `Editor Completion`
- Objective: Make find, replace, breadcrumbs, navigation history, and overlays artifact-aware.
- Why This Matters Now: Navigation features still assume classic file-backed editor semantics in places.
- Execution Gap Statement: Editor-adjacent overlays and navigation surfaces are not uniformly bound to the newer artifact model.
- User / Product Impact: Navigation can feel inconsistent across restored, unsaved, notebook, or split editor scenarios.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.h`, `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp`
- Prior Plan References: `v19` Phase 05, `v20` Phase 06, `v22` Phase 05 and 08
- Scope: Breadcrumbs, navigation history, find/replace host selection, editor-group targeting
- Out of Scope: New navigation features
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp`
- Related Features / Systems / Components: Navigation history, overlays, editor groups
- Current Behavior: Navigation surfaces are not yet uniformly artifact-aware.
- Intended Completed Behavior: Navigation overlays target the correct active artifact and group regardless of save state.
- Missing Pieces: Shared artifact-targeting contract for navigation services
- Technical Approach: Key navigation entries by artifact and group identity, not only file path.
- Implementation Steps: Update history entries; bind find/replace host resolution; align overlays and breadcrumbs.
- Validation Steps: Navigate across unsaved files, restored files, notebooks, and canvas-linked documents.
- Acceptance Criteria: Navigation overlays consistently resolve the intended active artifact context.
- Dependencies: `P02-T04`, `P03-T03`
- Parallelization Notes: Can proceed alongside search and diagnostics integration.
- Risks / Failure Modes: Mixed identity models can create broken back/forward behavior.
- Cleanup / Migration Notes: Remove file-path-only assumptions from navigation services.
- Observability / Diagnostics Notes: Record navigation target type and resolution source.
- Rollback / Safety Notes: Preserve backwards-compatible history parsing during migration.
- References / Context: Navigation service and active-document tracker
- Example scenarios where useful: `Go Back` returns to the prior unsaved file tab and cursor position, not just the prior disk path.
