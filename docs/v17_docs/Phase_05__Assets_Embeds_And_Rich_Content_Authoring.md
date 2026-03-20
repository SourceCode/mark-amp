# Phase 05: Assets Embeds And Rich Content Authoring

## Objective
Strengthen asset handling so images, files, embeds, PDFs, links, icons, and widgets behave like first-class canvas materials.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Image Placement: Make image insertion, scaling, and replacement intuitive for reference-heavy boards.
- W02 Video And Web Embeds: Support rich external content without destabilizing the board shell.
- W03 PDF Pages: Treat PDFs as editable board materials instead of opaque file blobs.
- W04 Bookmark Cards: Turn raw URLs into useful reference objects with preview quality and safe behavior.
- W05 App Widgets: Define a consistent UX contract for interactive widgets on the board.
- W06 Icon Insertion: Make icon use faster than importing arbitrary SVGs for common needs.
- W07 Drag And Drop Intake: Make external content ingestion predictable regardless of source application.
- W08 Clipboard Ingest: Support rich pastes from browsers, office tools, and screenshots.
- W09 Media Optimization: Balance quality and performance for boards that carry many assets.
- W10 Cropping And Masking: Support the basic media editing affordances users expect inside a board.
- W11 Captions And Metadata: Expose source, alt text, owner, and caption metadata where it matters.
- W12 Asset Library Panel: Give frequent creators a reusable asset shelf instead of repeated file picking.
- W13 Asset Styling: Let assets adopt frames, shadows, borders, and presentation treatments quickly.
- W14 Mixed Media Boards: Ensure text, media, and shapes compose cleanly in presentations and working boards.
- W15 Import And Convert: Offer better conversion paths from common external artifacts into editable canvas content.
- W16 Media Review Flows: Support review-centric actions on rich assets without leaving the board.
- W17 Licensing And Security: Handle external content safely while preserving expected workflows.
- W18 Asset Serialization And Sync: Preserve references, thumbnails, and metadata across save and sync paths.
- W19 Rich Content Export: Ensure rich objects export clearly to image, PDF, and HTML targets.
- W20 Asset Test Coverage: Build coverage for insertion, rendering, persistence, and failure handling across asset types.

## Workstream 01: Image Placement
Make image insertion, scaling, and replacement intuitive for reference-heavy boards.

### P05-T001: Image Placement UX Parity Audit
**Task Title:** Image Placement UX Parity Audit
**Definition:** Define the implementation contract for image placement by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make image insertion, scaling, and replacement intuitive for reference-heavy boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T002: Image Placement Domain Model And State Contract
**Task Title:** Image Placement Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support image placement.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make image insertion, scaling, and replacement intuitive for reference-heavy boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T003: Image Placement Commands Events And Context Keys
**Task Title:** Image Placement Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for image placement.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make image insertion, scaling, and replacement intuitive for reference-heavy boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T004: Image Placement Workspace Surface And Controls
**Task Title:** Image Placement Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose image placement in the main canvas workspace.
**Description:** Build the primary UI surfaces for make image insertion, scaling, and replacement intuitive for reference-heavy boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T005: Image Placement Pointer Interaction Flow
**Task Title:** Image Placement Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for image placement.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make image insertion, scaling, and replacement intuitive for reference-heavy boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T006: Image Placement Keyboard Gesture And Shortcut Flow
**Task Title:** Image Placement Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for image placement.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make image insertion, scaling, and replacement intuitive for reference-heavy boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T007: Image Placement Visual Feedback And Rendering Polish
**Task Title:** Image Placement Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make image placement legible in use.
**Description:** Implement the visible feedback for make image insertion, scaling, and replacement intuitive for reference-heavy boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T008: Image Placement Persistence Preferences And Serialization
**Task Title:** Image Placement Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by image placement.
**Description:** Identify what parts of make image insertion, scaling, and replacement intuitive for reference-heavy boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T009: Image Placement Safeguards Telemetry And Recovery
**Task Title:** Image Placement Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for image placement.
**Description:** Instrument make image insertion, scaling, and replacement intuitive for reference-heavy boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

### P05-T010: Image Placement Tests Documentation And Rollout Gate
**Task Title:** Image Placement Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship image placement.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make image insertion, scaling, and replacement intuitive for reference-heavy boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** drag image onto canvas; replace image while keeping size; double-click to open crop.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/ImageAssetModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ImagePreviewToolbar.cpp

## Workstream 02: Video And Web Embeds
Support rich external content without destabilizing the board shell.

### P05-T011: Video And Web Embeds UX Parity Audit
**Task Title:** Video And Web Embeds UX Parity Audit
**Definition:** Define the implementation contract for video and web embeds by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support rich external content without destabilizing the board shell.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T012: Video And Web Embeds Domain Model And State Contract
**Task Title:** Video And Web Embeds Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support video and web embeds.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support rich external content without destabilizing the board shell.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T013: Video And Web Embeds Commands Events And Context Keys
**Task Title:** Video And Web Embeds Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for video and web embeds.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support rich external content without destabilizing the board shell. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T014: Video And Web Embeds Workspace Surface And Controls
**Task Title:** Video And Web Embeds Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose video and web embeds in the main canvas workspace.
**Description:** Build the primary UI surfaces for support rich external content without destabilizing the board shell., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T015: Video And Web Embeds Pointer Interaction Flow
**Task Title:** Video And Web Embeds Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for video and web embeds.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support rich external content without destabilizing the board shell.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T016: Video And Web Embeds Keyboard Gesture And Shortcut Flow
**Task Title:** Video And Web Embeds Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for video and web embeds.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support rich external content without destabilizing the board shell.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T017: Video And Web Embeds Visual Feedback And Rendering Polish
**Task Title:** Video And Web Embeds Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make video and web embeds legible in use.
**Description:** Implement the visible feedback for support rich external content without destabilizing the board shell.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T018: Video And Web Embeds Persistence Preferences And Serialization
**Task Title:** Video And Web Embeds Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by video and web embeds.
**Description:** Identify what parts of support rich external content without destabilizing the board shell. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T019: Video And Web Embeds Safeguards Telemetry And Recovery
**Task Title:** Video And Web Embeds Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for video and web embeds.
**Description:** Instrument support rich external content without destabilizing the board shell. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

### P05-T020: Video And Web Embeds Tests Documentation And Rollout Gate
**Task Title:** Video And Web Embeds Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship video and web embeds.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support rich external content without destabilizing the board shell.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** paste video URL; preview embed card; open inline or external.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.h
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp

## Workstream 03: PDF Pages
Treat PDFs as editable board materials instead of opaque file blobs.

### P05-T021: PDF Pages UX Parity Audit
**Task Title:** PDF Pages UX Parity Audit
**Definition:** Define the implementation contract for pdf pages by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around treat pdfs as editable board materials instead of opaque file blobs.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T022: PDF Pages Domain Model And State Contract
**Task Title:** PDF Pages Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support pdf pages.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around treat pdfs as editable board materials instead of opaque file blobs.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T023: PDF Pages Commands Events And Context Keys
**Task Title:** PDF Pages Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for pdf pages.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make treat pdfs as editable board materials instead of opaque file blobs. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T024: PDF Pages Workspace Surface And Controls
**Task Title:** PDF Pages Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose pdf pages in the main canvas workspace.
**Description:** Build the primary UI surfaces for treat pdfs as editable board materials instead of opaque file blobs., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T025: PDF Pages Pointer Interaction Flow
**Task Title:** PDF Pages Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for pdf pages.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for treat pdfs as editable board materials instead of opaque file blobs.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T026: PDF Pages Keyboard Gesture And Shortcut Flow
**Task Title:** PDF Pages Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for pdf pages.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for treat pdfs as editable board materials instead of opaque file blobs.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T027: PDF Pages Visual Feedback And Rendering Polish
**Task Title:** PDF Pages Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make pdf pages legible in use.
**Description:** Implement the visible feedback for treat pdfs as editable board materials instead of opaque file blobs.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T028: PDF Pages Persistence Preferences And Serialization
**Task Title:** PDF Pages Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by pdf pages.
**Description:** Identify what parts of treat pdfs as editable board materials instead of opaque file blobs. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T029: PDF Pages Safeguards Telemetry And Recovery
**Task Title:** PDF Pages Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for pdf pages.
**Description:** Instrument treat pdfs as editable board materials instead of opaque file blobs. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

### P05-T030: PDF Pages Tests Documentation And Rollout Gate
**Task Title:** PDF Pages Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship pdf pages.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for treat pdfs as editable board materials instead of opaque file blobs.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** insert page range; resize page thumbnail; open source PDF.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PdfObjectModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.cpp

## Workstream 04: Bookmark Cards
Turn raw URLs into useful reference objects with preview quality and safe behavior.

### P05-T031: Bookmark Cards UX Parity Audit
**Task Title:** Bookmark Cards UX Parity Audit
**Definition:** Define the implementation contract for bookmark cards by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn raw urls into useful reference objects with preview quality and safe behavior.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T032: Bookmark Cards Domain Model And State Contract
**Task Title:** Bookmark Cards Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support bookmark cards.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn raw urls into useful reference objects with preview quality and safe behavior.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T033: Bookmark Cards Commands Events And Context Keys
**Task Title:** Bookmark Cards Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for bookmark cards.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn raw urls into useful reference objects with preview quality and safe behavior. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T034: Bookmark Cards Workspace Surface And Controls
**Task Title:** Bookmark Cards Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose bookmark cards in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn raw urls into useful reference objects with preview quality and safe behavior., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T035: Bookmark Cards Pointer Interaction Flow
**Task Title:** Bookmark Cards Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for bookmark cards.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn raw urls into useful reference objects with preview quality and safe behavior.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T036: Bookmark Cards Keyboard Gesture And Shortcut Flow
**Task Title:** Bookmark Cards Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for bookmark cards.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn raw urls into useful reference objects with preview quality and safe behavior.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T037: Bookmark Cards Visual Feedback And Rendering Polish
**Task Title:** Bookmark Cards Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make bookmark cards legible in use.
**Description:** Implement the visible feedback for turn raw urls into useful reference objects with preview quality and safe behavior.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T038: Bookmark Cards Persistence Preferences And Serialization
**Task Title:** Bookmark Cards Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by bookmark cards.
**Description:** Identify what parts of turn raw urls into useful reference objects with preview quality and safe behavior. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T039: Bookmark Cards Safeguards Telemetry And Recovery
**Task Title:** Bookmark Cards Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for bookmark cards.
**Description:** Instrument turn raw urls into useful reference objects with preview quality and safe behavior. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

### P05-T040: Bookmark Cards Tests Documentation And Rollout Gate
**Task Title:** Bookmark Cards Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship bookmark cards.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn raw urls into useful reference objects with preview quality and safe behavior.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** auto-preview pasted URL; editable title and summary; favicon and domain chip.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.cpp

## Workstream 05: App Widgets
Define a consistent UX contract for interactive widgets on the board.

### P05-T041: App Widgets UX Parity Audit
**Task Title:** App Widgets UX Parity Audit
**Definition:** Define the implementation contract for app widgets by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around define a consistent ux contract for interactive widgets on the board.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T042: App Widgets Domain Model And State Contract
**Task Title:** App Widgets Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support app widgets.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around define a consistent ux contract for interactive widgets on the board.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T043: App Widgets Commands Events And Context Keys
**Task Title:** App Widgets Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for app widgets.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make define a consistent ux contract for interactive widgets on the board. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T044: App Widgets Workspace Surface And Controls
**Task Title:** App Widgets Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose app widgets in the main canvas workspace.
**Description:** Build the primary UI surfaces for define a consistent ux contract for interactive widgets on the board., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T045: App Widgets Pointer Interaction Flow
**Task Title:** App Widgets Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for app widgets.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for define a consistent ux contract for interactive widgets on the board.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T046: App Widgets Keyboard Gesture And Shortcut Flow
**Task Title:** App Widgets Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for app widgets.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for define a consistent ux contract for interactive widgets on the board.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T047: App Widgets Visual Feedback And Rendering Polish
**Task Title:** App Widgets Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make app widgets legible in use.
**Description:** Implement the visible feedback for define a consistent ux contract for interactive widgets on the board.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T048: App Widgets Persistence Preferences And Serialization
**Task Title:** App Widgets Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by app widgets.
**Description:** Identify what parts of define a consistent ux contract for interactive widgets on the board. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T049: App Widgets Safeguards Telemetry And Recovery
**Task Title:** App Widgets Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for app widgets.
**Description:** Instrument define a consistent ux contract for interactive widgets on the board. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

### P05-T050: App Widgets Tests Documentation And Rollout Gate
**Task Title:** App Widgets Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship app widgets.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for define a consistent ux contract for interactive widgets on the board.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** widget insert menu; resize live widget; widget loading state.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasAppManifest.h
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.h
- /Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp

## Workstream 06: Icon Insertion
Make icon use faster than importing arbitrary SVGs for common needs.

### P05-T051: Icon Insertion UX Parity Audit
**Task Title:** Icon Insertion UX Parity Audit
**Definition:** Define the implementation contract for icon insertion by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make icon use faster than importing arbitrary svgs for common needs.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T052: Icon Insertion Domain Model And State Contract
**Task Title:** Icon Insertion Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support icon insertion.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make icon use faster than importing arbitrary svgs for common needs.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T053: Icon Insertion Commands Events And Context Keys
**Task Title:** Icon Insertion Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for icon insertion.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make icon use faster than importing arbitrary svgs for common needs. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T054: Icon Insertion Workspace Surface And Controls
**Task Title:** Icon Insertion Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose icon insertion in the main canvas workspace.
**Description:** Build the primary UI surfaces for make icon use faster than importing arbitrary svgs for common needs., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T055: Icon Insertion Pointer Interaction Flow
**Task Title:** Icon Insertion Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for icon insertion.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make icon use faster than importing arbitrary svgs for common needs.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T056: Icon Insertion Keyboard Gesture And Shortcut Flow
**Task Title:** Icon Insertion Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for icon insertion.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make icon use faster than importing arbitrary svgs for common needs.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T057: Icon Insertion Visual Feedback And Rendering Polish
**Task Title:** Icon Insertion Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make icon insertion legible in use.
**Description:** Implement the visible feedback for make icon use faster than importing arbitrary svgs for common needs.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T058: Icon Insertion Persistence Preferences And Serialization
**Task Title:** Icon Insertion Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by icon insertion.
**Description:** Identify what parts of make icon use faster than importing arbitrary svgs for common needs. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T059: Icon Insertion Safeguards Telemetry And Recovery
**Task Title:** Icon Insertion Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for icon insertion.
**Description:** Instrument make icon use faster than importing arbitrary svgs for common needs. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

### P05-T060: Icon Insertion Tests Documentation And Rollout Gate
**Task Title:** Icon Insertion Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship icon insertion.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make icon use faster than importing arbitrary svgs for common needs.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** search icon set; insert with theme color; swap icon in place.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.h

## Workstream 07: Drag And Drop Intake
Make external content ingestion predictable regardless of source application.

### P05-T061: Drag And Drop Intake UX Parity Audit
**Task Title:** Drag And Drop Intake UX Parity Audit
**Definition:** Define the implementation contract for drag and drop intake by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make external content ingestion predictable regardless of source application.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T062: Drag And Drop Intake Domain Model And State Contract
**Task Title:** Drag And Drop Intake Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support drag and drop intake.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make external content ingestion predictable regardless of source application.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T063: Drag And Drop Intake Commands Events And Context Keys
**Task Title:** Drag And Drop Intake Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for drag and drop intake.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make external content ingestion predictable regardless of source application. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T064: Drag And Drop Intake Workspace Surface And Controls
**Task Title:** Drag And Drop Intake Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose drag and drop intake in the main canvas workspace.
**Description:** Build the primary UI surfaces for make external content ingestion predictable regardless of source application., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T065: Drag And Drop Intake Pointer Interaction Flow
**Task Title:** Drag And Drop Intake Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for drag and drop intake.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make external content ingestion predictable regardless of source application.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T066: Drag And Drop Intake Keyboard Gesture And Shortcut Flow
**Task Title:** Drag And Drop Intake Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for drag and drop intake.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make external content ingestion predictable regardless of source application.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T067: Drag And Drop Intake Visual Feedback And Rendering Polish
**Task Title:** Drag And Drop Intake Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make drag and drop intake legible in use.
**Description:** Implement the visible feedback for make external content ingestion predictable regardless of source application.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T068: Drag And Drop Intake Persistence Preferences And Serialization
**Task Title:** Drag And Drop Intake Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by drag and drop intake.
**Description:** Identify what parts of make external content ingestion predictable regardless of source application. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T069: Drag And Drop Intake Safeguards Telemetry And Recovery
**Task Title:** Drag And Drop Intake Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for drag and drop intake.
**Description:** Instrument make external content ingestion predictable regardless of source application. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P05-T070: Drag And Drop Intake Tests Documentation And Rollout Gate
**Task Title:** Drag And Drop Intake Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship drag and drop intake.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make external content ingestion predictable regardless of source application.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** drop image file; drop markdown snippet; drop URL from browser.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DropTargetHighlighter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

## Workstream 08: Clipboard Ingest
Support rich pastes from browsers, office tools, and screenshots.

### P05-T071: Clipboard Ingest UX Parity Audit
**Task Title:** Clipboard Ingest UX Parity Audit
**Definition:** Define the implementation contract for clipboard ingest by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support rich pastes from browsers, office tools, and screenshots.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T072: Clipboard Ingest Domain Model And State Contract
**Task Title:** Clipboard Ingest Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support clipboard ingest.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support rich pastes from browsers, office tools, and screenshots.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T073: Clipboard Ingest Commands Events And Context Keys
**Task Title:** Clipboard Ingest Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for clipboard ingest.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support rich pastes from browsers, office tools, and screenshots. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T074: Clipboard Ingest Workspace Surface And Controls
**Task Title:** Clipboard Ingest Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose clipboard ingest in the main canvas workspace.
**Description:** Build the primary UI surfaces for support rich pastes from browsers, office tools, and screenshots., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T075: Clipboard Ingest Pointer Interaction Flow
**Task Title:** Clipboard Ingest Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for clipboard ingest.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support rich pastes from browsers, office tools, and screenshots.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T076: Clipboard Ingest Keyboard Gesture And Shortcut Flow
**Task Title:** Clipboard Ingest Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for clipboard ingest.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support rich pastes from browsers, office tools, and screenshots.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T077: Clipboard Ingest Visual Feedback And Rendering Polish
**Task Title:** Clipboard Ingest Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make clipboard ingest legible in use.
**Description:** Implement the visible feedback for support rich pastes from browsers, office tools, and screenshots.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T078: Clipboard Ingest Persistence Preferences And Serialization
**Task Title:** Clipboard Ingest Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by clipboard ingest.
**Description:** Identify what parts of support rich pastes from browsers, office tools, and screenshots. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T079: Clipboard Ingest Safeguards Telemetry And Recovery
**Task Title:** Clipboard Ingest Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for clipboard ingest.
**Description:** Instrument support rich pastes from browsers, office tools, and screenshots. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

### P05-T080: Clipboard Ingest Tests Documentation And Rollout Gate
**Task Title:** Clipboard Ingest Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship clipboard ingest.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support rich pastes from browsers, office tools, and screenshots.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** paste screenshot; paste rich text as card; paste spreadsheet range.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardSanitizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.h

## Workstream 09: Media Optimization
Balance quality and performance for boards that carry many assets.

### P05-T081: Media Optimization UX Parity Audit
**Task Title:** Media Optimization UX Parity Audit
**Definition:** Define the implementation contract for media optimization by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around balance quality and performance for boards that carry many assets.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T082: Media Optimization Domain Model And State Contract
**Task Title:** Media Optimization Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support media optimization.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around balance quality and performance for boards that carry many assets.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T083: Media Optimization Commands Events And Context Keys
**Task Title:** Media Optimization Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for media optimization.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make balance quality and performance for boards that carry many assets. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T084: Media Optimization Workspace Surface And Controls
**Task Title:** Media Optimization Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose media optimization in the main canvas workspace.
**Description:** Build the primary UI surfaces for balance quality and performance for boards that carry many assets., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T085: Media Optimization Pointer Interaction Flow
**Task Title:** Media Optimization Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for media optimization.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for balance quality and performance for boards that carry many assets.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T086: Media Optimization Keyboard Gesture And Shortcut Flow
**Task Title:** Media Optimization Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for media optimization.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for balance quality and performance for boards that carry many assets.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T087: Media Optimization Visual Feedback And Rendering Polish
**Task Title:** Media Optimization Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make media optimization legible in use.
**Description:** Implement the visible feedback for balance quality and performance for boards that carry many assets.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T088: Media Optimization Persistence Preferences And Serialization
**Task Title:** Media Optimization Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by media optimization.
**Description:** Identify what parts of balance quality and performance for boards that carry many assets. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T089: Media Optimization Safeguards Telemetry And Recovery
**Task Title:** Media Optimization Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for media optimization.
**Description:** Instrument balance quality and performance for boards that carry many assets. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

### P05-T090: Media Optimization Tests Documentation And Rollout Gate
**Task Title:** Media Optimization Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship media optimization.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for balance quality and performance for boards that carry many assets.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** deferred thumbnail generation; lazy load offscreen media; image compression on save.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetOptimizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp

## Workstream 10: Cropping And Masking
Support the basic media editing affordances users expect inside a board.

### P05-T091: Cropping And Masking UX Parity Audit
**Task Title:** Cropping And Masking UX Parity Audit
**Definition:** Define the implementation contract for cropping and masking by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support the basic media editing affordances users expect inside a board.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T092: Cropping And Masking Domain Model And State Contract
**Task Title:** Cropping And Masking Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support cropping and masking.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support the basic media editing affordances users expect inside a board.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T093: Cropping And Masking Commands Events And Context Keys
**Task Title:** Cropping And Masking Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for cropping and masking.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support the basic media editing affordances users expect inside a board. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T094: Cropping And Masking Workspace Surface And Controls
**Task Title:** Cropping And Masking Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose cropping and masking in the main canvas workspace.
**Description:** Build the primary UI surfaces for support the basic media editing affordances users expect inside a board., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T095: Cropping And Masking Pointer Interaction Flow
**Task Title:** Cropping And Masking Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for cropping and masking.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support the basic media editing affordances users expect inside a board.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T096: Cropping And Masking Keyboard Gesture And Shortcut Flow
**Task Title:** Cropping And Masking Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for cropping and masking.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support the basic media editing affordances users expect inside a board.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T097: Cropping And Masking Visual Feedback And Rendering Polish
**Task Title:** Cropping And Masking Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make cropping and masking legible in use.
**Description:** Implement the visible feedback for support the basic media editing affordances users expect inside a board.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T098: Cropping And Masking Persistence Preferences And Serialization
**Task Title:** Cropping And Masking Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by cropping and masking.
**Description:** Identify what parts of support the basic media editing affordances users expect inside a board. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T099: Cropping And Masking Safeguards Telemetry And Recovery
**Task Title:** Cropping And Masking Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for cropping and masking.
**Description:** Instrument support the basic media editing affordances users expect inside a board. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P05-T100: Cropping And Masking Tests Documentation And Rollout Gate
**Task Title:** Cropping And Masking Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship cropping and masking.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support the basic media editing affordances users expect inside a board.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** crop image; round mask avatar; fit/fill image modes.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

## Workstream 11: Captions And Metadata
Expose source, alt text, owner, and caption metadata where it matters.

### P05-T101: Captions And Metadata UX Parity Audit
**Task Title:** Captions And Metadata UX Parity Audit
**Definition:** Define the implementation contract for captions and metadata by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose source, alt text, owner, and caption metadata where it matters.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** image caption; embed source link; asset alt text.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T102: Captions And Metadata Domain Model And State Contract
**Task Title:** Captions And Metadata Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support captions and metadata.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose source, alt text, owner, and caption metadata where it matters.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** image caption; embed source link; asset alt text.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T103: Captions And Metadata Commands Events And Context Keys
**Task Title:** Captions And Metadata Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for captions and metadata.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose source, alt text, owner, and caption metadata where it matters. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** image caption; embed source link; asset alt text.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T104: Captions And Metadata Workspace Surface And Controls
**Task Title:** Captions And Metadata Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose captions and metadata in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose source, alt text, owner, and caption metadata where it matters., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** image caption; embed source link; asset alt text.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T105: Captions And Metadata Pointer Interaction Flow
**Task Title:** Captions And Metadata Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for captions and metadata.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose source, alt text, owner, and caption metadata where it matters.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** image caption; embed source link; asset alt text.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T106: Captions And Metadata Keyboard Gesture And Shortcut Flow
**Task Title:** Captions And Metadata Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for captions and metadata.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose source, alt text, owner, and caption metadata where it matters.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** image caption; embed source link; asset alt text.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T107: Captions And Metadata Visual Feedback And Rendering Polish
**Task Title:** Captions And Metadata Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make captions and metadata legible in use.
**Description:** Implement the visible feedback for expose source, alt text, owner, and caption metadata where it matters.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** image caption; embed source link; asset alt text.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T108: Captions And Metadata Persistence Preferences And Serialization
**Task Title:** Captions And Metadata Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by captions and metadata.
**Description:** Identify what parts of expose source, alt text, owner, and caption metadata where it matters. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** image caption; embed source link; asset alt text.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T109: Captions And Metadata Safeguards Telemetry And Recovery
**Task Title:** Captions And Metadata Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for captions and metadata.
**Description:** Instrument expose source, alt text, owner, and caption metadata where it matters. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** image caption; embed source link; asset alt text.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

### P05-T110: Captions And Metadata Tests Documentation And Rollout Gate
**Task Title:** Captions And Metadata Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship captions and metadata.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose source, alt text, owner, and caption metadata where it matters.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** image caption; embed source link; asset alt text.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetTypes.h
- /Users/ryanrentfro/code/markamp/src/core/AccessibilityManager.cpp

## Workstream 12: Asset Library Panel
Give frequent creators a reusable asset shelf instead of repeated file picking.

### P05-T111: Asset Library Panel UX Parity Audit
**Task Title:** Asset Library Panel UX Parity Audit
**Definition:** Define the implementation contract for asset library panel by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around give frequent creators a reusable asset shelf instead of repeated file picking.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T112: Asset Library Panel Domain Model And State Contract
**Task Title:** Asset Library Panel Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support asset library panel.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around give frequent creators a reusable asset shelf instead of repeated file picking.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T113: Asset Library Panel Commands Events And Context Keys
**Task Title:** Asset Library Panel Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for asset library panel.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make give frequent creators a reusable asset shelf instead of repeated file picking. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T114: Asset Library Panel Workspace Surface And Controls
**Task Title:** Asset Library Panel Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose asset library panel in the main canvas workspace.
**Description:** Build the primary UI surfaces for give frequent creators a reusable asset shelf instead of repeated file picking., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T115: Asset Library Panel Pointer Interaction Flow
**Task Title:** Asset Library Panel Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for asset library panel.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for give frequent creators a reusable asset shelf instead of repeated file picking.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T116: Asset Library Panel Keyboard Gesture And Shortcut Flow
**Task Title:** Asset Library Panel Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for asset library panel.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for give frequent creators a reusable asset shelf instead of repeated file picking.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T117: Asset Library Panel Visual Feedback And Rendering Polish
**Task Title:** Asset Library Panel Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make asset library panel legible in use.
**Description:** Implement the visible feedback for give frequent creators a reusable asset shelf instead of repeated file picking.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T118: Asset Library Panel Persistence Preferences And Serialization
**Task Title:** Asset Library Panel Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by asset library panel.
**Description:** Identify what parts of give frequent creators a reusable asset shelf instead of repeated file picking. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T119: Asset Library Panel Safeguards Telemetry And Recovery
**Task Title:** Asset Library Panel Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for asset library panel.
**Description:** Instrument give frequent creators a reusable asset shelf instead of repeated file picking. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

### P05-T120: Asset Library Panel Tests Documentation And Rollout Gate
**Task Title:** Asset Library Panel Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship asset library panel.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for give frequent creators a reusable asset shelf instead of repeated file picking.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** recent uploads; favorite icons; team image kit.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FileTreeModel.cpp

## Workstream 13: Asset Styling
Let assets adopt frames, shadows, borders, and presentation treatments quickly.

### P05-T121: Asset Styling UX Parity Audit
**Task Title:** Asset Styling UX Parity Audit
**Definition:** Define the implementation contract for asset styling by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around let assets adopt frames, shadows, borders, and presentation treatments quickly.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T122: Asset Styling Domain Model And State Contract
**Task Title:** Asset Styling Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support asset styling.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around let assets adopt frames, shadows, borders, and presentation treatments quickly.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T123: Asset Styling Commands Events And Context Keys
**Task Title:** Asset Styling Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for asset styling.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make let assets adopt frames, shadows, borders, and presentation treatments quickly. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T124: Asset Styling Workspace Surface And Controls
**Task Title:** Asset Styling Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose asset styling in the main canvas workspace.
**Description:** Build the primary UI surfaces for let assets adopt frames, shadows, borders, and presentation treatments quickly., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T125: Asset Styling Pointer Interaction Flow
**Task Title:** Asset Styling Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for asset styling.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for let assets adopt frames, shadows, borders, and presentation treatments quickly.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T126: Asset Styling Keyboard Gesture And Shortcut Flow
**Task Title:** Asset Styling Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for asset styling.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for let assets adopt frames, shadows, borders, and presentation treatments quickly.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T127: Asset Styling Visual Feedback And Rendering Polish
**Task Title:** Asset Styling Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make asset styling legible in use.
**Description:** Implement the visible feedback for let assets adopt frames, shadows, borders, and presentation treatments quickly.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T128: Asset Styling Persistence Preferences And Serialization
**Task Title:** Asset Styling Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by asset styling.
**Description:** Identify what parts of let assets adopt frames, shadows, borders, and presentation treatments quickly. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T129: Asset Styling Safeguards Telemetry And Recovery
**Task Title:** Asset Styling Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for asset styling.
**Description:** Instrument let assets adopt frames, shadows, borders, and presentation treatments quickly. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

### P05-T130: Asset Styling Tests Documentation And Rollout Gate
**Task Title:** Asset Styling Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship asset styling.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for let assets adopt frames, shadows, borders, and presentation treatments quickly.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** polaroid frame; device mock frame; image border presets.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp

## Workstream 14: Mixed Media Boards
Ensure text, media, and shapes compose cleanly in presentations and working boards.

### P05-T131: Mixed Media Boards UX Parity Audit
**Task Title:** Mixed Media Boards UX Parity Audit
**Definition:** Define the implementation contract for mixed media boards by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around ensure text, media, and shapes compose cleanly in presentations and working boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T132: Mixed Media Boards Domain Model And State Contract
**Task Title:** Mixed Media Boards Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support mixed media boards.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around ensure text, media, and shapes compose cleanly in presentations and working boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T133: Mixed Media Boards Commands Events And Context Keys
**Task Title:** Mixed Media Boards Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for mixed media boards.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make ensure text, media, and shapes compose cleanly in presentations and working boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T134: Mixed Media Boards Workspace Surface And Controls
**Task Title:** Mixed Media Boards Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose mixed media boards in the main canvas workspace.
**Description:** Build the primary UI surfaces for ensure text, media, and shapes compose cleanly in presentations and working boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T135: Mixed Media Boards Pointer Interaction Flow
**Task Title:** Mixed Media Boards Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for mixed media boards.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for ensure text, media, and shapes compose cleanly in presentations and working boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T136: Mixed Media Boards Keyboard Gesture And Shortcut Flow
**Task Title:** Mixed Media Boards Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for mixed media boards.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for ensure text, media, and shapes compose cleanly in presentations and working boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T137: Mixed Media Boards Visual Feedback And Rendering Polish
**Task Title:** Mixed Media Boards Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make mixed media boards legible in use.
**Description:** Implement the visible feedback for ensure text, media, and shapes compose cleanly in presentations and working boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T138: Mixed Media Boards Persistence Preferences And Serialization
**Task Title:** Mixed Media Boards Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by mixed media boards.
**Description:** Identify what parts of ensure text, media, and shapes compose cleanly in presentations and working boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T139: Mixed Media Boards Safeguards Telemetry And Recovery
**Task Title:** Mixed Media Boards Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for mixed media boards.
**Description:** Instrument ensure text, media, and shapes compose cleanly in presentations and working boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

### P05-T140: Mixed Media Boards Tests Documentation And Rollout Gate
**Task Title:** Mixed Media Boards Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship mixed media boards.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for ensure text, media, and shapes compose cleanly in presentations and working boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** captioned screenshot cluster; video with notes; PDF page with annotation.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.h

## Workstream 15: Import And Convert
Offer better conversion paths from common external artifacts into editable canvas content.

### P05-T141: Import And Convert UX Parity Audit
**Task Title:** Import And Convert UX Parity Audit
**Definition:** Define the implementation contract for import and convert by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around offer better conversion paths from common external artifacts into editable canvas content.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T142: Import And Convert Domain Model And State Contract
**Task Title:** Import And Convert Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support import and convert.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around offer better conversion paths from common external artifacts into editable canvas content.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T143: Import And Convert Commands Events And Context Keys
**Task Title:** Import And Convert Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for import and convert.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make offer better conversion paths from common external artifacts into editable canvas content. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T144: Import And Convert Workspace Surface And Controls
**Task Title:** Import And Convert Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose import and convert in the main canvas workspace.
**Description:** Build the primary UI surfaces for offer better conversion paths from common external artifacts into editable canvas content., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T145: Import And Convert Pointer Interaction Flow
**Task Title:** Import And Convert Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for import and convert.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for offer better conversion paths from common external artifacts into editable canvas content.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T146: Import And Convert Keyboard Gesture And Shortcut Flow
**Task Title:** Import And Convert Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for import and convert.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for offer better conversion paths from common external artifacts into editable canvas content.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T147: Import And Convert Visual Feedback And Rendering Polish
**Task Title:** Import And Convert Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make import and convert legible in use.
**Description:** Implement the visible feedback for offer better conversion paths from common external artifacts into editable canvas content.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T148: Import And Convert Persistence Preferences And Serialization
**Task Title:** Import And Convert Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by import and convert.
**Description:** Identify what parts of offer better conversion paths from common external artifacts into editable canvas content. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T149: Import And Convert Safeguards Telemetry And Recovery
**Task Title:** Import And Convert Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for import and convert.
**Description:** Instrument offer better conversion paths from common external artifacts into editable canvas content. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

### P05-T150: Import And Convert Tests Documentation And Rollout Gate
**Task Title:** Import And Convert Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship import and convert.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for offer better conversion paths from common external artifacts into editable canvas content.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** import slide as images; convert URL list to cards; turn markdown list into notes.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/DocumentImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/MarkdownImporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/HtmlImporter.cpp

## Workstream 16: Media Review Flows
Support review-centric actions on rich assets without leaving the board.

### P05-T151: Media Review Flows UX Parity Audit
**Task Title:** Media Review Flows UX Parity Audit
**Definition:** Define the implementation contract for media review flows by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support review-centric actions on rich assets without leaving the board.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T152: Media Review Flows Domain Model And State Contract
**Task Title:** Media Review Flows Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support media review flows.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support review-centric actions on rich assets without leaving the board.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T153: Media Review Flows Commands Events And Context Keys
**Task Title:** Media Review Flows Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for media review flows.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support review-centric actions on rich assets without leaving the board. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T154: Media Review Flows Workspace Surface And Controls
**Task Title:** Media Review Flows Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose media review flows in the main canvas workspace.
**Description:** Build the primary UI surfaces for support review-centric actions on rich assets without leaving the board., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T155: Media Review Flows Pointer Interaction Flow
**Task Title:** Media Review Flows Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for media review flows.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support review-centric actions on rich assets without leaving the board.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T156: Media Review Flows Keyboard Gesture And Shortcut Flow
**Task Title:** Media Review Flows Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for media review flows.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support review-centric actions on rich assets without leaving the board.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T157: Media Review Flows Visual Feedback And Rendering Polish
**Task Title:** Media Review Flows Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make media review flows legible in use.
**Description:** Implement the visible feedback for support review-centric actions on rich assets without leaving the board.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T158: Media Review Flows Persistence Preferences And Serialization
**Task Title:** Media Review Flows Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by media review flows.
**Description:** Identify what parts of support review-centric actions on rich assets without leaving the board. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T159: Media Review Flows Safeguards Telemetry And Recovery
**Task Title:** Media Review Flows Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for media review flows.
**Description:** Instrument support review-centric actions on rich assets without leaving the board. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P05-T160: Media Review Flows Tests Documentation And Rollout Gate
**Task Title:** Media Review Flows Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship media review flows.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support review-centric actions on rich assets without leaving the board.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** comment on image region; approve embed; resolve PDF annotation.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

## Workstream 17: Licensing And Security
Handle external content safely while preserving expected workflows.

### P05-T161: Licensing And Security UX Parity Audit
**Task Title:** Licensing And Security UX Parity Audit
**Definition:** Define the implementation contract for licensing and security by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around handle external content safely while preserving expected workflows.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T162: Licensing And Security Domain Model And State Contract
**Task Title:** Licensing And Security Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support licensing and security.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around handle external content safely while preserving expected workflows.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T163: Licensing And Security Commands Events And Context Keys
**Task Title:** Licensing And Security Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for licensing and security.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make handle external content safely while preserving expected workflows. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T164: Licensing And Security Workspace Surface And Controls
**Task Title:** Licensing And Security Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose licensing and security in the main canvas workspace.
**Description:** Build the primary UI surfaces for handle external content safely while preserving expected workflows., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T165: Licensing And Security Pointer Interaction Flow
**Task Title:** Licensing And Security Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for licensing and security.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for handle external content safely while preserving expected workflows.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T166: Licensing And Security Keyboard Gesture And Shortcut Flow
**Task Title:** Licensing And Security Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for licensing and security.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for handle external content safely while preserving expected workflows.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T167: Licensing And Security Visual Feedback And Rendering Polish
**Task Title:** Licensing And Security Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make licensing and security legible in use.
**Description:** Implement the visible feedback for handle external content safely while preserving expected workflows.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T168: Licensing And Security Persistence Preferences And Serialization
**Task Title:** Licensing And Security Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by licensing and security.
**Description:** Identify what parts of handle external content safely while preserving expected workflows. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T169: Licensing And Security Safeguards Telemetry And Recovery
**Task Title:** Licensing And Security Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for licensing and security.
**Description:** Instrument handle external content safely while preserving expected workflows. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

### P05-T170: Licensing And Security Tests Documentation And Rollout Gate
**Task Title:** Licensing And Security Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship licensing and security.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for handle external content safely while preserving expected workflows.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** blocked insecure embed; license metadata on asset; trusted domain allowlist.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ContentSecurityPolicy.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp
- /Users/ryanrentfro/code/markamp/src/core/UrlSanitizer.cpp

## Workstream 18: Asset Serialization And Sync
Preserve references, thumbnails, and metadata across save and sync paths.

### P05-T171: Asset Serialization And Sync UX Parity Audit
**Task Title:** Asset Serialization And Sync UX Parity Audit
**Definition:** Define the implementation contract for asset serialization and sync by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around preserve references, thumbnails, and metadata across save and sync paths.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T172: Asset Serialization And Sync Domain Model And State Contract
**Task Title:** Asset Serialization And Sync Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support asset serialization and sync.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around preserve references, thumbnails, and metadata across save and sync paths.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T173: Asset Serialization And Sync Commands Events And Context Keys
**Task Title:** Asset Serialization And Sync Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for asset serialization and sync.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make preserve references, thumbnails, and metadata across save and sync paths. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T174: Asset Serialization And Sync Workspace Surface And Controls
**Task Title:** Asset Serialization And Sync Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose asset serialization and sync in the main canvas workspace.
**Description:** Build the primary UI surfaces for preserve references, thumbnails, and metadata across save and sync paths., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T175: Asset Serialization And Sync Pointer Interaction Flow
**Task Title:** Asset Serialization And Sync Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for asset serialization and sync.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for preserve references, thumbnails, and metadata across save and sync paths.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T176: Asset Serialization And Sync Keyboard Gesture And Shortcut Flow
**Task Title:** Asset Serialization And Sync Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for asset serialization and sync.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for preserve references, thumbnails, and metadata across save and sync paths.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T177: Asset Serialization And Sync Visual Feedback And Rendering Polish
**Task Title:** Asset Serialization And Sync Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make asset serialization and sync legible in use.
**Description:** Implement the visible feedback for preserve references, thumbnails, and metadata across save and sync paths.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T178: Asset Serialization And Sync Persistence Preferences And Serialization
**Task Title:** Asset Serialization And Sync Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by asset serialization and sync.
**Description:** Identify what parts of preserve references, thumbnails, and metadata across save and sync paths. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T179: Asset Serialization And Sync Safeguards Telemetry And Recovery
**Task Title:** Asset Serialization And Sync Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for asset serialization and sync.
**Description:** Instrument preserve references, thumbnails, and metadata across save and sync paths. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

### P05-T180: Asset Serialization And Sync Tests Documentation And Rollout Gate
**Task Title:** Asset Serialization And Sync Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship asset serialization and sync.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for preserve references, thumbnails, and metadata across save and sync paths.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** embedded image round-trip; link integrity after move; asset sync conflict resolution.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetLinkIntegrity.h

## Workstream 19: Rich Content Export
Ensure rich objects export clearly to image, PDF, and HTML targets.

### P05-T181: Rich Content Export UX Parity Audit
**Task Title:** Rich Content Export UX Parity Audit
**Definition:** Define the implementation contract for rich content export by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around ensure rich objects export clearly to image, pdf, and html targets.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T182: Rich Content Export Domain Model And State Contract
**Task Title:** Rich Content Export Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support rich content export.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around ensure rich objects export clearly to image, pdf, and html targets.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T183: Rich Content Export Commands Events And Context Keys
**Task Title:** Rich Content Export Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for rich content export.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make ensure rich objects export clearly to image, pdf, and html targets. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T184: Rich Content Export Workspace Surface And Controls
**Task Title:** Rich Content Export Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose rich content export in the main canvas workspace.
**Description:** Build the primary UI surfaces for ensure rich objects export clearly to image, pdf, and html targets., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T185: Rich Content Export Pointer Interaction Flow
**Task Title:** Rich Content Export Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for rich content export.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for ensure rich objects export clearly to image, pdf, and html targets.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T186: Rich Content Export Keyboard Gesture And Shortcut Flow
**Task Title:** Rich Content Export Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for rich content export.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for ensure rich objects export clearly to image, pdf, and html targets.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T187: Rich Content Export Visual Feedback And Rendering Polish
**Task Title:** Rich Content Export Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make rich content export legible in use.
**Description:** Implement the visible feedback for ensure rich objects export clearly to image, pdf, and html targets.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T188: Rich Content Export Persistence Preferences And Serialization
**Task Title:** Rich Content Export Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by rich content export.
**Description:** Identify what parts of ensure rich objects export clearly to image, pdf, and html targets. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T189: Rich Content Export Safeguards Telemetry And Recovery
**Task Title:** Rich Content Export Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for rich content export.
**Description:** Instrument ensure rich objects export clearly to image, pdf, and html targets. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

### P05-T190: Rich Content Export Tests Documentation And Rollout Gate
**Task Title:** Rich Content Export Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship rich content export.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for ensure rich objects export clearly to image, pdf, and html targets.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** export image with caption; PDF export of mixed-media frame; HTML export keeps links.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.h
- /Users/ryanrentfro/code/markamp/src/core/HtmlExporter.cpp
- /Users/ryanrentfro/code/markamp/src/core/PdfExporter.cpp

## Workstream 20: Asset Test Coverage
Build coverage for insertion, rendering, persistence, and failure handling across asset types.

### P05-T191: Asset Test Coverage UX Parity Audit
**Task Title:** Asset Test Coverage UX Parity Audit
**Definition:** Define the implementation contract for asset test coverage by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around build coverage for insertion, rendering, persistence, and failure handling across asset types.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T192: Asset Test Coverage Domain Model And State Contract
**Task Title:** Asset Test Coverage Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support asset test coverage.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around build coverage for insertion, rendering, persistence, and failure handling across asset types.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T193: Asset Test Coverage Commands Events And Context Keys
**Task Title:** Asset Test Coverage Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for asset test coverage.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make build coverage for insertion, rendering, persistence, and failure handling across asset types. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T194: Asset Test Coverage Workspace Surface And Controls
**Task Title:** Asset Test Coverage Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose asset test coverage in the main canvas workspace.
**Description:** Build the primary UI surfaces for build coverage for insertion, rendering, persistence, and failure handling across asset types., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T195: Asset Test Coverage Pointer Interaction Flow
**Task Title:** Asset Test Coverage Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for asset test coverage.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for build coverage for insertion, rendering, persistence, and failure handling across asset types.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T196: Asset Test Coverage Keyboard Gesture And Shortcut Flow
**Task Title:** Asset Test Coverage Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for asset test coverage.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for build coverage for insertion, rendering, persistence, and failure handling across asset types.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T197: Asset Test Coverage Visual Feedback And Rendering Polish
**Task Title:** Asset Test Coverage Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make asset test coverage legible in use.
**Description:** Implement the visible feedback for build coverage for insertion, rendering, persistence, and failure handling across asset types.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T198: Asset Test Coverage Persistence Preferences And Serialization
**Task Title:** Asset Test Coverage Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by asset test coverage.
**Description:** Identify what parts of build coverage for insertion, rendering, persistence, and failure handling across asset types. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T199: Asset Test Coverage Safeguards Telemetry And Recovery
**Task Title:** Asset Test Coverage Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for asset test coverage.
**Description:** Instrument build coverage for insertion, rendering, persistence, and failure handling across asset types. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P05-T200: Asset Test Coverage Tests Documentation And Rollout Gate
**Task Title:** Asset Test Coverage Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship asset test coverage.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for build coverage for insertion, rendering, persistence, and failure handling across asset types.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** image load failure test; bookmark preview fallback; PDF page serialization test.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MediaEmbedModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PDFPageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BookmarkCardObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AppWidgetObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/AssetService.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
