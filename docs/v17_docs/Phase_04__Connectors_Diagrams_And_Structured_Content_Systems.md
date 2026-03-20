# Phase 04: Connectors Diagrams And Structured Content Systems

## Objective
Make connectors, diagrams, maps, structured content, and board organization strong enough for process design, systems thinking, and planning boards.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Connector Creation: Reduce connector creation friction across object types and board densities.
- W02 Connector Routing: Support straight, curved, and orthogonal routing with reliable reflow.
- W03 Connector Labels And Ports: Make relationships readable by exposing labels, port choice, and endpoint semantics.
- W04 Diagram Library: Offer a high-signal library of flowchart and systems shapes.
- W05 Flowchart Grammar: Encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.
- W06 Mind Maps: Make central-node ideation and branch editing feel purpose-built.
- W07 Kanban Boards: Support structured planning boards without forcing users into a separate product mode.
- W08 Frames And Sections: Use structural containers to organize large diagrams and workshop spaces.
- W09 Swimlanes And Grids: Add layout primitives for service design, journey maps, and process boards.
- W10 Container Auto Layout: Help diagrams stay tidy by letting containers manage spacing and order.
- W11 Advanced Tables: Increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.
- W12 Cross Board Links: Let creators connect related boards and navigate systems of boards.
- W13 Diagram From Selection: Accelerate structure creation from existing content clusters.
- W14 Connected Layout Cleanup: Preserve connector readability while users align, distribute, and move connected objects.
- W15 Relationship Metadata: Support metadata on links and diagrams so boards can carry richer meaning.
- W16 Diagram Templates: Seed structured content creation with reusable diagram starters.
- W17 Dependency Validation: Catch broken or invalid diagram states before they confuse users.
- W18 Diagram Export Readiness: Prepare structured boards for export without losing legibility.
- W19 Collaborative Diagram Editing: Make multi-user diagram editing feel safe and predictable.
- W20 Diagram Test Matrix: Build durable coverage for structured content interactions and regressions.

## Workstream 01: Connector Creation
Reduce connector creation friction across object types and board densities.

### P04-T001: Connector Creation UX Parity Audit
**Task Title:** Connector Creation UX Parity Audit
**Definition:** Define the implementation contract for connector creation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around reduce connector creation friction across object types and board densities.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T002: Connector Creation Domain Model And State Contract
**Task Title:** Connector Creation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support connector creation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around reduce connector creation friction across object types and board densities.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T003: Connector Creation Commands Events And Context Keys
**Task Title:** Connector Creation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for connector creation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make reduce connector creation friction across object types and board densities. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T004: Connector Creation Workspace Surface And Controls
**Task Title:** Connector Creation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose connector creation in the main canvas workspace.
**Description:** Build the primary UI surfaces for reduce connector creation friction across object types and board densities., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T005: Connector Creation Pointer Interaction Flow
**Task Title:** Connector Creation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for connector creation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for reduce connector creation friction across object types and board densities.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T006: Connector Creation Keyboard Gesture And Shortcut Flow
**Task Title:** Connector Creation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for connector creation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for reduce connector creation friction across object types and board densities.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T007: Connector Creation Visual Feedback And Rendering Polish
**Task Title:** Connector Creation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make connector creation legible in use.
**Description:** Implement the visible feedback for reduce connector creation friction across object types and board densities.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T008: Connector Creation Persistence Preferences And Serialization
**Task Title:** Connector Creation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by connector creation.
**Description:** Identify what parts of reduce connector creation friction across object types and board densities. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T009: Connector Creation Safeguards Telemetry And Recovery
**Task Title:** Connector Creation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for connector creation.
**Description:** Instrument reduce connector creation friction across object types and board densities. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

### P04-T010: Connector Creation Tests Documentation And Rollout Gate
**Task Title:** Connector Creation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship connector creation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for reduce connector creation friction across object types and board densities.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** drag from port; click-click connector; connector from quick action.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectLifecycleEvents.h

## Workstream 02: Connector Routing
Support straight, curved, and orthogonal routing with reliable reflow.

### P04-T011: Connector Routing UX Parity Audit
**Task Title:** Connector Routing UX Parity Audit
**Definition:** Define the implementation contract for connector routing by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support straight, curved, and orthogonal routing with reliable reflow.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T012: Connector Routing Domain Model And State Contract
**Task Title:** Connector Routing Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support connector routing.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support straight, curved, and orthogonal routing with reliable reflow.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T013: Connector Routing Commands Events And Context Keys
**Task Title:** Connector Routing Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for connector routing.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support straight, curved, and orthogonal routing with reliable reflow. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T014: Connector Routing Workspace Surface And Controls
**Task Title:** Connector Routing Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose connector routing in the main canvas workspace.
**Description:** Build the primary UI surfaces for support straight, curved, and orthogonal routing with reliable reflow., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T015: Connector Routing Pointer Interaction Flow
**Task Title:** Connector Routing Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for connector routing.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support straight, curved, and orthogonal routing with reliable reflow.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T016: Connector Routing Keyboard Gesture And Shortcut Flow
**Task Title:** Connector Routing Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for connector routing.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support straight, curved, and orthogonal routing with reliable reflow.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T017: Connector Routing Visual Feedback And Rendering Polish
**Task Title:** Connector Routing Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make connector routing legible in use.
**Description:** Implement the visible feedback for support straight, curved, and orthogonal routing with reliable reflow.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T018: Connector Routing Persistence Preferences And Serialization
**Task Title:** Connector Routing Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by connector routing.
**Description:** Identify what parts of support straight, curved, and orthogonal routing with reliable reflow. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T019: Connector Routing Safeguards Telemetry And Recovery
**Task Title:** Connector Routing Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for connector routing.
**Description:** Instrument support straight, curved, and orthogonal routing with reliable reflow. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

### P04-T020: Connector Routing Tests Documentation And Rollout Gate
**Task Title:** Connector Routing Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship connector routing.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support straight, curved, and orthogonal routing with reliable reflow.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** orthogonal elbow; curved line; reroute around obstacles.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRoutingService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.h

## Workstream 03: Connector Labels And Ports
Make relationships readable by exposing labels, port choice, and endpoint semantics.

### P04-T021: Connector Labels And Ports UX Parity Audit
**Task Title:** Connector Labels And Ports UX Parity Audit
**Definition:** Define the implementation contract for connector labels and ports by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make relationships readable by exposing labels, port choice, and endpoint semantics.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T022: Connector Labels And Ports Domain Model And State Contract
**Task Title:** Connector Labels And Ports Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support connector labels and ports.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make relationships readable by exposing labels, port choice, and endpoint semantics.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T023: Connector Labels And Ports Commands Events And Context Keys
**Task Title:** Connector Labels And Ports Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for connector labels and ports.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make relationships readable by exposing labels, port choice, and endpoint semantics. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T024: Connector Labels And Ports Workspace Surface And Controls
**Task Title:** Connector Labels And Ports Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose connector labels and ports in the main canvas workspace.
**Description:** Build the primary UI surfaces for make relationships readable by exposing labels, port choice, and endpoint semantics., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T025: Connector Labels And Ports Pointer Interaction Flow
**Task Title:** Connector Labels And Ports Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for connector labels and ports.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make relationships readable by exposing labels, port choice, and endpoint semantics.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T026: Connector Labels And Ports Keyboard Gesture And Shortcut Flow
**Task Title:** Connector Labels And Ports Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for connector labels and ports.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make relationships readable by exposing labels, port choice, and endpoint semantics.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T027: Connector Labels And Ports Visual Feedback And Rendering Polish
**Task Title:** Connector Labels And Ports Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make connector labels and ports legible in use.
**Description:** Implement the visible feedback for make relationships readable by exposing labels, port choice, and endpoint semantics.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T028: Connector Labels And Ports Persistence Preferences And Serialization
**Task Title:** Connector Labels And Ports Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by connector labels and ports.
**Description:** Identify what parts of make relationships readable by exposing labels, port choice, and endpoint semantics. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T029: Connector Labels And Ports Safeguards Telemetry And Recovery
**Task Title:** Connector Labels And Ports Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for connector labels and ports.
**Description:** Instrument make relationships readable by exposing labels, port choice, and endpoint semantics. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

### P04-T030: Connector Labels And Ports Tests Documentation And Rollout Gate
**Task Title:** Connector Labels And Ports Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship connector labels and ports.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make relationships readable by exposing labels, port choice, and endpoint semantics.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** label on connection; switch endpoint port; show directional arrowhead.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorAnchorModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.h

## Workstream 04: Diagram Library
Offer a high-signal library of flowchart and systems shapes.

### P04-T031: Diagram Library UX Parity Audit
**Task Title:** Diagram Library UX Parity Audit
**Definition:** Define the implementation contract for diagram library by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around offer a high-signal library of flowchart and systems shapes.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T032: Diagram Library Domain Model And State Contract
**Task Title:** Diagram Library Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support diagram library.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around offer a high-signal library of flowchart and systems shapes.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T033: Diagram Library Commands Events And Context Keys
**Task Title:** Diagram Library Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for diagram library.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make offer a high-signal library of flowchart and systems shapes. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T034: Diagram Library Workspace Surface And Controls
**Task Title:** Diagram Library Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose diagram library in the main canvas workspace.
**Description:** Build the primary UI surfaces for offer a high-signal library of flowchart and systems shapes., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T035: Diagram Library Pointer Interaction Flow
**Task Title:** Diagram Library Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for diagram library.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for offer a high-signal library of flowchart and systems shapes.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T036: Diagram Library Keyboard Gesture And Shortcut Flow
**Task Title:** Diagram Library Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for diagram library.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for offer a high-signal library of flowchart and systems shapes.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T037: Diagram Library Visual Feedback And Rendering Polish
**Task Title:** Diagram Library Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make diagram library legible in use.
**Description:** Implement the visible feedback for offer a high-signal library of flowchart and systems shapes.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T038: Diagram Library Persistence Preferences And Serialization
**Task Title:** Diagram Library Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by diagram library.
**Description:** Identify what parts of offer a high-signal library of flowchart and systems shapes. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T039: Diagram Library Safeguards Telemetry And Recovery
**Task Title:** Diagram Library Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for diagram library.
**Description:** Instrument offer a high-signal library of flowchart and systems shapes. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

### P04-T040: Diagram Library Tests Documentation And Rollout Gate
**Task Title:** Diagram Library Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship diagram library.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for offer a high-signal library of flowchart and systems shapes.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** flowchart diamond; database cylinder; service hexagon.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramShapeObject.h
- /Users/ryanrentfro/code/markamp/src/ui/IconRenderer.cpp

## Workstream 05: Flowchart Grammar
Encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.

### P04-T041: Flowchart Grammar UX Parity Audit
**Task Title:** Flowchart Grammar UX Parity Audit
**Definition:** Define the implementation contract for flowchart grammar by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T042: Flowchart Grammar Domain Model And State Contract
**Task Title:** Flowchart Grammar Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support flowchart grammar.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T043: Flowchart Grammar Commands Events And Context Keys
**Task Title:** Flowchart Grammar Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for flowchart grammar.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T044: Flowchart Grammar Workspace Surface And Controls
**Task Title:** Flowchart Grammar Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose flowchart grammar in the main canvas workspace.
**Description:** Build the primary UI surfaces for encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T045: Flowchart Grammar Pointer Interaction Flow
**Task Title:** Flowchart Grammar Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for flowchart grammar.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T046: Flowchart Grammar Keyboard Gesture And Shortcut Flow
**Task Title:** Flowchart Grammar Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for flowchart grammar.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T047: Flowchart Grammar Visual Feedback And Rendering Polish
**Task Title:** Flowchart Grammar Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make flowchart grammar legible in use.
**Description:** Implement the visible feedback for encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T048: Flowchart Grammar Persistence Preferences And Serialization
**Task Title:** Flowchart Grammar Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by flowchart grammar.
**Description:** Identify what parts of encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T049: Flowchart Grammar Safeguards Telemetry And Recovery
**Task Title:** Flowchart Grammar Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for flowchart grammar.
**Description:** Instrument encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

### P04-T050: Flowchart Grammar Tests Documentation And Rollout Gate
**Task Title:** Flowchart Grammar Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship flowchart grammar.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for encode shape defaults and connector semantics so diagrams are faster to build and cleaner to read.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** decision shape defaults; connector arrow presets; label placement rules.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp

## Workstream 06: Mind Maps
Make central-node ideation and branch editing feel purpose-built.

### P04-T051: Mind Maps UX Parity Audit
**Task Title:** Mind Maps UX Parity Audit
**Definition:** Define the implementation contract for mind maps by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make central-node ideation and branch editing feel purpose-built.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T052: Mind Maps Domain Model And State Contract
**Task Title:** Mind Maps Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support mind maps.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make central-node ideation and branch editing feel purpose-built.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T053: Mind Maps Commands Events And Context Keys
**Task Title:** Mind Maps Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for mind maps.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make central-node ideation and branch editing feel purpose-built. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T054: Mind Maps Workspace Surface And Controls
**Task Title:** Mind Maps Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose mind maps in the main canvas workspace.
**Description:** Build the primary UI surfaces for make central-node ideation and branch editing feel purpose-built., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T055: Mind Maps Pointer Interaction Flow
**Task Title:** Mind Maps Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for mind maps.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make central-node ideation and branch editing feel purpose-built.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T056: Mind Maps Keyboard Gesture And Shortcut Flow
**Task Title:** Mind Maps Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for mind maps.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make central-node ideation and branch editing feel purpose-built.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T057: Mind Maps Visual Feedback And Rendering Polish
**Task Title:** Mind Maps Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make mind maps legible in use.
**Description:** Implement the visible feedback for make central-node ideation and branch editing feel purpose-built.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T058: Mind Maps Persistence Preferences And Serialization
**Task Title:** Mind Maps Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by mind maps.
**Description:** Identify what parts of make central-node ideation and branch editing feel purpose-built. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T059: Mind Maps Safeguards Telemetry And Recovery
**Task Title:** Mind Maps Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for mind maps.
**Description:** Instrument make central-node ideation and branch editing feel purpose-built. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T060: Mind Maps Tests Documentation And Rollout Gate
**Task Title:** Mind Maps Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship mind maps.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make central-node ideation and branch editing feel purpose-built.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** tab adds child; enter adds sibling; collapse branch.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapNode.h
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

## Workstream 07: Kanban Boards
Support structured planning boards without forcing users into a separate product mode.

### P04-T061: Kanban Boards UX Parity Audit
**Task Title:** Kanban Boards UX Parity Audit
**Definition:** Define the implementation contract for kanban boards by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support structured planning boards without forcing users into a separate product mode.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T062: Kanban Boards Domain Model And State Contract
**Task Title:** Kanban Boards Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support kanban boards.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support structured planning boards without forcing users into a separate product mode.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T063: Kanban Boards Commands Events And Context Keys
**Task Title:** Kanban Boards Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for kanban boards.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support structured planning boards without forcing users into a separate product mode. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T064: Kanban Boards Workspace Surface And Controls
**Task Title:** Kanban Boards Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose kanban boards in the main canvas workspace.
**Description:** Build the primary UI surfaces for support structured planning boards without forcing users into a separate product mode., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T065: Kanban Boards Pointer Interaction Flow
**Task Title:** Kanban Boards Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for kanban boards.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support structured planning boards without forcing users into a separate product mode.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T066: Kanban Boards Keyboard Gesture And Shortcut Flow
**Task Title:** Kanban Boards Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for kanban boards.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support structured planning boards without forcing users into a separate product mode.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T067: Kanban Boards Visual Feedback And Rendering Polish
**Task Title:** Kanban Boards Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make kanban boards legible in use.
**Description:** Implement the visible feedback for support structured planning boards without forcing users into a separate product mode.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T068: Kanban Boards Persistence Preferences And Serialization
**Task Title:** Kanban Boards Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by kanban boards.
**Description:** Identify what parts of support structured planning boards without forcing users into a separate product mode. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T069: Kanban Boards Safeguards Telemetry And Recovery
**Task Title:** Kanban Boards Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for kanban boards.
**Description:** Instrument support structured planning boards without forcing users into a separate product mode. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

### P04-T070: Kanban Boards Tests Documentation And Rollout Gate
**Task Title:** Kanban Boards Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship kanban boards.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support structured planning boards without forcing users into a separate product mode.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** insert kanban; drag card between lanes; swimlane settings.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanObjects.h
- /Users/ryanrentfro/code/markamp/src/ui/av/AVKanbanPanel.cpp

## Workstream 08: Frames And Sections
Use structural containers to organize large diagrams and workshop spaces.

### P04-T071: Frames And Sections UX Parity Audit
**Task Title:** Frames And Sections UX Parity Audit
**Definition:** Define the implementation contract for frames and sections by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around use structural containers to organize large diagrams and workshop spaces.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** frame title; section color band; quick jump to section.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T072: Frames And Sections Domain Model And State Contract
**Task Title:** Frames And Sections Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support frames and sections.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around use structural containers to organize large diagrams and workshop spaces.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** frame title; section color band; quick jump to section.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T073: Frames And Sections Commands Events And Context Keys
**Task Title:** Frames And Sections Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for frames and sections.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make use structural containers to organize large diagrams and workshop spaces. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** frame title; section color band; quick jump to section.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T074: Frames And Sections Workspace Surface And Controls
**Task Title:** Frames And Sections Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose frames and sections in the main canvas workspace.
**Description:** Build the primary UI surfaces for use structural containers to organize large diagrams and workshop spaces., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** frame title; section color band; quick jump to section.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T075: Frames And Sections Pointer Interaction Flow
**Task Title:** Frames And Sections Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for frames and sections.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for use structural containers to organize large diagrams and workshop spaces.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** frame title; section color band; quick jump to section.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T076: Frames And Sections Keyboard Gesture And Shortcut Flow
**Task Title:** Frames And Sections Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for frames and sections.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for use structural containers to organize large diagrams and workshop spaces.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** frame title; section color band; quick jump to section.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T077: Frames And Sections Visual Feedback And Rendering Polish
**Task Title:** Frames And Sections Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make frames and sections legible in use.
**Description:** Implement the visible feedback for use structural containers to organize large diagrams and workshop spaces.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** frame title; section color band; quick jump to section.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T078: Frames And Sections Persistence Preferences And Serialization
**Task Title:** Frames And Sections Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by frames and sections.
**Description:** Identify what parts of use structural containers to organize large diagrams and workshop spaces. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** frame title; section color band; quick jump to section.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T079: Frames And Sections Safeguards Telemetry And Recovery
**Task Title:** Frames And Sections Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for frames and sections.
**Description:** Instrument use structural containers to organize large diagrams and workshop spaces. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** frame title; section color band; quick jump to section.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

### P04-T080: Frames And Sections Tests Documentation And Rollout Gate
**Task Title:** Frames And Sections Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship frames and sections.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for use structural containers to organize large diagrams and workshop spaces.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** frame title; section color band; quick jump to section.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameRenderer.h
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OutlinePanel.h

## Workstream 09: Swimlanes And Grids
Add layout primitives for service design, journey maps, and process boards.

### P04-T081: Swimlanes And Grids UX Parity Audit
**Task Title:** Swimlanes And Grids UX Parity Audit
**Definition:** Define the implementation contract for swimlanes and grids by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around add layout primitives for service design, journey maps, and process boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T082: Swimlanes And Grids Domain Model And State Contract
**Task Title:** Swimlanes And Grids Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support swimlanes and grids.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around add layout primitives for service design, journey maps, and process boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T083: Swimlanes And Grids Commands Events And Context Keys
**Task Title:** Swimlanes And Grids Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for swimlanes and grids.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make add layout primitives for service design, journey maps, and process boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T084: Swimlanes And Grids Workspace Surface And Controls
**Task Title:** Swimlanes And Grids Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose swimlanes and grids in the main canvas workspace.
**Description:** Build the primary UI surfaces for add layout primitives for service design, journey maps, and process boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T085: Swimlanes And Grids Pointer Interaction Flow
**Task Title:** Swimlanes And Grids Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for swimlanes and grids.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for add layout primitives for service design, journey maps, and process boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T086: Swimlanes And Grids Keyboard Gesture And Shortcut Flow
**Task Title:** Swimlanes And Grids Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for swimlanes and grids.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for add layout primitives for service design, journey maps, and process boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T087: Swimlanes And Grids Visual Feedback And Rendering Polish
**Task Title:** Swimlanes And Grids Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make swimlanes and grids legible in use.
**Description:** Implement the visible feedback for add layout primitives for service design, journey maps, and process boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T088: Swimlanes And Grids Persistence Preferences And Serialization
**Task Title:** Swimlanes And Grids Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by swimlanes and grids.
**Description:** Identify what parts of add layout primitives for service design, journey maps, and process boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T089: Swimlanes And Grids Safeguards Telemetry And Recovery
**Task Title:** Swimlanes And Grids Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for swimlanes and grids.
**Description:** Instrument add layout primitives for service design, journey maps, and process boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

### P04-T090: Swimlanes And Grids Tests Documentation And Rollout Gate
**Task Title:** Swimlanes And Grids Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship swimlanes and grids.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for add layout primitives for service design, journey maps, and process boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** lane headers; resizable lanes; grid templates.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/LayoutEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/GridGuideController.h

## Workstream 10: Container Auto Layout
Help diagrams stay tidy by letting containers manage spacing and order.

### P04-T091: Container Auto Layout UX Parity Audit
**Task Title:** Container Auto Layout UX Parity Audit
**Definition:** Define the implementation contract for container auto layout by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around help diagrams stay tidy by letting containers manage spacing and order.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T092: Container Auto Layout Domain Model And State Contract
**Task Title:** Container Auto Layout Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support container auto layout.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around help diagrams stay tidy by letting containers manage spacing and order.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T093: Container Auto Layout Commands Events And Context Keys
**Task Title:** Container Auto Layout Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for container auto layout.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make help diagrams stay tidy by letting containers manage spacing and order. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T094: Container Auto Layout Workspace Surface And Controls
**Task Title:** Container Auto Layout Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose container auto layout in the main canvas workspace.
**Description:** Build the primary UI surfaces for help diagrams stay tidy by letting containers manage spacing and order., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T095: Container Auto Layout Pointer Interaction Flow
**Task Title:** Container Auto Layout Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for container auto layout.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for help diagrams stay tidy by letting containers manage spacing and order.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T096: Container Auto Layout Keyboard Gesture And Shortcut Flow
**Task Title:** Container Auto Layout Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for container auto layout.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for help diagrams stay tidy by letting containers manage spacing and order.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T097: Container Auto Layout Visual Feedback And Rendering Polish
**Task Title:** Container Auto Layout Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make container auto layout legible in use.
**Description:** Implement the visible feedback for help diagrams stay tidy by letting containers manage spacing and order.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T098: Container Auto Layout Persistence Preferences And Serialization
**Task Title:** Container Auto Layout Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by container auto layout.
**Description:** Identify what parts of help diagrams stay tidy by letting containers manage spacing and order. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T099: Container Auto Layout Safeguards Telemetry And Recovery
**Task Title:** Container Auto Layout Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for container auto layout.
**Description:** Instrument help diagrams stay tidy by letting containers manage spacing and order. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

### P04-T100: Container Auto Layout Tests Documentation And Rollout Gate
**Task Title:** Container Auto Layout Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship container auto layout.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for help diagrams stay tidy by letting containers manage spacing and order.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** auto-stack cards; equal spacing in frame; wrap items within section.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameContainerModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.cpp

## Workstream 11: Advanced Tables
Increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.

### P04-T101: Advanced Tables UX Parity Audit
**Task Title:** Advanced Tables UX Parity Audit
**Definition:** Define the implementation contract for advanced tables by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T102: Advanced Tables Domain Model And State Contract
**Task Title:** Advanced Tables Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support advanced tables.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T103: Advanced Tables Commands Events And Context Keys
**Task Title:** Advanced Tables Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for advanced tables.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make increase the usefulness of canvas tables for comparison, tracking, and lightweight databases. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T104: Advanced Tables Workspace Surface And Controls
**Task Title:** Advanced Tables Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose advanced tables in the main canvas workspace.
**Description:** Build the primary UI surfaces for increase the usefulness of canvas tables for comparison, tracking, and lightweight databases., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T105: Advanced Tables Pointer Interaction Flow
**Task Title:** Advanced Tables Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for advanced tables.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T106: Advanced Tables Keyboard Gesture And Shortcut Flow
**Task Title:** Advanced Tables Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for advanced tables.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T107: Advanced Tables Visual Feedback And Rendering Polish
**Task Title:** Advanced Tables Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make advanced tables legible in use.
**Description:** Implement the visible feedback for increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T108: Advanced Tables Persistence Preferences And Serialization
**Task Title:** Advanced Tables Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by advanced tables.
**Description:** Identify what parts of increase the usefulness of canvas tables for comparison, tracking, and lightweight databases. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T109: Advanced Tables Safeguards Telemetry And Recovery
**Task Title:** Advanced Tables Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for advanced tables.
**Description:** Instrument increase the usefulness of canvas tables for comparison, tracking, and lightweight databases. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

### P04-T110: Advanced Tables Tests Documentation And Rollout Gate
**Task Title:** Advanced Tables Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship advanced tables.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for increase the usefulness of canvas tables for comparison, tracking, and lightweight databases.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** header freeze; reorder columns; cell alignment presets.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/TableRenderer.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp

## Workstream 12: Cross Board Links
Let creators connect related boards and navigate systems of boards.

### P04-T111: Cross Board Links UX Parity Audit
**Task Title:** Cross Board Links UX Parity Audit
**Definition:** Define the implementation contract for cross board links by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around let creators connect related boards and navigate systems of boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T112: Cross Board Links Domain Model And State Contract
**Task Title:** Cross Board Links Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support cross board links.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around let creators connect related boards and navigate systems of boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T113: Cross Board Links Commands Events And Context Keys
**Task Title:** Cross Board Links Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for cross board links.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make let creators connect related boards and navigate systems of boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T114: Cross Board Links Workspace Surface And Controls
**Task Title:** Cross Board Links Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose cross board links in the main canvas workspace.
**Description:** Build the primary UI surfaces for let creators connect related boards and navigate systems of boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T115: Cross Board Links Pointer Interaction Flow
**Task Title:** Cross Board Links Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for cross board links.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for let creators connect related boards and navigate systems of boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T116: Cross Board Links Keyboard Gesture And Shortcut Flow
**Task Title:** Cross Board Links Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for cross board links.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for let creators connect related boards and navigate systems of boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T117: Cross Board Links Visual Feedback And Rendering Polish
**Task Title:** Cross Board Links Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make cross board links legible in use.
**Description:** Implement the visible feedback for let creators connect related boards and navigate systems of boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T118: Cross Board Links Persistence Preferences And Serialization
**Task Title:** Cross Board Links Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by cross board links.
**Description:** Identify what parts of let creators connect related boards and navigate systems of boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T119: Cross Board Links Safeguards Telemetry And Recovery
**Task Title:** Cross Board Links Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for cross board links.
**Description:** Instrument let creators connect related boards and navigate systems of boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

### P04-T120: Cross Board Links Tests Documentation And Rollout Gate
**Task Title:** Cross Board Links Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship cross board links.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for let creators connect related boards and navigate systems of boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** link object to board; preview linked board; open in new tab.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CrossBoardLink.cpp
- /Users/ryanrentfro/code/markamp/src/core/CrossSurfaceRouter.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp

## Workstream 13: Diagram From Selection
Accelerate structure creation from existing content clusters.

### P04-T121: Diagram From Selection UX Parity Audit
**Task Title:** Diagram From Selection UX Parity Audit
**Definition:** Define the implementation contract for diagram from selection by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around accelerate structure creation from existing content clusters.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T122: Diagram From Selection Domain Model And State Contract
**Task Title:** Diagram From Selection Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support diagram from selection.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around accelerate structure creation from existing content clusters.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T123: Diagram From Selection Commands Events And Context Keys
**Task Title:** Diagram From Selection Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for diagram from selection.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make accelerate structure creation from existing content clusters. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T124: Diagram From Selection Workspace Surface And Controls
**Task Title:** Diagram From Selection Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose diagram from selection in the main canvas workspace.
**Description:** Build the primary UI surfaces for accelerate structure creation from existing content clusters., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T125: Diagram From Selection Pointer Interaction Flow
**Task Title:** Diagram From Selection Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for diagram from selection.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for accelerate structure creation from existing content clusters.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T126: Diagram From Selection Keyboard Gesture And Shortcut Flow
**Task Title:** Diagram From Selection Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for diagram from selection.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for accelerate structure creation from existing content clusters.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T127: Diagram From Selection Visual Feedback And Rendering Polish
**Task Title:** Diagram From Selection Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make diagram from selection legible in use.
**Description:** Implement the visible feedback for accelerate structure creation from existing content clusters.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T128: Diagram From Selection Persistence Preferences And Serialization
**Task Title:** Diagram From Selection Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by diagram from selection.
**Description:** Identify what parts of accelerate structure creation from existing content clusters. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T129: Diagram From Selection Safeguards Telemetry And Recovery
**Task Title:** Diagram From Selection Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for diagram from selection.
**Description:** Instrument accelerate structure creation from existing content clusters. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

### P04-T130: Diagram From Selection Tests Documentation And Rollout Gate
**Task Title:** Diagram From Selection Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship diagram from selection.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for accelerate structure creation from existing content clusters.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** convert notes to flowchart; auto-connect selected boxes; generate frame per cluster.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectFactory.h
- /Users/ryanrentfro/code/markamp/src/canvas/AutoLayoutModel.h

## Workstream 14: Connected Layout Cleanup
Preserve connector readability while users align, distribute, and move connected objects.

### P04-T131: Connected Layout Cleanup UX Parity Audit
**Task Title:** Connected Layout Cleanup UX Parity Audit
**Definition:** Define the implementation contract for connected layout cleanup by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around preserve connector readability while users align, distribute, and move connected objects.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T132: Connected Layout Cleanup Domain Model And State Contract
**Task Title:** Connected Layout Cleanup Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support connected layout cleanup.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around preserve connector readability while users align, distribute, and move connected objects.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T133: Connected Layout Cleanup Commands Events And Context Keys
**Task Title:** Connected Layout Cleanup Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for connected layout cleanup.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make preserve connector readability while users align, distribute, and move connected objects. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T134: Connected Layout Cleanup Workspace Surface And Controls
**Task Title:** Connected Layout Cleanup Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose connected layout cleanup in the main canvas workspace.
**Description:** Build the primary UI surfaces for preserve connector readability while users align, distribute, and move connected objects., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T135: Connected Layout Cleanup Pointer Interaction Flow
**Task Title:** Connected Layout Cleanup Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for connected layout cleanup.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for preserve connector readability while users align, distribute, and move connected objects.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T136: Connected Layout Cleanup Keyboard Gesture And Shortcut Flow
**Task Title:** Connected Layout Cleanup Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for connected layout cleanup.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for preserve connector readability while users align, distribute, and move connected objects.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T137: Connected Layout Cleanup Visual Feedback And Rendering Polish
**Task Title:** Connected Layout Cleanup Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make connected layout cleanup legible in use.
**Description:** Implement the visible feedback for preserve connector readability while users align, distribute, and move connected objects.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T138: Connected Layout Cleanup Persistence Preferences And Serialization
**Task Title:** Connected Layout Cleanup Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by connected layout cleanup.
**Description:** Identify what parts of preserve connector readability while users align, distribute, and move connected objects. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T139: Connected Layout Cleanup Safeguards Telemetry And Recovery
**Task Title:** Connected Layout Cleanup Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for connected layout cleanup.
**Description:** Instrument preserve connector readability while users align, distribute, and move connected objects. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

### P04-T140: Connected Layout Cleanup Tests Documentation And Rollout Gate
**Task Title:** Connected Layout Cleanup Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship connected layout cleanup.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for preserve connector readability while users align, distribute, and move connected objects.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** reroute after align; keep labels visible; respect locked endpoints.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasObjectAligner.h
- /Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp

## Workstream 15: Relationship Metadata
Support metadata on links and diagrams so boards can carry richer meaning.

### P04-T141: Relationship Metadata UX Parity Audit
**Task Title:** Relationship Metadata UX Parity Audit
**Definition:** Define the implementation contract for relationship metadata by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support metadata on links and diagrams so boards can carry richer meaning.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T142: Relationship Metadata Domain Model And State Contract
**Task Title:** Relationship Metadata Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support relationship metadata.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support metadata on links and diagrams so boards can carry richer meaning.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T143: Relationship Metadata Commands Events And Context Keys
**Task Title:** Relationship Metadata Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for relationship metadata.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support metadata on links and diagrams so boards can carry richer meaning. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T144: Relationship Metadata Workspace Surface And Controls
**Task Title:** Relationship Metadata Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose relationship metadata in the main canvas workspace.
**Description:** Build the primary UI surfaces for support metadata on links and diagrams so boards can carry richer meaning., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T145: Relationship Metadata Pointer Interaction Flow
**Task Title:** Relationship Metadata Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for relationship metadata.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support metadata on links and diagrams so boards can carry richer meaning.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T146: Relationship Metadata Keyboard Gesture And Shortcut Flow
**Task Title:** Relationship Metadata Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for relationship metadata.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support metadata on links and diagrams so boards can carry richer meaning.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T147: Relationship Metadata Visual Feedback And Rendering Polish
**Task Title:** Relationship Metadata Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make relationship metadata legible in use.
**Description:** Implement the visible feedback for support metadata on links and diagrams so boards can carry richer meaning.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T148: Relationship Metadata Persistence Preferences And Serialization
**Task Title:** Relationship Metadata Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by relationship metadata.
**Description:** Identify what parts of support metadata on links and diagrams so boards can carry richer meaning. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T149: Relationship Metadata Safeguards Telemetry And Recovery
**Task Title:** Relationship Metadata Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for relationship metadata.
**Description:** Instrument support metadata on links and diagrams so boards can carry richer meaning. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

### P04-T150: Relationship Metadata Tests Documentation And Rollout Gate
**Task Title:** Relationship Metadata Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship relationship metadata.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support metadata on links and diagrams so boards can carry richer meaning.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** connector type property; owner on flow step; status chip on node.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp
- /Users/ryanrentfro/code/markamp/src/core/AttributeService.cpp

## Workstream 16: Diagram Templates
Seed structured content creation with reusable diagram starters.

### P04-T151: Diagram Templates UX Parity Audit
**Task Title:** Diagram Templates UX Parity Audit
**Definition:** Define the implementation contract for diagram templates by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around seed structured content creation with reusable diagram starters.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T152: Diagram Templates Domain Model And State Contract
**Task Title:** Diagram Templates Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support diagram templates.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around seed structured content creation with reusable diagram starters.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T153: Diagram Templates Commands Events And Context Keys
**Task Title:** Diagram Templates Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for diagram templates.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make seed structured content creation with reusable diagram starters. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T154: Diagram Templates Workspace Surface And Controls
**Task Title:** Diagram Templates Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose diagram templates in the main canvas workspace.
**Description:** Build the primary UI surfaces for seed structured content creation with reusable diagram starters., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T155: Diagram Templates Pointer Interaction Flow
**Task Title:** Diagram Templates Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for diagram templates.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for seed structured content creation with reusable diagram starters.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T156: Diagram Templates Keyboard Gesture And Shortcut Flow
**Task Title:** Diagram Templates Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for diagram templates.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for seed structured content creation with reusable diagram starters.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T157: Diagram Templates Visual Feedback And Rendering Polish
**Task Title:** Diagram Templates Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make diagram templates legible in use.
**Description:** Implement the visible feedback for seed structured content creation with reusable diagram starters.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T158: Diagram Templates Persistence Preferences And Serialization
**Task Title:** Diagram Templates Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by diagram templates.
**Description:** Identify what parts of seed structured content creation with reusable diagram starters. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T159: Diagram Templates Safeguards Telemetry And Recovery
**Task Title:** Diagram Templates Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for diagram templates.
**Description:** Instrument seed structured content creation with reusable diagram starters. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

### P04-T160: Diagram Templates Tests Documentation And Rollout Gate
**Task Title:** Diagram Templates Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship diagram templates.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for seed structured content creation with reusable diagram starters.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** service blueprint template; sequence flow template; org chart starter.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramLibraryPanel.h

## Workstream 17: Dependency Validation
Catch broken or invalid diagram states before they confuse users.

### P04-T161: Dependency Validation UX Parity Audit
**Task Title:** Dependency Validation UX Parity Audit
**Definition:** Define the implementation contract for dependency validation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around catch broken or invalid diagram states before they confuse users.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T162: Dependency Validation Domain Model And State Contract
**Task Title:** Dependency Validation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support dependency validation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around catch broken or invalid diagram states before they confuse users.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T163: Dependency Validation Commands Events And Context Keys
**Task Title:** Dependency Validation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for dependency validation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make catch broken or invalid diagram states before they confuse users. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T164: Dependency Validation Workspace Surface And Controls
**Task Title:** Dependency Validation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose dependency validation in the main canvas workspace.
**Description:** Build the primary UI surfaces for catch broken or invalid diagram states before they confuse users., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T165: Dependency Validation Pointer Interaction Flow
**Task Title:** Dependency Validation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for dependency validation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for catch broken or invalid diagram states before they confuse users.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T166: Dependency Validation Keyboard Gesture And Shortcut Flow
**Task Title:** Dependency Validation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for dependency validation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for catch broken or invalid diagram states before they confuse users.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T167: Dependency Validation Visual Feedback And Rendering Polish
**Task Title:** Dependency Validation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make dependency validation legible in use.
**Description:** Implement the visible feedback for catch broken or invalid diagram states before they confuse users.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T168: Dependency Validation Persistence Preferences And Serialization
**Task Title:** Dependency Validation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by dependency validation.
**Description:** Identify what parts of catch broken or invalid diagram states before they confuse users. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T169: Dependency Validation Safeguards Telemetry And Recovery
**Task Title:** Dependency Validation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for dependency validation.
**Description:** Instrument catch broken or invalid diagram states before they confuse users. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

### P04-T170: Dependency Validation Tests Documentation And Rollout Gate
**Task Title:** Dependency Validation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship dependency validation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for catch broken or invalid diagram states before they confuse users.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** dangling connector warning; cyclic dependency marker; invalid port assignment.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorModel.h

## Workstream 18: Diagram Export Readiness
Prepare structured boards for export without losing legibility.

### P04-T171: Diagram Export Readiness UX Parity Audit
**Task Title:** Diagram Export Readiness UX Parity Audit
**Definition:** Define the implementation contract for diagram export readiness by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around prepare structured boards for export without losing legibility.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T172: Diagram Export Readiness Domain Model And State Contract
**Task Title:** Diagram Export Readiness Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support diagram export readiness.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around prepare structured boards for export without losing legibility.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T173: Diagram Export Readiness Commands Events And Context Keys
**Task Title:** Diagram Export Readiness Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for diagram export readiness.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make prepare structured boards for export without losing legibility. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T174: Diagram Export Readiness Workspace Surface And Controls
**Task Title:** Diagram Export Readiness Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose diagram export readiness in the main canvas workspace.
**Description:** Build the primary UI surfaces for prepare structured boards for export without losing legibility., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T175: Diagram Export Readiness Pointer Interaction Flow
**Task Title:** Diagram Export Readiness Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for diagram export readiness.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for prepare structured boards for export without losing legibility.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T176: Diagram Export Readiness Keyboard Gesture And Shortcut Flow
**Task Title:** Diagram Export Readiness Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for diagram export readiness.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for prepare structured boards for export without losing legibility.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T177: Diagram Export Readiness Visual Feedback And Rendering Polish
**Task Title:** Diagram Export Readiness Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make diagram export readiness legible in use.
**Description:** Implement the visible feedback for prepare structured boards for export without losing legibility.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T178: Diagram Export Readiness Persistence Preferences And Serialization
**Task Title:** Diagram Export Readiness Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by diagram export readiness.
**Description:** Identify what parts of prepare structured boards for export without losing legibility. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T179: Diagram Export Readiness Safeguards Telemetry And Recovery
**Task Title:** Diagram Export Readiness Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for diagram export readiness.
**Description:** Instrument prepare structured boards for export without losing legibility. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

### P04-T180: Diagram Export Readiness Tests Documentation And Rollout Gate
**Task Title:** Diagram Export Readiness Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship diagram export readiness.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for prepare structured boards for export without losing legibility.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** fit export bounds to frame; preserve connector labels; export transparent background.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp
- /Users/ryanrentfro/code/markamp/src/core/ExportService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ExportModel.h

## Workstream 19: Collaborative Diagram Editing
Make multi-user diagram editing feel safe and predictable.

### P04-T181: Collaborative Diagram Editing UX Parity Audit
**Task Title:** Collaborative Diagram Editing UX Parity Audit
**Definition:** Define the implementation contract for collaborative diagram editing by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make multi-user diagram editing feel safe and predictable.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T182: Collaborative Diagram Editing Domain Model And State Contract
**Task Title:** Collaborative Diagram Editing Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support collaborative diagram editing.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make multi-user diagram editing feel safe and predictable.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T183: Collaborative Diagram Editing Commands Events And Context Keys
**Task Title:** Collaborative Diagram Editing Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for collaborative diagram editing.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make multi-user diagram editing feel safe and predictable. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T184: Collaborative Diagram Editing Workspace Surface And Controls
**Task Title:** Collaborative Diagram Editing Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose collaborative diagram editing in the main canvas workspace.
**Description:** Build the primary UI surfaces for make multi-user diagram editing feel safe and predictable., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T185: Collaborative Diagram Editing Pointer Interaction Flow
**Task Title:** Collaborative Diagram Editing Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for collaborative diagram editing.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make multi-user diagram editing feel safe and predictable.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T186: Collaborative Diagram Editing Keyboard Gesture And Shortcut Flow
**Task Title:** Collaborative Diagram Editing Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for collaborative diagram editing.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make multi-user diagram editing feel safe and predictable.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T187: Collaborative Diagram Editing Visual Feedback And Rendering Polish
**Task Title:** Collaborative Diagram Editing Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make collaborative diagram editing legible in use.
**Description:** Implement the visible feedback for make multi-user diagram editing feel safe and predictable.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T188: Collaborative Diagram Editing Persistence Preferences And Serialization
**Task Title:** Collaborative Diagram Editing Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by collaborative diagram editing.
**Description:** Identify what parts of make multi-user diagram editing feel safe and predictable. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T189: Collaborative Diagram Editing Safeguards Telemetry And Recovery
**Task Title:** Collaborative Diagram Editing Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for collaborative diagram editing.
**Description:** Instrument make multi-user diagram editing feel safe and predictable. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

### P04-T190: Collaborative Diagram Editing Tests Documentation And Rollout Gate
**Task Title:** Collaborative Diagram Editing Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship collaborative diagram editing.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make multi-user diagram editing feel safe and predictable.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** remote connector reroute; shared node editing; conflict message on same label.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp

## Workstream 20: Diagram Test Matrix
Build durable coverage for structured content interactions and regressions.

### P04-T191: Diagram Test Matrix UX Parity Audit
**Task Title:** Diagram Test Matrix UX Parity Audit
**Definition:** Define the implementation contract for diagram test matrix by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around build durable coverage for structured content interactions and regressions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T192: Diagram Test Matrix Domain Model And State Contract
**Task Title:** Diagram Test Matrix Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support diagram test matrix.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around build durable coverage for structured content interactions and regressions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T193: Diagram Test Matrix Commands Events And Context Keys
**Task Title:** Diagram Test Matrix Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for diagram test matrix.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make build durable coverage for structured content interactions and regressions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T194: Diagram Test Matrix Workspace Surface And Controls
**Task Title:** Diagram Test Matrix Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose diagram test matrix in the main canvas workspace.
**Description:** Build the primary UI surfaces for build durable coverage for structured content interactions and regressions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T195: Diagram Test Matrix Pointer Interaction Flow
**Task Title:** Diagram Test Matrix Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for diagram test matrix.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for build durable coverage for structured content interactions and regressions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T196: Diagram Test Matrix Keyboard Gesture And Shortcut Flow
**Task Title:** Diagram Test Matrix Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for diagram test matrix.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for build durable coverage for structured content interactions and regressions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T197: Diagram Test Matrix Visual Feedback And Rendering Polish
**Task Title:** Diagram Test Matrix Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make diagram test matrix legible in use.
**Description:** Implement the visible feedback for build durable coverage for structured content interactions and regressions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T198: Diagram Test Matrix Persistence Preferences And Serialization
**Task Title:** Diagram Test Matrix Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by diagram test matrix.
**Description:** Identify what parts of build durable coverage for structured content interactions and regressions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T199: Diagram Test Matrix Safeguards Telemetry And Recovery
**Task Title:** Diagram Test Matrix Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for diagram test matrix.
**Description:** Instrument build durable coverage for structured content interactions and regressions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

### P04-T200: Diagram Test Matrix Tests Documentation And Rollout Gate
**Task Title:** Diagram Test Matrix Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship diagram test matrix.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for build durable coverage for structured content interactions and regressions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** connector round-trip tests; mind map layout tests; kanban drag regression suite.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorData.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/ConnectorRouteEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/DiagramCoordinator.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MindMapController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/KanbanController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FrameObject.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/canvas/BenchmarkModel.h
- /Users/ryanrentfro/code/markamp/src/canvas/IntegrityModel.h

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
