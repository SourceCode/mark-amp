# Phase 13: Canvas Collaboration and Facilitation

## Overview
CanvasCollabService, CanvasCommentsPanel, CanvasFacilitationPanel, and CommentObject exist with event infrastructure (session, presence, cursors, votes, timers) but are not wired to functional UI. This phase enables real-time collaborative workflows on the canvas.

## Prerequisites
- Phase 11 (Canvas workbench shell)
- Phase 12 (Canvas advanced objects)

## Tasks

### Task 1: Wire CanvasCollabService Session Management
**Files:** `src/core/CanvasCollabService.cpp`, `src/core/CanvasCollabService.h`
**Description:** Wire session create/join/leave lifecycle. Local single-user mode simulates a session for consistent API; multi-user requires WebSocket transport (stub the transport, wire the session state machine).
**Acceptance Criteria:**
- `create_session()` initializes a new collaborative session
- `join_session(session_id)` joins an existing session
- `leave_session()` cleanly exits
- `CanvasSessionJoinedEvent` / `CanvasSessionLeftEvent` emitted
- Single-user mode: implicit session, no transport needed

### Task 2: Wire Collaboration Top Bar UI
**Files:** `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Add collaboration bar showing: participant avatars (or initials), "Share" button, session status indicator, and facilitation actions.
**Acceptance Criteria:**
- Participant list shows colored initials for each participant
- "Share" button copies session link
- Session status: "Solo", "Connected", "Reconnecting"
- Facilitation buttons: Timer, Vote, Follow

### Task 3: Wire Remote Cursor Rendering
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Render remote participant cursors as named colored pointers on the canvas. Each participant has a unique color derived from their ID.
**Acceptance Criteria:**
- Remote cursors shown with participant name label
- Colors unique per participant
- Cursor position updates smoothly (interpolated)
- Cursors fade when participant is idle

### Task 4: Wire Remote Selection Rendering
**Files:** `src/canvas/SelectionRenderer.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Show which objects remote participants have selected with colored borders matching their cursor color.
**Acceptance Criteria:**
- Selected objects show colored border matching remote user's color
- Multiple remote selections shown simultaneously
- Local selection distinct from remote selections
- Selection updates in real-time

### Task 5: Wire Comment Object Creation and Threading
**Files:** `src/canvas/CommentObject.cpp`, `src/ui/CanvasCommentsPanel.cpp`
**Description:** CommentObject and CanvasCommentsPanel exist. Wire comment creation: click to place comment pin, type message, thread replies. Comments panel shows all threads.
**Acceptance Criteria:**
- Click "Comment" tool, then click canvas to place comment pin
- Comment input popup for initial message
- Reply to existing comments creates thread
- Comments panel lists all threads
- Resolve/unresolve threads

### Task 6: Wire Facilitation Timer
**Files:** `src/ui/CanvasFacilitationPanel.cpp` (TODO at line 49)
**Description:** CanvasFacilitationPanel has TODO for timer display. Wire countdown timer: start, pause, reset, visible to all participants.
**Acceptance Criteria:**
- Timer controls: start, pause, reset, set duration
- Timer visible in collaboration bar
- `CanvasTimerUpdatedEvent` emitted every second
- Audio/visual alert when timer expires
- All participants see same timer state

### Task 7: Wire Facilitation Voting
**Files:** `src/ui/CanvasFacilitationPanel.cpp` (TODO at line 91)
**Description:** CanvasFacilitationPanel has TODO for voting. Wire voting: create vote with topic and options, participants vote, results displayed live.
**Acceptance Criteria:**
- "Start Vote" opens vote creation dialog
- Topic and options (2-6) configurable
- Participants cast votes
- Results shown in real-time (bar chart)
- `CanvasVoteUpdatedEvent` emitted on each vote

### Task 8: Wire Follow Presenter Mode
**Files:** `src/canvas/BoardNavigator.cpp`, `src/canvas/MinimapPanel.cpp`
**Description:** "Follow" mode: one participant becomes presenter, others' viewports track the presenter's viewport.
**Acceptance Criteria:**
- "Follow" button in collaboration bar
- Clicking follows selected participant's viewport
- Followed participant shown in minimap
- Opt-out: any manual pan/zoom breaks follow
- Presenter can "Bring Everyone Here" to sync all viewports

### Task 9: Wire Private and Reveal Modes
**Files:** `src/ui/CanvasFacilitationPanel.cpp`
**Description:** Private mode hides all objects created by other participants until "Reveal" is triggered. Useful for brainstorming before sharing.
**Acceptance Criteria:**
- "Private Mode" hides remote objects
- Each participant works on their own view
- "Reveal" makes all objects visible simultaneously
- Private mode state indicated in UI

### Task 10: Wire Board Lock During Sessions
**Files:** `src/canvas/LockingService.cpp`, `src/core/CanvasCollabService.cpp`
**Description:** Facilitator can lock the board (read-only for all except facilitator) or lock specific object interactions during sessions.
**Acceptance Criteria:**
- "Lock Board" makes board read-only for non-facilitators
- Locked state shown with visual indicator
- Facilitator can still edit
- Unlock returns to normal editing

### Task 11: Wire Undo/Redo for Collaborative Patches
**Files:** `src/canvas/UndoRedoStack.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Separate local undo from remote patches: undo only reverses local operations, remote operations are not undoable locally.
**Acceptance Criteria:**
- Local undo only reverses user's own operations
- Remote operations not affected by local undo
- Undo history per participant
- Conflict resolution: last-write-wins for same object

### Task 12: Wire Comment Anchoring to Objects
**Files:** `src/canvas/CommentObject.cpp`
**Description:** Comments can be anchored to specific canvas objects. When the object moves, the comment moves with it.
**Acceptance Criteria:**
- Comments can reference a canvas object
- Anchored comments move with their object
- Deleting object keeps comment but marks as "orphaned"
- Comment shows object reference in thread

### Task 13: Wire Vote/Timer Metadata to Objects
**Files:** `src/canvas/MetadataPanel.cpp`, `src/canvas/TagPanel.cpp`
**Description:** Vote results and timer context can be attached to canvas objects as metadata. Objects can be filtered by vote outcome or session tags.
**Acceptance Criteria:**
- Vote results attachable to objects
- Timer context attachable to objects
- Filter objects by vote outcome
- Metadata visible in inspector

### Task 14: Add Extension Contribution Points for Collab
**Files:** `src/core/ExtensionManifest.h`, `src/core/PluginContext.h`
**Description:** Add extension contribution points for: custom facilitation tools, comment workflows, and collaboration actions.
**Acceptance Criteria:**
- Extensions can register custom facilitation actions
- Extensions can hook into comment lifecycle
- Extensions can contribute collaboration toolbar buttons
- Manifest `canvasCollaboration` contribution point defined

### Task 15: Wire Collaboration Status in Status Bar
**Files:** `src/ui/StatusBarPanel.cpp`
**Description:** Status bar shows collaboration status: participant count, session state, active timer/vote indicator.
**Acceptance Criteria:**
- Participant count shown when in session
- Timer remaining shown when active
- Vote active indicator when voting
- Click to expand collaboration panel

### Task 16: Add Collaboration Event Logging
**Files:** `src/core/OutputChannelService.cpp`
**Description:** All collaboration events logged to a "Collaboration" output channel for debugging and audit.
**Acceptance Criteria:**
- Session join/leave logged
- Object patches logged with participant ID
- Vote and timer events logged
- Comment creation/resolution logged

### Task 17: Add Collaboration Offline Support
**Files:** `src/core/CanvasCollabService.cpp`
**Description:** When connection is lost, switch to offline mode: local edits continue, queue changes, and reconcile on reconnect.
**Acceptance Criteria:**
- Offline detection with "Offline" status indicator
- Local edits continue without blocking
- Changes queued for sync
- Reconnection merges queued changes
- Conflict resolution for overlapping edits

### Task 18: Add Collaboration Performance Tests
**Files:** `tests/unit/test_canvas_collab.cpp`
**Description:** Test collaboration under load: 10 simultaneous participants, 100 rapid operations, cursor update frequency.
**Acceptance Criteria:**
- 10 participants: no performance degradation
- 100 rapid operations: all applied in correct order
- Cursor updates: 30fps display even with 10 remote cursors
- Session state consistent after all operations

### Task 19: Add Comment Thread Tests
**Files:** `tests/unit/test_canvas_collab.cpp`
**Description:** Test comment lifecycle: create, reply, resolve, unresolve, delete, anchor to object, orphan handling.
**Acceptance Criteria:**
- Comment creation with thread
- Reply creates nested thread
- Resolve/unresolve toggles
- Object anchoring and orphan detection
- Serialization preserves comment state

### Task 20: Add Facilitation Flow Tests
**Files:** `tests/unit/test_canvas_collab.cpp`
**Description:** Test facilitation tools: timer lifecycle, vote lifecycle, follow mode, private/reveal mode, board lock.
**Acceptance Criteria:**
- Timer: start, tick, pause, resume, expire
- Vote: create, cast, close, results
- Follow: engage, track, disengage
- Private/Reveal: hide, reveal, transition
- Lock: engage, verify read-only, disengage

## Testing Requirements
- Session lifecycle: create, join, leave, reconnect
- Real-time sync: cursors, selections, object patches
- Facilitation tools: timer, voting, follow, lock
- Comment threads: create, reply, resolve, anchor

## Phase Completion Criteria
- Collaborative sessions with live cursors and selections
- Facilitation tools (timer, voting) functional
- Comment threads with object anchoring
- Follow presenter mode
- Offline support with reconnection
- All tests pass
