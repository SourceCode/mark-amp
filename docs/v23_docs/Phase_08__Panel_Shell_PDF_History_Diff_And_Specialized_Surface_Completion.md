# Phase 08: Panel Shell PDF History Diff And Specialized Surface Completion

## Outcome

Finish the specialized panels and secondary surfaces that still exist as phase-labeled stubs, thin shells, or placeholder-backed utilities so the workbench’s breadth becomes real product depth.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P08-T01

- Phase ID: P08
- Task ID: P08-T01
- Task Title: Replace PDF panel stubs with a finished viewer, thumbnail, and annotation workflow
- Priority: P0
- Category: Panel Completion
- Atomic Completion Tasks Covered: 60
- Objective: Turn the PDF subsystem from placeholder panels into a real document workflow.
- Why This Matters Now: PDF panels are explicitly stubbed yet visible in the repository as intended product features.
- Completion Gap Statement: PDF viewer, thumbnail strip, and annotation sidebar currently stop at shell-level placeholder behavior.
- User / Product Impact: PDF-related workflows remain clearly incomplete.
- Repository Evidence: [PDFViewerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp), [PDFThumbnailStrip.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFThumbnailStrip.cpp), [PDFAnnotationSidebar.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFAnnotationSidebar.cpp)
- Scope: PDF load, page rendering, thumbnails, annotation creation/editing, navigation, zoom, and shell integration.
- Out of Scope: Pure visual polish beyond necessary usability.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFThumbnailStrip.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFAnnotationSidebar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PdfViewerService.cpp`
- Related Features / Systems / Components: PDF viewer, annotations, panels, navigation.
- Current Behavior: PDF panels display thin shells and placeholder rendering assumptions.
- Intended Completed Behavior: PDF documents support real page rendering, navigation, and annotation workflows.
- Missing Pieces: Render backend, annotation persistence, panel integration, and navigation synchronization.
- Technical Approach: Finish PDF services first, then wire the viewer trio into one coherent surface family.
- Implementation Steps: Implement page rendering; wire thumbnails to viewer state; persist annotations; complete page and zoom synchronization; add error handling.
- Validation Steps: Load multi-page PDFs, navigate via thumbnails, add annotations, and reopen the document.
- Acceptance Criteria: PDF support no longer depends on phase-labeled stub panels.
- Dependencies: Phase 07, Phase 13.
- Risks / Failure Modes: PDF backends can add memory and platform differences that need explicit handling.
- Cleanup / Migration Notes where relevant: Delete stub comments and placeholder draw code once the backend is complete.
- Observability / Diagnostics Notes where relevant: Track PDF load times, page-render failures, and annotation persistence errors.
- Rollback / Safety Notes: Gate PDF features behind backend availability checks if dependencies are absent.
- References / Context: This is a concrete example of breadth that still needs true completion.
- Example scenarios where useful: Opening a PDF yields real pages, clickable thumbnails, and persisted annotations instead of one painted status line.

### P08-T02

- Phase ID: P08
- Task ID: P08-T02
- Task Title: Finish history and diff surfaces with real compare, rollback, and navigation behavior
- Priority: P1
- Category: Panel Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make history and diff views functional companions to repository and file workflows.
- Why This Matters Now: History and diff surfaces exist, but still depend on thin or stubbed backing behavior.
- Completion Gap Statement: History panel, diff panel, and repository compare behavior are not yet finished as a user workflow.
- User / Product Impact: Users can see history-related affordances without dependable comparison depth.
- Repository Evidence: [HistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp), [DiffPanel.h](/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.h), [RepositoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp)
- Scope: History timelines, diff opening, compare selection, rollback commands, and surface synchronization.
- Out of Scope: Git-native source-control history in Phase 09.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.h`; `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp`
- Related Features / Systems / Components: History, diff, rollback, repository snapshots.
- Current Behavior: History shell content exists, but the deeper compare and rollback path is incomplete.
- Intended Completed Behavior: Users can browse history, compare versions, and roll back through finished surfaces.
- Missing Pieces: Diff model completion, compare materialization, interaction wiring, and rollback confirmations tied to real data.
- Technical Approach: Finish the underlying compare services and route history panel actions through them.
- Implementation Steps: Complete snapshot compare; finish diff surface classes; wire history selections to compare; add rollback guards and post-rollback refresh.
- Validation Steps: Populate history, compare entries, navigate diffs, and execute rollback safely.
- Acceptance Criteria: History and diff panels are no longer thin wrappers over incomplete backend behavior.
- Dependencies: Phase 04, Phase 09.
- Risks / Failure Modes: Large diffs can expose performance issues if loading strategies remain naive.
- Cleanup / Migration Notes where relevant: Remove shell text that implies timeline functionality beyond what the panel currently does.
- Observability / Diagnostics Notes where relevant: Track diff generation time and rollback outcomes.
- Rollback / Safety Notes: Require safety snapshot creation before destructive rollback.
- References / Context: This phase finishes specialized surfaces that already exist in the workbench.
- Example scenarios where useful: A user can compare two stored document states and then restore one intentionally.

### P08-T03

- Phase ID: P08
- Task ID: P08-T03
- Task Title: Complete specialized sidebar surfaces that still rely on placeholder content or partial services
- Priority: P1
- Category: Panel Completion
- Atomic Completion Tasks Covered: 60
- Objective: Finish specialized panel families that still rely on empty-state-only or summary-only content.
- Why This Matters Now: Secondary surfaces are where unfinished product breadth is easiest to hide.
- Completion Gap Statement: Outline, backlinks, graph minimap, extension details, and similar specialized surfaces still rely on partial content logic.
- User / Product Impact: Secondary panels feel shallower than the shell implies.
- Repository Evidence: [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp), [ExplorerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp), [ExtensionDetailPanel.h](/Users/ryanrentfro/code/markamp/src/ui/ExtensionDetailPanel.h)
- Scope: Outline, backlinks, graph minimap, extension detail, empty/sparse-state transitions, and active-context sync.
- Out of Scope: Pure visual design work already covered in `v22`.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExtensionDetailPanel.h`; `/Users/ryanrentfro/code/markamp/src/core/BacklinkService.h`
- Related Features / Systems / Components: Sidebars, secondary panels, knowledge tools, extension browsing.
- Current Behavior: Several specialized sidebars stop at summary shells or partial content feeds.
- Intended Completed Behavior: Specialized panels are content-complete and context-aware where they are exposed.
- Missing Pieces: Real data feeds, navigation actions, context sync, and sparse-state differentiation.
- Technical Approach: Finish the content services and context propagation behind existing specialized panels.
- Implementation Steps: Replace summary-only content; add real item lists and navigation; sync active document context; remove placeholder-only registrations.
- Validation Steps: Open documents, links, graph views, and extensions and verify specialized panels populate correctly.
- Acceptance Criteria: Specialized sidebars no longer read as placeholder adjuncts to the main surfaces.
- Dependencies: Phase 02, Phase 05, Phase 11, Phase 15.
- Risks / Failure Modes: Some specialized panels may depend on incomplete upstream indices or registries.
- Cleanup / Migration Notes where relevant: Gate any remaining exploratory panels until their content path is finished.
- Observability / Diagnostics Notes where relevant: Report panel population failures and empty-state reasons separately.
- Rollback / Safety Notes: Keep secondary panels hidden if required upstream services are unavailable.
- References / Context: This turns “nice to have” shells into real workbench capabilities.
- Example scenarios where useful: A backlinks panel shows real linked documents and navigates to them, not just a static empty-state message.

### P08-T04

- Phase ID: P08
- Task ID: P08-T04
- Task Title: Finish search, problems, output, and debug-adjacent panel behaviors that remain partial
- Priority: P1
- Category: Panel Completion
- Atomic Completion Tasks Covered: 60
- Objective: Complete panel behaviors that already exist but still have partial command, data, or async paths.
- Why This Matters Now: Panel shells are only trustworthy if their action and data chains fully complete.
- Completion Gap Statement: Search, problems, output, and related panels still include placeholder task management or deferred remediation behavior.
- User / Product Impact: Core IDE panels remain less dependable than expected.
- Repository Evidence: [SearchSidebarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp), [ProblemsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp), [OutputPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp)
- Scope: Async search task ownership, problem navigation/fix, output actions, debug console hooks, build/debug panel readiness.
- Out of Scope: Full debug engine semantics in Phase 09.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`
- Related Features / Systems / Components: Search, problems, output, debug-adjacent surfaces.
- Current Behavior: Panel behavior is present but still contains temporary ownership or deferred actions.
- Intended Completed Behavior: Core IDE panels complete their intended command, state, and feedback chains.
- Missing Pieces: Async orchestration, problem remediation hooks, debug/build integration, and command parity.
- Technical Approach: Close behavior gaps by pairing each panel with its underlying service and command contracts.
- Implementation Steps: Replace temporary async search management; complete problems actions; finish output command handling; wire debug/build panel backends.
- Validation Steps: Exercise core panels during real searches, build failures, output streaming, and debug sessions.
- Acceptance Criteria: Core workbench panels no longer rely on partial behavior behind finished-looking shells.
- Dependencies: Phase 05, Phase 09.
- Risks / Failure Modes: Panel behavior can race with shell lifecycle if async ownership is not centralized.
- Cleanup / Migration Notes where relevant: Remove comments that describe “temporary placeholder for proper async task management.”
- Observability / Diagnostics Notes where relevant: Add task-queue and panel-action diagnostics for search/problems/output.
- Rollback / Safety Notes: Use explicit unavailable states instead of leaving broken actions live during migration.
- References / Context: This phase focuses on behavior completion, not just panel presence.
- Example scenarios where useful: A search started from the sidebar is cancellable, updates incrementally, and navigates reliably.

### P08-T05

- Phase ID: P08
- Task ID: P08-T05
- Task Title: Add specialized-panel completion suites and placeholder-surface eradication checks
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Ensure specialized surfaces cannot quietly regress back into thin shells or stubs.
- Why This Matters Now: Specialized panels are frequently under-tested compared with primary editor surfaces.
- Completion Gap Statement: The repo lacks enough high-value tests that assert specialized panel depth instead of panel existence.
- User / Product Impact: Regressions in secondary workflows can persist unnoticed and weaken the product’s overall completeness.
- Repository Evidence: [test_v21_primary_sidebar.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_v21_primary_sidebar.cpp), [test_v21_panel_lifecycle.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_v21_panel_lifecycle.cpp)
- Scope: PDF, history, outline, backlinks, search, problems, output, and related specialized panels.
- Out of Scope: Pixel-perfect UI tests.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_primary_sidebar.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_panel_lifecycle.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- Related Features / Systems / Components: Panels, regression checks, lifecycle tests.
- Current Behavior: Existing tests mostly audit placeholder states instead of proving final completion.
- Intended Completed Behavior: Tests verify specialized panels are content-complete, action-complete, and correctly gated.
- Missing Pieces: Scenario tests, content fixtures, and final gate assertions.
- Technical Approach: Convert readiness-audit tests into completion assertions and add integration coverage for high-value panels.
- Implementation Steps: Replace placeholder-count assertions with completed-behavior assertions; add panel interaction tests; wire panel-ready gates into CI.
- Validation Steps: Reintroduce a stub panel and confirm tests and gates fail.
- Acceptance Criteria: Specialized panel regressions are visible and blocking.
- Dependencies: P08-T01 through P08-T04.
- Risks / Failure Modes: Overly strict tests can become brittle if panel data is asynchronous and not stabilized.
- Cleanup / Migration Notes where relevant: Remove legacy readiness tests that only prove the audit model can detect a stub.
- Observability / Diagnostics Notes where relevant: Publish panel-completion matrices as test artifacts.
- Rollback / Safety Notes: Keep deterministic test fixtures and stable fake backends where external services are not involved.
- References / Context: Specialized panel completion needs the same rigor as primary editor flows.
- Example scenarios where useful: A history panel test asserts real entries, compare actions, and rollback refresh instead of only that the panel can paint.
