# Phase 11: Canvas Workbench Shell

## Overview
The canvas system has 50+ source files (Board, CanvasObject, SelectionManager, SnapEngine, tools, renderers) but UI integration is shallow: CanvasPanel (226 lines) and CanvasWorkspacePanel exist but the canvas is not reachable as a first-class workspace mode. This phase makes canvas a fully integrated workspace surface.

## Prerequisites
- Phase 06 (Workbench navigation with Canvas mode in activity bar)
- Phase 03 (Theme engine for canvas theming)

## Tasks

### Task 1: Register Canvas as First-Class Sidebar/Workspace Mode
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Wire CanvasWorkspacePanel into the layout manager panel registry so selecting Canvas mode in the activity bar activates the canvas workspace with tool rail, inspector, and minimap.
**Acceptance Criteria:**
- Canvas mode in activity bar shows CanvasWorkspacePanel
- Tool rail (40px), canvas area (flex), inspector (240px), minimap (120px) layout
- Context bar shows board title and zoom controls
- Theme-aware: all canvas chrome uses theme tokens

### Task 2: Wire Board Open/Create Commands
**Files:** `src/ui/MainFrame.cpp`, `src/core/Events.h`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Wire `BoardOpenRequestEvent`: command palette commands "New Board" and "Open Board" emit the event. CanvasWorkspacePanel handles by creating or loading a board.
**Acceptance Criteria:**
- "New Board" creates empty board and activates canvas mode
- "Open Board" shows board picker (from workspace .markamp/boards/)
- Board loads and renders in CanvasWorkspacePanel
- `BoardLoadedEvent` emitted on successful load

### Task 3: Wire Canvas Tool Switching
**Files:** `src/canvas/CanvasInputManager.cpp`, `src/canvas/CanvasTool.h`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Connect tool rail buttons to CanvasInputManager tool switching. Tools: Select, Pan, Text, Shape, Connector, Freehand, Eraser.
**Acceptance Criteria:**
- Each tool rail button activates corresponding ICanvasTool
- Keyboard accelerators: V=Select, H=Pan, T=Text, S=Shape, C=Connector, D=Draw
- Active tool highlighted in tool rail
- `CanvasToolChangedEvent` emitted on switch
- Cursor changes to match active tool

### Task 4: Wire Canvas Object Creation Flow
**Files:** `src/canvas/DrawTool.cpp`, `src/canvas/SelectTool.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Complete the tool lifecycle for creating objects: mouse down starts creation, drag shows preview, mouse up commits the object. Wire StickyNote, TextBox, ShapeData, and ConnectorData creation.
**Acceptance Criteria:**
- Sticky notes: click to create at position with default size
- Text boxes: click and drag to define bounds
- Shapes: click and drag, shape picker for type selection
- Connectors: click source object, drag to target object
- Each creation emits `CanvasObjectAddedEvent`

### Task 5: Wire Selection and Transform Operations
**Files:** `src/canvas/SelectionManager.cpp`, `src/canvas/SelectionRenderer.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** SelectionManager (705 lines) exists with selection logic. Wire it to CanvasPanel mouse events: click to select, shift-click to add, drag to move, handle drag to resize, marquee select.
**Acceptance Criteria:**
- Click selects single object
- Shift+click adds to selection
- Click empty area deselects all
- Drag selected objects moves them
- Corner handles resize objects
- Marquee drag selects all objects in rectangle

### Task 6: Wire Undo/Redo for Canvas Operations
**Files:** `src/canvas/UndoRedoStack.cpp`, `src/canvas/CanvasCommands.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** UndoRedoStack and CanvasCommands (520 lines) exist. Wire Cmd+Z/Ctrl+Z for undo and Cmd+Shift+Z/Ctrl+Shift+Z for redo. Every canvas operation (create, move, resize, delete) must be undoable.
**Acceptance Criteria:**
- Undo reverses last canvas operation
- Redo re-applies reversed operation
- All operation types undoable: create, delete, move, resize, style change
- `CanvasUndoRedoChangedEvent` emitted after each operation
- Undo stack depth: 100 operations

### Task 7: Wire Snap Engine and Alignment Guides
**Files:** `src/canvas/SnapEngine.cpp`, `src/canvas/AlignmentGuides.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** SnapEngine and AlignmentGuides exist. Wire snapping during drag operations: snap to grid, snap to other objects (edges, centers), and show alignment guide lines.
**Acceptance Criteria:**
- Grid snapping: objects snap to grid during drag
- Object snapping: edges and centers snap to nearby objects
- Alignment guides: visible lines during snap
- Snap toggle in toolbar (default on)
- Guide line color from theme accent

### Task 8: Wire Board Serialization
**Files:** `src/canvas/BoardSerializer.cpp`, `src/canvas/Board.cpp`
**Description:** BoardSerializer exists. Wire auto-save: board state serialized to JSON on changes (debounced 2 seconds). Load board from JSON on open.
**Acceptance Criteria:**
- Board saved as JSON in `.markamp/boards/{board_id}.json`
- Auto-save on change (2 second debounce)
- Load restores all objects with positions, styles, and metadata
- `BoardSavedEvent` emitted on save

### Task 9: Wire Minimap Panel
**Files:** `src/canvas/MinimapPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** MinimapPanel exists. Wire it to show a scaled-down view of the entire board with viewport indicator. Clicking minimap pans the main view.
**Acceptance Criteria:**
- Minimap shows all objects at scale
- Current viewport highlighted as rectangle
- Click on minimap pans main canvas to that area
- Drag viewport rectangle in minimap pans in real-time
- Minimap updates on object changes

### Task 10: Wire Inspector Panel for Object Properties
**Files:** `src/canvas/MetadataPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** MetadataPanel (296 lines) exists in canvas. Wire it as the inspector in CanvasWorkspacePanel. When objects are selected, inspector shows their properties: position, size, color, text, style.
**Acceptance Criteria:**
- Inspector shows properties of selected object(s)
- Properties are editable: position (x, y), size (w, h), rotation, color, text
- Multi-select shows common properties with "mixed" indicator
- Changes apply immediately with undo support
- No selection: inspector shows board properties

### Task 11: Add Canvas Context Menu
**Files:** `src/ui/CanvasPanel.cpp`
**Description:** Right-click on canvas shows context menu: on object (Cut, Copy, Paste, Delete, Bring Forward, Send Back, Lock, Group), on empty space (Paste, New Sticky Note, New Text Box, Zoom to Fit).
**Acceptance Criteria:**
- Context menu appears at click position
- Object context menu with all standard actions
- Empty-space context menu with creation and view actions
- Menu items use theme styling
- Keyboard: context menu key shows menu

### Task 12: Add Canvas Zoom Controls
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/ViewportTransform.cpp`
**Description:** Wire zoom controls: scroll wheel zoom (centered on cursor), pinch zoom (trackpad), zoom slider in context bar, Zoom to Fit and Zoom to Selection commands.
**Acceptance Criteria:**
- Mouse wheel zooms centered on cursor position
- Pinch-to-zoom on trackpad
- Zoom slider in context bar (25% to 400%)
- "Zoom to Fit" centers all objects in viewport
- "Zoom to Selection" centers selected objects
- Zoom level shown in context bar

### Task 13: Add Canvas Keyboard Shortcuts
**Files:** `src/ui/MainFrame.cpp`, `src/core/ShortcutManager.cpp`
**Description:** Register all canvas keyboard shortcuts: tool switching, selection operations, view operations, object operations.
**Acceptance Criteria:**
- Delete/Backspace: delete selected objects
- Cmd+A / Ctrl+A: select all
- Cmd+G / Ctrl+G: group selected
- Cmd+Shift+G / Ctrl+Shift+G: ungroup
- Space+drag: pan canvas
- All shortcuts in command palette

### Task 14: Add Canvas Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register canvas commands: "Canvas: New Board", "Canvas: Open Board", "Canvas: Zoom to Fit", "Canvas: Center Selection", "Canvas: Toggle Grid", "Canvas: Toggle Snap", "Canvas: Export Board".
**Acceptance Criteria:**
- All canvas commands registered
- Commands categorized under "Canvas:" prefix
- Commands only active when canvas mode is active
- Commands ranked higher when in canvas mode

### Task 15: Add Board Template Gallery
**Files:** `src/canvas/BoardTemplate.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** BoardTemplate (242 lines) exists with template logic. Add template selection when creating new boards: blank, brainstorm, flowchart, kanban, mindmap, wireframe.
**Acceptance Criteria:**
- New Board shows template picker dialog
- At least 6 templates: blank, brainstorm, flowchart, kanban, mindmap, wireframe
- Template creates pre-configured objects and layout
- "Blank" option for empty board

### Task 16: Add Canvas Performance Monitoring
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Track and enforce canvas performance: frame time, object count rendering cost, spatial query time. Log warnings when budgets exceeded.
**Acceptance Criteria:**
- Frame time budget: 16ms (60fps target)
- Render time logged per frame in debug mode
- Warning when frame time > 32ms
- Object count impact measured per frame

### Task 17: Add Canvas Theme Integration
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Canvas background, grid, selection handles, alignment guides, and minimap all use theme tokens. Theme changes update canvas immediately.
**Acceptance Criteria:**
- Canvas background color from theme `canvas_bg` token
- Grid color from theme `canvas_grid` token
- Selection handles from theme accent color
- Alignment guides from theme `canvas_guide` token
- Theme change refreshes canvas immediately

### Task 18: Add Canvas Object Delete with Confirmation
**Files:** `src/ui/CanvasPanel.cpp`
**Description:** Deleting multiple objects shows confirmation. Single object deletes immediately (undoable). Shift+Delete force-deletes without confirmation.
**Acceptance Criteria:**
- Single object: delete immediately, undoable
- Multiple objects (>3): show confirmation dialog with count
- Shift+Delete: bypass confirmation
- Locked objects: show "Cannot delete locked objects" message

### Task 19: Add Canvas Tests
**Files:** `tests/unit/test_canvas_input.cpp`, `tests/unit/test_canvas_workspace.cpp`, `tests/unit/test_selection_manager.cpp`
**Description:** Comprehensive canvas tests: object creation, selection, transform, undo/redo, serialization, and performance.
**Acceptance Criteria:**
- Object creation: each type creates correctly
- Selection: single, multi, marquee, shift-add
- Transform: move, resize, rotate with correct bounds
- Undo/Redo: each operation type reversible
- Serialization: save/load round-trip preserves all data

### Task 20: Add Canvas Accessibility
**Files:** `tests/unit/test_accessibility.cpp`
**Description:** Canvas keyboard navigation: Tab cycles through objects, arrow keys move selected objects, Enter opens object editor, screen reader announces object type and label.
**Acceptance Criteria:**
- Tab cycles through objects in z-order
- Arrow keys move selected objects by grid unit
- Enter opens inline editor for text objects
- Screen reader announces: object type, label, position
- Focus ring visible around focused object

## Testing Requirements
- Tool lifecycle: each tool creates/edits correctly
- Selection and transform: all operations work
- Serialization: board round-trip preserves all data
- Performance: 60fps with 100 objects

## Phase Completion Criteria
- Canvas reachable as first-class workspace mode
- All core tools functional: select, pan, text, shape, connector, draw
- Selection, transform, snapping, and undo/redo working
- Board serialization with auto-save
- Minimap, inspector, and context menu functional
- All tests pass
