# Phase 08: Canvas Workbench And Cross Surface Integration

## Phase Purpose
Finish canvas as a true workbench surface that shares the shell’s command, event, panel, persistence, and navigation systems.

## Measurable Outcome
- Canvas no longer uses isolated event wiring.
- Board open and canvas mode transitions are shell-controlled.
- Canvas adjunct panels and cross-surface navigation behave coherently.

## Tasks

### P08-T01
- Phase ID: P08
- Task ID: P08-T01
- Task Class: Foundational
- Task Title: Remove Canvas Event Bus Isolation And Bind Canvas To The Shared Shell Runtime
- Priority: Critical
- Objective: Ensure canvas surfaces consume the same event bus and shell state as the rest of the application.
- Why This Matters Now: The current canvas workspace creates a separate event bus for `CanvasPanel`, which breaks integration by construction.
- Problem Statement: `CanvasWorkspacePanel` constructs `std::make_shared<core::EventBus>()` for `CanvasPanel`, isolating canvas object events from the main shell graph.
- Scope: canvas panel construction, board events, toolbar/tool events, object events, and shell-mode events.
- Out of Scope: Rewriting the internal canvas engine itself.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasWorkbenchMode.cpp`
- Related Systems / Components: canvas panel, shell event bus, board open, workbench mode, object lifecycle events.
- Current Behavior: Canvas workspace uses shared shell event bus at the outer panel level but an isolated bus for the actual canvas panel.
- Intended Behavior: Canvas emits and consumes the same shared shell events as the rest of the product.
- Technical Approach: Pass the real app event bus through canvas construction and remove local isolation unless there is a justified scoped bridge layer.
- Implementation Steps:
  1. Replace the local canvas event bus allocation with the shared bus.
  2. Audit canvas event consumers for assumptions about isolation.
  3. Add explicit namespacing or typed adapters only where event volume requires it.
  4. Verify canvas events are observable by shared shell services.
- Validation Steps:
  1. Change tools, add/remove objects, and watch shared diagnostics.
  2. Verify canvas adjunct panels react to board activity without custom glue.
- Acceptance Criteria: Canvas event flow is part of the main application event graph.
- Dependencies: P01-T01.
- Risks / Failure Modes: Event volume may expose performance problems once the canvas joins the main bus.
- UX Notes: This is a hidden but essential reliability fix for canvas-mode trust.
- Observability / Diagnostics Notes: Add canvas event counters and publish latency sampling once merged onto the shared bus.
- Rollback / Safety Notes: If direct shared-bus usage is too noisy, add a typed adapter layer rather than returning to total isolation.
- References / Context:
  - `CanvasWorkspacePanel.cpp` currently creates a new `EventBus` for `CanvasPanel`.
- Example Scenarios Where Useful:
  - A canvas selection change should be available to shell-level focus, status, and adjunct panel services without bespoke bridging.

### P08-T02
- Phase ID: P08
- Task ID: P08-T02
- Task Class: Workflow
- Task Title: Promote Board Open Canvas Mode And Session Context Into One Shell Flow
- Priority: High
- Objective: Make board open, canvas mode entry, active board state, and adjunct panel binding part of one real shell lifecycle.
- Why This Matters Now: Canvas commands exist, but the shell still treats canvas as a partial alternate view with fragile local state.
- Problem Statement: `CanvasWorkbenchMode`, `CanvasSessionContext`, `LayoutManager::ShowCanvasWorkspace()`, and board-open events are only partially connected.
- Scope: board open request, canvas mode activation, last board restore, active board title/state, adjunct panel rebinding.
- Out of Scope: Full multiplayer session UX.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasWorkbenchMode.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasSessionContext.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CrossSurfaceNavigator.cpp`
- Related Systems / Components: board open, canvas mode, shell controller, canvas session context, restore.
- Current Behavior: Canvas mode can show and hide, but board and session state are not yet part of one durable shell contract.
- Intended Behavior: Opening a board selects canvas mode, binds adjunct state, and restores the right board context on relaunch.
- Technical Approach: Route board activation through shell controller and make `CanvasWorkbenchMode` and `CanvasSessionContext` authoritative for board lifecycle.
- Implementation Steps:
  1. Define canonical board-open entry and active-board state.
  2. Connect board-open events to canvas mode entry.
  3. Persist and restore last board through session restore.
  4. Rebind comments, facilitation, inspector, and collaboration panels on board change.
- Validation Steps:
  1. Open board from palette and cross-surface navigation.
  2. Switch boards and verify adjunct panels reset and rebind correctly.
  3. Relaunch and verify last board restore.
- Acceptance Criteria: Canvas board selection and canvas mode are one shell workflow.
- Dependencies: P08-T01, P03-T02.
- Risks / Failure Modes: Board restore may run before canvas host construction unless sequencing is explicit.
- UX Notes: Canvas must feel like opening another serious workspace surface, not toggling a novelty mode.
- Observability / Diagnostics Notes: Trace board-open source, board-load duration, and adjunct panel bind results.
- Rollback / Safety Notes: If board restore fails, fall back to entering canvas mode with a clear empty state rather than stale bindings.
- References / Context:
  - `CanvasWorkbenchMode.cpp` currently persists board metadata but does not clearly own shell integration.
- Example Scenarios Where Useful:
  - Jumping from a backlink or command to a board should reliably enter canvas mode and focus the right board context.

### P08-T03
- Phase ID: P08
- Task ID: P08-T03
- Task Class: Workflow
- Task Title: Unify Canvas Selection Clipboard Undo And Cross Surface Navigation
- Priority: High
- Objective: Make canvas editing semantics feel consistent with the rest of the workbench.
- Why This Matters Now: Canvas functionality exists, but its shell interactions still feel separate from the editor and explorer worlds.
- Problem Statement: Canvas has its own selection, clipboard, and undo models, while shell-level navigation and command context do not yet treat canvas as first-class.
- Scope: active selection context, clipboard routing, undo/redo command dispatch, and navigation into canvas from other surfaces.
- Out of Scope: Deep canvas tool redesign.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/canvas/CanvasClipboardService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/canvas/UndoRedoStack.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CrossSurfaceNavigator.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasNavigationIntegration.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Systems / Components: selection context, clipboard service, undo/redo, cross-surface navigation, command dispatcher.
- Current Behavior: Canvas owns internal editing models, but shell-level command and navigation semantics are not fully harmonized.
- Intended Behavior: Undo/redo, copy/paste, and navigation work naturally when canvas is the active context.
- Technical Approach: Extend the active-context and command-dispatch system to treat canvas as a first-class command target.
- Implementation Steps:
  1. Publish canvas active-context changes into the command system.
  2. Route undo/redo and clipboard commands to canvas when canvas is active.
  3. Complete `CrossSurfaceNavigator` board navigation behavior.
  4. Ensure selection and focus updates propagate to relevant shell surfaces.
- Validation Steps:
  1. Use keyboard undo/redo and clipboard commands in canvas mode.
  2. Navigate to canvas from another surface and verify focus/context correctness.
- Acceptance Criteria: Canvas editing semantics match overall workbench expectations.
- Dependencies: P02-T02, P08-T01, P08-T02.
- Risks / Failure Modes: Context switching between editor and canvas may still race if focus ownership is weak.
- UX Notes: Users should not need to learn separate shell rules when they enter canvas mode.
- Observability / Diagnostics Notes: Record command-target resolution for undo/redo and clipboard actions.
- Rollback / Safety Notes: If command routing is uncertain, prefer explicit no-op with feedback rather than routing to the wrong surface.
- References / Context:
  - `CrossSurfaceNavigator.cpp` currently logs canvas navigation intent without a complete behavior.
- Example Scenarios Where Useful:
  - Pressing `Cmd+Z` in canvas mode should affect the board, not the last text editor.
