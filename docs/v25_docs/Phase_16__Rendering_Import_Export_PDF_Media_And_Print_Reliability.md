# Phase 16: Rendering, Import, Export, PDF, Media, And Print Reliability

## Phase Intent

Finish the fidelity and workflow gaps that still affect visible document handling on the release path.

## Release-Ready Exit Criteria

- release-path rendering degrades honestly and predictably,
- import/export/PDF/media workflows that remain visible are trustworthy,
- no visible rendering or document-conversion flow relies on silent placeholder success.

## Task Count

3

## Task P16-T01

- Phase ID: P16
- Task ID: P16-T01
- Task Title: Close unacceptable placeholder rendering branches on release-path content
- Priority: P1
- Category: Rendering / Import / Export
- Objective: distinguish acceptable graceful degradation from unacceptable placeholder success and remove the latter from visible release workflows.
- Why This Matters Now: users can tolerate clear unsupported states, but not misleading rendered output.
- Release Gap Statement: rendering code still contains placeholder and unavailable branches that need explicit release-scope decisions.
- User / Product Impact: content fidelity and trust are central to an editor/notebook application.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`
- Prior Plan References: `v23 Phase 13`; `v24 Phase 16`
- Scope: Markdown preview, Mermaid, embeds, math and media paths that remain on the release path.
- Out of Scope: unsupported future renderers not exposed to users.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/rendering/CodeBlockRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/rendering/EmbedBlockRenderer.cpp`
- Related Features / Systems / Components: preview; export; rendering; diagnostics
- Current Behavior: placeholders can still stand in for real rendering in visible flows.
- Intended Release-Ready Behavior: supported content renders; unsupported content fails honestly and observably.
- Missing Pieces: supported-renderer matrix; fallback policy; user-facing diagnostics.
- Technical Approach: classify renderer paths by release support and enforce explicit behavior for each class.
- Implementation Steps:
1. Inventory visible placeholder rendering branches.
2. Decide keep-as-explicit-fallback versus must-finish.
3. Add renderer capability reporting and tests.
- Validation Steps:
1. Render representative supported and unsupported content.
2. Confirm visible behavior matches release policy.
- Acceptance Criteria: no release-path rendering branch silently pretends completion through placeholder output.
- Dependencies: P06-T02
- Parallelization Notes: PDF/media panel work can proceed in parallel.
- Risks / Failure Modes: over-gating useful fallbacks; under-reporting unsupported cases.
- Observability / Diagnostics Notes: capture renderer selection and unsupported-feature reasons.
- Rollback / Safety Notes: prefer explicit textual fallback to misleading pseudo-rendering.
- References / Context: `/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlockRenderer.cpp`

## Task P16-T02

- Phase ID: P16
- Task ID: P16-T02
- Task Title: Finish or gate PDF, media, and print flows on the visible release path
- Priority: P1
- Category: Release Readiness
- Objective: ensure document viewing and export surfaces exposed in the shell have backing depth proportional to their visibility.
- Why This Matters Now: PDF and media surfaces are visible trust surfaces and still show stub indicators.
- Release Gap Statement: PDF-related panels remain stubbed and media/rendering fidelity varies.
- User / Product Impact: users will assume these surfaces are product commitments if they remain visible.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PdfViewerService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExportService.cpp`
- Prior Plan References: `v23 Phase 13`; `v24 Phase 16`
- Scope: PDF view, thumbnails, annotations visibility, print/export entry points, media embeds that remain exposed.
- Out of Scope: advanced annotation feature expansion beyond release support.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PDFThumbnailStrip.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFAnnotationSidebar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp`
- Related Features / Systems / Components: PDF; print; export; panel readiness
- Current Behavior: some PDF and media surfaces remain explicit stubs or partials.
- Intended Release-Ready Behavior: only supported PDF/media/print flows remain visible and dependable.
- Missing Pieces: supported-scope definition; panel gating; flow validation.
- Technical Approach: either complete the minimum release-path PDF/media flows or remove them from the visible shell.
- Implementation Steps:
1. Classify visible PDF/media/print features as must-finish or gated.
2. Complete must-have data paths and panel behavior.
3. Remove unsupported surface entry points from release scope.
- Validation Steps:
1. Open supported PDF/media flows and verify core interactions.
2. Trigger print/export on supported document types.
- Acceptance Criteria: no visible PDF/media/print feature remains obviously stubbed on the release path.
- Dependencies: P05-T01; P16-T01
- Parallelization Notes: can be divided by PDF and export/media owners.
- Risks / Failure Modes: UI affordances survive after gating; incomplete service depth.
- Release Notes / Cleanup Notes: release scope must explicitly list gated document features.
- Observability / Diagnostics Notes: add open/export failure logging and panel readiness reports.
- Rollback / Safety Notes: hide unsupported features rather than shipping partial shells.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`

## Task P16-T03

- Phase ID: P16
- Task ID: P16-T03
- Task Title: Add import/export fidelity and renderer regression checks for supported flows
- Priority: P1
- Category: Testing / Regression Protection
- Objective: ensure supported document conversion and rendering paths do not silently regress late in the release wave.
- Why This Matters Now: conversion and rendering bugs often escape until release without targeted regression gates.
- Release Gap Statement: visible conversion fidelity still lacks enough release-focused regression proof.
- User / Product Impact: broken export or corrupted import directly damages trust and data portability.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ExportService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp`; `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp`
- Prior Plan References: `v24 Phase 16`; `v24 Phase 19`
- Scope: supported Markdown/HTML/PDF/media import-export and preview fidelity checks.
- Out of Scope: unsupported format breadth.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/testing/ThemeCoverageChecker.cpp`; `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.h`
- Related Features / Systems / Components: exporters; importers; visual regression; smoke tests
- Current Behavior: regression infrastructure exists, but fidelity gates still need stronger release-path focus.
- Intended Release-Ready Behavior: supported conversion and rendering paths have explicit regression evidence.
- Missing Pieces: supported-flow matrix; representative fixtures; gate integration.
- Technical Approach: create a focused conversion/rendering regression suite rather than broad format expansion.
- Implementation Steps:
1. Define representative supported document fixtures.
2. Add import/export and preview checks for those fixtures.
3. Tie failures into release signoff.
- Validation Steps:
1. Run the conversion suite on supported formats.
2. Verify meaningful diffs and failure diagnostics.
- Acceptance Criteria: supported render/import/export flows have repeatable regression evidence.
- Dependencies: P01-T03; P16-T01; P16-T02
- Parallelization Notes: low-conflict validation track.
- Risks / Failure Modes: brittle snapshot tests; missing fixture coverage.
- Observability / Diagnostics Notes: store artifact-based fidelity reports and diff summaries.
- Rollback / Safety Notes: use representative high-signal fixtures only.
- References / Context: `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp`
