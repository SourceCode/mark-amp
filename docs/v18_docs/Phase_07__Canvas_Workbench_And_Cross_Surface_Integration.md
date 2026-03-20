# Phase 07: Canvas Workbench And Cross Surface Integration

## Phase Goal
Make canvas a real workbench mode with coherent shell, command, state, and feedback behavior.

## Tasks
### Task 1
- Phase ID: P07
- Task ID: P07-T01
- Task Title: Promote Canvas Workbench Mode To A First Class Shell Surface
- Objective: Ensure canvas mode switching behaves like a workbench mode, not a one-off show/hide swap.
- Problem Statement: Canvas workspace exists, but shell integration and mode semantics remain thin.
- Scope: Mode switching, title/state updates, active workspace metadata, menu/toolbar/palette entries, restore.
- Out of Scope: New canvas features.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp`
- Related Systems / Components: WorkbenchMode, canvas workspace, activity bar, shell restore.
- Current Behavior: `ShowCanvasWorkspace()` mainly hides editor content and shows the canvas widget.
- Intended Behavior: Canvas mode updates shell commands, restore state, focus targets, and active-surface metadata coherently.
- Technical Approach: Extend workbench mode orchestration to treat canvas as a first-class surface with persisted state.
- Implementation Steps:
  1. Define mode entry/exit hooks.
  2. Update shell metadata and title behavior.
  3. Persist last active canvas state where appropriate.
  4. Normalize menu/toolbar/action availability in canvas mode.
- Edge Cases / Failure Modes: Unsaved editor edits while switching modes; no board loaded; switching during panel operations.
- UX Considerations: Canvas should feel native to the IDE, not bolted on.
- Dependencies: P03-T02 and P03-T04.
- Validation Steps: Switch between editor and canvas from all entry points and relaunch.
- Acceptance Criteria: Canvas mode is consistently represented across the shell.
- Rollback / Safety Notes: Preserve current lazy canvas creation strategy if needed for performance.
- References / Context: `LayoutManager::SetWorkbenchMode()` currently maps canvas with minimal surrounding shell logic.
- Example interactions or usage scenarios where helpful: Opening a board from the palette should move the shell into canvas mode and preserve that state until the user switches out.

### Task 2
- Phase ID: P07
- Task ID: P07-T02
- Task Title: Wire Canvas Tool Rail Inspector Minimap And Shell Commands Together
- Objective: Ensure visible canvas controls all operate on the same board and tool state.
- Problem Statement: Canvas workspace, tool rail, context bar, inspector, and minimap exist but need systematic end-to-end verification and integration.
- Scope: Tool selection, zoom, inspector toggle, minimap toggle, board title/dirty state, shell actions.
- Out of Scope: New drawing tools.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`
- Related Systems / Components: CanvasInputManager, CanvasInspector, MinimapPanel, canvas state events.
- Current Behavior: Several canvas shell controls exist, but product-grade synchronization needs explicit completion.
- Intended Behavior: Tool, zoom, inspector, minimap, and board metadata are all synchronized.
- Technical Approach: Define a shared canvas workbench state model consumed by workspace panel and shell actions.
- Implementation Steps:
  1. Map current control-to-state bindings.
  2. Fill missing event/state propagation.
  3. Ensure toggles persist when appropriate.
  4. Standardize feedback for unavailable actions.
- Edge Cases / Failure Modes: No active board; inspector open with no selection; zoom controls fighting viewport changes.
- UX Considerations: Canvas chrome should feel precise and low-latency.
- Dependencies: P02-T02 and P07-T01.
- Validation Steps: Exercise every visible canvas shell control and verify resulting state.
- Acceptance Criteria: No visible canvas shell control is a dead or partially wired affordance.
- Rollback / Safety Notes: Keep heavy board operations off the UI thread where possible.
- References / Context: `CanvasWorkspacePanel` is the key composition layer.
- Example interactions or usage scenarios where helpful: Toggling inspector from canvas shell or command palette should update the same visual state.

### Task 3
- Phase ID: P07
- Task ID: P07-T03
- Task Title: Integrate Canvas Comments Facilitation And Collaboration Panels With Board State
- Objective: Stop standalone canvas adjunct panels from existing outside a coherent board/session model.
- Problem Statement: Comments and facilitation panels exist, and collaboration service exists, but shell-level integration is incomplete.
- Scope: Comment panel, facilitation panel, collaboration session state, board lock/private reveal/timer/vote actions, board selection context.
- Out of Scope: Network transport redesign.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: CanvasCollabService, board context, side panels, notifications/status.
- Current Behavior: Collaboration-related UI and service layers exist but need shell integration, visibility rules, and feedback plumbing.
- Intended Behavior: Collaboration features are accessible from the canvas workbench and reflect actual board/session state.
- Technical Approach: Create a canvas session context and bind adjunct panels to it rather than to isolated local state.
- Implementation Steps:
  1. Define active board/session context.
  2. Bind comments/facilitation panels to that context.
  3. Surface join/leave and action feedback.
  4. Ensure state clears cleanly when switching boards or leaving canvas mode.
- Edge Cases / Failure Modes: No session active; switching boards mid-session; stale participant selections.
- UX Considerations: Collaboration controls should be discoverable without cluttering solo workflows.
- Dependencies: P07-T01 and P09-T01.
- Validation Steps: Join session, use facilitation controls, switch boards, and observe panel state.
- Acceptance Criteria: Collaboration-adjacent canvas UI is meaningfully connected to the active board.
- Rollback / Safety Notes: Keep local-only behavior available when collaboration is inactive.
- References / Context: `CanvasCollabService` already publishes rich events.
- Example interactions or usage scenarios where helpful: Starting a timer from the facilitation panel should update the active canvas session and visible feedback surfaces.

### Task 4
- Phase ID: P07
- Task ID: P07-T04
- Task Title: Connect Canvas Open Export And Board Navigation To Shared Workbench Flows
- Objective: Ensure boards participate in the same open/save/export/navigation patterns as documents.
- Problem Statement: Canvas has its own open/export/state pathways that are not yet fully aligned with shell workflows.
- Scope: Board open requests, new board, board navigator, export, title/recents/session state, command palette exposure.
- Out of Scope: New export formats.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.cpp`
  - `/Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: board open events, export service, workspace recents, shell title/session restore.
- Current Behavior: Board mode switching exists, but broader workbench integration is partial.
- Intended Behavior: Opening or exporting boards follows the same command, status, and restore patterns as document workflows.
- Technical Approach: Treat boards as first-class workbench resources with explicit navigation and persistence hooks.
- Implementation Steps:
  1. Normalize board-open command routing.
  2. Add status/title/session updates for active board.
  3. Route export through shared progress/notification patterns.
  4. Record board navigation history where appropriate.
- Edge Cases / Failure Modes: Export with empty board; board deleted while active; switching from editor unsaved state.
- UX Considerations: Boards should feel like durable resources, not modal detours.
- Dependencies: P02-T05 and P03-T05.
- Validation Steps: Create board, open board, export board, relaunch, restore state.
- Acceptance Criteria: Canvas resources behave like integrated workbench assets.
- Rollback / Safety Notes: Keep board export failure paths non-destructive.
- References / Context: `BoardOpenRequestEvent` and canvas export service are already present.
- Example interactions or usage scenarios where helpful: Using a command palette action to open a board should place it into recent state and shell context.

### Task 5
- Phase ID: P07
- Task ID: P07-T05
- Task Title: Standardize Cross Surface Links Between Editor Graph And Canvas
- Objective: Make cross-surface navigation coherent where content, graph, and board links intersect.
- Problem Statement: MarkAmp has graph, backlink, and surface-link infrastructure, but user navigation among surfaces is not yet standardized.
- Scope: Surface links, deep links, editor-to-board navigation, graph/sidebar interactions that open canvas resources, backlink-driven navigation.
- Out of Scope: New knowledge graph computation.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/SurfaceLink.h`
  - `/Users/ryanrentfro/code/markamp/src/core/DeepLinkService.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Systems / Components: deep link service, surface link router, graph sidebar, canvas workbench, editor navigation.
- Current Behavior: Cross-surface infrastructure exists, but workflow-level UX cohesion is unclear.
- Intended Behavior: A link can move the user to the correct surface, resource, and focus target reliably.
- Technical Approach: Define a cross-surface navigation orchestrator over existing link/deep-link services.
- Implementation Steps:
  1. Inventory cross-surface navigation entry points.
  2. Normalize target resolution and mode switching.
  3. Ensure history and focus are updated.
  4. Add fallback UI for unresolved targets.
- Edge Cases / Failure Modes: Broken links; target surface unavailable; resource deleted.
- UX Considerations: Cross-surface jumps should preserve orientation and allow easy return.
- Dependencies: P04-T04 and P07-T01.
- Validation Steps: Navigate among editor, graph, and canvas through links and sidebars.
- Acceptance Criteria: Cross-surface navigation feels intentional and reversible.
- Rollback / Safety Notes: Use nonfatal fallback states for unresolved links.
- References / Context: `SurfaceLinkRouter` and `DeepLinkService` are already present.
- Example interactions or usage scenarios where helpful: Clicking a graph sidebar item tied to a board should switch to canvas mode and focus that board.

