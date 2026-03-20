# Phase 07: Navigation Search Organization And Large Board Wayfinding

## Objective
Make large boards explorable through minimaps, outlines, search, metadata, navigation history, and saved viewpoints.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Minimap Navigation: Make the minimap accurate, interactive, and useful at both small and huge scales.
- W02 Outline Navigation: Turn the outline into a serious structural navigator, not just an object list.
- W03 Search And Find: Support fast search across text, labels, comments, and metadata directly on boards.
- W04 Tagging And Filters: Help users isolate board slices using tags, object types, and collaboration state.
- W05 Sections And Landmarks: Use structural landmarks to anchor orientation on large canvases.
- W06 Large Board Streaming: Keep navigation responsive as object counts and board size increase.
- W07 Breadcrumbs And History: Track where users have been so they can recover orientation quickly.
- W08 Zoom Presets: Expose practical zoom presets and fit modes instead of raw percentage control alone.
- W09 Board Bookmarks: Let users save camera states and important regions for repeat navigation.
- W10 Metadata Driven Navigation: Surface owner, status, and object type data as navigation aids.
- W11 Cross Board Traversal: Make linked boards feel like part of one navigable workspace.
- W12 Selection Sync: Keep side panels and navigation surfaces synced with the active canvas context.
- W13 Saved Views: Package complex board states into reusable named views.
- W14 Presentation Navigation: Bridge working-board navigation and presentation-style movement through a board.
- W15 Semantic Navigation: Use clustering and object meaning to help users move through idea-heavy boards.
- W16 Quick Action Navigation: Let users navigate by intent, not only by visual scanning.
- W17 Discoverability: Improve the learnability of navigation controls and shortcuts.
- W18 Responsive Layouts: Keep navigation surfaces usable across narrow windows and high-density desktops.
- W19 Wayfinding Telemetry: Measure where users get lost so navigation work stays grounded in real friction.
- W20 Navigation Coverage: Build coverage around large-board travel, search, and synchronized panels.

## Workstream 01: Minimap Navigation
Make the minimap accurate, interactive, and useful at both small and huge scales.

### P07-T001: Minimap Navigation UX Parity Audit
**Task Title:** Minimap Navigation UX Parity Audit
**Definition:** Define the implementation contract for minimap navigation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make the minimap accurate, interactive, and useful at both small and huge scales.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** viewport drag; section labels; selection markers.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T002: Minimap Navigation Domain Model And State Contract
**Task Title:** Minimap Navigation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support minimap navigation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make the minimap accurate, interactive, and useful at both small and huge scales.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** viewport drag; section labels; selection markers.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T003: Minimap Navigation Commands Events And Context Keys
**Task Title:** Minimap Navigation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for minimap navigation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make the minimap accurate, interactive, and useful at both small and huge scales. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** viewport drag; section labels; selection markers.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T004: Minimap Navigation Workspace Surface And Controls
**Task Title:** Minimap Navigation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose minimap navigation in the main canvas workspace.
**Description:** Build the primary UI surfaces for make the minimap accurate, interactive, and useful at both small and huge scales., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** viewport drag; section labels; selection markers.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T005: Minimap Navigation Pointer Interaction Flow
**Task Title:** Minimap Navigation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for minimap navigation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make the minimap accurate, interactive, and useful at both small and huge scales.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** viewport drag; section labels; selection markers.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T006: Minimap Navigation Keyboard Gesture And Shortcut Flow
**Task Title:** Minimap Navigation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for minimap navigation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make the minimap accurate, interactive, and useful at both small and huge scales.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** viewport drag; section labels; selection markers.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T007: Minimap Navigation Visual Feedback And Rendering Polish
**Task Title:** Minimap Navigation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make minimap navigation legible in use.
**Description:** Implement the visible feedback for make the minimap accurate, interactive, and useful at both small and huge scales.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** viewport drag; section labels; selection markers.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T008: Minimap Navigation Persistence Preferences And Serialization
**Task Title:** Minimap Navigation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by minimap navigation.
**Description:** Identify what parts of make the minimap accurate, interactive, and useful at both small and huge scales. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** viewport drag; section labels; selection markers.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T009: Minimap Navigation Safeguards Telemetry And Recovery
**Task Title:** Minimap Navigation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for minimap navigation.
**Description:** Instrument make the minimap accurate, interactive, and useful at both small and huge scales. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** viewport drag; section labels; selection markers.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

### P07-T010: Minimap Navigation Tests Documentation And Rollout Gate
**Task Title:** Minimap Navigation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship minimap navigation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make the minimap accurate, interactive, and useful at both small and huge scales.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** viewport drag; section labels; selection markers.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.h

## Workstream 02: Outline Navigation
Turn the outline into a serious structural navigator, not just an object list.

### P07-T011: Outline Navigation UX Parity Audit
**Task Title:** Outline Navigation UX Parity Audit
**Definition:** Define the implementation contract for outline navigation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn the outline into a serious structural navigator, not just an object list.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T012: Outline Navigation Domain Model And State Contract
**Task Title:** Outline Navigation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support outline navigation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn the outline into a serious structural navigator, not just an object list.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T013: Outline Navigation Commands Events And Context Keys
**Task Title:** Outline Navigation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for outline navigation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn the outline into a serious structural navigator, not just an object list. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T014: Outline Navigation Workspace Surface And Controls
**Task Title:** Outline Navigation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose outline navigation in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn the outline into a serious structural navigator, not just an object list., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T015: Outline Navigation Pointer Interaction Flow
**Task Title:** Outline Navigation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for outline navigation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn the outline into a serious structural navigator, not just an object list.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T016: Outline Navigation Keyboard Gesture And Shortcut Flow
**Task Title:** Outline Navigation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for outline navigation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn the outline into a serious structural navigator, not just an object list.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T017: Outline Navigation Visual Feedback And Rendering Polish
**Task Title:** Outline Navigation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make outline navigation legible in use.
**Description:** Implement the visible feedback for turn the outline into a serious structural navigator, not just an object list.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T018: Outline Navigation Persistence Preferences And Serialization
**Task Title:** Outline Navigation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by outline navigation.
**Description:** Identify what parts of turn the outline into a serious structural navigator, not just an object list. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T019: Outline Navigation Safeguards Telemetry And Recovery
**Task Title:** Outline Navigation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for outline navigation.
**Description:** Instrument turn the outline into a serious structural navigator, not just an object list. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

### P07-T020: Outline Navigation Tests Documentation And Rollout Gate
**Task Title:** Outline Navigation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship outline navigation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn the outline into a serious structural navigator, not just an object list.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** hierarchy by frame; inline rename from outline; jump to selected object.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.h
- /Users/ryanrentfro/code/markamp/src/canvas/GroupObject.h

## Workstream 03: Search And Find
Support fast search across text, labels, comments, and metadata directly on boards.

### P07-T021: Search And Find UX Parity Audit
**Task Title:** Search And Find UX Parity Audit
**Definition:** Define the implementation contract for search and find by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support fast search across text, labels, comments, and metadata directly on boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T022: Search And Find Domain Model And State Contract
**Task Title:** Search And Find Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support search and find.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support fast search across text, labels, comments, and metadata directly on boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T023: Search And Find Commands Events And Context Keys
**Task Title:** Search And Find Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for search and find.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support fast search across text, labels, comments, and metadata directly on boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T024: Search And Find Workspace Surface And Controls
**Task Title:** Search And Find Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose search and find in the main canvas workspace.
**Description:** Build the primary UI surfaces for support fast search across text, labels, comments, and metadata directly on boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T025: Search And Find Pointer Interaction Flow
**Task Title:** Search And Find Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for search and find.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support fast search across text, labels, comments, and metadata directly on boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T026: Search And Find Keyboard Gesture And Shortcut Flow
**Task Title:** Search And Find Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for search and find.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support fast search across text, labels, comments, and metadata directly on boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T027: Search And Find Visual Feedback And Rendering Polish
**Task Title:** Search And Find Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make search and find legible in use.
**Description:** Implement the visible feedback for support fast search across text, labels, comments, and metadata directly on boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T028: Search And Find Persistence Preferences And Serialization
**Task Title:** Search And Find Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by search and find.
**Description:** Identify what parts of support fast search across text, labels, comments, and metadata directly on boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T029: Search And Find Safeguards Telemetry And Recovery
**Task Title:** Search And Find Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for search and find.
**Description:** Instrument support fast search across text, labels, comments, and metadata directly on boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

### P07-T030: Search And Find Tests Documentation And Rollout Gate
**Task Title:** Search And Find Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship search and find.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support fast search across text, labels, comments, and metadata directly on boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** find note text; jump to next match; highlight all results.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearchModel.h
- /Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.cpp

## Workstream 04: Tagging And Filters
Help users isolate board slices using tags, object types, and collaboration state.

### P07-T031: Tagging And Filters UX Parity Audit
**Task Title:** Tagging And Filters UX Parity Audit
**Definition:** Define the implementation contract for tagging and filters by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around help users isolate board slices using tags, object types, and collaboration state.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T032: Tagging And Filters Domain Model And State Contract
**Task Title:** Tagging And Filters Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support tagging and filters.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around help users isolate board slices using tags, object types, and collaboration state.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T033: Tagging And Filters Commands Events And Context Keys
**Task Title:** Tagging And Filters Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for tagging and filters.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make help users isolate board slices using tags, object types, and collaboration state. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T034: Tagging And Filters Workspace Surface And Controls
**Task Title:** Tagging And Filters Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose tagging and filters in the main canvas workspace.
**Description:** Build the primary UI surfaces for help users isolate board slices using tags, object types, and collaboration state., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T035: Tagging And Filters Pointer Interaction Flow
**Task Title:** Tagging And Filters Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for tagging and filters.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for help users isolate board slices using tags, object types, and collaboration state.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T036: Tagging And Filters Keyboard Gesture And Shortcut Flow
**Task Title:** Tagging And Filters Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for tagging and filters.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for help users isolate board slices using tags, object types, and collaboration state.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T037: Tagging And Filters Visual Feedback And Rendering Polish
**Task Title:** Tagging And Filters Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make tagging and filters legible in use.
**Description:** Implement the visible feedback for help users isolate board slices using tags, object types, and collaboration state.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T038: Tagging And Filters Persistence Preferences And Serialization
**Task Title:** Tagging And Filters Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by tagging and filters.
**Description:** Identify what parts of help users isolate board slices using tags, object types, and collaboration state. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T039: Tagging And Filters Safeguards Telemetry And Recovery
**Task Title:** Tagging And Filters Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for tagging and filters.
**Description:** Instrument help users isolate board slices using tags, object types, and collaboration state. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

### P07-T040: Tagging And Filters Tests Documentation And Rollout Gate
**Task Title:** Tagging And Filters Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship tagging and filters.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for help users isolate board slices using tags, object types, and collaboration state.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** filter by tag; show only unresolved comments; filter by owner.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TagPanel.h
- /Users/ryanrentfro/code/markamp/src/core/Tag.h

## Workstream 05: Sections And Landmarks
Use structural landmarks to anchor orientation on large canvases.

### P07-T041: Sections And Landmarks UX Parity Audit
**Task Title:** Sections And Landmarks UX Parity Audit
**Definition:** Define the implementation contract for sections and landmarks by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around use structural landmarks to anchor orientation on large canvases.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T042: Sections And Landmarks Domain Model And State Contract
**Task Title:** Sections And Landmarks Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support sections and landmarks.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around use structural landmarks to anchor orientation on large canvases.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T043: Sections And Landmarks Commands Events And Context Keys
**Task Title:** Sections And Landmarks Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for sections and landmarks.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make use structural landmarks to anchor orientation on large canvases. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T044: Sections And Landmarks Workspace Surface And Controls
**Task Title:** Sections And Landmarks Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose sections and landmarks in the main canvas workspace.
**Description:** Build the primary UI surfaces for use structural landmarks to anchor orientation on large canvases., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T045: Sections And Landmarks Pointer Interaction Flow
**Task Title:** Sections And Landmarks Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for sections and landmarks.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for use structural landmarks to anchor orientation on large canvases.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T046: Sections And Landmarks Keyboard Gesture And Shortcut Flow
**Task Title:** Sections And Landmarks Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for sections and landmarks.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for use structural landmarks to anchor orientation on large canvases.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T047: Sections And Landmarks Visual Feedback And Rendering Polish
**Task Title:** Sections And Landmarks Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make sections and landmarks legible in use.
**Description:** Implement the visible feedback for use structural landmarks to anchor orientation on large canvases.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T048: Sections And Landmarks Persistence Preferences And Serialization
**Task Title:** Sections And Landmarks Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by sections and landmarks.
**Description:** Identify what parts of use structural landmarks to anchor orientation on large canvases. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T049: Sections And Landmarks Safeguards Telemetry And Recovery
**Task Title:** Sections And Landmarks Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for sections and landmarks.
**Description:** Instrument use structural landmarks to anchor orientation on large canvases. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

### P07-T050: Sections And Landmarks Tests Documentation And Rollout Gate
**Task Title:** Sections And Landmarks Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship sections and landmarks.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for use structural landmarks to anchor orientation on large canvases.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** section navigator; landmark headers; jump to frame.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.cpp

## Workstream 06: Large Board Streaming
Keep navigation responsive as object counts and board size increase.

### P07-T051: Large Board Streaming UX Parity Audit
**Task Title:** Large Board Streaming UX Parity Audit
**Definition:** Define the implementation contract for large board streaming by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around keep navigation responsive as object counts and board size increase.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T052: Large Board Streaming Domain Model And State Contract
**Task Title:** Large Board Streaming Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support large board streaming.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around keep navigation responsive as object counts and board size increase.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T053: Large Board Streaming Commands Events And Context Keys
**Task Title:** Large Board Streaming Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for large board streaming.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make keep navigation responsive as object counts and board size increase. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T054: Large Board Streaming Workspace Surface And Controls
**Task Title:** Large Board Streaming Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose large board streaming in the main canvas workspace.
**Description:** Build the primary UI surfaces for keep navigation responsive as object counts and board size increase., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T055: Large Board Streaming Pointer Interaction Flow
**Task Title:** Large Board Streaming Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for large board streaming.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for keep navigation responsive as object counts and board size increase.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T056: Large Board Streaming Keyboard Gesture And Shortcut Flow
**Task Title:** Large Board Streaming Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for large board streaming.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for keep navigation responsive as object counts and board size increase.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T057: Large Board Streaming Visual Feedback And Rendering Polish
**Task Title:** Large Board Streaming Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make large board streaming legible in use.
**Description:** Implement the visible feedback for keep navigation responsive as object counts and board size increase.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T058: Large Board Streaming Persistence Preferences And Serialization
**Task Title:** Large Board Streaming Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by large board streaming.
**Description:** Identify what parts of keep navigation responsive as object counts and board size increase. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T059: Large Board Streaming Safeguards Telemetry And Recovery
**Task Title:** Large Board Streaming Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for large board streaming.
**Description:** Instrument keep navigation responsive as object counts and board size increase. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

### P07-T060: Large Board Streaming Tests Documentation And Rollout Gate
**Task Title:** Large Board Streaming Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship large board streaming.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for keep navigation responsive as object counts and board size increase.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** lazy render offscreen objects; stream metadata into outline; fast camera jumps.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/Quadtree.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.h
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.h

## Workstream 07: Breadcrumbs And History
Track where users have been so they can recover orientation quickly.

### P07-T061: Breadcrumbs And History UX Parity Audit
**Task Title:** Breadcrumbs And History UX Parity Audit
**Definition:** Define the implementation contract for breadcrumbs and history by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around track where users have been so they can recover orientation quickly.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T062: Breadcrumbs And History Domain Model And State Contract
**Task Title:** Breadcrumbs And History Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support breadcrumbs and history.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around track where users have been so they can recover orientation quickly.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T063: Breadcrumbs And History Commands Events And Context Keys
**Task Title:** Breadcrumbs And History Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for breadcrumbs and history.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make track where users have been so they can recover orientation quickly. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T064: Breadcrumbs And History Workspace Surface And Controls
**Task Title:** Breadcrumbs And History Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose breadcrumbs and history in the main canvas workspace.
**Description:** Build the primary UI surfaces for track where users have been so they can recover orientation quickly., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T065: Breadcrumbs And History Pointer Interaction Flow
**Task Title:** Breadcrumbs And History Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for breadcrumbs and history.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for track where users have been so they can recover orientation quickly.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T066: Breadcrumbs And History Keyboard Gesture And Shortcut Flow
**Task Title:** Breadcrumbs And History Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for breadcrumbs and history.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for track where users have been so they can recover orientation quickly.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T067: Breadcrumbs And History Visual Feedback And Rendering Polish
**Task Title:** Breadcrumbs And History Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make breadcrumbs and history legible in use.
**Description:** Implement the visible feedback for track where users have been so they can recover orientation quickly.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T068: Breadcrumbs And History Persistence Preferences And Serialization
**Task Title:** Breadcrumbs And History Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by breadcrumbs and history.
**Description:** Identify what parts of track where users have been so they can recover orientation quickly. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T069: Breadcrumbs And History Safeguards Telemetry And Recovery
**Task Title:** Breadcrumbs And History Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for breadcrumbs and history.
**Description:** Instrument track where users have been so they can recover orientation quickly. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

### P07-T070: Breadcrumbs And History Tests Documentation And Rollout Gate
**Task Title:** Breadcrumbs And History Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship breadcrumbs and history.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for track where users have been so they can recover orientation quickly.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** board breadcrumb; back to previous view; open recent frames.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkNavigator.h

## Workstream 08: Zoom Presets
Expose practical zoom presets and fit modes instead of raw percentage control alone.

### P07-T071: Zoom Presets UX Parity Audit
**Task Title:** Zoom Presets UX Parity Audit
**Definition:** Define the implementation contract for zoom presets by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose practical zoom presets and fit modes instead of raw percentage control alone.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** fit board; fit selection; 100 percent.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T072: Zoom Presets Domain Model And State Contract
**Task Title:** Zoom Presets Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support zoom presets.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose practical zoom presets and fit modes instead of raw percentage control alone.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** fit board; fit selection; 100 percent.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T073: Zoom Presets Commands Events And Context Keys
**Task Title:** Zoom Presets Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for zoom presets.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose practical zoom presets and fit modes instead of raw percentage control alone. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** fit board; fit selection; 100 percent.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T074: Zoom Presets Workspace Surface And Controls
**Task Title:** Zoom Presets Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose zoom presets in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose practical zoom presets and fit modes instead of raw percentage control alone., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** fit board; fit selection; 100 percent.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T075: Zoom Presets Pointer Interaction Flow
**Task Title:** Zoom Presets Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for zoom presets.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose practical zoom presets and fit modes instead of raw percentage control alone.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** fit board; fit selection; 100 percent.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T076: Zoom Presets Keyboard Gesture And Shortcut Flow
**Task Title:** Zoom Presets Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for zoom presets.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose practical zoom presets and fit modes instead of raw percentage control alone.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** fit board; fit selection; 100 percent.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T077: Zoom Presets Visual Feedback And Rendering Polish
**Task Title:** Zoom Presets Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make zoom presets legible in use.
**Description:** Implement the visible feedback for expose practical zoom presets and fit modes instead of raw percentage control alone.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** fit board; fit selection; 100 percent.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T078: Zoom Presets Persistence Preferences And Serialization
**Task Title:** Zoom Presets Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by zoom presets.
**Description:** Identify what parts of expose practical zoom presets and fit modes instead of raw percentage control alone. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** fit board; fit selection; 100 percent.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T079: Zoom Presets Safeguards Telemetry And Recovery
**Task Title:** Zoom Presets Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for zoom presets.
**Description:** Instrument expose practical zoom presets and fit modes instead of raw percentage control alone. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** fit board; fit selection; 100 percent.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

### P07-T080: Zoom Presets Tests Documentation And Rollout Gate
**Task Title:** Zoom Presets Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship zoom presets.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose practical zoom presets and fit modes instead of raw percentage control alone.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** fit board; fit selection; 100 percent.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ViewportTransform.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CameraModel.cpp

## Workstream 09: Board Bookmarks
Let users save camera states and important regions for repeat navigation.

### P07-T081: Board Bookmarks UX Parity Audit
**Task Title:** Board Bookmarks UX Parity Audit
**Definition:** Define the implementation contract for board bookmarks by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around let users save camera states and important regions for repeat navigation.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T082: Board Bookmarks Domain Model And State Contract
**Task Title:** Board Bookmarks Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support board bookmarks.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around let users save camera states and important regions for repeat navigation.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T083: Board Bookmarks Commands Events And Context Keys
**Task Title:** Board Bookmarks Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for board bookmarks.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make let users save camera states and important regions for repeat navigation. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T084: Board Bookmarks Workspace Surface And Controls
**Task Title:** Board Bookmarks Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose board bookmarks in the main canvas workspace.
**Description:** Build the primary UI surfaces for let users save camera states and important regions for repeat navigation., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T085: Board Bookmarks Pointer Interaction Flow
**Task Title:** Board Bookmarks Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for board bookmarks.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for let users save camera states and important regions for repeat navigation.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T086: Board Bookmarks Keyboard Gesture And Shortcut Flow
**Task Title:** Board Bookmarks Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for board bookmarks.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for let users save camera states and important regions for repeat navigation.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T087: Board Bookmarks Visual Feedback And Rendering Polish
**Task Title:** Board Bookmarks Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make board bookmarks legible in use.
**Description:** Implement the visible feedback for let users save camera states and important regions for repeat navigation.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T088: Board Bookmarks Persistence Preferences And Serialization
**Task Title:** Board Bookmarks Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by board bookmarks.
**Description:** Identify what parts of let users save camera states and important regions for repeat navigation. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T089: Board Bookmarks Safeguards Telemetry And Recovery
**Task Title:** Board Bookmarks Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for board bookmarks.
**Description:** Instrument let users save camera states and important regions for repeat navigation. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

### P07-T090: Board Bookmarks Tests Documentation And Rollout Gate
**Task Title:** Board Bookmarks Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship board bookmarks.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for let users save camera states and important regions for repeat navigation.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** bookmark current view; named board waypoint; share view link.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/BookmarkService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h

## Workstream 10: Metadata Driven Navigation
Surface owner, status, and object type data as navigation aids.

### P07-T091: Metadata Driven Navigation UX Parity Audit
**Task Title:** Metadata Driven Navigation UX Parity Audit
**Definition:** Define the implementation contract for metadata driven navigation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around surface owner, status, and object type data as navigation aids.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T092: Metadata Driven Navigation Domain Model And State Contract
**Task Title:** Metadata Driven Navigation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support metadata driven navigation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around surface owner, status, and object type data as navigation aids.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T093: Metadata Driven Navigation Commands Events And Context Keys
**Task Title:** Metadata Driven Navigation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for metadata driven navigation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make surface owner, status, and object type data as navigation aids. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T094: Metadata Driven Navigation Workspace Surface And Controls
**Task Title:** Metadata Driven Navigation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose metadata driven navigation in the main canvas workspace.
**Description:** Build the primary UI surfaces for surface owner, status, and object type data as navigation aids., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T095: Metadata Driven Navigation Pointer Interaction Flow
**Task Title:** Metadata Driven Navigation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for metadata driven navigation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for surface owner, status, and object type data as navigation aids.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T096: Metadata Driven Navigation Keyboard Gesture And Shortcut Flow
**Task Title:** Metadata Driven Navigation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for metadata driven navigation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for surface owner, status, and object type data as navigation aids.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T097: Metadata Driven Navigation Visual Feedback And Rendering Polish
**Task Title:** Metadata Driven Navigation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make metadata driven navigation legible in use.
**Description:** Implement the visible feedback for surface owner, status, and object type data as navigation aids.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T098: Metadata Driven Navigation Persistence Preferences And Serialization
**Task Title:** Metadata Driven Navigation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by metadata driven navigation.
**Description:** Identify what parts of surface owner, status, and object type data as navigation aids. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T099: Metadata Driven Navigation Safeguards Telemetry And Recovery
**Task Title:** Metadata Driven Navigation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for metadata driven navigation.
**Description:** Instrument surface owner, status, and object type data as navigation aids. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

### P07-T100: Metadata Driven Navigation Tests Documentation And Rollout Gate
**Task Title:** Metadata Driven Navigation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship metadata driven navigation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for surface owner, status, and object type data as navigation aids.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** jump to overdue area; navigate by assignee; find all decision nodes.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.h

## Workstream 11: Cross Board Traversal
Make linked boards feel like part of one navigable workspace.

### P07-T101: Cross Board Traversal UX Parity Audit
**Task Title:** Cross Board Traversal UX Parity Audit
**Definition:** Define the implementation contract for cross board traversal by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make linked boards feel like part of one navigable workspace.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T102: Cross Board Traversal Domain Model And State Contract
**Task Title:** Cross Board Traversal Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support cross board traversal.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make linked boards feel like part of one navigable workspace.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T103: Cross Board Traversal Commands Events And Context Keys
**Task Title:** Cross Board Traversal Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for cross board traversal.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make linked boards feel like part of one navigable workspace. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T104: Cross Board Traversal Workspace Surface And Controls
**Task Title:** Cross Board Traversal Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose cross board traversal in the main canvas workspace.
**Description:** Build the primary UI surfaces for make linked boards feel like part of one navigable workspace., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T105: Cross Board Traversal Pointer Interaction Flow
**Task Title:** Cross Board Traversal Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for cross board traversal.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make linked boards feel like part of one navigable workspace.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T106: Cross Board Traversal Keyboard Gesture And Shortcut Flow
**Task Title:** Cross Board Traversal Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for cross board traversal.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make linked boards feel like part of one navigable workspace.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T107: Cross Board Traversal Visual Feedback And Rendering Polish
**Task Title:** Cross Board Traversal Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make cross board traversal legible in use.
**Description:** Implement the visible feedback for make linked boards feel like part of one navigable workspace.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T108: Cross Board Traversal Persistence Preferences And Serialization
**Task Title:** Cross Board Traversal Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by cross board traversal.
**Description:** Identify what parts of make linked boards feel like part of one navigable workspace. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T109: Cross Board Traversal Safeguards Telemetry And Recovery
**Task Title:** Cross Board Traversal Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for cross board traversal.
**Description:** Instrument make linked boards feel like part of one navigable workspace. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

### P07-T110: Cross Board Traversal Tests Documentation And Rollout Gate
**Task Title:** Cross Board Traversal Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship cross board traversal.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make linked boards feel like part of one navigable workspace.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** open linked board in split; back to source board; peek linked board.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.h
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/ui/EditorGroupManager.cpp

## Workstream 12: Selection Sync
Keep side panels and navigation surfaces synced with the active canvas context.

### P07-T111: Selection Sync UX Parity Audit
**Task Title:** Selection Sync UX Parity Audit
**Definition:** Define the implementation contract for selection sync by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around keep side panels and navigation surfaces synced with the active canvas context.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T112: Selection Sync Domain Model And State Contract
**Task Title:** Selection Sync Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support selection sync.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around keep side panels and navigation surfaces synced with the active canvas context.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T113: Selection Sync Commands Events And Context Keys
**Task Title:** Selection Sync Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for selection sync.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make keep side panels and navigation surfaces synced with the active canvas context. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T114: Selection Sync Workspace Surface And Controls
**Task Title:** Selection Sync Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose selection sync in the main canvas workspace.
**Description:** Build the primary UI surfaces for keep side panels and navigation surfaces synced with the active canvas context., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T115: Selection Sync Pointer Interaction Flow
**Task Title:** Selection Sync Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for selection sync.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for keep side panels and navigation surfaces synced with the active canvas context.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T116: Selection Sync Keyboard Gesture And Shortcut Flow
**Task Title:** Selection Sync Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for selection sync.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for keep side panels and navigation surfaces synced with the active canvas context.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T117: Selection Sync Visual Feedback And Rendering Polish
**Task Title:** Selection Sync Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make selection sync legible in use.
**Description:** Implement the visible feedback for keep side panels and navigation surfaces synced with the active canvas context.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T118: Selection Sync Persistence Preferences And Serialization
**Task Title:** Selection Sync Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by selection sync.
**Description:** Identify what parts of keep side panels and navigation surfaces synced with the active canvas context. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T119: Selection Sync Safeguards Telemetry And Recovery
**Task Title:** Selection Sync Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for selection sync.
**Description:** Instrument keep side panels and navigation surfaces synced with the active canvas context. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

### P07-T120: Selection Sync Tests Documentation And Rollout Gate
**Task Title:** Selection Sync Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship selection sync.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for keep side panels and navigation surfaces synced with the active canvas context.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** outline follows selection; inspector reveals selected object; minimap highlights selection.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapModel.cpp

## Workstream 13: Saved Views
Package complex board states into reusable named views.

### P07-T121: Saved Views UX Parity Audit
**Task Title:** Saved Views UX Parity Audit
**Definition:** Define the implementation contract for saved views by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around package complex board states into reusable named views.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T122: Saved Views Domain Model And State Contract
**Task Title:** Saved Views Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support saved views.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around package complex board states into reusable named views.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T123: Saved Views Commands Events And Context Keys
**Task Title:** Saved Views Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for saved views.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make package complex board states into reusable named views. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T124: Saved Views Workspace Surface And Controls
**Task Title:** Saved Views Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose saved views in the main canvas workspace.
**Description:** Build the primary UI surfaces for package complex board states into reusable named views., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T125: Saved Views Pointer Interaction Flow
**Task Title:** Saved Views Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for saved views.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for package complex board states into reusable named views.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T126: Saved Views Keyboard Gesture And Shortcut Flow
**Task Title:** Saved Views Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for saved views.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for package complex board states into reusable named views.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T127: Saved Views Visual Feedback And Rendering Polish
**Task Title:** Saved Views Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make saved views legible in use.
**Description:** Implement the visible feedback for package complex board states into reusable named views.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T128: Saved Views Persistence Preferences And Serialization
**Task Title:** Saved Views Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by saved views.
**Description:** Identify what parts of package complex board states into reusable named views. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T129: Saved Views Safeguards Telemetry And Recovery
**Task Title:** Saved Views Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for saved views.
**Description:** Instrument package complex board states into reusable named views. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

### P07-T130: Saved Views Tests Documentation And Rollout Gate
**Task Title:** Saved Views Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship saved views.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for package complex board states into reusable named views.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** review mode view; presentation view; editing focus view.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasThemeModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardSerializerModel.h

## Workstream 14: Presentation Navigation
Bridge working-board navigation and presentation-style movement through a board.

### P07-T131: Presentation Navigation UX Parity Audit
**Task Title:** Presentation Navigation UX Parity Audit
**Definition:** Define the implementation contract for presentation navigation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around bridge working-board navigation and presentation-style movement through a board.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** next frame; focus path; present from section.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T132: Presentation Navigation Domain Model And State Contract
**Task Title:** Presentation Navigation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support presentation navigation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around bridge working-board navigation and presentation-style movement through a board.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** next frame; focus path; present from section.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T133: Presentation Navigation Commands Events And Context Keys
**Task Title:** Presentation Navigation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for presentation navigation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make bridge working-board navigation and presentation-style movement through a board. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** next frame; focus path; present from section.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T134: Presentation Navigation Workspace Surface And Controls
**Task Title:** Presentation Navigation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose presentation navigation in the main canvas workspace.
**Description:** Build the primary UI surfaces for bridge working-board navigation and presentation-style movement through a board., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** next frame; focus path; present from section.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T135: Presentation Navigation Pointer Interaction Flow
**Task Title:** Presentation Navigation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for presentation navigation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for bridge working-board navigation and presentation-style movement through a board.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** next frame; focus path; present from section.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T136: Presentation Navigation Keyboard Gesture And Shortcut Flow
**Task Title:** Presentation Navigation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for presentation navigation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for bridge working-board navigation and presentation-style movement through a board.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** next frame; focus path; present from section.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T137: Presentation Navigation Visual Feedback And Rendering Polish
**Task Title:** Presentation Navigation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make presentation navigation legible in use.
**Description:** Implement the visible feedback for bridge working-board navigation and presentation-style movement through a board.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** next frame; focus path; present from section.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T138: Presentation Navigation Persistence Preferences And Serialization
**Task Title:** Presentation Navigation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by presentation navigation.
**Description:** Identify what parts of bridge working-board navigation and presentation-style movement through a board. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** next frame; focus path; present from section.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T139: Presentation Navigation Safeguards Telemetry And Recovery
**Task Title:** Presentation Navigation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for presentation navigation.
**Description:** Instrument bridge working-board navigation and presentation-style movement through a board. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** next frame; focus path; present from section.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

### P07-T140: Presentation Navigation Tests Documentation And Rollout Gate
**Task Title:** Presentation Navigation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship presentation navigation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for bridge working-board navigation and presentation-style movement through a board.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** next frame; focus path; present from section.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.h
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.h

## Workstream 15: Semantic Navigation
Use clustering and object meaning to help users move through idea-heavy boards.

### P07-T141: Semantic Navigation UX Parity Audit
**Task Title:** Semantic Navigation UX Parity Audit
**Definition:** Define the implementation contract for semantic navigation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around use clustering and object meaning to help users move through idea-heavy boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T142: Semantic Navigation Domain Model And State Contract
**Task Title:** Semantic Navigation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support semantic navigation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around use clustering and object meaning to help users move through idea-heavy boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T143: Semantic Navigation Commands Events And Context Keys
**Task Title:** Semantic Navigation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for semantic navigation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make use clustering and object meaning to help users move through idea-heavy boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T144: Semantic Navigation Workspace Surface And Controls
**Task Title:** Semantic Navigation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose semantic navigation in the main canvas workspace.
**Description:** Build the primary UI surfaces for use clustering and object meaning to help users move through idea-heavy boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T145: Semantic Navigation Pointer Interaction Flow
**Task Title:** Semantic Navigation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for semantic navigation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for use clustering and object meaning to help users move through idea-heavy boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T146: Semantic Navigation Keyboard Gesture And Shortcut Flow
**Task Title:** Semantic Navigation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for semantic navigation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for use clustering and object meaning to help users move through idea-heavy boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T147: Semantic Navigation Visual Feedback And Rendering Polish
**Task Title:** Semantic Navigation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make semantic navigation legible in use.
**Description:** Implement the visible feedback for use clustering and object meaning to help users move through idea-heavy boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T148: Semantic Navigation Persistence Preferences And Serialization
**Task Title:** Semantic Navigation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by semantic navigation.
**Description:** Identify what parts of use clustering and object meaning to help users move through idea-heavy boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T149: Semantic Navigation Safeguards Telemetry And Recovery
**Task Title:** Semantic Navigation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for semantic navigation.
**Description:** Instrument use clustering and object meaning to help users move through idea-heavy boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P07-T150: Semantic Navigation Tests Documentation And Rollout Gate
**Task Title:** Semantic Navigation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship semantic navigation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for use clustering and object meaning to help users move through idea-heavy boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** navigate by cluster; jump to all decisions; find all connectors with warnings.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/GraphService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

## Workstream 16: Quick Action Navigation
Let users navigate by intent, not only by visual scanning.

### P07-T151: Quick Action Navigation UX Parity Audit
**Task Title:** Quick Action Navigation UX Parity Audit
**Definition:** Define the implementation contract for quick action navigation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around let users navigate by intent, not only by visual scanning.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T152: Quick Action Navigation Domain Model And State Contract
**Task Title:** Quick Action Navigation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support quick action navigation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around let users navigate by intent, not only by visual scanning.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T153: Quick Action Navigation Commands Events And Context Keys
**Task Title:** Quick Action Navigation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for quick action navigation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make let users navigate by intent, not only by visual scanning. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T154: Quick Action Navigation Workspace Surface And Controls
**Task Title:** Quick Action Navigation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose quick action navigation in the main canvas workspace.
**Description:** Build the primary UI surfaces for let users navigate by intent, not only by visual scanning., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T155: Quick Action Navigation Pointer Interaction Flow
**Task Title:** Quick Action Navigation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for quick action navigation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for let users navigate by intent, not only by visual scanning.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T156: Quick Action Navigation Keyboard Gesture And Shortcut Flow
**Task Title:** Quick Action Navigation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for quick action navigation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for let users navigate by intent, not only by visual scanning.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T157: Quick Action Navigation Visual Feedback And Rendering Polish
**Task Title:** Quick Action Navigation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make quick action navigation legible in use.
**Description:** Implement the visible feedback for let users navigate by intent, not only by visual scanning.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T158: Quick Action Navigation Persistence Preferences And Serialization
**Task Title:** Quick Action Navigation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by quick action navigation.
**Description:** Identify what parts of let users navigate by intent, not only by visual scanning. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T159: Quick Action Navigation Safeguards Telemetry And Recovery
**Task Title:** Quick Action Navigation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for quick action navigation.
**Description:** Instrument let users navigate by intent, not only by visual scanning. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

### P07-T160: Quick Action Navigation Tests Documentation And Rollout Gate
**Task Title:** Quick Action Navigation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship quick action navigation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for let users navigate by intent, not only by visual scanning.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** jump to unresolved comments; center newest objects; show all locked items.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h
- /Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityCommandProvider.cpp

## Workstream 17: Discoverability
Improve the learnability of navigation controls and shortcuts.

### P07-T161: Discoverability UX Parity Audit
**Task Title:** Discoverability UX Parity Audit
**Definition:** Define the implementation contract for discoverability by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around improve the learnability of navigation controls and shortcuts.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T162: Discoverability Domain Model And State Contract
**Task Title:** Discoverability Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support discoverability.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around improve the learnability of navigation controls and shortcuts.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T163: Discoverability Commands Events And Context Keys
**Task Title:** Discoverability Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for discoverability.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make improve the learnability of navigation controls and shortcuts. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T164: Discoverability Workspace Surface And Controls
**Task Title:** Discoverability Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose discoverability in the main canvas workspace.
**Description:** Build the primary UI surfaces for improve the learnability of navigation controls and shortcuts., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T165: Discoverability Pointer Interaction Flow
**Task Title:** Discoverability Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for discoverability.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for improve the learnability of navigation controls and shortcuts.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T166: Discoverability Keyboard Gesture And Shortcut Flow
**Task Title:** Discoverability Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for discoverability.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for improve the learnability of navigation controls and shortcuts.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T167: Discoverability Visual Feedback And Rendering Polish
**Task Title:** Discoverability Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make discoverability legible in use.
**Description:** Implement the visible feedback for improve the learnability of navigation controls and shortcuts.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T168: Discoverability Persistence Preferences And Serialization
**Task Title:** Discoverability Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by discoverability.
**Description:** Identify what parts of improve the learnability of navigation controls and shortcuts. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T169: Discoverability Safeguards Telemetry And Recovery
**Task Title:** Discoverability Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for discoverability.
**Description:** Instrument improve the learnability of navigation controls and shortcuts. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

### P07-T170: Discoverability Tests Documentation And Rollout Gate
**Task Title:** Discoverability Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship discoverability.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for improve the learnability of navigation controls and shortcuts.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** tooltip with shortcut; discover saved views; first-use zoom tips.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasControlModel.cpp

## Workstream 18: Responsive Layouts
Keep navigation surfaces usable across narrow windows and high-density desktops.

### P07-T171: Responsive Layouts UX Parity Audit
**Task Title:** Responsive Layouts UX Parity Audit
**Definition:** Define the implementation contract for responsive layouts by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around keep navigation surfaces usable across narrow windows and high-density desktops.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T172: Responsive Layouts Domain Model And State Contract
**Task Title:** Responsive Layouts Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support responsive layouts.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around keep navigation surfaces usable across narrow windows and high-density desktops.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T173: Responsive Layouts Commands Events And Context Keys
**Task Title:** Responsive Layouts Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for responsive layouts.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make keep navigation surfaces usable across narrow windows and high-density desktops. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T174: Responsive Layouts Workspace Surface And Controls
**Task Title:** Responsive Layouts Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose responsive layouts in the main canvas workspace.
**Description:** Build the primary UI surfaces for keep navigation surfaces usable across narrow windows and high-density desktops., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T175: Responsive Layouts Pointer Interaction Flow
**Task Title:** Responsive Layouts Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for responsive layouts.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for keep navigation surfaces usable across narrow windows and high-density desktops.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T176: Responsive Layouts Keyboard Gesture And Shortcut Flow
**Task Title:** Responsive Layouts Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for responsive layouts.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for keep navigation surfaces usable across narrow windows and high-density desktops.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T177: Responsive Layouts Visual Feedback And Rendering Polish
**Task Title:** Responsive Layouts Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make responsive layouts legible in use.
**Description:** Implement the visible feedback for keep navigation surfaces usable across narrow windows and high-density desktops.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T178: Responsive Layouts Persistence Preferences And Serialization
**Task Title:** Responsive Layouts Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by responsive layouts.
**Description:** Identify what parts of keep navigation surfaces usable across narrow windows and high-density desktops. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T179: Responsive Layouts Safeguards Telemetry And Recovery
**Task Title:** Responsive Layouts Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for responsive layouts.
**Description:** Instrument keep navigation surfaces usable across narrow windows and high-density desktops. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

### P07-T180: Responsive Layouts Tests Documentation And Rollout Gate
**Task Title:** Responsive Layouts Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship responsive layouts.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for keep navigation surfaces usable across narrow windows and high-density desktops.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** collapse minimap; dock outline; compact canvas chrome.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CompactLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ComponentSizeResolver.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h

## Workstream 19: Wayfinding Telemetry
Measure where users get lost so navigation work stays grounded in real friction.

### P07-T181: Wayfinding Telemetry UX Parity Audit
**Task Title:** Wayfinding Telemetry UX Parity Audit
**Definition:** Define the implementation contract for wayfinding telemetry by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around measure where users get lost so navigation work stays grounded in real friction.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T182: Wayfinding Telemetry Domain Model And State Contract
**Task Title:** Wayfinding Telemetry Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support wayfinding telemetry.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around measure where users get lost so navigation work stays grounded in real friction.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T183: Wayfinding Telemetry Commands Events And Context Keys
**Task Title:** Wayfinding Telemetry Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for wayfinding telemetry.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make measure where users get lost so navigation work stays grounded in real friction. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T184: Wayfinding Telemetry Workspace Surface And Controls
**Task Title:** Wayfinding Telemetry Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose wayfinding telemetry in the main canvas workspace.
**Description:** Build the primary UI surfaces for measure where users get lost so navigation work stays grounded in real friction., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T185: Wayfinding Telemetry Pointer Interaction Flow
**Task Title:** Wayfinding Telemetry Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for wayfinding telemetry.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for measure where users get lost so navigation work stays grounded in real friction.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T186: Wayfinding Telemetry Keyboard Gesture And Shortcut Flow
**Task Title:** Wayfinding Telemetry Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for wayfinding telemetry.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for measure where users get lost so navigation work stays grounded in real friction.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T187: Wayfinding Telemetry Visual Feedback And Rendering Polish
**Task Title:** Wayfinding Telemetry Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make wayfinding telemetry legible in use.
**Description:** Implement the visible feedback for measure where users get lost so navigation work stays grounded in real friction.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T188: Wayfinding Telemetry Persistence Preferences And Serialization
**Task Title:** Wayfinding Telemetry Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by wayfinding telemetry.
**Description:** Identify what parts of measure where users get lost so navigation work stays grounded in real friction. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T189: Wayfinding Telemetry Safeguards Telemetry And Recovery
**Task Title:** Wayfinding Telemetry Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for wayfinding telemetry.
**Description:** Instrument measure where users get lost so navigation work stays grounded in real friction. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

### P07-T190: Wayfinding Telemetry Tests Documentation And Rollout Gate
**Task Title:** Wayfinding Telemetry Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship wayfinding telemetry.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for measure where users get lost so navigation work stays grounded in real friction.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** frequent zoom thrash; repeat search without clickthrough; outline-open after long idle.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp
- /Users/ryanrentfro/code/markamp/src/core/AppState.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp

## Workstream 20: Navigation Coverage
Build coverage around large-board travel, search, and synchronized panels.

### P07-T191: Navigation Coverage UX Parity Audit
**Task Title:** Navigation Coverage UX Parity Audit
**Definition:** Define the implementation contract for navigation coverage by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around build coverage around large-board travel, search, and synchronized panels.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T192: Navigation Coverage Domain Model And State Contract
**Task Title:** Navigation Coverage Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support navigation coverage.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around build coverage around large-board travel, search, and synchronized panels.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T193: Navigation Coverage Commands Events And Context Keys
**Task Title:** Navigation Coverage Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for navigation coverage.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make build coverage around large-board travel, search, and synchronized panels. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T194: Navigation Coverage Workspace Surface And Controls
**Task Title:** Navigation Coverage Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose navigation coverage in the main canvas workspace.
**Description:** Build the primary UI surfaces for build coverage around large-board travel, search, and synchronized panels., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T195: Navigation Coverage Pointer Interaction Flow
**Task Title:** Navigation Coverage Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for navigation coverage.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for build coverage around large-board travel, search, and synchronized panels.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T196: Navigation Coverage Keyboard Gesture And Shortcut Flow
**Task Title:** Navigation Coverage Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for navigation coverage.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for build coverage around large-board travel, search, and synchronized panels.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T197: Navigation Coverage Visual Feedback And Rendering Polish
**Task Title:** Navigation Coverage Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make navigation coverage legible in use.
**Description:** Implement the visible feedback for build coverage around large-board travel, search, and synchronized panels.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T198: Navigation Coverage Persistence Preferences And Serialization
**Task Title:** Navigation Coverage Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by navigation coverage.
**Description:** Identify what parts of build coverage around large-board travel, search, and synchronized panels. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T199: Navigation Coverage Safeguards Telemetry And Recovery
**Task Title:** Navigation Coverage Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for navigation coverage.
**Description:** Instrument build coverage around large-board travel, search, and synchronized panels. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

### P07-T200: Navigation Coverage Tests Documentation And Rollout Gate
**Task Title:** Navigation Coverage Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship navigation coverage.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for build coverage around large-board travel, search, and synchronized panels.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** minimap jump test; search-result focus test; saved view round-trip.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasSearch.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LargeBoardModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.cpp

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
