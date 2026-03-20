# Phase 02: Selection Transform Alignment And Spatial Editing

## Objective
Close the gap on selection, movement, transforms, alignment, grouping, and precision editing so authors can manipulate boards at Miro-like speed.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Single Selection: Make object hit selection reliable, legible, and reversible across all object classes.
- W02 Multi Selection: Enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.
- W03 Move Operations: Support smooth drag-move behavior with clear ownership, snap, and collision feedback.
- W04 Resize Operations: Deliver handle-based resize flows that preserve object intent and feel precise on high-DPI screens.
- W05 Rotation Operations: Add rotation controls that are discoverable for novices and efficient for expert users.
- W06 Alignment Guides: Make alignment intent visible through guides, measurements, and snap previews.
- W07 Object Snapping: Extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.
- W08 Distribute And Align Actions: Expose one-click cleanup actions for board tidying and diagram maintenance.
- W09 Smart Spacing: Visualize and preserve even spacing while users drag and nudge grouped elements.
- W10 Grouping: Make group creation, entry, exit, and nested editing understandable and low-risk.
- W11 Frame Membership: Clarify how objects join, leave, and inherit behavior from frames during transforms.
- W12 Layers And Z Order: Make stacking order explicit so creators can resolve overlaps quickly.
- W13 Locking And Pinning: Protect stable content while keeping inspection and collaboration possible.
- W14 Precision Nudge: Support keyboard nudges, big-step moves, and coordinate editing for exact placement.
- W15 Duplicate And Paste In Place: Make duplication preserve intent, ordering, and connector relationships.
- W16 Drag Autoscroll: Keep transforms fluid when users move objects beyond the current viewport edge.
- W17 Viewport-Aware Transforms: Prevent camera changes from breaking object transforms on very large boards.
- W18 Multi User Selection: Surface remote locks and live transforms so multiplayer editing stays legible.
- W19 Transform History Quality: Tune history granularity so undo feels human, not mechanical.
- W20 Selection Action Surfaces: Expose the highest-value transform actions in context-aware mini toolbars.

## Workstream 01: Single Selection
Make object hit selection reliable, legible, and reversible across all object classes.

### P02-T001: Single Selection UX Parity Audit
**Task Title:** Single Selection UX Parity Audit
**Definition:** Define the implementation contract for single selection by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make object hit selection reliable, legible, and reversible across all object classes.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T002: Single Selection Domain Model And State Contract
**Task Title:** Single Selection Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support single selection.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make object hit selection reliable, legible, and reversible across all object classes.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T003: Single Selection Commands Events And Context Keys
**Task Title:** Single Selection Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for single selection.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make object hit selection reliable, legible, and reversible across all object classes. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T004: Single Selection Workspace Surface And Controls
**Task Title:** Single Selection Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose single selection in the main canvas workspace.
**Description:** Build the primary UI surfaces for make object hit selection reliable, legible, and reversible across all object classes., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T005: Single Selection Pointer Interaction Flow
**Task Title:** Single Selection Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for single selection.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make object hit selection reliable, legible, and reversible across all object classes.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T006: Single Selection Keyboard Gesture And Shortcut Flow
**Task Title:** Single Selection Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for single selection.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make object hit selection reliable, legible, and reversible across all object classes.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T007: Single Selection Visual Feedback And Rendering Polish
**Task Title:** Single Selection Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make single selection legible in use.
**Description:** Implement the visible feedback for make object hit selection reliable, legible, and reversible across all object classes.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T008: Single Selection Persistence Preferences And Serialization
**Task Title:** Single Selection Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by single selection.
**Description:** Identify what parts of make object hit selection reliable, legible, and reversible across all object classes. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T009: Single Selection Safeguards Telemetry And Recovery
**Task Title:** Single Selection Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for single selection.
**Description:** Instrument make object hit selection reliable, legible, and reversible across all object classes. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

### P02-T010: Single Selection Tests Documentation And Rollout Gate
**Task Title:** Single Selection Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship single selection.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make object hit selection reliable, legible, and reversible across all object classes.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** click to select; selection halo; clear selection on empty click.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/HitTestModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObject.cpp

## Workstream 02: Multi Selection
Enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.

### P02-T011: Multi Selection UX Parity Audit
**Task Title:** Multi Selection UX Parity Audit
**Definition:** Define the implementation contract for multi selection by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T012: Multi Selection Domain Model And State Contract
**Task Title:** Multi Selection Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support multi selection.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T013: Multi Selection Commands Events And Context Keys
**Task Title:** Multi Selection Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for multi selection.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make enable box, additive, subtractive, and semantic multi-selection without accidental loss of context. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T014: Multi Selection Workspace Surface And Controls
**Task Title:** Multi Selection Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose multi selection in the main canvas workspace.
**Description:** Build the primary UI surfaces for enable box, additive, subtractive, and semantic multi-selection without accidental loss of context., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T015: Multi Selection Pointer Interaction Flow
**Task Title:** Multi Selection Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for multi selection.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T016: Multi Selection Keyboard Gesture And Shortcut Flow
**Task Title:** Multi Selection Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for multi selection.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T017: Multi Selection Visual Feedback And Rendering Polish
**Task Title:** Multi Selection Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make multi selection legible in use.
**Description:** Implement the visible feedback for enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T018: Multi Selection Persistence Preferences And Serialization
**Task Title:** Multi Selection Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by multi selection.
**Description:** Identify what parts of enable box, additive, subtractive, and semantic multi-selection without accidental loss of context. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T019: Multi Selection Safeguards Telemetry And Recovery
**Task Title:** Multi Selection Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for multi selection.
**Description:** Instrument enable box, additive, subtractive, and semantic multi-selection without accidental loss of context. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

### P02-T020: Multi Selection Tests Documentation And Rollout Gate
**Task Title:** Multi Selection Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship multi selection.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for enable box, additive, subtractive, and semantic multi-selection without accidental loss of context.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** shift-click add; marquee select; select all of same type.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionSemantics.h
- /Users/ryanrentfro/code/markamp/src/ui/BulkActionController.cpp

## Workstream 03: Move Operations
Support smooth drag-move behavior with clear ownership, snap, and collision feedback.

### P02-T021: Move Operations UX Parity Audit
**Task Title:** Move Operations UX Parity Audit
**Definition:** Define the implementation contract for move operations by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support smooth drag-move behavior with clear ownership, snap, and collision feedback.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T022: Move Operations Domain Model And State Contract
**Task Title:** Move Operations Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support move operations.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support smooth drag-move behavior with clear ownership, snap, and collision feedback.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T023: Move Operations Commands Events And Context Keys
**Task Title:** Move Operations Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for move operations.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support smooth drag-move behavior with clear ownership, snap, and collision feedback. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T024: Move Operations Workspace Surface And Controls
**Task Title:** Move Operations Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose move operations in the main canvas workspace.
**Description:** Build the primary UI surfaces for support smooth drag-move behavior with clear ownership, snap, and collision feedback., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T025: Move Operations Pointer Interaction Flow
**Task Title:** Move Operations Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for move operations.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support smooth drag-move behavior with clear ownership, snap, and collision feedback.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T026: Move Operations Keyboard Gesture And Shortcut Flow
**Task Title:** Move Operations Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for move operations.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support smooth drag-move behavior with clear ownership, snap, and collision feedback.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T027: Move Operations Visual Feedback And Rendering Polish
**Task Title:** Move Operations Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make move operations legible in use.
**Description:** Implement the visible feedback for support smooth drag-move behavior with clear ownership, snap, and collision feedback.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T028: Move Operations Persistence Preferences And Serialization
**Task Title:** Move Operations Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by move operations.
**Description:** Identify what parts of support smooth drag-move behavior with clear ownership, snap, and collision feedback. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T029: Move Operations Safeguards Telemetry And Recovery
**Task Title:** Move Operations Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for move operations.
**Description:** Instrument support smooth drag-move behavior with clear ownership, snap, and collision feedback. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

### P02-T030: Move Operations Tests Documentation And Rollout Gate
**Task Title:** Move Operations Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship move operations.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support smooth drag-move behavior with clear ownership, snap, and collision feedback.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** drag selection; hold space while moving; coalesced movement history.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PointerEventRouter.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp

## Workstream 04: Resize Operations
Deliver handle-based resize flows that preserve object intent and feel precise on high-DPI screens.

### P02-T031: Resize Operations UX Parity Audit
**Task Title:** Resize Operations UX Parity Audit
**Definition:** Define the implementation contract for resize operations by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T032: Resize Operations Domain Model And State Contract
**Task Title:** Resize Operations Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support resize operations.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T033: Resize Operations Commands Events And Context Keys
**Task Title:** Resize Operations Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for resize operations.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T034: Resize Operations Workspace Surface And Controls
**Task Title:** Resize Operations Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose resize operations in the main canvas workspace.
**Description:** Build the primary UI surfaces for deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T035: Resize Operations Pointer Interaction Flow
**Task Title:** Resize Operations Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for resize operations.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T036: Resize Operations Keyboard Gesture And Shortcut Flow
**Task Title:** Resize Operations Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for resize operations.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T037: Resize Operations Visual Feedback And Rendering Polish
**Task Title:** Resize Operations Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make resize operations legible in use.
**Description:** Implement the visible feedback for deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T038: Resize Operations Persistence Preferences And Serialization
**Task Title:** Resize Operations Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by resize operations.
**Description:** Identify what parts of deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T039: Resize Operations Safeguards Telemetry And Recovery
**Task Title:** Resize Operations Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for resize operations.
**Description:** Instrument deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

### P02-T040: Resize Operations Tests Documentation And Rollout Gate
**Task Title:** Resize Operations Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship resize operations.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for deliver handle-based resize flows that preserve object intent and feel precise on high-dpi screens.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** corner resize; edge resize; aspect ratio lock.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.h
- /Users/ryanrentfro/code/markamp/src/ui/DpiScalingController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTypes.cpp

## Workstream 05: Rotation Operations
Add rotation controls that are discoverable for novices and efficient for expert users.

### P02-T041: Rotation Operations UX Parity Audit
**Task Title:** Rotation Operations UX Parity Audit
**Definition:** Define the implementation contract for rotation operations by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around add rotation controls that are discoverable for novices and efficient for expert users.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T042: Rotation Operations Domain Model And State Contract
**Task Title:** Rotation Operations Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support rotation operations.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around add rotation controls that are discoverable for novices and efficient for expert users.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T043: Rotation Operations Commands Events And Context Keys
**Task Title:** Rotation Operations Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for rotation operations.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make add rotation controls that are discoverable for novices and efficient for expert users. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T044: Rotation Operations Workspace Surface And Controls
**Task Title:** Rotation Operations Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose rotation operations in the main canvas workspace.
**Description:** Build the primary UI surfaces for add rotation controls that are discoverable for novices and efficient for expert users., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T045: Rotation Operations Pointer Interaction Flow
**Task Title:** Rotation Operations Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for rotation operations.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for add rotation controls that are discoverable for novices and efficient for expert users.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T046: Rotation Operations Keyboard Gesture And Shortcut Flow
**Task Title:** Rotation Operations Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for rotation operations.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for add rotation controls that are discoverable for novices and efficient for expert users.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T047: Rotation Operations Visual Feedback And Rendering Polish
**Task Title:** Rotation Operations Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make rotation operations legible in use.
**Description:** Implement the visible feedback for add rotation controls that are discoverable for novices and efficient for expert users.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T048: Rotation Operations Persistence Preferences And Serialization
**Task Title:** Rotation Operations Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by rotation operations.
**Description:** Identify what parts of add rotation controls that are discoverable for novices and efficient for expert users. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T049: Rotation Operations Safeguards Telemetry And Recovery
**Task Title:** Rotation Operations Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for rotation operations.
**Description:** Instrument add rotation controls that are discoverable for novices and efficient for expert users. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

### P02-T050: Rotation Operations Tests Documentation And Rollout Gate
**Task Title:** Rotation Operations Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship rotation operations.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for add rotation controls that are discoverable for novices and efficient for expert users.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** rotation handle; angle snap; numeric rotation edit.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasCommands.cpp

## Workstream 06: Alignment Guides
Make alignment intent visible through guides, measurements, and snap previews.

### P02-T051: Alignment Guides UX Parity Audit
**Task Title:** Alignment Guides UX Parity Audit
**Definition:** Define the implementation contract for alignment guides by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make alignment intent visible through guides, measurements, and snap previews.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T052: Alignment Guides Domain Model And State Contract
**Task Title:** Alignment Guides Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support alignment guides.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make alignment intent visible through guides, measurements, and snap previews.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T053: Alignment Guides Commands Events And Context Keys
**Task Title:** Alignment Guides Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for alignment guides.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make alignment intent visible through guides, measurements, and snap previews. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T054: Alignment Guides Workspace Surface And Controls
**Task Title:** Alignment Guides Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose alignment guides in the main canvas workspace.
**Description:** Build the primary UI surfaces for make alignment intent visible through guides, measurements, and snap previews., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T055: Alignment Guides Pointer Interaction Flow
**Task Title:** Alignment Guides Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for alignment guides.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make alignment intent visible through guides, measurements, and snap previews.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T056: Alignment Guides Keyboard Gesture And Shortcut Flow
**Task Title:** Alignment Guides Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for alignment guides.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make alignment intent visible through guides, measurements, and snap previews.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T057: Alignment Guides Visual Feedback And Rendering Polish
**Task Title:** Alignment Guides Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make alignment guides legible in use.
**Description:** Implement the visible feedback for make alignment intent visible through guides, measurements, and snap previews.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T058: Alignment Guides Persistence Preferences And Serialization
**Task Title:** Alignment Guides Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by alignment guides.
**Description:** Identify what parts of make alignment intent visible through guides, measurements, and snap previews. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T059: Alignment Guides Safeguards Telemetry And Recovery
**Task Title:** Alignment Guides Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for alignment guides.
**Description:** Instrument make alignment intent visible through guides, measurements, and snap previews. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

### P02-T060: Alignment Guides Tests Documentation And Rollout Gate
**Task Title:** Alignment Guides Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship alignment guides.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make alignment intent visible through guides, measurements, and snap previews.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** center alignment guide; edge guide; distance overlay.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h

## Workstream 07: Object Snapping
Extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.

### P02-T061: Object Snapping UX Parity Audit
**Task Title:** Object Snapping UX Parity Audit
**Definition:** Define the implementation contract for object snapping by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T062: Object Snapping Domain Model And State Contract
**Task Title:** Object Snapping Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support object snapping.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T063: Object Snapping Commands Events And Context Keys
**Task Title:** Object Snapping Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for object snapping.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T064: Object Snapping Workspace Surface And Controls
**Task Title:** Object Snapping Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose object snapping in the main canvas workspace.
**Description:** Build the primary UI surfaces for extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T065: Object Snapping Pointer Interaction Flow
**Task Title:** Object Snapping Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for object snapping.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T066: Object Snapping Keyboard Gesture And Shortcut Flow
**Task Title:** Object Snapping Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for object snapping.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T067: Object Snapping Visual Feedback And Rendering Polish
**Task Title:** Object Snapping Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make object snapping legible in use.
**Description:** Implement the visible feedback for extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T068: Object Snapping Persistence Preferences And Serialization
**Task Title:** Object Snapping Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by object snapping.
**Description:** Identify what parts of extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T069: Object Snapping Safeguards Telemetry And Recovery
**Task Title:** Object Snapping Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for object snapping.
**Description:** Instrument extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

### P02-T070: Object Snapping Tests Documentation And Rollout Gate
**Task Title:** Object Snapping Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship object snapping.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for extend snapping across edges, centers, ports, and containers so moves feel spatially intelligent.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** snap to sibling edge; snap to frame center; snap to connector port.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h

## Workstream 08: Distribute And Align Actions
Expose one-click cleanup actions for board tidying and diagram maintenance.

### P02-T071: Distribute And Align Actions UX Parity Audit
**Task Title:** Distribute And Align Actions UX Parity Audit
**Definition:** Define the implementation contract for distribute and align actions by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose one-click cleanup actions for board tidying and diagram maintenance.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T072: Distribute And Align Actions Domain Model And State Contract
**Task Title:** Distribute And Align Actions Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support distribute and align actions.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose one-click cleanup actions for board tidying and diagram maintenance.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T073: Distribute And Align Actions Commands Events And Context Keys
**Task Title:** Distribute And Align Actions Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for distribute and align actions.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose one-click cleanup actions for board tidying and diagram maintenance. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T074: Distribute And Align Actions Workspace Surface And Controls
**Task Title:** Distribute And Align Actions Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose distribute and align actions in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose one-click cleanup actions for board tidying and diagram maintenance., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T075: Distribute And Align Actions Pointer Interaction Flow
**Task Title:** Distribute And Align Actions Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for distribute and align actions.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose one-click cleanup actions for board tidying and diagram maintenance.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T076: Distribute And Align Actions Keyboard Gesture And Shortcut Flow
**Task Title:** Distribute And Align Actions Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for distribute and align actions.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose one-click cleanup actions for board tidying and diagram maintenance.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T077: Distribute And Align Actions Visual Feedback And Rendering Polish
**Task Title:** Distribute And Align Actions Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make distribute and align actions legible in use.
**Description:** Implement the visible feedback for expose one-click cleanup actions for board tidying and diagram maintenance.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T078: Distribute And Align Actions Persistence Preferences And Serialization
**Task Title:** Distribute And Align Actions Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by distribute and align actions.
**Description:** Identify what parts of expose one-click cleanup actions for board tidying and diagram maintenance. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T079: Distribute And Align Actions Safeguards Telemetry And Recovery
**Task Title:** Distribute And Align Actions Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for distribute and align actions.
**Description:** Instrument expose one-click cleanup actions for board tidying and diagram maintenance. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

### P02-T080: Distribute And Align Actions Tests Documentation And Rollout Gate
**Task Title:** Distribute And Align Actions Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship distribute and align actions.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose one-click cleanup actions for board tidying and diagram maintenance.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** align left; distribute horizontally; equalize sizes.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp

## Workstream 09: Smart Spacing
Visualize and preserve even spacing while users drag and nudge grouped elements.

### P02-T081: Smart Spacing UX Parity Audit
**Task Title:** Smart Spacing UX Parity Audit
**Definition:** Define the implementation contract for smart spacing by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around visualize and preserve even spacing while users drag and nudge grouped elements.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T082: Smart Spacing Domain Model And State Contract
**Task Title:** Smart Spacing Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support smart spacing.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around visualize and preserve even spacing while users drag and nudge grouped elements.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T083: Smart Spacing Commands Events And Context Keys
**Task Title:** Smart Spacing Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for smart spacing.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make visualize and preserve even spacing while users drag and nudge grouped elements. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T084: Smart Spacing Workspace Surface And Controls
**Task Title:** Smart Spacing Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose smart spacing in the main canvas workspace.
**Description:** Build the primary UI surfaces for visualize and preserve even spacing while users drag and nudge grouped elements., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T085: Smart Spacing Pointer Interaction Flow
**Task Title:** Smart Spacing Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for smart spacing.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for visualize and preserve even spacing while users drag and nudge grouped elements.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T086: Smart Spacing Keyboard Gesture And Shortcut Flow
**Task Title:** Smart Spacing Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for smart spacing.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for visualize and preserve even spacing while users drag and nudge grouped elements.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T087: Smart Spacing Visual Feedback And Rendering Polish
**Task Title:** Smart Spacing Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make smart spacing legible in use.
**Description:** Implement the visible feedback for visualize and preserve even spacing while users drag and nudge grouped elements.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T088: Smart Spacing Persistence Preferences And Serialization
**Task Title:** Smart Spacing Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by smart spacing.
**Description:** Identify what parts of visualize and preserve even spacing while users drag and nudge grouped elements. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T089: Smart Spacing Safeguards Telemetry And Recovery
**Task Title:** Smart Spacing Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for smart spacing.
**Description:** Instrument visualize and preserve even spacing while users drag and nudge grouped elements. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

### P02-T090: Smart Spacing Tests Documentation And Rollout Gate
**Task Title:** Smart Spacing Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship smart spacing.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for visualize and preserve even spacing while users drag and nudge grouped elements.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** same-gap preview; auto-snap to equal spacing; measurements between cards.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AlignmentGuides.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp

## Workstream 10: Grouping
Make group creation, entry, exit, and nested editing understandable and low-risk.

### P02-T091: Grouping UX Parity Audit
**Task Title:** Grouping UX Parity Audit
**Definition:** Define the implementation contract for grouping by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make group creation, entry, exit, and nested editing understandable and low-risk.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T092: Grouping Domain Model And State Contract
**Task Title:** Grouping Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support grouping.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make group creation, entry, exit, and nested editing understandable and low-risk.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T093: Grouping Commands Events And Context Keys
**Task Title:** Grouping Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for grouping.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make group creation, entry, exit, and nested editing understandable and low-risk. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T094: Grouping Workspace Surface And Controls
**Task Title:** Grouping Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose grouping in the main canvas workspace.
**Description:** Build the primary UI surfaces for make group creation, entry, exit, and nested editing understandable and low-risk., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T095: Grouping Pointer Interaction Flow
**Task Title:** Grouping Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for grouping.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make group creation, entry, exit, and nested editing understandable and low-risk.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T096: Grouping Keyboard Gesture And Shortcut Flow
**Task Title:** Grouping Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for grouping.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make group creation, entry, exit, and nested editing understandable and low-risk.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T097: Grouping Visual Feedback And Rendering Polish
**Task Title:** Grouping Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make grouping legible in use.
**Description:** Implement the visible feedback for make group creation, entry, exit, and nested editing understandable and low-risk.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T098: Grouping Persistence Preferences And Serialization
**Task Title:** Grouping Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by grouping.
**Description:** Identify what parts of make group creation, entry, exit, and nested editing understandable and low-risk. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T099: Grouping Safeguards Telemetry And Recovery
**Task Title:** Grouping Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for grouping.
**Description:** Instrument make group creation, entry, exit, and nested editing understandable and low-risk. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

### P02-T100: Grouping Tests Documentation And Rollout Gate
**Task Title:** Grouping Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship grouping.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make group creation, entry, exit, and nested editing understandable and low-risk.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** group selected objects; double-click to edit group; nested groups.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GroupingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp

## Workstream 11: Frame Membership
Clarify how objects join, leave, and inherit behavior from frames during transforms.

### P02-T101: Frame Membership UX Parity Audit
**Task Title:** Frame Membership UX Parity Audit
**Definition:** Define the implementation contract for frame membership by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around clarify how objects join, leave, and inherit behavior from frames during transforms.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T102: Frame Membership Domain Model And State Contract
**Task Title:** Frame Membership Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support frame membership.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around clarify how objects join, leave, and inherit behavior from frames during transforms.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T103: Frame Membership Commands Events And Context Keys
**Task Title:** Frame Membership Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for frame membership.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make clarify how objects join, leave, and inherit behavior from frames during transforms. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T104: Frame Membership Workspace Surface And Controls
**Task Title:** Frame Membership Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose frame membership in the main canvas workspace.
**Description:** Build the primary UI surfaces for clarify how objects join, leave, and inherit behavior from frames during transforms., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T105: Frame Membership Pointer Interaction Flow
**Task Title:** Frame Membership Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for frame membership.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for clarify how objects join, leave, and inherit behavior from frames during transforms.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T106: Frame Membership Keyboard Gesture And Shortcut Flow
**Task Title:** Frame Membership Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for frame membership.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for clarify how objects join, leave, and inherit behavior from frames during transforms.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T107: Frame Membership Visual Feedback And Rendering Polish
**Task Title:** Frame Membership Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make frame membership legible in use.
**Description:** Implement the visible feedback for clarify how objects join, leave, and inherit behavior from frames during transforms.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T108: Frame Membership Persistence Preferences And Serialization
**Task Title:** Frame Membership Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by frame membership.
**Description:** Identify what parts of clarify how objects join, leave, and inherit behavior from frames during transforms. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T109: Frame Membership Safeguards Telemetry And Recovery
**Task Title:** Frame Membership Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for frame membership.
**Description:** Instrument clarify how objects join, leave, and inherit behavior from frames during transforms. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

### P02-T110: Frame Membership Tests Documentation And Rollout Gate
**Task Title:** Frame Membership Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship frame membership.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for clarify how objects join, leave, and inherit behavior from frames during transforms.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** drop into frame; frame title follows selection; move frame and contents.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Board.cpp

## Workstream 12: Layers And Z Order
Make stacking order explicit so creators can resolve overlaps quickly.

### P02-T111: Layers And Z Order UX Parity Audit
**Task Title:** Layers And Z Order UX Parity Audit
**Definition:** Define the implementation contract for layers and z order by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make stacking order explicit so creators can resolve overlaps quickly.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T112: Layers And Z Order Domain Model And State Contract
**Task Title:** Layers And Z Order Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support layers and z order.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make stacking order explicit so creators can resolve overlaps quickly.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T113: Layers And Z Order Commands Events And Context Keys
**Task Title:** Layers And Z Order Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for layers and z order.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make stacking order explicit so creators can resolve overlaps quickly. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T114: Layers And Z Order Workspace Surface And Controls
**Task Title:** Layers And Z Order Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose layers and z order in the main canvas workspace.
**Description:** Build the primary UI surfaces for make stacking order explicit so creators can resolve overlaps quickly., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T115: Layers And Z Order Pointer Interaction Flow
**Task Title:** Layers And Z Order Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for layers and z order.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make stacking order explicit so creators can resolve overlaps quickly.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T116: Layers And Z Order Keyboard Gesture And Shortcut Flow
**Task Title:** Layers And Z Order Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for layers and z order.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make stacking order explicit so creators can resolve overlaps quickly.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T117: Layers And Z Order Visual Feedback And Rendering Polish
**Task Title:** Layers And Z Order Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make layers and z order legible in use.
**Description:** Implement the visible feedback for make stacking order explicit so creators can resolve overlaps quickly.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T118: Layers And Z Order Persistence Preferences And Serialization
**Task Title:** Layers And Z Order Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by layers and z order.
**Description:** Identify what parts of make stacking order explicit so creators can resolve overlaps quickly. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T119: Layers And Z Order Safeguards Telemetry And Recovery
**Task Title:** Layers And Z Order Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for layers and z order.
**Description:** Instrument make stacking order explicit so creators can resolve overlaps quickly. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

### P02-T120: Layers And Z Order Tests Documentation And Rollout Gate
**Task Title:** Layers And Z Order Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship layers and z order.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make stacking order explicit so creators can resolve overlaps quickly.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** bring to front; send backward; layer ordering in outline.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp

## Workstream 13: Locking And Pinning
Protect stable content while keeping inspection and collaboration possible.

### P02-T121: Locking And Pinning UX Parity Audit
**Task Title:** Locking And Pinning UX Parity Audit
**Definition:** Define the implementation contract for locking and pinning by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around protect stable content while keeping inspection and collaboration possible.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T122: Locking And Pinning Domain Model And State Contract
**Task Title:** Locking And Pinning Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support locking and pinning.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around protect stable content while keeping inspection and collaboration possible.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T123: Locking And Pinning Commands Events And Context Keys
**Task Title:** Locking And Pinning Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for locking and pinning.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make protect stable content while keeping inspection and collaboration possible. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T124: Locking And Pinning Workspace Surface And Controls
**Task Title:** Locking And Pinning Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose locking and pinning in the main canvas workspace.
**Description:** Build the primary UI surfaces for protect stable content while keeping inspection and collaboration possible., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T125: Locking And Pinning Pointer Interaction Flow
**Task Title:** Locking And Pinning Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for locking and pinning.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for protect stable content while keeping inspection and collaboration possible.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T126: Locking And Pinning Keyboard Gesture And Shortcut Flow
**Task Title:** Locking And Pinning Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for locking and pinning.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for protect stable content while keeping inspection and collaboration possible.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T127: Locking And Pinning Visual Feedback And Rendering Polish
**Task Title:** Locking And Pinning Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make locking and pinning legible in use.
**Description:** Implement the visible feedback for protect stable content while keeping inspection and collaboration possible.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T128: Locking And Pinning Persistence Preferences And Serialization
**Task Title:** Locking And Pinning Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by locking and pinning.
**Description:** Identify what parts of protect stable content while keeping inspection and collaboration possible. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T129: Locking And Pinning Safeguards Telemetry And Recovery
**Task Title:** Locking And Pinning Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for locking and pinning.
**Description:** Instrument protect stable content while keeping inspection and collaboration possible. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

### P02-T130: Locking And Pinning Tests Documentation And Rollout Gate
**Task Title:** Locking And Pinning Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship locking and pinning.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for protect stable content while keeping inspection and collaboration possible.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** lock position; unlock from inspector; pin board chrome.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasContextMenu.cpp

## Workstream 14: Precision Nudge
Support keyboard nudges, big-step moves, and coordinate editing for exact placement.

### P02-T131: Precision Nudge UX Parity Audit
**Task Title:** Precision Nudge UX Parity Audit
**Definition:** Define the implementation contract for precision nudge by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support keyboard nudges, big-step moves, and coordinate editing for exact placement.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T132: Precision Nudge Domain Model And State Contract
**Task Title:** Precision Nudge Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support precision nudge.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support keyboard nudges, big-step moves, and coordinate editing for exact placement.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T133: Precision Nudge Commands Events And Context Keys
**Task Title:** Precision Nudge Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for precision nudge.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support keyboard nudges, big-step moves, and coordinate editing for exact placement. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T134: Precision Nudge Workspace Surface And Controls
**Task Title:** Precision Nudge Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose precision nudge in the main canvas workspace.
**Description:** Build the primary UI surfaces for support keyboard nudges, big-step moves, and coordinate editing for exact placement., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T135: Precision Nudge Pointer Interaction Flow
**Task Title:** Precision Nudge Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for precision nudge.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support keyboard nudges, big-step moves, and coordinate editing for exact placement.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T136: Precision Nudge Keyboard Gesture And Shortcut Flow
**Task Title:** Precision Nudge Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for precision nudge.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support keyboard nudges, big-step moves, and coordinate editing for exact placement.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T137: Precision Nudge Visual Feedback And Rendering Polish
**Task Title:** Precision Nudge Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make precision nudge legible in use.
**Description:** Implement the visible feedback for support keyboard nudges, big-step moves, and coordinate editing for exact placement.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T138: Precision Nudge Persistence Preferences And Serialization
**Task Title:** Precision Nudge Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by precision nudge.
**Description:** Identify what parts of support keyboard nudges, big-step moves, and coordinate editing for exact placement. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T139: Precision Nudge Safeguards Telemetry And Recovery
**Task Title:** Precision Nudge Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for precision nudge.
**Description:** Instrument support keyboard nudges, big-step moves, and coordinate editing for exact placement. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

### P02-T140: Precision Nudge Tests Documentation And Rollout Gate
**Task Title:** Precision Nudge Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship precision nudge.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support keyboard nudges, big-step moves, and coordinate editing for exact placement.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** arrow-key move; shift-arrow larger step; x/y inspector edit.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KeyboardCommandModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h

## Workstream 15: Duplicate And Paste In Place
Make duplication preserve intent, ordering, and connector relationships.

### P02-T141: Duplicate And Paste In Place UX Parity Audit
**Task Title:** Duplicate And Paste In Place UX Parity Audit
**Definition:** Define the implementation contract for duplicate and paste in place by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make duplication preserve intent, ordering, and connector relationships.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T142: Duplicate And Paste In Place Domain Model And State Contract
**Task Title:** Duplicate And Paste In Place Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support duplicate and paste in place.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make duplication preserve intent, ordering, and connector relationships.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T143: Duplicate And Paste In Place Commands Events And Context Keys
**Task Title:** Duplicate And Paste In Place Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for duplicate and paste in place.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make duplication preserve intent, ordering, and connector relationships. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T144: Duplicate And Paste In Place Workspace Surface And Controls
**Task Title:** Duplicate And Paste In Place Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose duplicate and paste in place in the main canvas workspace.
**Description:** Build the primary UI surfaces for make duplication preserve intent, ordering, and connector relationships., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T145: Duplicate And Paste In Place Pointer Interaction Flow
**Task Title:** Duplicate And Paste In Place Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for duplicate and paste in place.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make duplication preserve intent, ordering, and connector relationships.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T146: Duplicate And Paste In Place Keyboard Gesture And Shortcut Flow
**Task Title:** Duplicate And Paste In Place Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for duplicate and paste in place.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make duplication preserve intent, ordering, and connector relationships.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T147: Duplicate And Paste In Place Visual Feedback And Rendering Polish
**Task Title:** Duplicate And Paste In Place Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make duplicate and paste in place legible in use.
**Description:** Implement the visible feedback for make duplication preserve intent, ordering, and connector relationships.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T148: Duplicate And Paste In Place Persistence Preferences And Serialization
**Task Title:** Duplicate And Paste In Place Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by duplicate and paste in place.
**Description:** Identify what parts of make duplication preserve intent, ordering, and connector relationships. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T149: Duplicate And Paste In Place Safeguards Telemetry And Recovery
**Task Title:** Duplicate And Paste In Place Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for duplicate and paste in place.
**Description:** Instrument make duplication preserve intent, ordering, and connector relationships. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

### P02-T150: Duplicate And Paste In Place Tests Documentation And Rollout Gate
**Task Title:** Duplicate And Paste In Place Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship duplicate and paste in place.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make duplication preserve intent, ordering, and connector relationships.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** duplicate with offset; paste in place; clone connected card cluster.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ClipboardModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ClipboardService.cpp

## Workstream 16: Drag Autoscroll
Keep transforms fluid when users move objects beyond the current viewport edge.

### P02-T151: Drag Autoscroll UX Parity Audit
**Task Title:** Drag Autoscroll UX Parity Audit
**Definition:** Define the implementation contract for drag autoscroll by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around keep transforms fluid when users move objects beyond the current viewport edge.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T152: Drag Autoscroll Domain Model And State Contract
**Task Title:** Drag Autoscroll Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support drag autoscroll.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around keep transforms fluid when users move objects beyond the current viewport edge.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T153: Drag Autoscroll Commands Events And Context Keys
**Task Title:** Drag Autoscroll Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for drag autoscroll.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make keep transforms fluid when users move objects beyond the current viewport edge. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T154: Drag Autoscroll Workspace Surface And Controls
**Task Title:** Drag Autoscroll Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose drag autoscroll in the main canvas workspace.
**Description:** Build the primary UI surfaces for keep transforms fluid when users move objects beyond the current viewport edge., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T155: Drag Autoscroll Pointer Interaction Flow
**Task Title:** Drag Autoscroll Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for drag autoscroll.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for keep transforms fluid when users move objects beyond the current viewport edge.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T156: Drag Autoscroll Keyboard Gesture And Shortcut Flow
**Task Title:** Drag Autoscroll Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for drag autoscroll.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for keep transforms fluid when users move objects beyond the current viewport edge.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T157: Drag Autoscroll Visual Feedback And Rendering Polish
**Task Title:** Drag Autoscroll Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make drag autoscroll legible in use.
**Description:** Implement the visible feedback for keep transforms fluid when users move objects beyond the current viewport edge.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T158: Drag Autoscroll Persistence Preferences And Serialization
**Task Title:** Drag Autoscroll Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by drag autoscroll.
**Description:** Identify what parts of keep transforms fluid when users move objects beyond the current viewport edge. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T159: Drag Autoscroll Safeguards Telemetry And Recovery
**Task Title:** Drag Autoscroll Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for drag autoscroll.
**Description:** Instrument keep transforms fluid when users move objects beyond the current viewport edge. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

### P02-T160: Drag Autoscroll Tests Documentation And Rollout Gate
**Task Title:** Drag Autoscroll Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship drag autoscroll.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for keep transforms fluid when users move objects beyond the current viewport edge.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** edge autoscroll; speed ramps with proximity; auto-pan while drawing selection box.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

## Workstream 17: Viewport-Aware Transforms
Prevent camera changes from breaking object transforms on very large boards.

### P02-T161: Viewport-Aware Transforms UX Parity Audit
**Task Title:** Viewport-Aware Transforms UX Parity Audit
**Definition:** Define the implementation contract for viewport-aware transforms by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around prevent camera changes from breaking object transforms on very large boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T162: Viewport-Aware Transforms Domain Model And State Contract
**Task Title:** Viewport-Aware Transforms Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support viewport-aware transforms.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around prevent camera changes from breaking object transforms on very large boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T163: Viewport-Aware Transforms Commands Events And Context Keys
**Task Title:** Viewport-Aware Transforms Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for viewport-aware transforms.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make prevent camera changes from breaking object transforms on very large boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T164: Viewport-Aware Transforms Workspace Surface And Controls
**Task Title:** Viewport-Aware Transforms Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose viewport-aware transforms in the main canvas workspace.
**Description:** Build the primary UI surfaces for prevent camera changes from breaking object transforms on very large boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T165: Viewport-Aware Transforms Pointer Interaction Flow
**Task Title:** Viewport-Aware Transforms Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for viewport-aware transforms.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for prevent camera changes from breaking object transforms on very large boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T166: Viewport-Aware Transforms Keyboard Gesture And Shortcut Flow
**Task Title:** Viewport-Aware Transforms Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for viewport-aware transforms.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for prevent camera changes from breaking object transforms on very large boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T167: Viewport-Aware Transforms Visual Feedback And Rendering Polish
**Task Title:** Viewport-Aware Transforms Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make viewport-aware transforms legible in use.
**Description:** Implement the visible feedback for prevent camera changes from breaking object transforms on very large boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T168: Viewport-Aware Transforms Persistence Preferences And Serialization
**Task Title:** Viewport-Aware Transforms Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by viewport-aware transforms.
**Description:** Identify what parts of prevent camera changes from breaking object transforms on very large boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T169: Viewport-Aware Transforms Safeguards Telemetry And Recovery
**Task Title:** Viewport-Aware Transforms Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for viewport-aware transforms.
**Description:** Instrument prevent camera changes from breaking object transforms on very large boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P02-T170: Viewport-Aware Transforms Tests Documentation And Rollout Gate
**Task Title:** Viewport-Aware Transforms Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship viewport-aware transforms.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for prevent camera changes from breaking object transforms on very large boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** zoom while dragging; pan mid-transform; retain handle tracking.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

## Workstream 18: Multi User Selection
Surface remote locks and live transforms so multiplayer editing stays legible.

### P02-T171: Multi User Selection UX Parity Audit
**Task Title:** Multi User Selection UX Parity Audit
**Definition:** Define the implementation contract for multi user selection by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around surface remote locks and live transforms so multiplayer editing stays legible.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T172: Multi User Selection Domain Model And State Contract
**Task Title:** Multi User Selection Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support multi user selection.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around surface remote locks and live transforms so multiplayer editing stays legible.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T173: Multi User Selection Commands Events And Context Keys
**Task Title:** Multi User Selection Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for multi user selection.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make surface remote locks and live transforms so multiplayer editing stays legible. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T174: Multi User Selection Workspace Surface And Controls
**Task Title:** Multi User Selection Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose multi user selection in the main canvas workspace.
**Description:** Build the primary UI surfaces for surface remote locks and live transforms so multiplayer editing stays legible., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T175: Multi User Selection Pointer Interaction Flow
**Task Title:** Multi User Selection Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for multi user selection.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for surface remote locks and live transforms so multiplayer editing stays legible.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T176: Multi User Selection Keyboard Gesture And Shortcut Flow
**Task Title:** Multi User Selection Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for multi user selection.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for surface remote locks and live transforms so multiplayer editing stays legible.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T177: Multi User Selection Visual Feedback And Rendering Polish
**Task Title:** Multi User Selection Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make multi user selection legible in use.
**Description:** Implement the visible feedback for surface remote locks and live transforms so multiplayer editing stays legible.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T178: Multi User Selection Persistence Preferences And Serialization
**Task Title:** Multi User Selection Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by multi user selection.
**Description:** Identify what parts of surface remote locks and live transforms so multiplayer editing stays legible. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T179: Multi User Selection Safeguards Telemetry And Recovery
**Task Title:** Multi User Selection Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for multi user selection.
**Description:** Instrument surface remote locks and live transforms so multiplayer editing stays legible. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

### P02-T180: Multi User Selection Tests Documentation And Rollout Gate
**Task Title:** Multi User Selection Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship multi user selection.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for surface remote locks and live transforms so multiplayer editing stays legible.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** remote selection outline; conflict warning on same object; follow collaborator transform.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp

## Workstream 19: Transform History Quality
Tune history granularity so undo feels human, not mechanical.

### P02-T181: Transform History Quality UX Parity Audit
**Task Title:** Transform History Quality UX Parity Audit
**Definition:** Define the implementation contract for transform history quality by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around tune history granularity so undo feels human, not mechanical.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T182: Transform History Quality Domain Model And State Contract
**Task Title:** Transform History Quality Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support transform history quality.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around tune history granularity so undo feels human, not mechanical.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T183: Transform History Quality Commands Events And Context Keys
**Task Title:** Transform History Quality Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for transform history quality.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make tune history granularity so undo feels human, not mechanical. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T184: Transform History Quality Workspace Surface And Controls
**Task Title:** Transform History Quality Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose transform history quality in the main canvas workspace.
**Description:** Build the primary UI surfaces for tune history granularity so undo feels human, not mechanical., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T185: Transform History Quality Pointer Interaction Flow
**Task Title:** Transform History Quality Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for transform history quality.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for tune history granularity so undo feels human, not mechanical.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T186: Transform History Quality Keyboard Gesture And Shortcut Flow
**Task Title:** Transform History Quality Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for transform history quality.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for tune history granularity so undo feels human, not mechanical.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T187: Transform History Quality Visual Feedback And Rendering Polish
**Task Title:** Transform History Quality Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make transform history quality legible in use.
**Description:** Implement the visible feedback for tune history granularity so undo feels human, not mechanical.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T188: Transform History Quality Persistence Preferences And Serialization
**Task Title:** Transform History Quality Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by transform history quality.
**Description:** Identify what parts of tune history granularity so undo feels human, not mechanical. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T189: Transform History Quality Safeguards Telemetry And Recovery
**Task Title:** Transform History Quality Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for transform history quality.
**Description:** Instrument tune history granularity so undo feels human, not mechanical. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

### P02-T190: Transform History Quality Tests Documentation And Rollout Gate
**Task Title:** Transform History Quality Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship transform history quality.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for tune history granularity so undo feels human, not mechanical.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** single undo for full drag; separate undo for rotate and resize; history labels.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp

## Workstream 20: Selection Action Surfaces
Expose the highest-value transform actions in context-aware mini toolbars.

### P02-T191: Selection Action Surfaces UX Parity Audit
**Task Title:** Selection Action Surfaces UX Parity Audit
**Definition:** Define the implementation contract for selection action surfaces by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose the highest-value transform actions in context-aware mini toolbars.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T192: Selection Action Surfaces Domain Model And State Contract
**Task Title:** Selection Action Surfaces Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support selection action surfaces.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose the highest-value transform actions in context-aware mini toolbars.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T193: Selection Action Surfaces Commands Events And Context Keys
**Task Title:** Selection Action Surfaces Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for selection action surfaces.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose the highest-value transform actions in context-aware mini toolbars. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T194: Selection Action Surfaces Workspace Surface And Controls
**Task Title:** Selection Action Surfaces Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose selection action surfaces in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose the highest-value transform actions in context-aware mini toolbars., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T195: Selection Action Surfaces Pointer Interaction Flow
**Task Title:** Selection Action Surfaces Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for selection action surfaces.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose the highest-value transform actions in context-aware mini toolbars.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T196: Selection Action Surfaces Keyboard Gesture And Shortcut Flow
**Task Title:** Selection Action Surfaces Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for selection action surfaces.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose the highest-value transform actions in context-aware mini toolbars.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T197: Selection Action Surfaces Visual Feedback And Rendering Polish
**Task Title:** Selection Action Surfaces Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make selection action surfaces legible in use.
**Description:** Implement the visible feedback for expose the highest-value transform actions in context-aware mini toolbars.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T198: Selection Action Surfaces Persistence Preferences And Serialization
**Task Title:** Selection Action Surfaces Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by selection action surfaces.
**Description:** Identify what parts of expose the highest-value transform actions in context-aware mini toolbars. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T199: Selection Action Surfaces Safeguards Telemetry And Recovery
**Task Title:** Selection Action Surfaces Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for selection action surfaces.
**Description:** Instrument expose the highest-value transform actions in context-aware mini toolbars. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P02-T200: Selection Action Surfaces Tests Documentation And Rollout Gate
**Task Title:** Selection Action Surfaces Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship selection action surfaces.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose the highest-value transform actions in context-aware mini toolbars.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** floating action bar; selection quick align; bulk style and arrange.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GeometryHandleController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LayeringService.cpp
- /Users/ryanrentfro/code/markamp/src/ui/DragAutoScroll.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SelectionActionBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FloatingToolbarManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
