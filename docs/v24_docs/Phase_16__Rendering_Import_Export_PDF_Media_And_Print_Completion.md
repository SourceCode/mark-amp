# Phase 16 - Rendering, Import, Export, PDF, Media, And Print Completion

## Phase Goal

Finish the content-fidelity surfaces that still depend on placeholders, weak extraction, or partial specialized panels.

## Measurable Outcome

- Rendering fidelity improves across preview, PDF, import/export, and media paths.
- Specialized PDF surfaces are either real or gated.
- Import/export workflows are backed by durable service behavior.

### Task P16-T01

- Phase ID: `P16`
- Task ID: `P16-T01`
- Task Title: Complete PDF viewing, thumbnail, annotation, and extraction seams
- Priority: `P1`
- Category: `Panel Completion`
- Objective: Turn PDF support from visible stubs into real product behavior or remove it from standard paths.
- Why This Matters Now: PDF surfaces are among the most visibly unfinished specialized panels.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/PDFThumbnailStrip.cpp`, and `/Users/ryanrentfro/code/markamp/src/ui/PDFAnnotationSidebar.cpp` are explicit stubs.
- User / Product Impact: Users can open obviously incomplete PDF surfaces.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/PdfViewerService.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ContentIndexer.cpp`
- Prior Plan References: `v23` Phase 08 and 13
- Scope: PDF page rendering, thumbnails, annotations, index extraction, panel lifecycle
- Out of Scope: Full PDF editing beyond current implied viewer scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp`
- Related Features / Systems / Components: PDF viewer, PDF importer, indexer
- Current Behavior: PDF support is still stub-heavy.
- Intended Completed Behavior: PDF support is real for the capabilities shown in the UI or gated out.
- Missing Pieces: Real page rendering, sidebar data, extraction fidelity
- Technical Approach: Complete the PDF service and bind viewer/thumbnails/sidebar to it; gate unsupported subfeatures explicitly.
- Implementation Steps: Replace simulated pages; complete extraction; wire panel navigation; add readiness gating for unsupported annotation features.
- Validation Steps: Open representative PDFs, navigate pages, inspect thumbnails, search indexed content, and verify panel behavior.
- Acceptance Criteria: PDF viewing surfaces are no longer production stubs.
- Dependencies: `P05-T05`, `P09-T04`
- Parallelization Notes: Can proceed with broader rendering/import work.
- Risks / Failure Modes: Large PDF performance and memory use can degrade quickly.
- Cleanup / Migration Notes: Remove stub panel painting and simulated page fallback code.
- Observability / Diagnostics Notes: Emit page-render timings, extraction success, and unsupported-feature diagnostics.
- Rollback / Safety Notes: Hide unsupported PDF features rather than exposing broken controls.
- References / Context: PDF viewer and importer services
- Example scenarios where useful: Opening a PDF shows real pages, real page count, and real thumbnails instead of simulated content.

### Task P16-T02

- Phase ID: `P16`
- Task ID: `P16-T02`
- Task Title: Complete import/export fidelity for markdown, PDF, HTML, and batch workflows
- Priority: `P1`
- Category: `Core Workflow Completion`
- Objective: Make import/export services match the product breadth already implied by the UI and docs.
- Why This Matters Now: Import/export is part of finishing content workflows, not an optional edge feature.
- Execution Gap Statement: Some rendering/export paths still depend on placeholder or thin logic.
- User / Product Impact: Content exchange feels unreliable or incomplete.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ExportService.h`, `/Users/ryanrentfro/code/markamp/src/core/DocumentImporter.h`
- Prior Plan References: `v23` Phase 13
- Scope: Export/import orchestration, validation, progress, template fidelity
- Out of Scope: New formats beyond current declared support
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ExportValidator.h`, `/Users/ryanrentfro/code/markamp/src/core/BatchExportEngine.h`
- Related Features / Systems / Components: Export/import, publishing profiles, templates
- Current Behavior: Core infrastructure exists, but some real-world fidelity and error paths remain under-specified.
- Intended Completed Behavior: Import/export behaves predictably across supported formats with accurate status and validation.
- Missing Pieces: Fidelity validation, failure handling, specialized content fallback rules
- Technical Approach: Harden format adapters and align progress/error behavior across services and dialogs.
- Implementation Steps: Validate format adapters; update failure paths; add batch and profile validation; align UI feedback.
- Validation Steps: Import/export representative documents with code, media, diagrams, and notebooks.
- Acceptance Criteria: Supported import/export workflows are deterministic, validated, and correctly surfaced to users.
- Dependencies: `P06-T03`, `P08-T04`
- Parallelization Notes: Can progress with preview/rendering work.
- Risks / Failure Modes: External tool dependencies like Pandoc can create environment-specific failures.
- Cleanup / Migration Notes: Gate or document unsupported external dependency states clearly.
- Observability / Diagnostics Notes: Emit format, duration, dependency availability, and validation results.
- Rollback / Safety Notes: Preserve original documents and failed-export artifacts where useful.
- References / Context: Export/import service stack
- Example scenarios where useful: Exporting a document with mermaid and math clearly reports whether it rendered fully or used defined degraded fallbacks.

### Task P16-T03

- Phase ID: `P16`
- Task ID: `P16-T03`
- Task Title: Complete media, image, and embedded object placeholder paths
- Priority: `P1`
- Category: `Placeholder / Stub Removal`
- Objective: Remove placeholder rendering for visible media flows where the product implies real support.
- Why This Matters Now: Media and embedded content placeholders still make the product feel unfinished.
- Execution Gap Statement: Several renderers and canvas/media surfaces still fall back to placeholder rectangles or synthetic metadata.
- User / Product Impact: Users cannot trust visual content fidelity.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/WidgetRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.h`
- Prior Plan References: `v22` Phase 14-16, `v23` Phase 13
- Scope: Embedded media metadata, image placeholders, widget rendering fallback paths
- Out of Scope: Entirely new media providers
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h`
- Related Features / Systems / Components: Media rendering, metadata, embeds
- Current Behavior: Some media flows still rely on placeholder rendering.
- Intended Completed Behavior: Media support either renders correctly or communicates explicit unsupported states without pretending to be done.
- Missing Pieces: Real metadata retrieval, widget rendering, media fallback policy
- Technical Approach: Replace synthetic metadata and placeholder renderers with real service hooks or clear gating.
- Implementation Steps: Implement metadata fetch service; complete widget render path; audit all media placeholder branches.
- Validation Steps: Load embedded media and image-heavy documents/canvases and verify rendering fidelity.
- Acceptance Criteria: Production media surfaces no longer default to placeholder renderers when support is claimed.
- Dependencies: `P09-T04`, `P15-T02`
- Parallelization Notes: Can proceed with rendering audits and UI quality work.
- Risks / Failure Modes: External content and network dependencies can destabilize render consistency.
- Cleanup / Migration Notes: Remove placeholder HTML and dummy metadata paths once real support exists.
- Observability / Diagnostics Notes: Emit embed load, metadata source, and fallback reason diagnostics.
- Rollback / Safety Notes: Prefer explicit unsupported states over silent placeholder rendering.
- References / Context: Canvas/media rendering files
- Example scenarios where useful: A bookmark card uses real scraped metadata instead of synthetic placeholder HTML.

### Task P16-T04

- Phase ID: `P16`
- Task ID: `P16-T04`
- Task Title: Finish print and publish output fidelity on top of completed rendering services
- Priority: `P2`
- Category: `Core Workflow Completion`
- Objective: Ensure print/publish paths reflect the final rendering behavior and not older placeholder assumptions.
- Why This Matters Now: Output fidelity matters for a product that already exposes export and publishing surfaces.
- Execution Gap Statement: Print/export fidelity work depends on earlier rendering and content completion.
- User / Product Impact: Final output can diverge from what users see in-app.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/PublishingProfileManager.h`, `/Users/ryanrentfro/code/markamp/src/core/PdfExporter.h`
- Prior Plan References: `v23` Phase 13
- Scope: Print, publish profiles, PDF/HTML output parity, validation
- Out of Scope: Online publishing services beyond current scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SlideExporter.h`
- Related Features / Systems / Components: Printing, publishing, export validation
- Current Behavior: Output features exist but final fidelity must follow renderer completion.
- Intended Completed Behavior: Print and publish outputs match supported in-app rendering behavior within defined fidelity bounds.
- Missing Pieces: Final parity rules, profile validation, regression fixtures
- Technical Approach: Revalidate output pipelines after core renderers and preview paths are completed.
- Implementation Steps: Compare output to preview; fix fidelity gaps; update validation and publishing profiles.
- Validation Steps: Generate output from representative documents and compare against approved baselines.
- Acceptance Criteria: Print and publish outputs no longer depend on outdated placeholder assumptions.
- Dependencies: `P16-T01` through `P16-T03`
- Parallelization Notes: Follows core renderer and PDF/media completion.
- Risks / Failure Modes: External toolchains can still create output drift.
- Cleanup / Migration Notes: Retire format-specific workaround paths that compensate for old placeholder behavior.
- Observability / Diagnostics Notes: Emit output-fidelity validation results and external dependency state.
- Rollback / Safety Notes: Preserve prior output baselines during migration.
- References / Context: Exporters and publishing profile manager
- Example scenarios where useful: Printed output preserves syntax colors and diagrams within defined theme/export rules.

### Task P16-T05

- Phase ID: `P16`
- Task ID: `P16-T05`
- Task Title: Add rendering and specialized-surface readiness gates
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Prevent placeholder renderers and specialized-panel stubs from slipping back into release.
- Why This Matters Now: Rendering and specialized surfaces are broad and easy to regress.
- Execution Gap Statement: Placeholder behavior is currently tolerated too often in media and specialized-surface flows.
- User / Product Impact: Content fidelity regressions erode trust quickly.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SpecializedSurfaceCompletionAuditor.h`
- Prior Plan References: `v23` Phase 08, 13, 19
- Scope: PDF readiness, preview fallback audits, import/export validation baselines
- Out of Scope: High-end performance benchmarking of all media scenarios
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp`
- Related Features / Systems / Components: Specialized surfaces, rendering QA, visual regression
- Current Behavior: Gate coverage is incomplete for these content-heavy surfaces.
- Intended Completed Behavior: Specialized surface and render-fidelity regressions fail validation before release.
- Missing Pieces: Baselines, fixtures, gate wiring
- Technical Approach: Add readiness audits and fixture-driven output comparisons for specialized surfaces.
- Implementation Steps: Build PDF/media fixtures; add render audits; wire to CI and release checks.
- Validation Steps: Introduce placeholder or degraded render regressions and verify gate failure.
- Acceptance Criteria: Placeholder specialized surfaces and fidelity regressions are caught automatically.
- Dependencies: `P16-T01` through `P16-T04`
- Parallelization Notes: Can scaffold alongside implementation.
- Risks / Failure Modes: Baseline drift can create false positives if approval flow is weak.
- Cleanup / Migration Notes: Remove manual-only signoff for these surfaces where automated gates exist.
- Observability / Diagnostics Notes: Publish fixture diffs and specialized-surface readiness summaries.
- Rollback / Safety Notes: Version baselines and audit thresholds carefully.
- References / Context: Visual regression and specialized-surface audit tools
- Example scenarios where useful: A PDF viewer reverting to simulated pages causes the readiness gate to fail immediately.
