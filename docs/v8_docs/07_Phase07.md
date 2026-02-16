Phase 12 Prompt (Realtime Collaboration + Facilitation Layer)

1. Add a collaboration core service in `/Volumes/SecondDrive/code2/mark-amp/src/core/CanvasCollabService.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/CanvasCollabService.cpp` to manage session state, participants, presence, and conflict-safe object operations.

2. Extend `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h` with collaboration events: `CanvasSessionJoinedEvent`, `CanvasSessionLeftEvent`, `CanvasPresenceUpdatedEvent`, `CanvasRemoteCursorMovedEvent`, `CanvasRemoteSelectionChangedEvent`, `CanvasRemoteObjectPatchedEvent`, `CanvasVoteStartedEvent`, `CanvasVoteUpdatedEvent`, and `CanvasTimerUpdatedEvent`.

3. Build a top collaboration bar in `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.h` with participant avatars, “Share”, session status, facilitation timer, and quick actions (`Follow`, `Bring Everyone Here`, `Start Vote`, `Start Timer`).

4. Add live multiplayer overlays in `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/canvas/CanvasRenderer.cpp` for remote cursors, named selections, viewport outlines, and soft presence halos around active objects.

5. Implement comment threads + mentions as first-class canvas objects in `/Volumes/SecondDrive/code2/mark-amp/src/canvas/CommentObject.h` and `/Volumes/SecondDrive/code2/mark-amp/src/canvas/CommentObject.cpp`, then surface thread management in `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasCommentsPanel.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasCommentsPanel.cpp`.

6. Add facilitation tools matching whiteboard workflows in `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasFacilitationPanel.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasFacilitationPanel.cpp`: timer, voting, private mode, reveal mode, and lock board/object interactions during sessions.

7. Integrate voting and timer metadata into object state via `/Volumes/SecondDrive/code2/mark-amp/src/canvas/MetadataPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/canvas/TagPanel.cpp` so sessions can filter by votes, owner, and status.

8. Add “follow presenter” and “bring to me” navigation support in `/Volumes/SecondDrive/code2/mark-amp/src/canvas/BoardNavigator.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/canvas/MinimapPanel.cpp`, including animated viewport transitions and opt-out controls.

9. Extend undo/redo + command model for collaborative patch streams in `/Volumes/SecondDrive/code2/mark-amp/src/canvas/UndoRedoStack.h` and `/Volumes/SecondDrive/code2/mark-amp/src/canvas/CanvasCommands.h`, separating local undo units from remote applied patches.

10. Add extension contribution points for collab UI/actions in `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionManifest.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/PluginContext.h`, and `/Volumes/SecondDrive/code2/mark-amp/src/core/PluginManager.cpp` so plugins can add facilitation commands, custom voting panels, and comment workflows.

11. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_collab.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_input.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_board_model.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_undo_redo.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_metadata_panel.cpp` for presence state, remote patch application, timer/vote lifecycle, thread resolution, and local-vs-remote undo behavior.

12. Acceptance criteria: collaborative sessions support live cursors/selections/comments, facilitation tools run end-to-end, presenter-follow works reliably, remote updates are conflict-safe, extension hooks are available for future canvas apps, and validation passes with `cmake --preset debug`, `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`, and `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`.
