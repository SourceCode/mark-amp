# Phase 45 -- Drag and Drop System

## Objective

Build a unified drag-and-drop system that supports tab reordering, tab dragging between editor groups, file dragging from the explorer to the editor, file dragging from the desktop into the application, panel reordering, and cross-panel drag coordination. Extend the existing DragDropModel with rendering (ghost previews, drop zone highlights, drop indicators) and integrate with PaneManager, TabBar, FileTreeCtrl, and ToolWindowHost.

## Prerequisites

- Phase 44 complete (Dialog and Modal System)
- `/Users/ryanrentfro/code/markamp/src/ui/DragDropModel.h` -- existing model with state machine, threshold, drop zones
- `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.h` -- split pane management with binary tree
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` -- tab strip rendering
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.h` -- file explorer tree
- `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.h` -- dockable panel management
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` -- layout coordination

## VSCode / WebStorm Reference Behavior

- Tab drag: grab tab, see ghost preview following cursor, drop on tab bar to reorder
- Tab drag to edge of editor area creates new split (left/right/top/bottom indicators)
- File drag from explorer: drag file from tree, ghost shows filename, drop on editor opens file
- File drag from OS: drag file from Finder/Explorer onto app window, opens in new tab
- Panel drag: drag panel header between sidebar/bottom bar, drop indicators show valid positions
- Drag threshold: 5px movement before drag initiates (prevents accidental drags)
- Escape cancels any drag operation
- Auto-scroll: dragging near edge of scrollable area scrolls automatically

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/DragDropModel.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/DragController.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_drag_drop_system.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Extend DragDropModel with Drag Payload Types

**Description:** Extend the existing DragDropModel to support typed drag payloads (tab, file, panel, external file) so drop zones can validate whether they accept the specific drag content.

**Implementation Details:** Add `DragPayloadType` enum: `kTab, kFile, kPanel, kExternalFile, kText`. Add `DragPayload` struct: `{ DragPayloadType type; string source_id; string display_name; vector<string> file_paths; // for multi-file drags map<string, string> metadata; }`. Extend `begin_drag()` to accept a DragPayload instead of just source_id. Extend DropZone with `accepted_types` set so zones can declare which payload types they accept. The `zone_validity()` method now checks type compatibility.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragDropModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp` (modify)

**Acceptance Criteria:**
- DragPayload carries type and metadata
- Drop zones declare accepted payload types
- zone_validity() rejects incompatible types
- Multi-file drag payloads carry all file paths
- Existing DragDropModel tests still pass

**Dependencies:** None

---

### Task 02 -- Create DragController (Centralized Drag Coordinator)

**Description:** Create a singleton DragController that coordinates drag operations across all panels. It owns the DragDropModel, manages the ghost window, and routes drag events to the appropriate drop targets.

**Implementation Details:** DragController manages the entire drag lifecycle: (1) receives `start_drag(wxWindow* source, DragPayload payload, wxPoint start_pos)`, (2) creates DragGhostWindow, (3) captures mouse globally, (4) on mouse move: updates ghost position, hit-tests all registered drop zones, highlights valid zones via DropIndicatorOverlay, (5) on mouse up: completes or cancels based on zone validity, (6) on Escape: cancels. The controller maintains a registry of `IDropTarget` interfaces that panels implement. Each IDropTarget provides: `hit_test(wxPoint) -> DropZone`, `accept_drop(DragPayload) -> bool`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (create)

**Acceptance Criteria:**
- DragController manages full drag lifecycle
- Mouse capture works globally during drag
- Hit testing routes to correct drop target
- Escape cancels the drag from any position
- Multiple panels can register as drop targets

**Dependencies:** Task 01

---

### Task 03 -- Implement DragGhostWindow

**Description:** Create a floating semi-transparent window that follows the cursor during drag operations, showing a preview of the dragged item.

**Implementation Details:** DragGhostWindow is a `wxPopupWindow` (no border, no title bar) that positions itself at `cursor_pos + offset(8, 8)`. For tab drags: renders tab title text with file icon at 70% opacity. For file drags: renders filename with file icon. For multi-file drags: renders first filename with "+N more" count badge. For panel drags: renders panel title with panel icon. The ghost window has a subtle rounded rectangle background with 1px border. Size auto-adjusts to content. The ghost window does not receive mouse events (pass-through using `wxPOPUP_WINDOW`).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp` (create)

**Acceptance Criteria:**
- Ghost window follows cursor with 8px offset
- Ghost renders correct content for each drag type
- Ghost is semi-transparent (70% opacity)
- Multi-file ghost shows count badge
- Ghost does not intercept mouse events

**Dependencies:** Task 02

---

### Task 04 -- Implement DropIndicatorOverlay

**Description:** Create a visual overlay that highlights valid drop zones during drag operations. Shows colored rectangles, insertion lines, and split-direction indicators.

**Implementation Details:** DropIndicatorOverlay is a transparent wxPanel that covers the target area. It renders different indicators based on drop type: (1) Tab reorder: vertical blue line (2px wide) at the insertion position between tabs, (2) Tab split: semi-transparent blue rectangle covering left/right/top/bottom half of the editor area, (3) Panel dock: semi-transparent rectangle at the target dock position, (4) File drop: border highlight around the target editor panel. Indicator colors: valid = accent_primary at 30% opacity, invalid = danger at 30% opacity. The overlay is created/destroyed by DragController as the cursor moves over different zones.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.cpp` (create)

**Acceptance Criteria:**
- Tab insertion line renders at correct position between tabs
- Split indicator covers correct half of editor area
- Panel dock indicator highlights target position
- Invalid drop zones show red indicator
- Overlay does not intercept mouse events

**Dependencies:** Task 02

---

### Task 05 -- Implement Tab Drag to Reorder

**Description:** Enable dragging tabs within the same tab bar to reorder them.

**Implementation Details:** In TabBar, on mouse-down on a tab, record the start position and tab index. On mouse-move, if distance exceeds drag threshold (5px), call `DragController::start_drag()` with a Tab payload. TabBar implements IDropTarget with a hit-test that returns insertion positions between tabs. During drag, the DropIndicatorOverlay shows a vertical insertion line. On drop, TabBar reorders the tab by moving it to the insertion index. The original tab position shows a subtle gap during drag to indicate where it came from. Publish `TabReorderedEvent` on completion.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (modify)

**Acceptance Criteria:**
- Dragging a tab shows ghost and insertion indicator
- Dropping reorders the tab to the new position
- Tab bar gap shows original position during drag
- Drag threshold prevents accidental reorders
- TabReorderedEvent fires with old and new indices

**Dependencies:** Tasks 02, 03, 04

---

### Task 06 -- Implement Tab Drag Between Editor Groups

**Description:** Enable dragging a tab from one editor group to another, moving the document between panes.

**Implementation Details:** When a tab is dragged outside its TabBar's bounds, the DragController checks other TabBars registered as drop targets. If the cursor is over another TabBar, show the insertion indicator in that bar. On drop, call `PaneManager::move_document()` to transfer the document from the source pane to the target pane. If the source pane becomes empty (last tab moved out), close the pane via `PaneManager::close_pane()`. Publish `DocumentMovedEvent` with source pane, target pane, and document ID.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.cpp` (modify)

**Acceptance Criteria:**
- Tab can be dragged from one editor group to another
- Document transfers between panes correctly
- Empty source pane closes automatically
- Drop indicator shows in target TabBar
- DocumentMovedEvent fires with correct data

**Dependencies:** Task 05

---

### Task 07 -- Implement Tab Drag to Create New Split

**Description:** When a tab is dragged to the edge of an editor area (top/bottom/left/right quarter), create a new split in that direction and move the tab to the new pane.

**Implementation Details:** Define 4 edge zones in each editor pane: top 25%, bottom 25%, left 25%, right 25%. When the cursor enters an edge zone during tab drag, the DropIndicatorOverlay shows a semi-transparent rectangle covering that region (indicating "drop here to split"). On drop: call `PaneManager::split_pane()` with the appropriate SplitDirection, then `PaneManager::move_document()` to the new pane. The split ratio defaults to 0.5. Edge zones are only active when the cursor is dragging a tab (not during file drags).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.cpp` (modify)

**Acceptance Criteria:**
- Dragging tab to edge shows split preview indicator
- Dropping creates new split in the indicated direction
- Document moves to the newly created pane
- Split ratio defaults to 50%
- Works for all 4 directions

**Dependencies:** Task 06

---

### Task 08 -- Implement File Drag from Explorer to Editor

**Description:** Enable dragging files from the FileTreeCtrl to the editor area to open them in a specific pane.

**Implementation Details:** In FileTreeCtrl, on mouse-down on a file item (not folder), record position. On mouse-move past threshold, call `DragController::start_drag()` with a File payload containing the file path. Editor panes implement IDropTarget for File payloads. On drop, call `PaneManager::open_in_pane()` to open the file in the target pane. For folder drags, the editor rejects the drop (zone_validity returns kInvalid). Multi-selection in the file tree creates a multi-file payload, opening all files in the target pane.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- File drag from explorer to editor opens the file
- Folder drag is rejected (invalid drop zone indicator)
- Multi-file drag opens all files
- Ghost shows filename during drag
- File opens in the specific pane that was targeted

**Dependencies:** Tasks 02, 03

---

### Task 09 -- Implement External File Drop (OS to App)

**Description:** Enable dragging files from the operating system (Finder/Explorer) into the application window to open them.

**Implementation Details:** Register the MainFrame as a `wxFileDropTarget`. Override `OnDropFiles(x, y, filenames)` to: (1) determine which pane the drop coordinates map to, (2) for each file, call `PaneManager::open_in_pane()` for the target pane. If dropped on the file tree area, add the file to the workspace (if it is outside the current workspace). Support dragging folders to open as workspace. The DragController detects OS-initiated drags via `wxDragResult` and shows the appropriate DropIndicatorOverlay during the OS drag hover.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.h` (modify)

**Acceptance Criteria:**
- Dragging a file from Finder/Explorer onto the app opens it
- Dragging multiple files opens all of them
- Dragging a folder opens it as workspace
- Drop indicator shows during OS drag hover
- Files open in the pane under the cursor

**Dependencies:** Task 02

---

### Task 10 -- Implement Panel Drag to Reorder

**Description:** Enable dragging tool window panels to reorder them within their dock position (e.g., reorder tabs in the bottom panel).

**Implementation Details:** Tool window tab headers are draggable. On mouse-down on a panel tab header, initiate drag with Panel payload. Other panel tabs in the same dock position act as drop targets with insertion indicators. On drop, call `ToolWindowHost::move_panel_after()` or `move_panel_before()` to reorder. The ghost shows the panel title and icon. Panels cannot be reordered if they are in different groups (ToolWindowHost groups constraint).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp` (modify)

**Acceptance Criteria:**
- Panel tabs can be dragged to reorder
- Insertion indicator shows between tabs
- Panel order persists after reorder
- Ghost shows panel title during drag
- Group constraints are respected

**Dependencies:** Tasks 02, 03, 04

---

### Task 11 -- Implement Panel Drag Between Dock Positions

**Description:** Enable dragging panels between dock positions (e.g., from bottom to right sidebar), respecting dock constraints.

**Implementation Details:** When a panel tab is dragged outside its dock area, the DragController checks other dock positions registered as drop targets. If the panel's `allowed_positions` includes the target dock position, show a valid drop indicator covering the target dock area. On drop, call `ToolWindowHost::set_dock_position()` to move the panel. If the target position is not allowed, show an invalid indicator. The ghost changes appearance when moving between dock positions (shows an arrow indicating the move direction).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)

**Acceptance Criteria:**
- Panels can be dragged between dock positions
- Dock constraints are enforced (invalid positions rejected)
- Drop indicator covers the target dock area
- Panel moves to new dock position on drop
- Layout updates immediately after panel move

**Dependencies:** Task 10

---

### Task 12 -- Implement Drag Auto-Scroll

**Description:** When dragging near the edge of a scrollable area (file tree, tab bar with overflow), automatically scroll the content to reveal more items.

**Implementation Details:** During drag, when the cursor is within 30px of the top or bottom edge of a scrollable panel, trigger auto-scroll in that direction. Scroll speed increases as the cursor gets closer to the edge: 30px zone = slow (1 line/100ms), 15px zone = medium (2 lines/100ms), 5px zone = fast (4 lines/100ms). Auto-scroll uses a wxTimer ticking at 100ms. Auto-scroll stops when the cursor moves away from the edge zone or the drag ends. Works for both vertical scroll (file tree, long tab lists) and horizontal scroll (wide tab bars).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)

**Acceptance Criteria:**
- Auto-scroll triggers when cursor is near scrollable edges during drag
- Scroll speed increases near the edge
- Auto-scroll stops when cursor moves away
- Works for both vertical and horizontal scrolling
- No scroll jitter or oscillation

**Dependencies:** Task 02

---

### Task 13 -- Implement Drag Cancel with Escape

**Description:** Pressing Escape during any drag operation cancels it, returning all items to their original positions with a snap-back animation on the ghost.

**Implementation Details:** DragController listens for `wxEVT_KEY_DOWN` with `WXK_ESCAPE` during active drag. On Escape: (1) animate the ghost window back to the original start position (200ms, ease-out), (2) remove all drop indicators, (3) set DragDropModel state to `kCancelled`, (4) release mouse capture, (5) destroy ghost window after animation completes. The model's `cancel()` method resets all internal state. No events are published for cancelled drags (source panel does not receive a drop event).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)

**Acceptance Criteria:**
- Escape cancels drag from any state
- Ghost animates back to start position
- All drop indicators are removed
- No side effects from cancelled drag
- Mouse capture is released cleanly

**Dependencies:** Task 02, Task 03

---

### Task 14 -- Implement File Drop to Create Link in Markdown

**Description:** When a file is dragged from the explorer and dropped into an active Markdown editor, insert a Markdown link at the drop position instead of opening the file.

**Implementation Details:** When EditorPanel receives a file drop (from FileTreeCtrl or OS) while in Markdown editing mode, detect the drop position in the document (character position from mouse coordinates). Instead of opening the file, insert a Markdown link: `[filename](relative_path)` for documents, `![filename](relative_path)` for images (png, jpg, gif, svg). The relative path is computed from the current document's directory to the dropped file. If the file is an image and the editor is in preview mode, show the image inline.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Dropping a file in Markdown editor inserts a link
- Image files insert image syntax (![])
- Links use relative paths
- Drop position determines insertion point in document
- Works for both explorer drag and OS file drag

**Dependencies:** Task 08, Task 09

---

### Task 15 -- Implement Multi-Selection Drag

**Description:** Support dragging multiple selected items (tabs, files) as a single drag operation with a ghost showing the count.

**Implementation Details:** When multiple tabs are selected (Cmd/Ctrl+Click) and one is dragged, all selected tabs are included in the drag payload as a multi-item Tab payload. The ghost shows the first tab title plus a count badge ("+N"). On drop, all tabs are moved/reordered together. Similarly for FileTreeCtrl: multi-selected files create a multi-file drag payload. The DragPayload `file_paths` vector carries all paths. Drop targets handle multi-item drops by processing each item sequentially.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)

**Acceptance Criteria:**
- Multiple selected tabs drag together
- Multiple selected files drag together
- Ghost shows count badge for multi-item drags
- All items are processed on drop
- Multi-drag between editor groups moves all tabs

**Dependencies:** Tasks 05, 08

---

### Task 16 -- Implement Drag Threshold Configuration

**Description:** Make the drag threshold (minimum pixels of mouse movement before drag initiates) configurable via settings.

**Implementation Details:** Add setting `editor.dragThreshold` (int, default 5, min 2, max 20). DragDropModel's `drag_threshold_` reads from this setting. The DragController passes the configured threshold to the model on initialization. A lower threshold makes drags more sensitive, a higher threshold prevents accidental drags. The setting appears in the "Editor" section of Settings panel as a NumberStepper.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragDropModel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (modify)

**Acceptance Criteria:**
- Drag threshold is configurable from 2 to 20 pixels
- Default threshold is 5 pixels
- Setting takes effect immediately
- Lower threshold makes drags more sensitive
- Setting appears in Settings panel

**Dependencies:** Task 02

---

### Task 17 -- Implement Drag Preview for Tab Content

**Description:** For tab drags, show a miniature preview of the tab's content (editor text or preview panel) in the ghost window instead of just the tab title.

**Implementation Details:** When a tab drag starts, capture a screenshot of the tab's content panel (using `wxClientDC` and `wxBitmap::Create()`). Scale the screenshot down to 120x80px and render it in the ghost window below the tab title. The preview is captured once at drag start (not updated during drag). For performance, limit the bitmap to 120x80 native pixels (not retina). If the capture fails (e.g., panel not visible), fall back to title-only ghost.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragGhostWindow.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (modify)

**Acceptance Criteria:**
- Ghost shows miniature preview of tab content
- Preview is 120x80px scaled down
- Preview is captured once at drag start (not continuously)
- Fallback to title-only if capture fails
- No performance impact from screenshot capture

**Dependencies:** Task 03

---

### Task 18 -- Implement Drag Feedback Sounds

**Description:** Add optional subtle sound feedback for drag operations: pick-up sound on drag start, drop sound on successful drop, cancel sound on escape.

**Implementation Details:** Add setting `editor.dragSounds` (bool, default false). When enabled: play a subtle click sound on drag start (50ms duration, low volume), a soft thud on successful drop, and a swoosh on cancel. Reuse the platform audio infrastructure from Phase 43 Task 10. Sounds do not play in DND mode. Provide 3 built-in sounds in the application bundle.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)

**Acceptance Criteria:**
- Sounds play on drag start, drop, and cancel when enabled
- Sounds are subtle and non-intrusive
- DND mode suppresses drag sounds
- Sounds can be disabled via settings
- No delay or latency in sound playback

**Dependencies:** Task 02, Phase 43 Task 10

---

### Task 19 -- Implement Drag-and-Drop Undo

**Description:** Tab moves and file reorders initiated by drag-and-drop are undoable via Cmd+Z. After undoing, the tab or panel returns to its original position.

**Implementation Details:** DragController records each successful drop as an undoable action. Create `DragUndoAction` implementing the command pattern: stores source pane, target pane, document ID (for tab moves) or original panel order (for panel reorders). On undo, reverse the operation (move document back, reorder panel back). Register the action with the existing Command undo stack. Undo only covers the most recent drag operation (not the entire drag history).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/Command.h` (modify if needed)

**Acceptance Criteria:**
- Tab move via drag is undoable with Cmd+Z
- Panel reorder via drag is undoable
- Undo restores original position exactly
- Redo redoes the drag operation
- Only successful drops are recorded (cancelled drags are not)

**Dependencies:** Task 05, Task 10

---

### Task 20 -- Wire Drop Zones for All Panels

**Description:** Register all draggable and droppable panels with the DragController: editor panes, tab bars, file tree, tool window host, sidebar areas.

**Implementation Details:** Each panel that participates in drag-and-drop implements `IDropTarget` and registers with DragController on creation: TabBar (accepts Tab and File payloads), EditorPanel (accepts File and ExternalFile payloads), FileTreeCtrl (accepts File payloads for move/copy), ToolWindowHost (accepts Panel payloads), Sidebar dock areas (accept Panel payloads). Each IDropTarget implementation provides `hit_test()` that maps screen coordinates to zone IDs and `accept_drop()` that executes the drop action.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp` (modify)

**Acceptance Criteria:**
- All panels register as drop targets with DragController
- Hit testing correctly identifies zones in each panel
- Drop actions execute correctly for each panel type
- Incompatible drops are rejected with invalid indicator
- Registration/deregistration handles panel lifecycle correctly

**Dependencies:** Tasks 05-11

---

### Task 21 -- Implement Cursor Feedback During Drag

**Description:** Change the cursor during drag operations to indicate drag state: grab cursor while dragging, copy cursor when Cmd/Ctrl is held (copy instead of move), no-drop cursor over invalid zones.

**Implementation Details:** DragController sets the cursor via `wxSetCursor()` based on drag context: `wxCURSOR_HAND` during normal drag, `wxCURSOR_COPY_ARROW` (custom) when Cmd/Ctrl is held (indicating copy operation), `wxCURSOR_NO_ENTRY` over invalid drop zones. The copy modifier (Cmd on macOS, Ctrl on Windows/Linux) changes the drag semantics from move to copy for file drags (creates a copy instead of moving). Tab drags always move (no copy semantics). Cursor updates immediately on modifier key press/release during drag.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)

**Acceptance Criteria:**
- Hand cursor during normal drag
- Copy cursor when modifier key is held
- No-entry cursor over invalid zones
- Cursor updates immediately on modifier press/release
- Cursor resets to default when drag ends

**Dependencies:** Task 02

---

### Task 22 -- Implement Drag Accessibility Announcements

**Description:** Provide screen reader announcements for drag operations so visually impaired users can understand what is being dragged and where it can be dropped.

**Implementation Details:** On drag start, announce "Dragging {item name}. Use arrow keys to move, Enter to drop, Escape to cancel." When entering a valid drop zone, announce "Over {zone name}. Press Enter to drop here." When entering an invalid zone, announce "Cannot drop here." On successful drop, announce "{item name} moved to {zone name}." On cancel, announce "Drag cancelled." Alternative keyboard drag mode: hold Alt+Shift and use arrow keys to move items between tabs/panes. This enables drag-and-drop without a mouse.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)

**Acceptance Criteria:**
- Screen reader announces drag start, zone changes, and completion
- Keyboard-only drag mode works with Alt+Shift+Arrows
- Invalid zones are announced
- Cancel is announced
- Alternative drag mode is documented in keyboard shortcuts

**Dependencies:** Task 02

---

### Task 23 -- Implement Cross-App Drag (Export)

**Description:** Enable dragging a file tab out of the MarkAmp window to the desktop or another application, creating a file reference.

**Implementation Details:** When a tab drag exits the application window bounds, transition from internal drag to OS drag by creating a `wxFileDataObject` with the file path. Use `wxDropSource::DoDragDrop()` to initiate the OS-level drag. The OS handles the external drag (e.g., dropping on Finder creates a file copy, dropping on another editor opens the file). If the user drags back into the MarkAmp window, cancel the OS drag and resume internal drag. This only works for files that exist on disk (not for unsaved/untitled documents).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DragController.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (modify)

**Acceptance Criteria:**
- Dragging tab outside app window initiates OS drag
- Dropping on desktop creates file copy
- Dragging back into app cancels OS drag and resumes
- Unsaved files cannot be dragged externally
- OS drag visual matches platform convention

**Dependencies:** Task 05

---

### Task 24 -- Performance Validation for Drag Operations

**Description:** Ensure drag operations maintain 60fps: ghost window updates, drop indicator rendering, and hit testing must all complete within frame budget.

**Implementation Details:** Profile drag operations using EditorPerformanceBudget. Measure: ghost window repositioning time, drop indicator paint time, hit-test computation time per frame. Assert all three are under 2ms each (total under 6ms, well within 16ms frame budget). Test with: 20 open tabs, 3 split panes, 1000-file project tree. Verify no frame drops during continuous drag movement. If hit testing is slow, implement spatial indexing (quadtree) for drop zones.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_drag_drop_system.cpp` (extend)

**Acceptance Criteria:**
- Ghost repositioning under 2ms per frame
- Drop indicator paint under 2ms per frame
- Hit testing under 2ms per frame
- No frame drops during continuous drag
- Works with 20 tabs, 3 panes, 1000 files

**Dependencies:** Tasks 02-12

---

### Task 25 -- Add CMake Integration and Unit Tests

**Description:** Add all new drag-and-drop files to CMakeLists.txt and create comprehensive unit tests for DragDropModel extensions, DragController lifecycle, and drop zone validation.

**Implementation Details:** Add all new .h/.cpp files to CMakeLists.txt. Create `test_drag_drop_system.cpp` with sections: (1) DragPayloadType validates accepted types correctly, (2) DragDropModel threshold prevents premature drag start, (3) DragDropModel state machine transitions (idle->pending->dragging->completed), (4) DragDropModel cancel resets all state, (5) Drop zone validity rejects incompatible types, (6) Multi-item payload carries all items, (7) Drag undo restores original state, (8) Auto-scroll zones calculate speed correctly, (9) Drop indicator positions calculate correctly for tab insertion, (10) Cross-pane document move updates both source and target.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_drag_drop_system.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles without errors
- All 10 test sections pass
- DragDropModel tests work without GUI
- No undefined symbol errors
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- Tab drag reorder works within same tab bar
- Tab drag between editor groups moves documents
- Tab drag to edge creates new split
- File drag from explorer opens file in target pane
- File drag from OS desktop opens in app
- Panel drag reorders and moves between dock positions
- Escape cancels all drag operations
- Auto-scroll works during drag near edges
- Ghost preview follows cursor smoothly at 60fps
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
