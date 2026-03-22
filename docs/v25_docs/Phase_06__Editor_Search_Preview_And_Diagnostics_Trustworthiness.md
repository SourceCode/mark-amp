# Phase 06: Editor, Search, Preview, And Diagnostics Trustworthiness

## Phase Intent

Finish the editor-adjacent workflows that still stop at placeholder or partial behavior.

## Release-Ready Exit Criteria

- release-path search returns real results,
- preview and diagnostics workflows are consistent with editor state,
- editor-side overlays and navigation no longer depend on synthetic or thin behavior.

## Task Count

3

## Task P06-T01

- Phase ID: P06
- Task ID: P06-T01
- Task Title: Replace empty search service behavior with real indexed release-path search
- Priority: P0
- Category: Release Blocker
- Objective: make keyword, phrase, and regex search deliver real results across the release-path document corpus.
- Why This Matters Now: empty or synthetic search behavior is incompatible with release-grade IDE expectations.
- Release Gap Statement: `SearchService` still returns empty results for core search modes.
- User / Product Impact: search is a high-frequency core workflow and a visible trust signal.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ContentIndexer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- Prior Plan References: `v19 Phase 05`; `v23 Phase 05`; `v24 Phase 06`
- Scope: keyword search, phrase search, regex search, result metadata, result grouping, sidebar integration.
- Out of Scope: SQL mode and future advanced analytics beyond release needs.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchQueryBuilder.h`; `/Users/ryanrentfro/code/markamp/src/core/SearchIndexWatcher.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SearchHistory.cpp`
- Related Features / Systems / Components: indexing; sidebar search; search navigation; result grouping
- Current Behavior: search core comments still describe pending database integration and placeholder stats.
- Intended Release-Ready Behavior: visible search flows produce real results with reasonable relevance and navigation continuity.
- Missing Pieces: real query execution; result materialization; integration with content index and workspace scope.
- Technical Approach: bind `SearchService` to indexed content and ensure sidebar/editor consumers use the real result path.
- Implementation Steps:
1. Implement real query execution for keyword, phrase, and regex modes.
2. Bind search scope to workspace and active content index.
3. Update sidebar and editor consumers to rely on the completed result path.
- Validation Steps:
1. Add integration tests for mixed document sets and regex validation.
2. Verify search results open the correct artifact and location.
- Acceptance Criteria: release-path search produces non-empty, accurate results for indexed content and no longer returns empty placeholders by design.
- Dependencies: P05-T02
- Parallelization Notes: preview/diagnostics work can proceed in parallel once result contracts are stable.
- Risks / Failure Modes: stale index; regex performance; incorrect result grouping.
- Observability / Diagnostics Notes: expose search elapsed time, index state, and scope diagnostics.
- Rollback / Safety Notes: keep safe fallbacks for invalid regex, not empty-success behavior.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`

## Task P06-T02

- Phase ID: P06
- Task ID: P06-T02
- Task Title: Align preview behavior with real renderer availability and diagnostics state
- Priority: P1
- Category: Editor Hardening
- Objective: ensure preview and editor overlays reflect the actual rendering and diagnostic state without misleading fallback paths.
- Why This Matters Now: preview and diagnostics are user-visible quality and trust signals.
- Release Gap Statement: rendering and preview still tolerate too many placeholder branches for release-path content.
- User / Product Impact: incorrect previews or weak diagnostics reduce confidence in Markdown, code, and notebook content.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp`
- Prior Plan References: `v20 Phase 08`; `v23 Phase 13`; `v24 Phase 06`; `v24 Phase 16`
- Scope: editor preview, Mermaid fallback behavior, preview diagnostics, invalid-content messaging.
- Out of Scope: non-release experimental renderers.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/CodeBlockRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`
- Related Features / Systems / Components: preview; renderer selection; diagnostics; editor overlays
- Current Behavior: placeholder and unavailable renderer branches still appear in production rendering paths.
- Intended Release-Ready Behavior: preview clearly renders, errors, or degrades in user-trustworthy ways with matching diagnostics.
- Missing Pieces: renderer capability reporting; consistent fallback messaging; preview smoke coverage.
- Technical Approach: centralize renderer capability checks and make preview surfaces consume the same diagnostics contracts as the editor.
- Implementation Steps:
1. Identify release-path rendering fallbacks that remain acceptable versus unacceptable.
2. Replace silent or confusing placeholders with explicit capability states.
3. Tie preview failures into diagnostics surfaces.
- Validation Steps:
1. Test valid, invalid, and unsupported content scenarios.
2. Verify preview messaging matches diagnostics output.
- Acceptance Criteria: preview never silently implies successful rendering where only a placeholder branch exists.
- Dependencies: P06-T01
- Parallelization Notes: can proceed with broader rendering work later.
- Risks / Failure Modes: over-failing benign content; user-hostile error states.
- Observability / Diagnostics Notes: emit renderer choice and failure-reason diagnostics.
- Rollback / Safety Notes: preserve text fallback for unsafe renderer failures.
- References / Context: `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`

## Task P06-T03

- Phase ID: P06
- Task ID: P06-T03
- Task Title: Finish editor-side diagnostics navigation, overlays, and workflow smoke coverage
- Priority: P1
- Category: Testing / Regression Protection
- Objective: make diagnostics, result navigation, and editor overlays part of verified workflow trust instead of incidental surface behavior.
- Why This Matters Now: search and diagnostics only matter if navigation and overlays are dependable.
- Release Gap Statement: validation depth still trails visible editor workflow ambition.
- User / Product Impact: unreliable diagnostics navigation undermines editing trust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v18 Phase 04`; `v23 Phase 05`; `v24 Phase 06`
- Scope: diagnostics list-to-editor navigation, overlay correctness, result focus restoration.
- Out of Scope: language-server depth beyond current release path.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchResultNavigator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Features / Systems / Components: diagnostics; navigation; editor overlays; problems panel
- Current Behavior: problems and diagnostics surfaces still show signs of deferred quick-fix and partial workflow depth.
- Intended Release-Ready Behavior: diagnostics navigation and overlays reliably target the right artifact, region, and active editor state.
- Missing Pieces: end-to-end workflow tests; problems-panel-to-editor proof; overlay accuracy checks.
- Technical Approach: add smoke and integration coverage around diagnostics publication, panel display, and editor navigation/selection.
- Implementation Steps:
1. Define the editor diagnostics workflow smoke scenarios.
2. Verify problems panel and editor consume the same diagnostic source.
3. Add regression tests for navigation and focus restoration.
- Validation Steps:
1. Raise diagnostics and navigate from panel to editor.
2. Confirm selection, scroll, and overlay behavior.
- Acceptance Criteria: diagnostics workflows are covered by smoke tests and no release-path navigation lands in the wrong file or location.
- Dependencies: P05-T01; P06-T01
- Parallelization Notes: can proceed with editor and problems panel owners independently.
- Risks / Failure Modes: stale diagnostics; incorrect editor activation; wrong line targeting.
- Observability / Diagnostics Notes: add diagnostic-event and navigation traces.
- Rollback / Safety Notes: keep panel and editor fallbacks read-only if navigation fails.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
