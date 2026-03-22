# Phase 05: Editor Preview Search Navigation And Diagnostics Completion

## Outcome

Replace editor-side placeholder behavior, empty search implementations, preview fallbacks, and incomplete diagnostic/navigation chains with finished, cross-surface feature behavior.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P05-T01

- Phase ID: P05
- Task ID: P05-T01
- Task Title: Replace empty search backends with real indexed and regex search execution
- Priority: P0
- Category: Search / Navigation Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make search results real across keyword, phrase, regex, ref-block, and embed-block paths.
- Why This Matters Now: The search service still returns empty results for core modes.
- Completion Gap Statement: Search UI exists, but its backend still short-circuits instead of performing finished work.
- User / Product Impact: Search, navigation, references, and template/asset lookup remain less trustworthy than they appear.
- Repository Evidence: [SearchService.cpp](/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp), [SearchSidebarPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp), [SearchIndexWatcher.h](/Users/ryanrentfro/code/markamp/src/core/SearchIndexWatcher.h)
- Scope: Keyword, phrase, regex, ref-block, embed-block, result grouping, filtering, pagination, and indexing hookup.
- Out of Scope: Pure SQL mode if intentionally delegated elsewhere.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SearchIndexWatcher.h`; `/Users/ryanrentfro/code/markamp/src/core/ContentIndexer.cpp`
- Related Features / Systems / Components: Search panel, references, embeds, navigation, indexing.
- Current Behavior: Core search modes return empty or shallow results and the panel still uses placeholder runtime assumptions.
- Intended Completed Behavior: Search executes real indexed and regex-backed queries with finished result semantics.
- Missing Pieces: Database/index hookup, regex scanning, highlight generation, result ranking, async task ownership.
- Technical Approach: Integrate real indexing and query execution behind the current search API while replacing placeholder task management in the panel.
- Implementation Steps: Wire FTS/index services; implement regex scan path; finish panel async orchestration; align filtering/grouping logic; add error handling.
- Validation Steps: Run search across mixed workspaces, large files, regex failures, and reference-target scenarios.
- Acceptance Criteria: Search no longer returns empty by design for supported modes.
- Dependencies: Phase 03, Phase 04.
- Risks / Failure Modes: Full scans can become too slow if index and async management are not finished together.
- Cleanup / Migration Notes where relevant: Remove current-working-directory and placeholder search-root fallbacks.
- Observability / Diagnostics Notes where relevant: Emit search timing, index freshness, and query-mode diagnostics.
- Rollback / Safety Notes: Keep unsupported query modes explicitly disabled rather than silently returning empty.
- References / Context: This closes one of the clearest core-service stubs in the repo.
- Example scenarios where useful: A phrase search launched from the sidebar returns ranked results and navigates to real hits.

### P05-T02

- Phase ID: P05
- Task ID: P05-T02
- Task Title: Finish preview rendering paths that still degrade to placeholders for core content types
- Priority: P1
- Category: Editor Completion
- Atomic Completion Tasks Covered: 60
- Objective: Remove known placeholder behavior from preview rendering for math, diagrams, and missing-resource cases where support is intended.
- Why This Matters Now: Preview quality still drops into placeholder output for several important content types.
- Completion Gap Statement: Preview and HTML rendering still rely on fallback placeholder rendering in multiple supported scenarios.
- User / Product Impact: Users see incomplete content support where the product implies richer rendering.
- Repository Evidence: [PreviewPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp), [MermaidBlockRenderer.cpp](/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp), [HtmlRenderer.cpp](/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp)
- Scope: Mermaid, math/KaTeX, missing images, diff placeholders, and related preview fallbacks.
- Out of Scope: Visual polish already covered in `v22`.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/DiffRenderer.cpp`
- Related Features / Systems / Components: Preview, markdown rendering, diagrams, math, diff display.
- Current Behavior: Rendering falls back to visible placeholders in supported feature families.
- Intended Completed Behavior: Supported preview content renders fully or fails with explicit unsupported-state handling rather than generic placeholders.
- Missing Pieces: Real renderer integration, math path completion, and fallback-policy tightening.
- Technical Approach: Finish the content renderers that are already structurally present and separate “unsupported” from “not yet wired.”
- Implementation Steps: Integrate real mermaid/math renderers; replace placeholder HTML paths; harden preview-panel progressive rendering and failure reporting.
- Validation Steps: Render mixed markdown with diagrams, math, missing media, and diff content.
- Acceptance Criteria: Placeholder rendering is no longer the default path for intended preview features.
- Dependencies: Phase 13.
- Risks / Failure Modes: Renderer integration can add sandboxing and performance concerns if not isolated properly.
- Cleanup / Migration Notes where relevant: Remove placeholder-specific tests once full render paths are in place.
- Observability / Diagnostics Notes where relevant: Log renderer availability and fallback reasons.
- Rollback / Safety Notes: Keep unsupported formats explicit and safe instead of pretending to render them.
- References / Context: Placeholder preview output is a completion debt, not just a visual issue.
- Example scenarios where useful: A Mermaid block renders an actual diagram in preview rather than a stored-source placeholder div.

### P05-T03

- Phase ID: P05
- Task ID: P05-T03
- Task Title: Complete editor-side navigation helpers, code lenses, peek, and symbol lookup paths
- Priority: P1
- Category: Editor Completion
- Atomic Completion Tasks Covered: 60
- Objective: Replace sample, dummy, or model-only editor augmentations with finished navigation behavior.
- Why This Matters Now: The editor advertises richer navigation concepts than some providers can actually deliver.
- Completion Gap Statement: Several navigation helpers still use sample actions, stub providers, or simplified result models.
- User / Product Impact: Code-intelligence and document-navigation affordances feel shallower than the UI suggests.
- Repository Evidence: [CodeLensProvider.cpp](/Users/ryanrentfro/code/markamp/src/ui/CodeLensProvider.cpp), [PeekProvider.cpp](/Users/ryanrentfro/code/markamp/src/core/PeekProvider.cpp), [LanguageProviderRegistry.h](/Users/ryanrentfro/code/markamp/src/core/LanguageProviderRegistry.h)
- Scope: Code lenses, peek, symbol context, jump targets, and document-structure helpers.
- Out of Scope: Full LSP adoption if not already in scope.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CodeLensProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PeekProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/core/LanguageProviderRegistry.h`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Features / Systems / Components: Editor navigation, code intelligence, structure browsing.
- Current Behavior: Some providers still return sample or stubbed navigation data.
- Intended Completed Behavior: Editor augmentations produce real context-aware navigation output where the surface is exposed.
- Missing Pieces: Real providers, provider routing, symbol context ownership, and result rendering parity.
- Technical Approach: Replace sample output with provider-backed implementations and align provider contracts across surfaces.
- Implementation Steps: Audit exposed editor augmentations; implement missing providers; remove sample actions; hook results into navigation and peek UI.
- Validation Steps: Use code lenses and peek flows on real documents with varying provider availability.
- Acceptance Criteria: Exposed editor augmentations no longer rely on sample or stubbed behavior.
- Dependencies: Phase 17.
- Risks / Failure Modes: Partial provider completion can create inconsistent behavior across languages and file types.
- Cleanup / Migration Notes where relevant: Delete sample-action placeholders once provider paths are live.
- Observability / Diagnostics Notes where relevant: Record provider hit rates and unsupported-feature fallbacks.
- Rollback / Safety Notes: Hide unfinished augmentations for unsupported languages rather than showing fake results.
- References / Context: This aligns editor behavior with the ambition already implied by the registry layer.
- Example scenarios where useful: “Peek” opens real symbol context instead of a model-layer stub list.

### P05-T04

- Phase ID: P05
- Task ID: P05-T04
- Task Title: Finish diagnostics, quick-fix, and problem-navigation chains from detection to resolution
- Priority: P1
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make diagnostics actionable rather than mostly descriptive.
- Why This Matters Now: Problems and diagnostic UI already exist, but quick-fix and resolution paths still lag.
- Completion Gap Statement: Diagnostics can be shown without finished remediation or precise navigation behavior.
- User / Product Impact: Users receive problem information without dependable next actions.
- Repository Evidence: [ProblemsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp), [DiagnosticsService.h](/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h), [SearchResultsTree.cpp](/Users/ryanrentfro/code/markamp/src/ui/SearchResultsTree.cpp)
- Scope: Diagnostic generation, navigation, quick-fix providers, grouping, and editor synchronization.
- Out of Scope: Pure visual restyling.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`; `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsTree.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Features / Systems / Components: Problems panel, diagnostics, quick fixes, navigation.
- Current Behavior: Quick-fix and some navigation paths are deferred or simplified.
- Intended Completed Behavior: Diagnostics can be navigated, resolved, and refreshed through a finished chain.
- Missing Pieces: Provider registration, fix execution, result sync, and error handling for unavailable fixes.
- Technical Approach: Finish the provider and action contracts behind existing diagnostics UI.
- Implementation Steps: Register fix providers; wire problems panel actions; sync diagnostics to editor positions; handle stale diagnostics and missing fixes.
- Validation Steps: Generate problems, navigate to them, apply fixes, and confirm refresh behavior.
- Acceptance Criteria: Diagnostics support finished resolution behavior where fixable issues are exposed.
- Dependencies: P02-T01, P05-T03.
- Risks / Failure Modes: Fix providers can become dangerous if edits are applied without document-version checks.
- Cleanup / Migration Notes where relevant: Remove placeholder quick-fix messaging once providers exist.
- Observability / Diagnostics Notes where relevant: Track quick-fix availability, execution success, and rejection reasons.
- Rollback / Safety Notes: Support undo and version checks for fix application.
- References / Context: This moves the diagnostics surface from informative to complete.
- Example scenarios where useful: Clicking a problem both opens the right location and offers a real fix when one exists.

### P05-T05

- Phase ID: P05
- Task ID: P05-T05
- Task Title: Add editor, preview, search, and diagnostics integration coverage for real workflows
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Prove that editor-side completion work is real and remains real.
- Why This Matters Now: Several current tests still tolerate placeholder rendering or empty backend behavior.
- Completion Gap Statement: Search, preview, and diagnostics flows lack enough end-to-end protection against regression into placeholder states.
- User / Product Impact: Important editor workflows can silently degrade back to stubs.
- Repository Evidence: [test_preview_integration.cpp](/Users/ryanrentfro/code/markamp/tests/integration/test_preview_integration.cpp), [test_preview_panel.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_preview_panel.cpp), [test_phase10_search.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase10_search.cpp)
- Scope: Search execution, preview rendering, diagnostic navigation, quick-fix application, provider fallback behavior.
- Out of Scope: Pure UI screenshot tests.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/integration/test_preview_integration.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_preview_panel.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase10_search.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_html_renderer.cpp`
- Related Features / Systems / Components: Editor workflows, preview, search, diagnostics, testing.
- Current Behavior: Existing coverage still accepts placeholder or empty behavior in places that should eventually be finished.
- Intended Completed Behavior: Tests assert real outputs, real results, and explicit unsupported states instead of quiet placeholders.
- Missing Pieces: Scenario breadth, runtime fixtures, and product-level assertions.
- Technical Approach: Promote current unit coverage into stronger integration coverage and replace placeholder assertions with completion assertions.
- Implementation Steps: Rewrite placeholder-accepting tests; add mixed-content fixtures; add end-to-end search and diagnostics scenarios; update smoke scripts.
- Validation Steps: Intentionally reintroduce an empty-search stub or mermaid placeholder and confirm tests fail.
- Acceptance Criteria: Editor-side completion regressions are caught automatically.
- Dependencies: P05-T01 through P05-T04.
- Risks / Failure Modes: Fixture brittleness can make tests noisy if external renderers are not controlled well.
- Cleanup / Migration Notes where relevant: Remove tests that explicitly bless placeholder preview output for intended features.
- Observability / Diagnostics Notes where relevant: Store rendered output fixtures and query logs for failed runs.
- Rollback / Safety Notes: Keep feature-detection branches explicit when external renderers are optional.
- References / Context: This phase keeps editor-side completion from sliding back into “for now” behavior.
- Example scenarios where useful: A preview regression from rendered math back to placeholder HTML fails CI immediately.
