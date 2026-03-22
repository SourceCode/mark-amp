# Phase 13: Rendering Markdown Mermaid Media PDF And Print Completion

## Outcome

Complete the rendering and document-output stack so markdown, diagrams, media, PDF/text extraction, export templates, and print preparation stop depending on placeholder or shallow rendering behavior.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P13-T01

- Phase ID: P13
- Task ID: P13-T01
- Task Title: Finish diagram and rich-content renderers that still fall back to placeholders
- Priority: P0
- Category: Placeholder Implementation
- Atomic Completion Tasks Covered: 60
- Objective: Replace placeholder rendering in supported rich-content formats with real renderers.
- Why This Matters Now: Placeholder output remains one of the clearest visible signs of unfinished rendering work.
- Completion Gap Statement: Mermaid and several extended diagram/rendering paths still depend on placeholders or stubs.
- User / Product Impact: Rich markdown and technical-content workflows remain visibly incomplete.
- Repository Evidence: [MermaidBlockRenderer.cpp](/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp), [EChartsRenderer.h](/Users/ryanrentfro/code/markamp/src/core/EChartsRenderer.h), [FlowchartRenderer.h](/Users/ryanrentfro/code/markamp/src/core/FlowchartRenderer.h)
- Scope: Mermaid, flowchart, ECharts, error overlays, and renderer availability handling.
- Out of Scope: Pure style tuning of already-working renderers.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/EChartsRenderer.h`; `/Users/ryanrentfro/code/markamp/src/core/FlowchartRenderer.h`; `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`
- Related Features / Systems / Components: Preview, export, diagrams, rendering.
- Current Behavior: Rich-content rendering still uses placeholder output in several intended feature families.
- Intended Completed Behavior: Supported diagram/content formats render fully or fail explicitly with accurate unsupported messaging.
- Missing Pieces: Renderer backends, content sanitization, availability checks, and export parity.
- Technical Approach: Finish the renderer backends and normalize unsupported states separate from placeholder states.
- Implementation Steps: Integrate missing renderers; remove placeholder HTML generation; finish export/preview parity; harden error-path behavior.
- Validation Steps: Render supported diagram fixtures in preview and export pipelines and compare outputs.
- Acceptance Criteria: Placeholder rich-content rendering is eliminated for intended supported formats.
- Dependencies: Phase 05.
- Risks / Failure Modes: External renderer backends can create sandbox and dependency concerns.
- Cleanup / Migration Notes where relevant: Delete placeholder render helpers once real rendering is authoritative.
- Observability / Diagnostics Notes where relevant: Record renderer availability and fallback/error reasons.
- Rollback / Safety Notes: Clearly disable unsupported renderers rather than emitting placeholder success-like output.
- References / Context: This phase converts content breadth into trustworthy rendering.
- Example scenarios where useful: A flowchart export contains a real diagram instead of a stubbed placeholder block.

### P13-T02

- Phase ID: P13
- Task ID: P13-T02
- Task Title: Finish PDF text extraction viewing and media fallback paths
- Priority: P1
- Category: Placeholder Implementation
- Atomic Completion Tasks Covered: 60
- Objective: Replace shallow PDF/media stand-ins with real extraction and viewing behavior.
- Why This Matters Now: PDF and media support touch search, preview, import, and panels simultaneously.
- Completion Gap Statement: PDF viewer services and extractors still rely on simulated pages or stubbed extraction.
- User / Product Impact: PDF-backed workflows remain incomplete across multiple surfaces.
- Repository Evidence: [PdfViewerService.cpp](/Users/ryanrentfro/code/markamp/src/core/PdfViewerService.cpp), [ContentIndexer.cpp](/Users/ryanrentfro/code/markamp/src/core/ContentIndexer.cpp), [ImageObjectRenderer.cpp](/Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp)
- Scope: PDF text extraction, PDF page access, media dimensions, missing-media fallback policy, search indexing integration.
- Out of Scope: Visual polish of viewers.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PdfViewerService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ContentIndexer.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp`
- Related Features / Systems / Components: PDF, media, indexing, preview, canvas.
- Current Behavior: PDF/media support includes simulated or placeholder-backed branches.
- Intended Completed Behavior: PDF and media support uses real extraction, metadata, and rendering behavior where supported.
- Missing Pieces: Backend integrations, extraction adapters, and unified fallback policy.
- Technical Approach: Finish PDF/media backends and align all consumers on the same capability model.
- Implementation Steps: Implement PDF extraction and viewing; replace heuristic media dimension fallbacks; integrate outputs into indexers and viewers.
- Validation Steps: Search and preview PDF/media fixtures and verify content, metadata, and fallback behavior.
- Acceptance Criteria: PDF/media features no longer depend on simulated pages or placeholder-only extraction behavior.
- Dependencies: Phase 08, Phase 12.
- Risks / Failure Modes: Large-media handling can expose memory and latency issues if streaming is not considered.
- Cleanup / Migration Notes where relevant: Remove placeholder-page and simple dimension heuristics once real backends exist.
- Observability / Diagnostics Notes where relevant: Track extractor failures, page-render latency, and media fallback reasons.
- Rollback / Safety Notes: Make unsupported media clearly unsupported, not silently placeholder-backed.
- References / Context: This finishes one of the most cross-cutting rendering debt clusters in the repo.
- Example scenarios where useful: PDF content is indexed and searchable rather than represented as a placeholder page count.

### P13-T03

- Phase ID: P13
- Task ID: P13-T03
- Task Title: Complete export template, print preparation, and document-output fidelity paths
- Priority: P1
- Category: Placeholder Implementation
- Atomic Completion Tasks Covered: 60
- Objective: Replace visible placeholder substitutions and shallow print/export helpers with finished output behavior.
- Why This Matters Now: Document output quality is part of product completion, not just auxiliary tooling.
- Completion Gap Statement: Export and print helpers still use visible placeholder insertion or simplified template behavior in some failure paths.
- User / Product Impact: Exported documents can reveal unfinished behavior even when editing worked well.
- Repository Evidence: [ExportTemplateEngine.cpp](/Users/ryanrentfro/code/markamp/src/core/ExportTemplateEngine.cpp), [PrintPreparationService.cpp](/Users/ryanrentfro/code/markamp/src/core/PrintPreparationService.cpp), [DocumentImporter.cpp](/Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp)
- Scope: Template variable substitution, missing-variable handling, print prep, export fidelity, import/export round trips.
- Out of Scope: Marketplace or cloud publishing integrations.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ExportTemplateEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PrintPreparationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExportService.cpp`
- Related Features / Systems / Components: Export, print, templates, import/export fidelity.
- Current Behavior: Some output paths still lean on visible placeholder substitutions or minimal helpers.
- Intended Completed Behavior: Export and print paths either produce complete output or fail with explicit validation.
- Missing Pieces: Strong validation, missing-variable policy, output verification, and round-trip tests.
- Technical Approach: Replace visible placeholder substitution with validation errors or explicit resolution strategies.
- Implementation Steps: Tighten template validation; finish print-prep data flow; align import/export paths; add fidelity verification.
- Validation Steps: Export and print documents with missing variables, rich content, and mixed assets.
- Acceptance Criteria: Output generation no longer uses placeholder comments as a production fallback for intended workflows.
- Dependencies: Phase 04, Phase 05.
- Risks / Failure Modes: Strict validation can break existing loose templates if migration guidance is absent.
- Cleanup / Migration Notes where relevant: Provide migration for older templates that relied on implicit placeholders.
- Observability / Diagnostics Notes where relevant: Emit template-validation and print-prep error diagnostics.
- Rollback / Safety Notes: Allow preview-before-export to avoid destructive changes to legacy templates.
- References / Context: Completed products do not ship visible placeholder text into exported output for supported cases.
- Example scenarios where useful: A broken export template produces a precise validation error instead of commented placeholder HTML.

### P13-T04

- Phase ID: P13
- Task ID: P13-T04
- Task Title: Finish tokenizer and content-pipeline fidelity where providers still return minimal or partial results
- Priority: P2
- Category: Editor Completion
- Atomic Completion Tasks Covered: 60
- Objective: Improve the lower-level content pipeline so rendering and indexing consumers stop receiving shallow data.
- Why This Matters Now: Several higher-level features depend on content pipelines that are still only partially complete.
- Completion Gap Statement: Incremental tokenization, expression evaluation, and other content helpers still include minimal or placeholder behavior.
- User / Product Impact: Search, syntax, and advanced content workflows remain capped by shallow pipeline outputs.
- Repository Evidence: [IncrementalTokenizer.h](/Users/ryanrentfro/code/markamp/src/core/IncrementalTokenizer.h), [ExpressionEvaluator.cpp](/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.cpp), [DataTransformPipeline.cpp](/Users/ryanrentfro/code/markamp/src/core/DataTransformPipeline.cpp)
- Scope: Token caching, expression evaluation, transform results, and downstream consumer alignment.
- Out of Scope: UI-only syntax styling covered in `v22`.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/IncrementalTokenizer.h`; `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DataTransformPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Features / Systems / Components: Tokenization, transforms, syntax, content consumers.
- Current Behavior: Some pipeline stages still echo inputs or provide minimal results.
- Intended Completed Behavior: Content pipelines emit semantically useful, production-grade outputs for downstream features.
- Missing Pieces: Real evaluation logic, richer token outputs, and alignment across consuming features.
- Technical Approach: Finish pipeline internals in a way that remains deterministic and testable.
- Implementation Steps: Replace echo/minimal evaluators; complete transform semantics; improve token pipeline contracts; update consumers.
- Validation Steps: Run tokenizer, transform, and evaluator fixtures through search, preview, and editor consumers.
- Acceptance Criteria: Production features no longer depend on trivial placeholder pipeline outputs.
- Dependencies: Phase 05, Phase 15.
- Risks / Failure Modes: Deepening content pipelines can change performance and must be measured.
- Cleanup / Migration Notes where relevant: Remove comments labeling transforms as simple placeholders.
- Observability / Diagnostics Notes where relevant: Log pipeline fallback and evaluation-error states.
- Rollback / Safety Notes: Keep strict validation around new evaluators to prevent silent data corruption.
- References / Context: This phase closes hidden content-depth gaps that block higher-level completion.
- Example scenarios where useful: A transform pipeline returns computed values rather than simply echoing the expression text.

### P13-T05

- Phase ID: P13
- Task ID: P13-T05
- Task Title: Add rendering and output fidelity regression gates
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Protect completed renderers and output paths against regressions back into placeholders or shallow fallbacks.
- Why This Matters Now: Rendering debt is prone to regress because fallback code paths are easy to reintroduce.
- Completion Gap Statement: Placeholder rendering has historically been tolerated by tests for several content types.
- User / Product Impact: Rich content quality can silently degrade if not guarded.
- Repository Evidence: [test_rendering_pipeline.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_rendering_pipeline.cpp), [test_html_renderer.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_html_renderer.cpp), [test_preview_panel.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_preview_panel.cpp)
- Scope: Diagram rendering, PDF extraction, export templates, print prep, tokenizer/evaluator output fidelity.
- Out of Scope: Pure visual aesthetics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_rendering_pipeline.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_html_renderer.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_preview_panel.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`
- Related Features / Systems / Components: Rendering, preview, export, print, tests.
- Current Behavior: Some tests still accept placeholder output for intended feature sets.
- Intended Completed Behavior: Tests distinguish unsupported features from regressions back into placeholder output.
- Missing Pieces: Fixture coverage, explicit supported/unsupported matrices, and output snapshots.
- Technical Approach: Promote output fidelity to a regression-gated artifact family.
- Implementation Steps: Replace placeholder-accepting expectations; add snapshot fixtures; define supported-format matrices; wire them into CI.
- Validation Steps: Reintroduce a placeholder Mermaid or PDF path and confirm the gate fails.
- Acceptance Criteria: Render/output fidelity regressions are treated as blockers for completed feature families.
- Dependencies: P13-T01 through P13-T04.
- Risks / Failure Modes: Snapshot tests can become too brittle if outputs are not normalized carefully.
- Cleanup / Migration Notes where relevant: Remove tests that explicitly bless placeholder HTML for supported formats.
- Observability / Diagnostics Notes where relevant: Attach render traces and output diffs to failing test artifacts.
- Rollback / Safety Notes: Normalize timestamps and non-deterministic IDs out of fixtures.
- References / Context: This phase prevents rendering completion from sliding back into fallback-heavy behavior.
- Example scenarios where useful: An export regression from real template validation back to visible placeholder comments fails CI immediately.
