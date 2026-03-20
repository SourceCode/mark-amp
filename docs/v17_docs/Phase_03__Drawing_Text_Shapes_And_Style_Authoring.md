# Phase 03: Drawing Text Shapes And Style Authoring

## Objective
Upgrade pen, shape, text, note, and style authoring so the canvas feels built for active creation rather than passive placement.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Freehand Pen Engine: Improve stroke capture, smoothing, replay, and cleanup for rapid sketching.
- W02 Pen Presets: Offer meaningful brush presets and recently used styles without cluttering the tool rail.
- W03 Eraser And Lasso: Add correction tools that are fast enough for active sketching sessions.
- W04 Shape Tool: Make shape creation, switching, and resizing feel immediate and coherent.
- W05 Text Box Authoring: Turn canvas text into a serious authoring surface with inline editing and robust layout behavior.
- W06 Sticky Notes: Make notes fast to create, easy to color, and optimized for workshop scale.
- W07 Color Authoring: Provide intentional palette workflows instead of raw color pickers everywhere.
- W08 Typography Controls: Expose the most useful typographic controls for boards, labels, and diagrams.
- W09 Fill Stroke And Effects: Support modern object styling without making objects visually noisy.
- W10 Style Presets: Make style reuse one-click so teams can author visually consistent boards.
- W11 Inline Rich Text: Allow more than plain text in objects where richer authoring improves board clarity.
- W12 Shape Library Quick Insert: Reduce time-to-object by improving discovery and insertion of common shapes.
- W13 Shape Recognition: Convert sketch intent into clean geometry when the user wants precision.
- W14 Image Annotation: Let users mark up screenshots and reference images without switching tools.
- W15 Comments And Callouts: Support review and critique flows directly on authored board content.
- W16 Tables For Authoring: Make simple tables useful for planning and comparison inside the canvas.
- W17 Icons Stickers And Emoji: Speed up expressive communication with lightweight graphic elements.
- W18 Creator Shortcuts: Map common authoring actions to shortcuts that scale for daily heavy use.
- W19 Bulk Style Editing: Make multi-object styling practical instead of a sequence of one-off edits.
- W20 Authoring Motion Feedback: Use restrained motion and previews to make creation and editing feel responsive.

## Workstream 01: Freehand Pen Engine
Improve stroke capture, smoothing, replay, and cleanup for rapid sketching.

### P03-T001: Freehand Pen Engine UX Parity Audit
**Task Title:** Freehand Pen Engine UX Parity Audit
**Definition:** Define the implementation contract for freehand pen engine by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around improve stroke capture, smoothing, replay, and cleanup for rapid sketching.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T002: Freehand Pen Engine Domain Model And State Contract
**Task Title:** Freehand Pen Engine Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support freehand pen engine.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around improve stroke capture, smoothing, replay, and cleanup for rapid sketching.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T003: Freehand Pen Engine Commands Events And Context Keys
**Task Title:** Freehand Pen Engine Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for freehand pen engine.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make improve stroke capture, smoothing, replay, and cleanup for rapid sketching. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T004: Freehand Pen Engine Workspace Surface And Controls
**Task Title:** Freehand Pen Engine Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose freehand pen engine in the main canvas workspace.
**Description:** Build the primary UI surfaces for improve stroke capture, smoothing, replay, and cleanup for rapid sketching., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T005: Freehand Pen Engine Pointer Interaction Flow
**Task Title:** Freehand Pen Engine Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for freehand pen engine.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for improve stroke capture, smoothing, replay, and cleanup for rapid sketching.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T006: Freehand Pen Engine Keyboard Gesture And Shortcut Flow
**Task Title:** Freehand Pen Engine Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for freehand pen engine.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for improve stroke capture, smoothing, replay, and cleanup for rapid sketching.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T007: Freehand Pen Engine Visual Feedback And Rendering Polish
**Task Title:** Freehand Pen Engine Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make freehand pen engine legible in use.
**Description:** Implement the visible feedback for improve stroke capture, smoothing, replay, and cleanup for rapid sketching.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T008: Freehand Pen Engine Persistence Preferences And Serialization
**Task Title:** Freehand Pen Engine Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by freehand pen engine.
**Description:** Identify what parts of improve stroke capture, smoothing, replay, and cleanup for rapid sketching. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T009: Freehand Pen Engine Safeguards Telemetry And Recovery
**Task Title:** Freehand Pen Engine Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for freehand pen engine.
**Description:** Instrument improve stroke capture, smoothing, replay, and cleanup for rapid sketching. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

### P03-T010: Freehand Pen Engine Tests Documentation And Rollout Gate
**Task Title:** Freehand Pen Engine Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship freehand pen engine.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for improve stroke capture, smoothing, replay, and cleanup for rapid sketching.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** pressure-sensitive stroke; stabilized line; ink that stays editable.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPath.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp

## Workstream 02: Pen Presets
Offer meaningful brush presets and recently used styles without cluttering the tool rail.

### P03-T011: Pen Presets UX Parity Audit
**Task Title:** Pen Presets UX Parity Audit
**Definition:** Define the implementation contract for pen presets by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around offer meaningful brush presets and recently used styles without cluttering the tool rail.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T012: Pen Presets Domain Model And State Contract
**Task Title:** Pen Presets Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support pen presets.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around offer meaningful brush presets and recently used styles without cluttering the tool rail.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T013: Pen Presets Commands Events And Context Keys
**Task Title:** Pen Presets Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for pen presets.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make offer meaningful brush presets and recently used styles without cluttering the tool rail. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T014: Pen Presets Workspace Surface And Controls
**Task Title:** Pen Presets Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose pen presets in the main canvas workspace.
**Description:** Build the primary UI surfaces for offer meaningful brush presets and recently used styles without cluttering the tool rail., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T015: Pen Presets Pointer Interaction Flow
**Task Title:** Pen Presets Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for pen presets.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for offer meaningful brush presets and recently used styles without cluttering the tool rail.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T016: Pen Presets Keyboard Gesture And Shortcut Flow
**Task Title:** Pen Presets Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for pen presets.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for offer meaningful brush presets and recently used styles without cluttering the tool rail.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T017: Pen Presets Visual Feedback And Rendering Polish
**Task Title:** Pen Presets Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make pen presets legible in use.
**Description:** Implement the visible feedback for offer meaningful brush presets and recently used styles without cluttering the tool rail.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T018: Pen Presets Persistence Preferences And Serialization
**Task Title:** Pen Presets Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by pen presets.
**Description:** Identify what parts of offer meaningful brush presets and recently used styles without cluttering the tool rail. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T019: Pen Presets Safeguards Telemetry And Recovery
**Task Title:** Pen Presets Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for pen presets.
**Description:** Instrument offer meaningful brush presets and recently used styles without cluttering the tool rail. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

### P03-T020: Pen Presets Tests Documentation And Rollout Gate
**Task Title:** Pen Presets Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship pen presets.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for offer meaningful brush presets and recently used styles without cluttering the tool rail.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** marker preset; highlighter preset; saved custom pen.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.h
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbar.cpp

## Workstream 03: Eraser And Lasso
Add correction tools that are fast enough for active sketching sessions.

### P03-T021: Eraser And Lasso UX Parity Audit
**Task Title:** Eraser And Lasso UX Parity Audit
**Definition:** Define the implementation contract for eraser and lasso by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around add correction tools that are fast enough for active sketching sessions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T022: Eraser And Lasso Domain Model And State Contract
**Task Title:** Eraser And Lasso Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support eraser and lasso.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around add correction tools that are fast enough for active sketching sessions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T023: Eraser And Lasso Commands Events And Context Keys
**Task Title:** Eraser And Lasso Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for eraser and lasso.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make add correction tools that are fast enough for active sketching sessions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T024: Eraser And Lasso Workspace Surface And Controls
**Task Title:** Eraser And Lasso Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose eraser and lasso in the main canvas workspace.
**Description:** Build the primary UI surfaces for add correction tools that are fast enough for active sketching sessions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T025: Eraser And Lasso Pointer Interaction Flow
**Task Title:** Eraser And Lasso Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for eraser and lasso.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for add correction tools that are fast enough for active sketching sessions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T026: Eraser And Lasso Keyboard Gesture And Shortcut Flow
**Task Title:** Eraser And Lasso Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for eraser and lasso.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for add correction tools that are fast enough for active sketching sessions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T027: Eraser And Lasso Visual Feedback And Rendering Polish
**Task Title:** Eraser And Lasso Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make eraser and lasso legible in use.
**Description:** Implement the visible feedback for add correction tools that are fast enough for active sketching sessions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T028: Eraser And Lasso Persistence Preferences And Serialization
**Task Title:** Eraser And Lasso Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by eraser and lasso.
**Description:** Identify what parts of add correction tools that are fast enough for active sketching sessions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T029: Eraser And Lasso Safeguards Telemetry And Recovery
**Task Title:** Eraser And Lasso Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for eraser and lasso.
**Description:** Instrument add correction tools that are fast enough for active sketching sessions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

### P03-T030: Eraser And Lasso Tests Documentation And Rollout Gate
**Task Title:** Eraser And Lasso Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship eraser and lasso.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for add correction tools that are fast enough for active sketching sessions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** stroke eraser; segment erase; lasso select hand-drawn content.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputModel.h

## Workstream 04: Shape Tool
Make shape creation, switching, and resizing feel immediate and coherent.

### P03-T031: Shape Tool UX Parity Audit
**Task Title:** Shape Tool UX Parity Audit
**Definition:** Define the implementation contract for shape tool by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make shape creation, switching, and resizing feel immediate and coherent.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T032: Shape Tool Domain Model And State Contract
**Task Title:** Shape Tool Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support shape tool.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make shape creation, switching, and resizing feel immediate and coherent.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T033: Shape Tool Commands Events And Context Keys
**Task Title:** Shape Tool Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for shape tool.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make shape creation, switching, and resizing feel immediate and coherent. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T034: Shape Tool Workspace Surface And Controls
**Task Title:** Shape Tool Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose shape tool in the main canvas workspace.
**Description:** Build the primary UI surfaces for make shape creation, switching, and resizing feel immediate and coherent., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T035: Shape Tool Pointer Interaction Flow
**Task Title:** Shape Tool Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for shape tool.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make shape creation, switching, and resizing feel immediate and coherent.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T036: Shape Tool Keyboard Gesture And Shortcut Flow
**Task Title:** Shape Tool Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for shape tool.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make shape creation, switching, and resizing feel immediate and coherent.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T037: Shape Tool Visual Feedback And Rendering Polish
**Task Title:** Shape Tool Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make shape tool legible in use.
**Description:** Implement the visible feedback for make shape creation, switching, and resizing feel immediate and coherent.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T038: Shape Tool Persistence Preferences And Serialization
**Task Title:** Shape Tool Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by shape tool.
**Description:** Identify what parts of make shape creation, switching, and resizing feel immediate and coherent. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T039: Shape Tool Safeguards Telemetry And Recovery
**Task Title:** Shape Tool Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for shape tool.
**Description:** Instrument make shape creation, switching, and resizing feel immediate and coherent. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

### P03-T040: Shape Tool Tests Documentation And Rollout Gate
**Task Title:** Shape Tool Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship shape tool.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make shape creation, switching, and resizing feel immediate and coherent.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** rectangle and ellipse drag-create; last-used shape memory; shape quick insert.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRenderer.cpp

## Workstream 05: Text Box Authoring
Turn canvas text into a serious authoring surface with inline editing and robust layout behavior.

### P03-T041: Text Box Authoring UX Parity Audit
**Task Title:** Text Box Authoring UX Parity Audit
**Definition:** Define the implementation contract for text box authoring by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn canvas text into a serious authoring surface with inline editing and robust layout behavior.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T042: Text Box Authoring Domain Model And State Contract
**Task Title:** Text Box Authoring Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support text box authoring.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn canvas text into a serious authoring surface with inline editing and robust layout behavior.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T043: Text Box Authoring Commands Events And Context Keys
**Task Title:** Text Box Authoring Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for text box authoring.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn canvas text into a serious authoring surface with inline editing and robust layout behavior. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T044: Text Box Authoring Workspace Surface And Controls
**Task Title:** Text Box Authoring Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose text box authoring in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn canvas text into a serious authoring surface with inline editing and robust layout behavior., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T045: Text Box Authoring Pointer Interaction Flow
**Task Title:** Text Box Authoring Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for text box authoring.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn canvas text into a serious authoring surface with inline editing and robust layout behavior.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T046: Text Box Authoring Keyboard Gesture And Shortcut Flow
**Task Title:** Text Box Authoring Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for text box authoring.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn canvas text into a serious authoring surface with inline editing and robust layout behavior.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T047: Text Box Authoring Visual Feedback And Rendering Polish
**Task Title:** Text Box Authoring Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make text box authoring legible in use.
**Description:** Implement the visible feedback for turn canvas text into a serious authoring surface with inline editing and robust layout behavior.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T048: Text Box Authoring Persistence Preferences And Serialization
**Task Title:** Text Box Authoring Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by text box authoring.
**Description:** Identify what parts of turn canvas text into a serious authoring surface with inline editing and robust layout behavior. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T049: Text Box Authoring Safeguards Telemetry And Recovery
**Task Title:** Text Box Authoring Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for text box authoring.
**Description:** Instrument turn canvas text into a serious authoring surface with inline editing and robust layout behavior. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

### P03-T050: Text Box Authoring Tests Documentation And Rollout Gate
**Task Title:** Text Box Authoring Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship text box authoring.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn canvas text into a serious authoring surface with inline editing and robust layout behavior.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** double-click to edit; auto-resizing text box; text selection inside object.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.h

## Workstream 06: Sticky Notes
Make notes fast to create, easy to color, and optimized for workshop scale.

### P03-T051: Sticky Notes UX Parity Audit
**Task Title:** Sticky Notes UX Parity Audit
**Definition:** Define the implementation contract for sticky notes by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make notes fast to create, easy to color, and optimized for workshop scale.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T052: Sticky Notes Domain Model And State Contract
**Task Title:** Sticky Notes Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support sticky notes.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make notes fast to create, easy to color, and optimized for workshop scale.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T053: Sticky Notes Commands Events And Context Keys
**Task Title:** Sticky Notes Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for sticky notes.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make notes fast to create, easy to color, and optimized for workshop scale. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T054: Sticky Notes Workspace Surface And Controls
**Task Title:** Sticky Notes Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose sticky notes in the main canvas workspace.
**Description:** Build the primary UI surfaces for make notes fast to create, easy to color, and optimized for workshop scale., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T055: Sticky Notes Pointer Interaction Flow
**Task Title:** Sticky Notes Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for sticky notes.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make notes fast to create, easy to color, and optimized for workshop scale.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T056: Sticky Notes Keyboard Gesture And Shortcut Flow
**Task Title:** Sticky Notes Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for sticky notes.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make notes fast to create, easy to color, and optimized for workshop scale.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T057: Sticky Notes Visual Feedback And Rendering Polish
**Task Title:** Sticky Notes Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make sticky notes legible in use.
**Description:** Implement the visible feedback for make notes fast to create, easy to color, and optimized for workshop scale.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T058: Sticky Notes Persistence Preferences And Serialization
**Task Title:** Sticky Notes Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by sticky notes.
**Description:** Identify what parts of make notes fast to create, easy to color, and optimized for workshop scale. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T059: Sticky Notes Safeguards Telemetry And Recovery
**Task Title:** Sticky Notes Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for sticky notes.
**Description:** Instrument make notes fast to create, easy to color, and optimized for workshop scale. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

### P03-T060: Sticky Notes Tests Documentation And Rollout Gate
**Task Title:** Sticky Notes Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship sticky notes.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make notes fast to create, easy to color, and optimized for workshop scale.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** click-to-create sticky; color cycle shortcut; author initials on note.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/StickyNoteRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.h

## Workstream 07: Color Authoring
Provide intentional palette workflows instead of raw color pickers everywhere.

### P03-T061: Color Authoring UX Parity Audit
**Task Title:** Color Authoring UX Parity Audit
**Definition:** Define the implementation contract for color authoring by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around provide intentional palette workflows instead of raw color pickers everywhere.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T062: Color Authoring Domain Model And State Contract
**Task Title:** Color Authoring Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support color authoring.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around provide intentional palette workflows instead of raw color pickers everywhere.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T063: Color Authoring Commands Events And Context Keys
**Task Title:** Color Authoring Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for color authoring.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make provide intentional palette workflows instead of raw color pickers everywhere. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T064: Color Authoring Workspace Surface And Controls
**Task Title:** Color Authoring Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose color authoring in the main canvas workspace.
**Description:** Build the primary UI surfaces for provide intentional palette workflows instead of raw color pickers everywhere., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T065: Color Authoring Pointer Interaction Flow
**Task Title:** Color Authoring Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for color authoring.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for provide intentional palette workflows instead of raw color pickers everywhere.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T066: Color Authoring Keyboard Gesture And Shortcut Flow
**Task Title:** Color Authoring Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for color authoring.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for provide intentional palette workflows instead of raw color pickers everywhere.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T067: Color Authoring Visual Feedback And Rendering Polish
**Task Title:** Color Authoring Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make color authoring legible in use.
**Description:** Implement the visible feedback for provide intentional palette workflows instead of raw color pickers everywhere.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T068: Color Authoring Persistence Preferences And Serialization
**Task Title:** Color Authoring Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by color authoring.
**Description:** Identify what parts of provide intentional palette workflows instead of raw color pickers everywhere. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T069: Color Authoring Safeguards Telemetry And Recovery
**Task Title:** Color Authoring Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for color authoring.
**Description:** Instrument provide intentional palette workflows instead of raw color pickers everywhere. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

### P03-T070: Color Authoring Tests Documentation And Rollout Gate
**Task Title:** Color Authoring Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship color authoring.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for provide intentional palette workflows instead of raw color pickers everywhere.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** theme palette swatches; recent colors; board palette.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/ColorPaletteGenerator.cpp
- /Users/ryanrentfro/code/markamp/src/core/Color.cpp

## Workstream 08: Typography Controls
Expose the most useful typographic controls for boards, labels, and diagrams.

### P03-T071: Typography Controls UX Parity Audit
**Task Title:** Typography Controls UX Parity Audit
**Definition:** Define the implementation contract for typography controls by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose the most useful typographic controls for boards, labels, and diagrams.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** font size presets; line height control; text alignment.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T072: Typography Controls Domain Model And State Contract
**Task Title:** Typography Controls Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support typography controls.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose the most useful typographic controls for boards, labels, and diagrams.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** font size presets; line height control; text alignment.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T073: Typography Controls Commands Events And Context Keys
**Task Title:** Typography Controls Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for typography controls.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose the most useful typographic controls for boards, labels, and diagrams. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** font size presets; line height control; text alignment.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T074: Typography Controls Workspace Surface And Controls
**Task Title:** Typography Controls Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose typography controls in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose the most useful typographic controls for boards, labels, and diagrams., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** font size presets; line height control; text alignment.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T075: Typography Controls Pointer Interaction Flow
**Task Title:** Typography Controls Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for typography controls.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose the most useful typographic controls for boards, labels, and diagrams.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** font size presets; line height control; text alignment.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T076: Typography Controls Keyboard Gesture And Shortcut Flow
**Task Title:** Typography Controls Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for typography controls.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose the most useful typographic controls for boards, labels, and diagrams.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** font size presets; line height control; text alignment.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T077: Typography Controls Visual Feedback And Rendering Polish
**Task Title:** Typography Controls Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make typography controls legible in use.
**Description:** Implement the visible feedback for expose the most useful typographic controls for boards, labels, and diagrams.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** font size presets; line height control; text alignment.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T078: Typography Controls Persistence Preferences And Serialization
**Task Title:** Typography Controls Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by typography controls.
**Description:** Identify what parts of expose the most useful typographic controls for boards, labels, and diagrams. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** font size presets; line height control; text alignment.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T079: Typography Controls Safeguards Telemetry And Recovery
**Task Title:** Typography Controls Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for typography controls.
**Description:** Instrument expose the most useful typographic controls for boards, labels, and diagrams. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** font size presets; line height control; text alignment.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

### P03-T080: Typography Controls Tests Documentation And Rollout Gate
**Task Title:** Typography Controls Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship typography controls.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose the most useful typographic controls for boards, labels, and diagrams.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** font size presets; line height control; text alignment.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.cpp

## Workstream 09: Fill Stroke And Effects
Support modern object styling without making objects visually noisy.

### P03-T081: Fill Stroke And Effects UX Parity Audit
**Task Title:** Fill Stroke And Effects UX Parity Audit
**Definition:** Define the implementation contract for fill stroke and effects by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support modern object styling without making objects visually noisy.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T082: Fill Stroke And Effects Domain Model And State Contract
**Task Title:** Fill Stroke And Effects Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support fill stroke and effects.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support modern object styling without making objects visually noisy.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T083: Fill Stroke And Effects Commands Events And Context Keys
**Task Title:** Fill Stroke And Effects Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for fill stroke and effects.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support modern object styling without making objects visually noisy. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T084: Fill Stroke And Effects Workspace Surface And Controls
**Task Title:** Fill Stroke And Effects Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose fill stroke and effects in the main canvas workspace.
**Description:** Build the primary UI surfaces for support modern object styling without making objects visually noisy., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T085: Fill Stroke And Effects Pointer Interaction Flow
**Task Title:** Fill Stroke And Effects Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for fill stroke and effects.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support modern object styling without making objects visually noisy.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T086: Fill Stroke And Effects Keyboard Gesture And Shortcut Flow
**Task Title:** Fill Stroke And Effects Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for fill stroke and effects.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support modern object styling without making objects visually noisy.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T087: Fill Stroke And Effects Visual Feedback And Rendering Polish
**Task Title:** Fill Stroke And Effects Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make fill stroke and effects legible in use.
**Description:** Implement the visible feedback for support modern object styling without making objects visually noisy.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T088: Fill Stroke And Effects Persistence Preferences And Serialization
**Task Title:** Fill Stroke And Effects Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by fill stroke and effects.
**Description:** Identify what parts of support modern object styling without making objects visually noisy. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T089: Fill Stroke And Effects Safeguards Telemetry And Recovery
**Task Title:** Fill Stroke And Effects Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for fill stroke and effects.
**Description:** Instrument support modern object styling without making objects visually noisy. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

### P03-T090: Fill Stroke And Effects Tests Documentation And Rollout Gate
**Task Title:** Fill Stroke And Effects Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship fill stroke and effects.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support modern object styling without making objects visually noisy.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** fill opacity; stroke width presets; soft shadow toggle.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FillEffectsController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasColorToken.h
- /Users/ryanrentfro/code/markamp/src/ui/ElevationSystem.cpp

## Workstream 10: Style Presets
Make style reuse one-click so teams can author visually consistent boards.

### P03-T091: Style Presets UX Parity Audit
**Task Title:** Style Presets UX Parity Audit
**Definition:** Define the implementation contract for style presets by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make style reuse one-click so teams can author visually consistent boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T092: Style Presets Domain Model And State Contract
**Task Title:** Style Presets Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support style presets.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make style reuse one-click so teams can author visually consistent boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T093: Style Presets Commands Events And Context Keys
**Task Title:** Style Presets Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for style presets.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make style reuse one-click so teams can author visually consistent boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T094: Style Presets Workspace Surface And Controls
**Task Title:** Style Presets Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose style presets in the main canvas workspace.
**Description:** Build the primary UI surfaces for make style reuse one-click so teams can author visually consistent boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T095: Style Presets Pointer Interaction Flow
**Task Title:** Style Presets Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for style presets.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make style reuse one-click so teams can author visually consistent boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T096: Style Presets Keyboard Gesture And Shortcut Flow
**Task Title:** Style Presets Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for style presets.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make style reuse one-click so teams can author visually consistent boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T097: Style Presets Visual Feedback And Rendering Polish
**Task Title:** Style Presets Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make style presets legible in use.
**Description:** Implement the visible feedback for make style reuse one-click so teams can author visually consistent boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T098: Style Presets Persistence Preferences And Serialization
**Task Title:** Style Presets Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by style presets.
**Description:** Identify what parts of make style reuse one-click so teams can author visually consistent boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T099: Style Presets Safeguards Telemetry And Recovery
**Task Title:** Style Presets Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for style presets.
**Description:** Instrument make style reuse one-click so teams can author visually consistent boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

### P03-T100: Style Presets Tests Documentation And Rollout Gate
**Task Title:** Style Presets Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship style presets.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make style reuse one-click so teams can author visually consistent boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** apply preset to selection; save brand preset; preset categories.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp

## Workstream 11: Inline Rich Text
Allow more than plain text in objects where richer authoring improves board clarity.

### P03-T101: Inline Rich Text UX Parity Audit
**Task Title:** Inline Rich Text UX Parity Audit
**Definition:** Define the implementation contract for inline rich text by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around allow more than plain text in objects where richer authoring improves board clarity.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T102: Inline Rich Text Domain Model And State Contract
**Task Title:** Inline Rich Text Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support inline rich text.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around allow more than plain text in objects where richer authoring improves board clarity.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T103: Inline Rich Text Commands Events And Context Keys
**Task Title:** Inline Rich Text Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for inline rich text.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make allow more than plain text in objects where richer authoring improves board clarity. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T104: Inline Rich Text Workspace Surface And Controls
**Task Title:** Inline Rich Text Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose inline rich text in the main canvas workspace.
**Description:** Build the primary UI surfaces for allow more than plain text in objects where richer authoring improves board clarity., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T105: Inline Rich Text Pointer Interaction Flow
**Task Title:** Inline Rich Text Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for inline rich text.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for allow more than plain text in objects where richer authoring improves board clarity.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T106: Inline Rich Text Keyboard Gesture And Shortcut Flow
**Task Title:** Inline Rich Text Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for inline rich text.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for allow more than plain text in objects where richer authoring improves board clarity.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T107: Inline Rich Text Visual Feedback And Rendering Polish
**Task Title:** Inline Rich Text Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make inline rich text legible in use.
**Description:** Implement the visible feedback for allow more than plain text in objects where richer authoring improves board clarity.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T108: Inline Rich Text Persistence Preferences And Serialization
**Task Title:** Inline Rich Text Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by inline rich text.
**Description:** Identify what parts of allow more than plain text in objects where richer authoring improves board clarity. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T109: Inline Rich Text Safeguards Telemetry And Recovery
**Task Title:** Inline Rich Text Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for inline rich text.
**Description:** Instrument allow more than plain text in objects where richer authoring improves board clarity. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

### P03-T110: Inline Rich Text Tests Documentation And Rollout Gate
**Task Title:** Inline Rich Text Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship inline rich text.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for allow more than plain text in objects where richer authoring improves board clarity.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** bold and italic inside text box; bullet lists in notes; inline links.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingFormatBar.h
- /Users/ryanrentfro/code/markamp/src/core/InlineFrontmatterRenderer.h

## Workstream 12: Shape Library Quick Insert
Reduce time-to-object by improving discovery and insertion of common shapes.

### P03-T111: Shape Library Quick Insert UX Parity Audit
**Task Title:** Shape Library Quick Insert UX Parity Audit
**Definition:** Define the implementation contract for shape library quick insert by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around reduce time-to-object by improving discovery and insertion of common shapes.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T112: Shape Library Quick Insert Domain Model And State Contract
**Task Title:** Shape Library Quick Insert Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support shape library quick insert.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around reduce time-to-object by improving discovery and insertion of common shapes.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T113: Shape Library Quick Insert Commands Events And Context Keys
**Task Title:** Shape Library Quick Insert Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for shape library quick insert.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make reduce time-to-object by improving discovery and insertion of common shapes. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T114: Shape Library Quick Insert Workspace Surface And Controls
**Task Title:** Shape Library Quick Insert Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose shape library quick insert in the main canvas workspace.
**Description:** Build the primary UI surfaces for reduce time-to-object by improving discovery and insertion of common shapes., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T115: Shape Library Quick Insert Pointer Interaction Flow
**Task Title:** Shape Library Quick Insert Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for shape library quick insert.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for reduce time-to-object by improving discovery and insertion of common shapes.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T116: Shape Library Quick Insert Keyboard Gesture And Shortcut Flow
**Task Title:** Shape Library Quick Insert Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for shape library quick insert.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for reduce time-to-object by improving discovery and insertion of common shapes.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T117: Shape Library Quick Insert Visual Feedback And Rendering Polish
**Task Title:** Shape Library Quick Insert Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make shape library quick insert legible in use.
**Description:** Implement the visible feedback for reduce time-to-object by improving discovery and insertion of common shapes.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T118: Shape Library Quick Insert Persistence Preferences And Serialization
**Task Title:** Shape Library Quick Insert Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by shape library quick insert.
**Description:** Identify what parts of reduce time-to-object by improving discovery and insertion of common shapes. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T119: Shape Library Quick Insert Safeguards Telemetry And Recovery
**Task Title:** Shape Library Quick Insert Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for shape library quick insert.
**Description:** Instrument reduce time-to-object by improving discovery and insertion of common shapes. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

### P03-T120: Shape Library Quick Insert Tests Documentation And Rollout Gate
**Task Title:** Shape Library Quick Insert Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship shape library quick insert.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for reduce time-to-object by improving discovery and insertion of common shapes.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** searchable shape picker; recent shapes; drag from library.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp

## Workstream 13: Shape Recognition
Convert sketch intent into clean geometry when the user wants precision.

### P03-T121: Shape Recognition UX Parity Audit
**Task Title:** Shape Recognition UX Parity Audit
**Definition:** Define the implementation contract for shape recognition by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around convert sketch intent into clean geometry when the user wants precision.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T122: Shape Recognition Domain Model And State Contract
**Task Title:** Shape Recognition Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support shape recognition.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around convert sketch intent into clean geometry when the user wants precision.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T123: Shape Recognition Commands Events And Context Keys
**Task Title:** Shape Recognition Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for shape recognition.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make convert sketch intent into clean geometry when the user wants precision. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T124: Shape Recognition Workspace Surface And Controls
**Task Title:** Shape Recognition Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose shape recognition in the main canvas workspace.
**Description:** Build the primary UI surfaces for convert sketch intent into clean geometry when the user wants precision., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T125: Shape Recognition Pointer Interaction Flow
**Task Title:** Shape Recognition Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for shape recognition.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for convert sketch intent into clean geometry when the user wants precision.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T126: Shape Recognition Keyboard Gesture And Shortcut Flow
**Task Title:** Shape Recognition Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for shape recognition.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for convert sketch intent into clean geometry when the user wants precision.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T127: Shape Recognition Visual Feedback And Rendering Polish
**Task Title:** Shape Recognition Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make shape recognition legible in use.
**Description:** Implement the visible feedback for convert sketch intent into clean geometry when the user wants precision.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T128: Shape Recognition Persistence Preferences And Serialization
**Task Title:** Shape Recognition Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by shape recognition.
**Description:** Identify what parts of convert sketch intent into clean geometry when the user wants precision. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T129: Shape Recognition Safeguards Telemetry And Recovery
**Task Title:** Shape Recognition Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for shape recognition.
**Description:** Instrument convert sketch intent into clean geometry when the user wants precision. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

### P03-T130: Shape Recognition Tests Documentation And Rollout Gate
**Task Title:** Shape Recognition Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship shape recognition.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for convert sketch intent into clean geometry when the user wants precision.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** recognize rough rectangle; suggest arrow cleanup; keep hand-drawn when confidence is low.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ShapeRecognizer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.cpp

## Workstream 14: Image Annotation
Let users mark up screenshots and reference images without switching tools.

### P03-T131: Image Annotation UX Parity Audit
**Task Title:** Image Annotation UX Parity Audit
**Definition:** Define the implementation contract for image annotation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around let users mark up screenshots and reference images without switching tools.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T132: Image Annotation Domain Model And State Contract
**Task Title:** Image Annotation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support image annotation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around let users mark up screenshots and reference images without switching tools.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T133: Image Annotation Commands Events And Context Keys
**Task Title:** Image Annotation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for image annotation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make let users mark up screenshots and reference images without switching tools. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T134: Image Annotation Workspace Surface And Controls
**Task Title:** Image Annotation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose image annotation in the main canvas workspace.
**Description:** Build the primary UI surfaces for let users mark up screenshots and reference images without switching tools., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T135: Image Annotation Pointer Interaction Flow
**Task Title:** Image Annotation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for image annotation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for let users mark up screenshots and reference images without switching tools.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T136: Image Annotation Keyboard Gesture And Shortcut Flow
**Task Title:** Image Annotation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for image annotation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for let users mark up screenshots and reference images without switching tools.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T137: Image Annotation Visual Feedback And Rendering Polish
**Task Title:** Image Annotation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make image annotation legible in use.
**Description:** Implement the visible feedback for let users mark up screenshots and reference images without switching tools.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T138: Image Annotation Persistence Preferences And Serialization
**Task Title:** Image Annotation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by image annotation.
**Description:** Identify what parts of let users mark up screenshots and reference images without switching tools. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T139: Image Annotation Safeguards Telemetry And Recovery
**Task Title:** Image Annotation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for image annotation.
**Description:** Instrument let users mark up screenshots and reference images without switching tools. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

### P03-T140: Image Annotation Tests Documentation And Rollout Gate
**Task Title:** Image Annotation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship image annotation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for let users mark up screenshots and reference images without switching tools.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** draw over image; add pin comment; blur a sensitive area.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ImageObjectRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.cpp

## Workstream 15: Comments And Callouts
Support review and critique flows directly on authored board content.

### P03-T141: Comments And Callouts UX Parity Audit
**Task Title:** Comments And Callouts UX Parity Audit
**Definition:** Define the implementation contract for comments and callouts by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support review and critique flows directly on authored board content.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T142: Comments And Callouts Domain Model And State Contract
**Task Title:** Comments And Callouts Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support comments and callouts.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support review and critique flows directly on authored board content.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T143: Comments And Callouts Commands Events And Context Keys
**Task Title:** Comments And Callouts Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for comments and callouts.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support review and critique flows directly on authored board content. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T144: Comments And Callouts Workspace Surface And Controls
**Task Title:** Comments And Callouts Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose comments and callouts in the main canvas workspace.
**Description:** Build the primary UI surfaces for support review and critique flows directly on authored board content., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T145: Comments And Callouts Pointer Interaction Flow
**Task Title:** Comments And Callouts Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for comments and callouts.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support review and critique flows directly on authored board content.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T146: Comments And Callouts Keyboard Gesture And Shortcut Flow
**Task Title:** Comments And Callouts Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for comments and callouts.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support review and critique flows directly on authored board content.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T147: Comments And Callouts Visual Feedback And Rendering Polish
**Task Title:** Comments And Callouts Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make comments and callouts legible in use.
**Description:** Implement the visible feedback for support review and critique flows directly on authored board content.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T148: Comments And Callouts Persistence Preferences And Serialization
**Task Title:** Comments And Callouts Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by comments and callouts.
**Description:** Identify what parts of support review and critique flows directly on authored board content. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T149: Comments And Callouts Safeguards Telemetry And Recovery
**Task Title:** Comments And Callouts Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for comments and callouts.
**Description:** Instrument support review and critique flows directly on authored board content. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P03-T150: Comments And Callouts Tests Documentation And Rollout Gate
**Task Title:** Comments And Callouts Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship comments and callouts.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support review and critique flows directly on authored board content.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** callout arrow with text; threaded comment badge; resolve comment.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

## Workstream 16: Tables For Authoring
Make simple tables useful for planning and comparison inside the canvas.

### P03-T151: Tables For Authoring UX Parity Audit
**Task Title:** Tables For Authoring UX Parity Audit
**Definition:** Define the implementation contract for tables for authoring by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make simple tables useful for planning and comparison inside the canvas.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T152: Tables For Authoring Domain Model And State Contract
**Task Title:** Tables For Authoring Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support tables for authoring.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make simple tables useful for planning and comparison inside the canvas.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T153: Tables For Authoring Commands Events And Context Keys
**Task Title:** Tables For Authoring Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for tables for authoring.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make simple tables useful for planning and comparison inside the canvas. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T154: Tables For Authoring Workspace Surface And Controls
**Task Title:** Tables For Authoring Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose tables for authoring in the main canvas workspace.
**Description:** Build the primary UI surfaces for make simple tables useful for planning and comparison inside the canvas., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T155: Tables For Authoring Pointer Interaction Flow
**Task Title:** Tables For Authoring Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for tables for authoring.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make simple tables useful for planning and comparison inside the canvas.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T156: Tables For Authoring Keyboard Gesture And Shortcut Flow
**Task Title:** Tables For Authoring Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for tables for authoring.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make simple tables useful for planning and comparison inside the canvas.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T157: Tables For Authoring Visual Feedback And Rendering Polish
**Task Title:** Tables For Authoring Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make tables for authoring legible in use.
**Description:** Implement the visible feedback for make simple tables useful for planning and comparison inside the canvas.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T158: Tables For Authoring Persistence Preferences And Serialization
**Task Title:** Tables For Authoring Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by tables for authoring.
**Description:** Identify what parts of make simple tables useful for planning and comparison inside the canvas. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T159: Tables For Authoring Safeguards Telemetry And Recovery
**Task Title:** Tables For Authoring Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for tables for authoring.
**Description:** Instrument make simple tables useful for planning and comparison inside the canvas. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

### P03-T160: Tables For Authoring Tests Documentation And Rollout Gate
**Task Title:** Tables For Authoring Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship tables for authoring.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make simple tables useful for planning and comparison inside the canvas.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** insert table; resize column; paste spreadsheet cells.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.cpp

## Workstream 17: Icons Stickers And Emoji
Speed up expressive communication with lightweight graphic elements.

### P03-T161: Icons Stickers And Emoji UX Parity Audit
**Task Title:** Icons Stickers And Emoji UX Parity Audit
**Definition:** Define the implementation contract for icons stickers and emoji by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around speed up expressive communication with lightweight graphic elements.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T162: Icons Stickers And Emoji Domain Model And State Contract
**Task Title:** Icons Stickers And Emoji Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support icons stickers and emoji.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around speed up expressive communication with lightweight graphic elements.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T163: Icons Stickers And Emoji Commands Events And Context Keys
**Task Title:** Icons Stickers And Emoji Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for icons stickers and emoji.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make speed up expressive communication with lightweight graphic elements. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T164: Icons Stickers And Emoji Workspace Surface And Controls
**Task Title:** Icons Stickers And Emoji Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose icons stickers and emoji in the main canvas workspace.
**Description:** Build the primary UI surfaces for speed up expressive communication with lightweight graphic elements., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T165: Icons Stickers And Emoji Pointer Interaction Flow
**Task Title:** Icons Stickers And Emoji Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for icons stickers and emoji.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for speed up expressive communication with lightweight graphic elements.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T166: Icons Stickers And Emoji Keyboard Gesture And Shortcut Flow
**Task Title:** Icons Stickers And Emoji Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for icons stickers and emoji.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for speed up expressive communication with lightweight graphic elements.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T167: Icons Stickers And Emoji Visual Feedback And Rendering Polish
**Task Title:** Icons Stickers And Emoji Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make icons stickers and emoji legible in use.
**Description:** Implement the visible feedback for speed up expressive communication with lightweight graphic elements.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T168: Icons Stickers And Emoji Persistence Preferences And Serialization
**Task Title:** Icons Stickers And Emoji Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by icons stickers and emoji.
**Description:** Identify what parts of speed up expressive communication with lightweight graphic elements. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T169: Icons Stickers And Emoji Safeguards Telemetry And Recovery
**Task Title:** Icons Stickers And Emoji Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for icons stickers and emoji.
**Description:** Instrument speed up expressive communication with lightweight graphic elements. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

### P03-T170: Icons Stickers And Emoji Tests Documentation And Rollout Gate
**Task Title:** Icons Stickers And Emoji Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship icons stickers and emoji.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for speed up expressive communication with lightweight graphic elements.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** emoji picker; colorable icon; sticker pack.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IconObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/IconLibrary.h
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp

## Workstream 18: Creator Shortcuts
Map common authoring actions to shortcuts that scale for daily heavy use.

### P03-T171: Creator Shortcuts UX Parity Audit
**Task Title:** Creator Shortcuts UX Parity Audit
**Definition:** Define the implementation contract for creator shortcuts by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around map common authoring actions to shortcuts that scale for daily heavy use.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T172: Creator Shortcuts Domain Model And State Contract
**Task Title:** Creator Shortcuts Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support creator shortcuts.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around map common authoring actions to shortcuts that scale for daily heavy use.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T173: Creator Shortcuts Commands Events And Context Keys
**Task Title:** Creator Shortcuts Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for creator shortcuts.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make map common authoring actions to shortcuts that scale for daily heavy use. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T174: Creator Shortcuts Workspace Surface And Controls
**Task Title:** Creator Shortcuts Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose creator shortcuts in the main canvas workspace.
**Description:** Build the primary UI surfaces for map common authoring actions to shortcuts that scale for daily heavy use., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T175: Creator Shortcuts Pointer Interaction Flow
**Task Title:** Creator Shortcuts Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for creator shortcuts.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for map common authoring actions to shortcuts that scale for daily heavy use.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T176: Creator Shortcuts Keyboard Gesture And Shortcut Flow
**Task Title:** Creator Shortcuts Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for creator shortcuts.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for map common authoring actions to shortcuts that scale for daily heavy use.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T177: Creator Shortcuts Visual Feedback And Rendering Polish
**Task Title:** Creator Shortcuts Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make creator shortcuts legible in use.
**Description:** Implement the visible feedback for map common authoring actions to shortcuts that scale for daily heavy use.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T178: Creator Shortcuts Persistence Preferences And Serialization
**Task Title:** Creator Shortcuts Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by creator shortcuts.
**Description:** Identify what parts of map common authoring actions to shortcuts that scale for daily heavy use. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T179: Creator Shortcuts Safeguards Telemetry And Recovery
**Task Title:** Creator Shortcuts Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for creator shortcuts.
**Description:** Instrument map common authoring actions to shortcuts that scale for daily heavy use. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

### P03-T180: Creator Shortcuts Tests Documentation And Rollout Gate
**Task Title:** Creator Shortcuts Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship creator shortcuts.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for map common authoring actions to shortcuts that scale for daily heavy use.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** toggle pen tool; duplicate style; cycle note color.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h
- /Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp

## Workstream 19: Bulk Style Editing
Make multi-object styling practical instead of a sequence of one-off edits.

### P03-T181: Bulk Style Editing UX Parity Audit
**Task Title:** Bulk Style Editing UX Parity Audit
**Definition:** Define the implementation contract for bulk style editing by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make multi-object styling practical instead of a sequence of one-off edits.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T182: Bulk Style Editing Domain Model And State Contract
**Task Title:** Bulk Style Editing Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support bulk style editing.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make multi-object styling practical instead of a sequence of one-off edits.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T183: Bulk Style Editing Commands Events And Context Keys
**Task Title:** Bulk Style Editing Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for bulk style editing.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make multi-object styling practical instead of a sequence of one-off edits. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T184: Bulk Style Editing Workspace Surface And Controls
**Task Title:** Bulk Style Editing Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose bulk style editing in the main canvas workspace.
**Description:** Build the primary UI surfaces for make multi-object styling practical instead of a sequence of one-off edits., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T185: Bulk Style Editing Pointer Interaction Flow
**Task Title:** Bulk Style Editing Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for bulk style editing.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make multi-object styling practical instead of a sequence of one-off edits.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T186: Bulk Style Editing Keyboard Gesture And Shortcut Flow
**Task Title:** Bulk Style Editing Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for bulk style editing.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make multi-object styling practical instead of a sequence of one-off edits.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T187: Bulk Style Editing Visual Feedback And Rendering Polish
**Task Title:** Bulk Style Editing Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make bulk style editing legible in use.
**Description:** Implement the visible feedback for make multi-object styling practical instead of a sequence of one-off edits.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T188: Bulk Style Editing Persistence Preferences And Serialization
**Task Title:** Bulk Style Editing Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by bulk style editing.
**Description:** Identify what parts of make multi-object styling practical instead of a sequence of one-off edits. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T189: Bulk Style Editing Safeguards Telemetry And Recovery
**Task Title:** Bulk Style Editing Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for bulk style editing.
**Description:** Instrument make multi-object styling practical instead of a sequence of one-off edits. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P03-T190: Bulk Style Editing Tests Documentation And Rollout Gate
**Task Title:** Bulk Style Editing Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship bulk style editing.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make multi-object styling practical instead of a sequence of one-off edits.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** apply one text style to 40 notes; normalize border widths; batch recolor selection.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

## Workstream 20: Authoring Motion Feedback
Use restrained motion and previews to make creation and editing feel responsive.

### P03-T191: Authoring Motion Feedback UX Parity Audit
**Task Title:** Authoring Motion Feedback UX Parity Audit
**Definition:** Define the implementation contract for authoring motion feedback by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around use restrained motion and previews to make creation and editing feel responsive.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T192: Authoring Motion Feedback Domain Model And State Contract
**Task Title:** Authoring Motion Feedback Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support authoring motion feedback.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around use restrained motion and previews to make creation and editing feel responsive.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T193: Authoring Motion Feedback Commands Events And Context Keys
**Task Title:** Authoring Motion Feedback Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for authoring motion feedback.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make use restrained motion and previews to make creation and editing feel responsive. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T194: Authoring Motion Feedback Workspace Surface And Controls
**Task Title:** Authoring Motion Feedback Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose authoring motion feedback in the main canvas workspace.
**Description:** Build the primary UI surfaces for use restrained motion and previews to make creation and editing feel responsive., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T195: Authoring Motion Feedback Pointer Interaction Flow
**Task Title:** Authoring Motion Feedback Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for authoring motion feedback.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for use restrained motion and previews to make creation and editing feel responsive.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T196: Authoring Motion Feedback Keyboard Gesture And Shortcut Flow
**Task Title:** Authoring Motion Feedback Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for authoring motion feedback.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for use restrained motion and previews to make creation and editing feel responsive.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T197: Authoring Motion Feedback Visual Feedback And Rendering Polish
**Task Title:** Authoring Motion Feedback Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make authoring motion feedback legible in use.
**Description:** Implement the visible feedback for use restrained motion and previews to make creation and editing feel responsive.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T198: Authoring Motion Feedback Persistence Preferences And Serialization
**Task Title:** Authoring Motion Feedback Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by authoring motion feedback.
**Description:** Identify what parts of use restrained motion and previews to make creation and editing feel responsive. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T199: Authoring Motion Feedback Safeguards Telemetry And Recovery
**Task Title:** Authoring Motion Feedback Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for authoring motion feedback.
**Description:** Instrument use restrained motion and previews to make creation and editing feel responsive. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

### P03-T200: Authoring Motion Feedback Tests Documentation And Rollout Gate
**Task Title:** Authoring Motion Feedback Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship authoring motion feedback.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for use restrained motion and previews to make creation and editing feel responsive.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** shape creation preview; subtle selection fade; toolbar reveal animation.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/DrawTool.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TextBoxRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FreehandPathRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasStylePresetService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ColorPaletteController.cpp
- /Users/ryanrentfro/code/markamp/src/ui/AnimationDriver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlAnimator.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ControlMotionTokens.cpp

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
