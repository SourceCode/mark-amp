# Phase 06: Collaboration Facilitation And Multiplayer Canvas UX

## Objective
Raise real-time and async collaboration so teams can workshop, review, and facilitate directly on the canvas with trust in shared state.

## AI Agent Execution Rules
- Execute workstreams in order unless a later task is explicitly unblocked by completed model and command contracts.
- Prefer extending existing canvas, UI, and core services before introducing new parallel abstractions.
- Every implementation task should finish with tests or a documented testing gap.
- Preserve theme, accessibility, undo/redo, and serialization behavior as default quality bars rather than follow-up work.
- When adding references to new code during implementation, keep using full file paths in planning notes and PR write-ups.

## Workstreams
- W01 Live Presence: Show collaborators clearly without drowning the board in noise.
- W02 Remote Selections And Follow: Make shared focus visible and controllable during collaboration sessions.
- W03 Conflict Handling: Reduce surprise when two people edit the same thing at once.
- W04 Comment Threads: Turn comments into anchored, navigable review conversations.
- W05 Voting And Reactions: Support lightweight facilitation mechanics used in workshops and prioritization.
- W06 Timer And Facilitation Controls: Give facilitators on-board controls instead of requiring external tools.
- W07 Private Reveal And Presentation: Support workshop reveal patterns without duplicating the whole presentation system.
- W08 Permissions And Shared Locks: Let collaborators know what is editable, review-only, or temporarily protected.
- W09 Offline Queue: Protect collaboration flows across intermittent connectivity.
- W10 Activity Feed: Expose meaningful recent activity without turning the UI into a log viewer.
- W11 Change Highlights: Help users reorient to what changed since they last looked at a board.
- W12 Invites And Session Entry: Make joining a canvas session simple and comprehensible for new collaborators.
- W13 Collaborator Panels: Provide optional side panels for awareness, moderation, and facilitation.
- W14 Co Editing Text: Support multiplayer text editing inside canvas objects without corrupting content.
- W15 Workshop Templates: Package recurring collaborative rituals as ready-to-run board flows.
- W16 Async Review: Support asynchronous sign-off and review workflows beyond live sessions.
- W17 Moderation And Recovery: Give facilitators tools to recover from chaos or misuse during live boards.
- W18 Multiplayer Performance: Keep presence and live updates lightweight as collaboration scale grows.
- W19 Collaboration Analytics: Measure the right workshop signals without over-collecting.
- W20 Collaboration Coverage: Build testable contracts for multi-user behavior and edge conditions.

## Workstream 01: Live Presence
Show collaborators clearly without drowning the board in noise.

### P06-T001: Live Presence UX Parity Audit
**Task Title:** Live Presence UX Parity Audit
**Definition:** Define the implementation contract for live presence by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around show collaborators clearly without drowning the board in noise.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T002: Live Presence Domain Model And State Contract
**Task Title:** Live Presence Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support live presence.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around show collaborators clearly without drowning the board in noise.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T003: Live Presence Commands Events And Context Keys
**Task Title:** Live Presence Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for live presence.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make show collaborators clearly without drowning the board in noise. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T004: Live Presence Workspace Surface And Controls
**Task Title:** Live Presence Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose live presence in the main canvas workspace.
**Description:** Build the primary UI surfaces for show collaborators clearly without drowning the board in noise., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T005: Live Presence Pointer Interaction Flow
**Task Title:** Live Presence Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for live presence.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for show collaborators clearly without drowning the board in noise.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T006: Live Presence Keyboard Gesture And Shortcut Flow
**Task Title:** Live Presence Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for live presence.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for show collaborators clearly without drowning the board in noise.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T007: Live Presence Visual Feedback And Rendering Polish
**Task Title:** Live Presence Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make live presence legible in use.
**Description:** Implement the visible feedback for show collaborators clearly without drowning the board in noise.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T008: Live Presence Persistence Preferences And Serialization
**Task Title:** Live Presence Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by live presence.
**Description:** Identify what parts of show collaborators clearly without drowning the board in noise. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T009: Live Presence Safeguards Telemetry And Recovery
**Task Title:** Live Presence Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for live presence.
**Description:** Instrument show collaborators clearly without drowning the board in noise. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

### P06-T010: Live Presence Tests Documentation And Rollout Gate
**Task Title:** Live Presence Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship live presence.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for show collaborators clearly without drowning the board in noise.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** colored remote cursors; presence list; author badge on active object.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/ui/Badge.cpp

## Workstream 02: Remote Selections And Follow
Make shared focus visible and controllable during collaboration sessions.

### P06-T011: Remote Selections And Follow UX Parity Audit
**Task Title:** Remote Selections And Follow UX Parity Audit
**Definition:** Define the implementation contract for remote selections and follow by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make shared focus visible and controllable during collaboration sessions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T012: Remote Selections And Follow Domain Model And State Contract
**Task Title:** Remote Selections And Follow Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support remote selections and follow.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make shared focus visible and controllable during collaboration sessions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T013: Remote Selections And Follow Commands Events And Context Keys
**Task Title:** Remote Selections And Follow Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for remote selections and follow.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make shared focus visible and controllable during collaboration sessions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T014: Remote Selections And Follow Workspace Surface And Controls
**Task Title:** Remote Selections And Follow Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose remote selections and follow in the main canvas workspace.
**Description:** Build the primary UI surfaces for make shared focus visible and controllable during collaboration sessions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T015: Remote Selections And Follow Pointer Interaction Flow
**Task Title:** Remote Selections And Follow Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for remote selections and follow.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make shared focus visible and controllable during collaboration sessions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T016: Remote Selections And Follow Keyboard Gesture And Shortcut Flow
**Task Title:** Remote Selections And Follow Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for remote selections and follow.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make shared focus visible and controllable during collaboration sessions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T017: Remote Selections And Follow Visual Feedback And Rendering Polish
**Task Title:** Remote Selections And Follow Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make remote selections and follow legible in use.
**Description:** Implement the visible feedback for make shared focus visible and controllable during collaboration sessions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T018: Remote Selections And Follow Persistence Preferences And Serialization
**Task Title:** Remote Selections And Follow Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by remote selections and follow.
**Description:** Identify what parts of make shared focus visible and controllable during collaboration sessions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T019: Remote Selections And Follow Safeguards Telemetry And Recovery
**Task Title:** Remote Selections And Follow Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for remote selections and follow.
**Description:** Instrument make shared focus visible and controllable during collaboration sessions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

### P06-T020: Remote Selections And Follow Tests Documentation And Rollout Gate
**Task Title:** Remote Selections And Follow Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship remote selections and follow.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make shared focus visible and controllable during collaboration sessions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** follow presenter; show remote selection box; jump to teammate.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.h
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.h
- /Users/ryanrentfro/code/markamp/src/canvas/BoardNavigator.h

## Workstream 03: Conflict Handling
Reduce surprise when two people edit the same thing at once.

### P06-T021: Conflict Handling UX Parity Audit
**Task Title:** Conflict Handling UX Parity Audit
**Definition:** Define the implementation contract for conflict handling by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around reduce surprise when two people edit the same thing at once.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T022: Conflict Handling Domain Model And State Contract
**Task Title:** Conflict Handling Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support conflict handling.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around reduce surprise when two people edit the same thing at once.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T023: Conflict Handling Commands Events And Context Keys
**Task Title:** Conflict Handling Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for conflict handling.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make reduce surprise when two people edit the same thing at once. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T024: Conflict Handling Workspace Surface And Controls
**Task Title:** Conflict Handling Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose conflict handling in the main canvas workspace.
**Description:** Build the primary UI surfaces for reduce surprise when two people edit the same thing at once., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T025: Conflict Handling Pointer Interaction Flow
**Task Title:** Conflict Handling Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for conflict handling.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for reduce surprise when two people edit the same thing at once.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T026: Conflict Handling Keyboard Gesture And Shortcut Flow
**Task Title:** Conflict Handling Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for conflict handling.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for reduce surprise when two people edit the same thing at once.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T027: Conflict Handling Visual Feedback And Rendering Polish
**Task Title:** Conflict Handling Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make conflict handling legible in use.
**Description:** Implement the visible feedback for reduce surprise when two people edit the same thing at once.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T028: Conflict Handling Persistence Preferences And Serialization
**Task Title:** Conflict Handling Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by conflict handling.
**Description:** Identify what parts of reduce surprise when two people edit the same thing at once. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T029: Conflict Handling Safeguards Telemetry And Recovery
**Task Title:** Conflict Handling Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for conflict handling.
**Description:** Instrument reduce surprise when two people edit the same thing at once. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

### P06-T030: Conflict Handling Tests Documentation And Rollout Gate
**Task Title:** Conflict Handling Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship conflict handling.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for reduce surprise when two people edit the same thing at once.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** soft lock on text edit; merge-safe style changes; conflict toast with recovery action.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.cpp
- /Users/ryanrentfro/code/markamp/src/ui/ErrorRecoveryModel.h

## Workstream 04: Comment Threads
Turn comments into anchored, navigable review conversations.

### P06-T031: Comment Threads UX Parity Audit
**Task Title:** Comment Threads UX Parity Audit
**Definition:** Define the implementation contract for comment threads by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around turn comments into anchored, navigable review conversations.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T032: Comment Threads Domain Model And State Contract
**Task Title:** Comment Threads Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support comment threads.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around turn comments into anchored, navigable review conversations.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T033: Comment Threads Commands Events And Context Keys
**Task Title:** Comment Threads Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for comment threads.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make turn comments into anchored, navigable review conversations. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T034: Comment Threads Workspace Surface And Controls
**Task Title:** Comment Threads Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose comment threads in the main canvas workspace.
**Description:** Build the primary UI surfaces for turn comments into anchored, navigable review conversations., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T035: Comment Threads Pointer Interaction Flow
**Task Title:** Comment Threads Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for comment threads.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for turn comments into anchored, navigable review conversations.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T036: Comment Threads Keyboard Gesture And Shortcut Flow
**Task Title:** Comment Threads Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for comment threads.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for turn comments into anchored, navigable review conversations.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T037: Comment Threads Visual Feedback And Rendering Polish
**Task Title:** Comment Threads Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make comment threads legible in use.
**Description:** Implement the visible feedback for turn comments into anchored, navigable review conversations.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T038: Comment Threads Persistence Preferences And Serialization
**Task Title:** Comment Threads Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by comment threads.
**Description:** Identify what parts of turn comments into anchored, navigable review conversations. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T039: Comment Threads Safeguards Telemetry And Recovery
**Task Title:** Comment Threads Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for comment threads.
**Description:** Instrument turn comments into anchored, navigable review conversations. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

### P06-T040: Comment Threads Tests Documentation And Rollout Gate
**Task Title:** Comment Threads Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship comment threads.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for turn comments into anchored, navigable review conversations.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** pin comment to object; thread replies; resolve and reopen.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CommentObject.h
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.h

## Workstream 05: Voting And Reactions
Support lightweight facilitation mechanics used in workshops and prioritization.

### P06-T041: Voting And Reactions UX Parity Audit
**Task Title:** Voting And Reactions UX Parity Audit
**Definition:** Define the implementation contract for voting and reactions by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support lightweight facilitation mechanics used in workshops and prioritization.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T042: Voting And Reactions Domain Model And State Contract
**Task Title:** Voting And Reactions Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support voting and reactions.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support lightweight facilitation mechanics used in workshops and prioritization.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T043: Voting And Reactions Commands Events And Context Keys
**Task Title:** Voting And Reactions Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for voting and reactions.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support lightweight facilitation mechanics used in workshops and prioritization. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T044: Voting And Reactions Workspace Surface And Controls
**Task Title:** Voting And Reactions Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose voting and reactions in the main canvas workspace.
**Description:** Build the primary UI surfaces for support lightweight facilitation mechanics used in workshops and prioritization., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T045: Voting And Reactions Pointer Interaction Flow
**Task Title:** Voting And Reactions Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for voting and reactions.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support lightweight facilitation mechanics used in workshops and prioritization.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T046: Voting And Reactions Keyboard Gesture And Shortcut Flow
**Task Title:** Voting And Reactions Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for voting and reactions.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support lightweight facilitation mechanics used in workshops and prioritization.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T047: Voting And Reactions Visual Feedback And Rendering Polish
**Task Title:** Voting And Reactions Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make voting and reactions legible in use.
**Description:** Implement the visible feedback for support lightweight facilitation mechanics used in workshops and prioritization.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T048: Voting And Reactions Persistence Preferences And Serialization
**Task Title:** Voting And Reactions Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by voting and reactions.
**Description:** Identify what parts of support lightweight facilitation mechanics used in workshops and prioritization. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T049: Voting And Reactions Safeguards Telemetry And Recovery
**Task Title:** Voting And Reactions Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for voting and reactions.
**Description:** Instrument support lightweight facilitation mechanics used in workshops and prioritization. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

### P06-T050: Voting And Reactions Tests Documentation And Rollout Gate
**Task Title:** Voting And Reactions Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship voting and reactions.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support lightweight facilitation mechanics used in workshops and prioritization.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** dot voting; emoji reaction burst; timer-bound voting session.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.h
- /Users/ryanrentfro/code/markamp/src/canvas/PolishModel.cpp

## Workstream 06: Timer And Facilitation Controls
Give facilitators on-board controls instead of requiring external tools.

### P06-T051: Timer And Facilitation Controls UX Parity Audit
**Task Title:** Timer And Facilitation Controls UX Parity Audit
**Definition:** Define the implementation contract for timer and facilitation controls by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around give facilitators on-board controls instead of requiring external tools.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T052: Timer And Facilitation Controls Domain Model And State Contract
**Task Title:** Timer And Facilitation Controls Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support timer and facilitation controls.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around give facilitators on-board controls instead of requiring external tools.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T053: Timer And Facilitation Controls Commands Events And Context Keys
**Task Title:** Timer And Facilitation Controls Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for timer and facilitation controls.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make give facilitators on-board controls instead of requiring external tools. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T054: Timer And Facilitation Controls Workspace Surface And Controls
**Task Title:** Timer And Facilitation Controls Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose timer and facilitation controls in the main canvas workspace.
**Description:** Build the primary UI surfaces for give facilitators on-board controls instead of requiring external tools., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T055: Timer And Facilitation Controls Pointer Interaction Flow
**Task Title:** Timer And Facilitation Controls Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for timer and facilitation controls.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for give facilitators on-board controls instead of requiring external tools.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T056: Timer And Facilitation Controls Keyboard Gesture And Shortcut Flow
**Task Title:** Timer And Facilitation Controls Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for timer and facilitation controls.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for give facilitators on-board controls instead of requiring external tools.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T057: Timer And Facilitation Controls Visual Feedback And Rendering Polish
**Task Title:** Timer And Facilitation Controls Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make timer and facilitation controls legible in use.
**Description:** Implement the visible feedback for give facilitators on-board controls instead of requiring external tools.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T058: Timer And Facilitation Controls Persistence Preferences And Serialization
**Task Title:** Timer And Facilitation Controls Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by timer and facilitation controls.
**Description:** Identify what parts of give facilitators on-board controls instead of requiring external tools. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T059: Timer And Facilitation Controls Safeguards Telemetry And Recovery
**Task Title:** Timer And Facilitation Controls Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for timer and facilitation controls.
**Description:** Instrument give facilitators on-board controls instead of requiring external tools. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

### P06-T060: Timer And Facilitation Controls Tests Documentation And Rollout Gate
**Task Title:** Timer And Facilitation Controls Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship timer and facilitation controls.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for give facilitators on-board controls instead of requiring external tools.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** countdown timer; agenda checkpoints; lock board during instructions.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.cpp
- /Users/ryanrentfro/code/markamp/src/core/AutomationRule.cpp

## Workstream 07: Private Reveal And Presentation
Support workshop reveal patterns without duplicating the whole presentation system.

### P06-T061: Private Reveal And Presentation UX Parity Audit
**Task Title:** Private Reveal And Presentation UX Parity Audit
**Definition:** Define the implementation contract for private reveal and presentation by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support workshop reveal patterns without duplicating the whole presentation system.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T062: Private Reveal And Presentation Domain Model And State Contract
**Task Title:** Private Reveal And Presentation Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support private reveal and presentation.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support workshop reveal patterns without duplicating the whole presentation system.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T063: Private Reveal And Presentation Commands Events And Context Keys
**Task Title:** Private Reveal And Presentation Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for private reveal and presentation.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support workshop reveal patterns without duplicating the whole presentation system. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T064: Private Reveal And Presentation Workspace Surface And Controls
**Task Title:** Private Reveal And Presentation Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose private reveal and presentation in the main canvas workspace.
**Description:** Build the primary UI surfaces for support workshop reveal patterns without duplicating the whole presentation system., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T065: Private Reveal And Presentation Pointer Interaction Flow
**Task Title:** Private Reveal And Presentation Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for private reveal and presentation.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support workshop reveal patterns without duplicating the whole presentation system.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T066: Private Reveal And Presentation Keyboard Gesture And Shortcut Flow
**Task Title:** Private Reveal And Presentation Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for private reveal and presentation.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support workshop reveal patterns without duplicating the whole presentation system.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T067: Private Reveal And Presentation Visual Feedback And Rendering Polish
**Task Title:** Private Reveal And Presentation Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make private reveal and presentation legible in use.
**Description:** Implement the visible feedback for support workshop reveal patterns without duplicating the whole presentation system.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T068: Private Reveal And Presentation Persistence Preferences And Serialization
**Task Title:** Private Reveal And Presentation Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by private reveal and presentation.
**Description:** Identify what parts of support workshop reveal patterns without duplicating the whole presentation system. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T069: Private Reveal And Presentation Safeguards Telemetry And Recovery
**Task Title:** Private Reveal And Presentation Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for private reveal and presentation.
**Description:** Instrument support workshop reveal patterns without duplicating the whole presentation system. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

### P06-T070: Private Reveal And Presentation Tests Documentation And Rollout Gate
**Task Title:** Private Reveal And Presentation Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship private reveal and presentation.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support workshop reveal patterns without duplicating the whole presentation system.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** hide unrevealed frame; reveal next section; private prep area.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/PrivateRevealController.cpp
- /Users/ryanrentfro/code/markamp/src/core/PresentationManager.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/SectionRenderer.h

## Workstream 08: Permissions And Shared Locks
Let collaborators know what is editable, review-only, or temporarily protected.

### P06-T071: Permissions And Shared Locks UX Parity Audit
**Task Title:** Permissions And Shared Locks UX Parity Audit
**Definition:** Define the implementation contract for permissions and shared locks by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around let collaborators know what is editable, review-only, or temporarily protected.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T072: Permissions And Shared Locks Domain Model And State Contract
**Task Title:** Permissions And Shared Locks Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support permissions and shared locks.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around let collaborators know what is editable, review-only, or temporarily protected.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T073: Permissions And Shared Locks Commands Events And Context Keys
**Task Title:** Permissions And Shared Locks Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for permissions and shared locks.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make let collaborators know what is editable, review-only, or temporarily protected. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T074: Permissions And Shared Locks Workspace Surface And Controls
**Task Title:** Permissions And Shared Locks Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose permissions and shared locks in the main canvas workspace.
**Description:** Build the primary UI surfaces for let collaborators know what is editable, review-only, or temporarily protected., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T075: Permissions And Shared Locks Pointer Interaction Flow
**Task Title:** Permissions And Shared Locks Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for permissions and shared locks.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for let collaborators know what is editable, review-only, or temporarily protected.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T076: Permissions And Shared Locks Keyboard Gesture And Shortcut Flow
**Task Title:** Permissions And Shared Locks Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for permissions and shared locks.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for let collaborators know what is editable, review-only, or temporarily protected.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T077: Permissions And Shared Locks Visual Feedback And Rendering Polish
**Task Title:** Permissions And Shared Locks Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make permissions and shared locks legible in use.
**Description:** Implement the visible feedback for let collaborators know what is editable, review-only, or temporarily protected.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T078: Permissions And Shared Locks Persistence Preferences And Serialization
**Task Title:** Permissions And Shared Locks Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by permissions and shared locks.
**Description:** Identify what parts of let collaborators know what is editable, review-only, or temporarily protected. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T079: Permissions And Shared Locks Safeguards Telemetry And Recovery
**Task Title:** Permissions And Shared Locks Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for permissions and shared locks.
**Description:** Instrument let collaborators know what is editable, review-only, or temporarily protected. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

### P06-T080: Permissions And Shared Locks Tests Documentation And Rollout Gate
**Task Title:** Permissions And Shared Locks Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship permissions and shared locks.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for let collaborators know what is editable, review-only, or temporarily protected.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** editable vs locked frame; role-based control; unlock request.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/RuntimePolicy.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/LockingService.cpp
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.h

## Workstream 09: Offline Queue
Protect collaboration flows across intermittent connectivity.

### P06-T081: Offline Queue UX Parity Audit
**Task Title:** Offline Queue UX Parity Audit
**Definition:** Define the implementation contract for offline queue by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around protect collaboration flows across intermittent connectivity.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T082: Offline Queue Domain Model And State Contract
**Task Title:** Offline Queue Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support offline queue.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around protect collaboration flows across intermittent connectivity.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T083: Offline Queue Commands Events And Context Keys
**Task Title:** Offline Queue Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for offline queue.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make protect collaboration flows across intermittent connectivity. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T084: Offline Queue Workspace Surface And Controls
**Task Title:** Offline Queue Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose offline queue in the main canvas workspace.
**Description:** Build the primary UI surfaces for protect collaboration flows across intermittent connectivity., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T085: Offline Queue Pointer Interaction Flow
**Task Title:** Offline Queue Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for offline queue.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for protect collaboration flows across intermittent connectivity.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T086: Offline Queue Keyboard Gesture And Shortcut Flow
**Task Title:** Offline Queue Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for offline queue.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for protect collaboration flows across intermittent connectivity.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T087: Offline Queue Visual Feedback And Rendering Polish
**Task Title:** Offline Queue Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make offline queue legible in use.
**Description:** Implement the visible feedback for protect collaboration flows across intermittent connectivity.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T088: Offline Queue Persistence Preferences And Serialization
**Task Title:** Offline Queue Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by offline queue.
**Description:** Identify what parts of protect collaboration flows across intermittent connectivity. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T089: Offline Queue Safeguards Telemetry And Recovery
**Task Title:** Offline Queue Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for offline queue.
**Description:** Instrument protect collaboration flows across intermittent connectivity. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T090: Offline Queue Tests Documentation And Rollout Gate
**Task Title:** Offline Queue Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship offline queue.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for protect collaboration flows across intermittent connectivity.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** queue edits offline; replay with status; surface sync conflict.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.h
- /Users/ryanrentfro/code/markamp/src/core/CloudSyncTypes.h
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

## Workstream 10: Activity Feed
Expose meaningful recent activity without turning the UI into a log viewer.

### P06-T091: Activity Feed UX Parity Audit
**Task Title:** Activity Feed UX Parity Audit
**Definition:** Define the implementation contract for activity feed by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around expose meaningful recent activity without turning the ui into a log viewer.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T092: Activity Feed Domain Model And State Contract
**Task Title:** Activity Feed Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support activity feed.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around expose meaningful recent activity without turning the ui into a log viewer.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T093: Activity Feed Commands Events And Context Keys
**Task Title:** Activity Feed Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for activity feed.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make expose meaningful recent activity without turning the ui into a log viewer. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T094: Activity Feed Workspace Surface And Controls
**Task Title:** Activity Feed Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose activity feed in the main canvas workspace.
**Description:** Build the primary UI surfaces for expose meaningful recent activity without turning the ui into a log viewer., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T095: Activity Feed Pointer Interaction Flow
**Task Title:** Activity Feed Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for activity feed.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for expose meaningful recent activity without turning the ui into a log viewer.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T096: Activity Feed Keyboard Gesture And Shortcut Flow
**Task Title:** Activity Feed Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for activity feed.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for expose meaningful recent activity without turning the ui into a log viewer.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T097: Activity Feed Visual Feedback And Rendering Polish
**Task Title:** Activity Feed Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make activity feed legible in use.
**Description:** Implement the visible feedback for expose meaningful recent activity without turning the ui into a log viewer.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T098: Activity Feed Persistence Preferences And Serialization
**Task Title:** Activity Feed Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by activity feed.
**Description:** Identify what parts of expose meaningful recent activity without turning the ui into a log viewer. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T099: Activity Feed Safeguards Telemetry And Recovery
**Task Title:** Activity Feed Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for activity feed.
**Description:** Instrument expose meaningful recent activity without turning the ui into a log viewer. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

### P06-T100: Activity Feed Tests Documentation And Rollout Gate
**Task Title:** Activity Feed Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship activity feed.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for expose meaningful recent activity without turning the ui into a log viewer.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** who moved frame; who resolved comment; board activity summary.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityTimeline.cpp
- /Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.h

## Workstream 11: Change Highlights
Help users reorient to what changed since they last looked at a board.

### P06-T101: Change Highlights UX Parity Audit
**Task Title:** Change Highlights UX Parity Audit
**Definition:** Define the implementation contract for change highlights by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around help users reorient to what changed since they last looked at a board.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T102: Change Highlights Domain Model And State Contract
**Task Title:** Change Highlights Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support change highlights.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around help users reorient to what changed since they last looked at a board.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T103: Change Highlights Commands Events And Context Keys
**Task Title:** Change Highlights Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for change highlights.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make help users reorient to what changed since they last looked at a board. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T104: Change Highlights Workspace Surface And Controls
**Task Title:** Change Highlights Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose change highlights in the main canvas workspace.
**Description:** Build the primary UI surfaces for help users reorient to what changed since they last looked at a board., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T105: Change Highlights Pointer Interaction Flow
**Task Title:** Change Highlights Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for change highlights.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for help users reorient to what changed since they last looked at a board.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T106: Change Highlights Keyboard Gesture And Shortcut Flow
**Task Title:** Change Highlights Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for change highlights.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for help users reorient to what changed since they last looked at a board.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T107: Change Highlights Visual Feedback And Rendering Polish
**Task Title:** Change Highlights Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make change highlights legible in use.
**Description:** Implement the visible feedback for help users reorient to what changed since they last looked at a board.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T108: Change Highlights Persistence Preferences And Serialization
**Task Title:** Change Highlights Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by change highlights.
**Description:** Identify what parts of help users reorient to what changed since they last looked at a board. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T109: Change Highlights Safeguards Telemetry And Recovery
**Task Title:** Change Highlights Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for change highlights.
**Description:** Instrument help users reorient to what changed since they last looked at a board. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

### P06-T110: Change Highlights Tests Documentation And Rollout Gate
**Task Title:** Change Highlights Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship change highlights.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for help users reorient to what changed since they last looked at a board.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** recent edits glow; jump to new comments; highlight unresolved changes.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/AuditModel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp

## Workstream 12: Invites And Session Entry
Make joining a canvas session simple and comprehensible for new collaborators.

### P06-T111: Invites And Session Entry UX Parity Audit
**Task Title:** Invites And Session Entry UX Parity Audit
**Definition:** Define the implementation contract for invites and session entry by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around make joining a canvas session simple and comprehensible for new collaborators.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T112: Invites And Session Entry Domain Model And State Contract
**Task Title:** Invites And Session Entry Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support invites and session entry.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around make joining a canvas session simple and comprehensible for new collaborators.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T113: Invites And Session Entry Commands Events And Context Keys
**Task Title:** Invites And Session Entry Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for invites and session entry.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make make joining a canvas session simple and comprehensible for new collaborators. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T114: Invites And Session Entry Workspace Surface And Controls
**Task Title:** Invites And Session Entry Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose invites and session entry in the main canvas workspace.
**Description:** Build the primary UI surfaces for make joining a canvas session simple and comprehensible for new collaborators., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T115: Invites And Session Entry Pointer Interaction Flow
**Task Title:** Invites And Session Entry Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for invites and session entry.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for make joining a canvas session simple and comprehensible for new collaborators.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T116: Invites And Session Entry Keyboard Gesture And Shortcut Flow
**Task Title:** Invites And Session Entry Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for invites and session entry.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for make joining a canvas session simple and comprehensible for new collaborators.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T117: Invites And Session Entry Visual Feedback And Rendering Polish
**Task Title:** Invites And Session Entry Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make invites and session entry legible in use.
**Description:** Implement the visible feedback for make joining a canvas session simple and comprehensible for new collaborators.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T118: Invites And Session Entry Persistence Preferences And Serialization
**Task Title:** Invites And Session Entry Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by invites and session entry.
**Description:** Identify what parts of make joining a canvas session simple and comprehensible for new collaborators. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T119: Invites And Session Entry Safeguards Telemetry And Recovery
**Task Title:** Invites And Session Entry Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for invites and session entry.
**Description:** Instrument make joining a canvas session simple and comprehensible for new collaborators. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

### P06-T120: Invites And Session Entry Tests Documentation And Rollout Gate
**Task Title:** Invites And Session Entry Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship invites and session entry.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for make joining a canvas session simple and comprehensible for new collaborators.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** join from link; guest entry state; first-run collab hints.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h
- /Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.h
- /Users/ryanrentfro/code/markamp/src/core/EnvironmentService.cpp

## Workstream 13: Collaborator Panels
Provide optional side panels for awareness, moderation, and facilitation.

### P06-T121: Collaborator Panels UX Parity Audit
**Task Title:** Collaborator Panels UX Parity Audit
**Definition:** Define the implementation contract for collaborator panels by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around provide optional side panels for awareness, moderation, and facilitation.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T122: Collaborator Panels Domain Model And State Contract
**Task Title:** Collaborator Panels Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support collaborator panels.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around provide optional side panels for awareness, moderation, and facilitation.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T123: Collaborator Panels Commands Events And Context Keys
**Task Title:** Collaborator Panels Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for collaborator panels.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make provide optional side panels for awareness, moderation, and facilitation. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T124: Collaborator Panels Workspace Surface And Controls
**Task Title:** Collaborator Panels Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose collaborator panels in the main canvas workspace.
**Description:** Build the primary UI surfaces for provide optional side panels for awareness, moderation, and facilitation., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T125: Collaborator Panels Pointer Interaction Flow
**Task Title:** Collaborator Panels Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for collaborator panels.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for provide optional side panels for awareness, moderation, and facilitation.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T126: Collaborator Panels Keyboard Gesture And Shortcut Flow
**Task Title:** Collaborator Panels Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for collaborator panels.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for provide optional side panels for awareness, moderation, and facilitation.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T127: Collaborator Panels Visual Feedback And Rendering Polish
**Task Title:** Collaborator Panels Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make collaborator panels legible in use.
**Description:** Implement the visible feedback for provide optional side panels for awareness, moderation, and facilitation.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T128: Collaborator Panels Persistence Preferences And Serialization
**Task Title:** Collaborator Panels Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by collaborator panels.
**Description:** Identify what parts of provide optional side panels for awareness, moderation, and facilitation. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T129: Collaborator Panels Safeguards Telemetry And Recovery
**Task Title:** Collaborator Panels Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for collaborator panels.
**Description:** Instrument provide optional side panels for awareness, moderation, and facilitation. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

### P06-T130: Collaborator Panels Tests Documentation And Rollout Gate
**Task Title:** Collaborator Panels Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship collaborator panels.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for provide optional side panels for awareness, moderation, and facilitation.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** participants drawer; speaker indicator; moderator actions.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.h
- /Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp
- /Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp

## Workstream 14: Co Editing Text
Support multiplayer text editing inside canvas objects without corrupting content.

### P06-T131: Co Editing Text UX Parity Audit
**Task Title:** Co Editing Text UX Parity Audit
**Definition:** Define the implementation contract for co editing text by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support multiplayer text editing inside canvas objects without corrupting content.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T132: Co Editing Text Domain Model And State Contract
**Task Title:** Co Editing Text Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support co editing text.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support multiplayer text editing inside canvas objects without corrupting content.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T133: Co Editing Text Commands Events And Context Keys
**Task Title:** Co Editing Text Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for co editing text.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support multiplayer text editing inside canvas objects without corrupting content. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T134: Co Editing Text Workspace Surface And Controls
**Task Title:** Co Editing Text Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose co editing text in the main canvas workspace.
**Description:** Build the primary UI surfaces for support multiplayer text editing inside canvas objects without corrupting content., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T135: Co Editing Text Pointer Interaction Flow
**Task Title:** Co Editing Text Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for co editing text.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support multiplayer text editing inside canvas objects without corrupting content.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T136: Co Editing Text Keyboard Gesture And Shortcut Flow
**Task Title:** Co Editing Text Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for co editing text.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support multiplayer text editing inside canvas objects without corrupting content.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T137: Co Editing Text Visual Feedback And Rendering Polish
**Task Title:** Co Editing Text Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make co editing text legible in use.
**Description:** Implement the visible feedback for support multiplayer text editing inside canvas objects without corrupting content.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T138: Co Editing Text Persistence Preferences And Serialization
**Task Title:** Co Editing Text Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by co editing text.
**Description:** Identify what parts of support multiplayer text editing inside canvas objects without corrupting content. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T139: Co Editing Text Safeguards Telemetry And Recovery
**Task Title:** Co Editing Text Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for co editing text.
**Description:** Instrument support multiplayer text editing inside canvas objects without corrupting content. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

### P06-T140: Co Editing Text Tests Documentation And Rollout Gate
**Task Title:** Co Editing Text Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship co editing text.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support multiplayer text editing inside canvas objects without corrupting content.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** shared text cursor; presence in text object; safe exit from inline editor.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.h
- /Users/ryanrentfro/code/markamp/src/core/AIConversationHistory.cpp
- /Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h

## Workstream 15: Workshop Templates
Package recurring collaborative rituals as ready-to-run board flows.

### P06-T141: Workshop Templates UX Parity Audit
**Task Title:** Workshop Templates UX Parity Audit
**Definition:** Define the implementation contract for workshop templates by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around package recurring collaborative rituals as ready-to-run board flows.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T142: Workshop Templates Domain Model And State Contract
**Task Title:** Workshop Templates Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support workshop templates.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around package recurring collaborative rituals as ready-to-run board flows.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T143: Workshop Templates Commands Events And Context Keys
**Task Title:** Workshop Templates Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for workshop templates.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make package recurring collaborative rituals as ready-to-run board flows. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T144: Workshop Templates Workspace Surface And Controls
**Task Title:** Workshop Templates Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose workshop templates in the main canvas workspace.
**Description:** Build the primary UI surfaces for package recurring collaborative rituals as ready-to-run board flows., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T145: Workshop Templates Pointer Interaction Flow
**Task Title:** Workshop Templates Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for workshop templates.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for package recurring collaborative rituals as ready-to-run board flows.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T146: Workshop Templates Keyboard Gesture And Shortcut Flow
**Task Title:** Workshop Templates Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for workshop templates.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for package recurring collaborative rituals as ready-to-run board flows.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T147: Workshop Templates Visual Feedback And Rendering Polish
**Task Title:** Workshop Templates Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make workshop templates legible in use.
**Description:** Implement the visible feedback for package recurring collaborative rituals as ready-to-run board flows.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T148: Workshop Templates Persistence Preferences And Serialization
**Task Title:** Workshop Templates Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by workshop templates.
**Description:** Identify what parts of package recurring collaborative rituals as ready-to-run board flows. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T149: Workshop Templates Safeguards Telemetry And Recovery
**Task Title:** Workshop Templates Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for workshop templates.
**Description:** Instrument package recurring collaborative rituals as ready-to-run board flows. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

### P06-T150: Workshop Templates Tests Documentation And Rollout Gate
**Task Title:** Workshop Templates Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship workshop templates.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for package recurring collaborative rituals as ready-to-run board flows.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** retrospective board; brainwriting template; impact-effort matrix workshop.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardTemplate.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OnboardingModel.h

## Workstream 16: Async Review
Support asynchronous sign-off and review workflows beyond live sessions.

### P06-T151: Async Review UX Parity Audit
**Task Title:** Async Review UX Parity Audit
**Definition:** Define the implementation contract for async review by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around support asynchronous sign-off and review workflows beyond live sessions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** approve frame; request changes; review status chip.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T152: Async Review Domain Model And State Contract
**Task Title:** Async Review Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support async review.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around support asynchronous sign-off and review workflows beyond live sessions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** approve frame; request changes; review status chip.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T153: Async Review Commands Events And Context Keys
**Task Title:** Async Review Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for async review.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make support asynchronous sign-off and review workflows beyond live sessions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** approve frame; request changes; review status chip.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T154: Async Review Workspace Surface And Controls
**Task Title:** Async Review Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose async review in the main canvas workspace.
**Description:** Build the primary UI surfaces for support asynchronous sign-off and review workflows beyond live sessions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** approve frame; request changes; review status chip.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T155: Async Review Pointer Interaction Flow
**Task Title:** Async Review Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for async review.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for support asynchronous sign-off and review workflows beyond live sessions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** approve frame; request changes; review status chip.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T156: Async Review Keyboard Gesture And Shortcut Flow
**Task Title:** Async Review Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for async review.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for support asynchronous sign-off and review workflows beyond live sessions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** approve frame; request changes; review status chip.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T157: Async Review Visual Feedback And Rendering Polish
**Task Title:** Async Review Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make async review legible in use.
**Description:** Implement the visible feedback for support asynchronous sign-off and review workflows beyond live sessions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** approve frame; request changes; review status chip.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T158: Async Review Persistence Preferences And Serialization
**Task Title:** Async Review Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by async review.
**Description:** Identify what parts of support asynchronous sign-off and review workflows beyond live sessions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** approve frame; request changes; review status chip.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T159: Async Review Safeguards Telemetry And Recovery
**Task Title:** Async Review Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for async review.
**Description:** Instrument support asynchronous sign-off and review workflows beyond live sessions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** approve frame; request changes; review status chip.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

### P06-T160: Async Review Tests Documentation And Rollout Gate
**Task Title:** Async Review Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship async review.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for support asynchronous sign-off and review workflows beyond live sessions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** approve frame; request changes; review status chip.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp
- /Users/ryanrentfro/code/markamp/src/core/TaskService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/MetadataPanel.cpp

## Workstream 17: Moderation And Recovery
Give facilitators tools to recover from chaos or misuse during live boards.

### P06-T161: Moderation And Recovery UX Parity Audit
**Task Title:** Moderation And Recovery UX Parity Audit
**Definition:** Define the implementation contract for moderation and recovery by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around give facilitators tools to recover from chaos or misuse during live boards.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T162: Moderation And Recovery Domain Model And State Contract
**Task Title:** Moderation And Recovery Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support moderation and recovery.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around give facilitators tools to recover from chaos or misuse during live boards.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T163: Moderation And Recovery Commands Events And Context Keys
**Task Title:** Moderation And Recovery Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for moderation and recovery.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make give facilitators tools to recover from chaos or misuse during live boards. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T164: Moderation And Recovery Workspace Surface And Controls
**Task Title:** Moderation And Recovery Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose moderation and recovery in the main canvas workspace.
**Description:** Build the primary UI surfaces for give facilitators tools to recover from chaos or misuse during live boards., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T165: Moderation And Recovery Pointer Interaction Flow
**Task Title:** Moderation And Recovery Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for moderation and recovery.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for give facilitators tools to recover from chaos or misuse during live boards.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T166: Moderation And Recovery Keyboard Gesture And Shortcut Flow
**Task Title:** Moderation And Recovery Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for moderation and recovery.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for give facilitators tools to recover from chaos or misuse during live boards.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T167: Moderation And Recovery Visual Feedback And Rendering Polish
**Task Title:** Moderation And Recovery Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make moderation and recovery legible in use.
**Description:** Implement the visible feedback for give facilitators tools to recover from chaos or misuse during live boards.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T168: Moderation And Recovery Persistence Preferences And Serialization
**Task Title:** Moderation And Recovery Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by moderation and recovery.
**Description:** Identify what parts of give facilitators tools to recover from chaos or misuse during live boards. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T169: Moderation And Recovery Safeguards Telemetry And Recovery
**Task Title:** Moderation And Recovery Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for moderation and recovery.
**Description:** Instrument give facilitators tools to recover from chaos or misuse during live boards. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

### P06-T170: Moderation And Recovery Tests Documentation And Rollout Gate
**Task Title:** Moderation And Recovery Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship moderation and recovery.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for give facilitators tools to recover from chaos or misuse during live boards.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** freeze edits; clear reactions; remove disruptive collaborator from follow mode.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/BoardLockController.h
- /Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp

## Workstream 18: Multiplayer Performance
Keep presence and live updates lightweight as collaboration scale grows.

### P06-T171: Multiplayer Performance UX Parity Audit
**Task Title:** Multiplayer Performance UX Parity Audit
**Definition:** Define the implementation contract for multiplayer performance by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around keep presence and live updates lightweight as collaboration scale grows.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T172: Multiplayer Performance Domain Model And State Contract
**Task Title:** Multiplayer Performance Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support multiplayer performance.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around keep presence and live updates lightweight as collaboration scale grows.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T173: Multiplayer Performance Commands Events And Context Keys
**Task Title:** Multiplayer Performance Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for multiplayer performance.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make keep presence and live updates lightweight as collaboration scale grows. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T174: Multiplayer Performance Workspace Surface And Controls
**Task Title:** Multiplayer Performance Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose multiplayer performance in the main canvas workspace.
**Description:** Build the primary UI surfaces for keep presence and live updates lightweight as collaboration scale grows., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T175: Multiplayer Performance Pointer Interaction Flow
**Task Title:** Multiplayer Performance Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for multiplayer performance.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for keep presence and live updates lightweight as collaboration scale grows.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T176: Multiplayer Performance Keyboard Gesture And Shortcut Flow
**Task Title:** Multiplayer Performance Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for multiplayer performance.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for keep presence and live updates lightweight as collaboration scale grows.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T177: Multiplayer Performance Visual Feedback And Rendering Polish
**Task Title:** Multiplayer Performance Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make multiplayer performance legible in use.
**Description:** Implement the visible feedback for keep presence and live updates lightweight as collaboration scale grows.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T178: Multiplayer Performance Persistence Preferences And Serialization
**Task Title:** Multiplayer Performance Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by multiplayer performance.
**Description:** Identify what parts of keep presence and live updates lightweight as collaboration scale grows. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T179: Multiplayer Performance Safeguards Telemetry And Recovery
**Task Title:** Multiplayer Performance Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for multiplayer performance.
**Description:** Instrument keep presence and live updates lightweight as collaboration scale grows. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

### P06-T180: Multiplayer Performance Tests Documentation And Rollout Gate
**Task Title:** Multiplayer Performance Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship multiplayer performance.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for keep presence and live updates lightweight as collaboration scale grows.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** defer offscreen remote cursors; batch remote selection updates; presence throttling.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h
- /Users/ryanrentfro/code/markamp/src/canvas/CanvasPerformanceMonitor.cpp
- /Users/ryanrentfro/code/markamp/src/core/Backpressure.h

## Workstream 19: Collaboration Analytics
Measure the right workshop signals without over-collecting.

### P06-T181: Collaboration Analytics UX Parity Audit
**Task Title:** Collaboration Analytics UX Parity Audit
**Definition:** Define the implementation contract for collaboration analytics by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around measure the right workshop signals without over-collecting.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T182: Collaboration Analytics Domain Model And State Contract
**Task Title:** Collaboration Analytics Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support collaboration analytics.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around measure the right workshop signals without over-collecting.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T183: Collaboration Analytics Commands Events And Context Keys
**Task Title:** Collaboration Analytics Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for collaboration analytics.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make measure the right workshop signals without over-collecting. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T184: Collaboration Analytics Workspace Surface And Controls
**Task Title:** Collaboration Analytics Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose collaboration analytics in the main canvas workspace.
**Description:** Build the primary UI surfaces for measure the right workshop signals without over-collecting., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T185: Collaboration Analytics Pointer Interaction Flow
**Task Title:** Collaboration Analytics Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for collaboration analytics.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for measure the right workshop signals without over-collecting.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T186: Collaboration Analytics Keyboard Gesture And Shortcut Flow
**Task Title:** Collaboration Analytics Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for collaboration analytics.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for measure the right workshop signals without over-collecting.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T187: Collaboration Analytics Visual Feedback And Rendering Polish
**Task Title:** Collaboration Analytics Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make collaboration analytics legible in use.
**Description:** Implement the visible feedback for measure the right workshop signals without over-collecting.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T188: Collaboration Analytics Persistence Preferences And Serialization
**Task Title:** Collaboration Analytics Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by collaboration analytics.
**Description:** Identify what parts of measure the right workshop signals without over-collecting. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T189: Collaboration Analytics Safeguards Telemetry And Recovery
**Task Title:** Collaboration Analytics Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for collaboration analytics.
**Description:** Instrument measure the right workshop signals without over-collecting. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

### P06-T190: Collaboration Analytics Tests Documentation And Rollout Gate
**Task Title:** Collaboration Analytics Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship collaboration analytics.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for measure the right workshop signals without over-collecting.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** active collaborators count; comment resolution time; voting participation.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/ActivityFeed.h
- /Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.h
- /Users/ryanrentfro/code/markamp/src/core/PrivacyManager.cpp

## Workstream 20: Collaboration Coverage
Build testable contracts for multi-user behavior and edge conditions.

### P06-T191: Collaboration Coverage UX Parity Audit
**Task Title:** Collaboration Coverage UX Parity Audit
**Definition:** Define the implementation contract for collaboration coverage by auditing current behavior, parity gaps, and release-critical outcomes.
**Description:** Review the current implementation around build testable contracts for multi-user behavior and edge conditions.. Convert product intent into explicit acceptance criteria, anti-goals, and sequencing notes so later agents can code against a stable contract instead of broad UX language.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** This task front-loads ambiguity reduction. It is the cheapest place to align Miro-parity expectations with the existing MarkAmp architecture before code changes spread across multiple subsystems.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T192: Collaboration Coverage Domain Model And State Contract
**Task Title:** Collaboration Coverage Domain Model And State Contract
**Definition:** Extend or normalize the internal state and model contracts needed to support collaboration coverage.
**Description:** Audit the relevant state holders, board model seams, and service boundaries around build testable contracts for multi-user behavior and edge conditions.. Add or refine domain objects, enums, transient state, and ownership rules so UI work does not have to smuggle logic into renderers or event handlers.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** MarkAmp already has dedicated canvas models and services. This task keeps the roadmap aligned with those patterns and avoids one-off UI state that will later block automation, multiplayer, or testing.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T193: Collaboration Coverage Commands Events And Context Keys
**Task Title:** Collaboration Coverage Commands Events And Context Keys
**Definition:** Wire the command, event, and context-key vocabulary needed for collaboration coverage.
**Description:** Add or revise the commands, events, shortcuts, and context key checks that make build testable contracts for multi-user behavior and edge conditions. accessible from the command palette, toolbars, menus, automation, and tests. Prefer stable contracts over anonymous callback chains.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** Miro-style surfaces feel coherent when every capability is reachable from multiple interaction paths. In this codebase that requires clean command and event wiring, not only direct UI hookups.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T194: Collaboration Coverage Workspace Surface And Controls
**Task Title:** Collaboration Coverage Workspace Surface And Controls
**Definition:** Implement or refine the visible controls that expose collaboration coverage in the main canvas workspace.
**Description:** Build the primary UI surfaces for build testable contracts for multi-user behavior and edge conditions., including placement, labels, empty states, toggles, and quick actions. Reuse existing chrome and panel models where possible so the feature lands as part of the application rather than a local exception.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** This task is where the capability becomes discoverable. AI coding agents should treat it as shell integration work, not a license to duplicate existing panel infrastructure.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T195: Collaboration Coverage Pointer Interaction Flow
**Task Title:** Collaboration Coverage Pointer Interaction Flow
**Definition:** Implement the mouse, touch, and stylus interaction flow for collaboration coverage.
**Description:** Define pointer-down, drag, hover, cancel, and completion behavior for build testable contracts for multi-user behavior and edge conditions.. Keep hit-testing, capture, previews, and cancellation explicit so interactions remain stable across high DPI input, large boards, and overlapping objects.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** Canvas parity depends heavily on pointer quality. This task translates the feature into deterministic interaction rules that can be replayed and tested.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T196: Collaboration Coverage Keyboard Gesture And Shortcut Flow
**Task Title:** Collaboration Coverage Keyboard Gesture And Shortcut Flow
**Definition:** Support keyboard-first access and modifier behavior for collaboration coverage.
**Description:** Map the highest-value shortcuts, modifier semantics, and keyboard-only fallbacks for build testable contracts for multi-user behavior and edge conditions.. Ensure the behavior composes with existing tool modes, command routing, accessibility expectations, and cross-platform accelerator differences.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** Power users and accessibility workflows both depend on this layer. Treat shortcut work as part of the product contract, not optional polish.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T197: Collaboration Coverage Visual Feedback And Rendering Polish
**Task Title:** Collaboration Coverage Visual Feedback And Rendering Polish
**Definition:** Add the rendering, motion, and feedback treatments that make collaboration coverage legible in use.
**Description:** Implement the visible feedback for build testable contracts for multi-user behavior and edge conditions.: affordances, overlays, preview states, selection styling, guide lines, placeholder states, and restrained motion. Keep the rendering tied to theme tokens and performance budgets.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** Without clear feedback, technically correct behavior still feels broken. This task closes the perception gap that often separates feature presence from usable parity.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T198: Collaboration Coverage Persistence Preferences And Serialization
**Task Title:** Collaboration Coverage Persistence Preferences And Serialization
**Definition:** Persist the board state, user settings, and serialization details required by collaboration coverage.
**Description:** Identify what parts of build testable contracts for multi-user behavior and edge conditions. belong in board data, workspace configuration, profile state, or transient session state. Update serializers, migration paths, default values, and repair logic so the feature survives reopen, sync, export, and rollback scenarios.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** Mature canvas tools remember user intent. This task prevents regressions where a feature works in-session but collapses at rest, in sync, or after migration.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T199: Collaboration Coverage Safeguards Telemetry And Recovery
**Task Title:** Collaboration Coverage Safeguards Telemetry And Recovery
**Definition:** Add instrumentation, safeguards, and failure handling for collaboration coverage.
**Description:** Instrument build testable contracts for multi-user behavior and edge conditions. with the metrics, warnings, fallbacks, and recovery hooks needed to operate it confidently. Cover invalid states, degraded performance, partial data, multiplayer conflicts, and user-facing recovery messaging where relevant.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** AI agents should not stop at the happy path. This task ensures every major capability has an operational envelope that engineering can support in the field.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

### P06-T200: Collaboration Coverage Tests Documentation And Rollout Gate
**Task Title:** Collaboration Coverage Tests Documentation And Rollout Gate
**Definition:** Add the tests, documentation, and exit criteria needed to ship collaboration coverage.
**Description:** Create or expand unit, integration, snapshot, and end-to-end coverage for build testable contracts for multi-user behavior and edge conditions.. Update docs, examples, and release checks so the feature can be validated repeatedly by humans, CI, and future agents.
**Examples:** offline replay test; same-object edit race; presenter follow regression.
**Context:** This task converts implementation into durable product capability. Without it, parity progress will regress as adjacent canvas work lands.
**References:**
- /Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/FollowModeController.cpp
- /Users/ryanrentfro/code/markamp/src/canvas/OfflineSyncQueue.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp
- /Users/ryanrentfro/code/markamp/tests/unit
- /Users/ryanrentfro/code/markamp/src/core/CrossModuleTestSuite.h
- /Users/ryanrentfro/code/markamp/src/canvas/CollabEventLogger.cpp

## Phase Exit Criteria
- All 200 tasks have either shipped, been explicitly deferred with rationale, or been converted into implementation issues with owners.
- Critical workflows in this phase have unit, integration, and at least one end-to-end validation path or a documented gap.
- The shipped work is theme-aware, accessibility-aware, serialization-safe, and undo/redo-safe by default.
- The phase produces measurable progress toward Miro-level editing, drawing, moving, and content authoring quality.
