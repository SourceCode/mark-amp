# Phase 12: Canvas Advanced Objects and Diagrams

## Overview
The canvas has basic object types but advanced objects (kanban, tables, mind maps, diagram shapes, frames, sections) exist as code (KanbanController, TableObject, MindMapController, DiagramShapeObject, FrameObject, SectionObject) without full UX integration. This phase completes the object ecosystem.

## Prerequisites
- Phase 11 (Canvas workbench shell)

## Tasks

### Task 1: Wire Frame Object for Canvas Organization
**Files:** `src/canvas/FrameObject.cpp`, `src/canvas/FrameRenderer.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** FrameObject exists. Wire it as a visual grouping container: objects inside a frame move with the frame, frames have titles, and frames can be resized.
**Acceptance Criteria:**
- Frame creation via tool or command
- Objects inside frame bounds are automatically grouped with frame
- Moving frame moves all contained objects
- Frame title editable inline
- Frames visible in board outline/navigator

### Task 2: Wire Section Object for Large Canvas Organization
**Files:** `src/canvas/SectionObject.cpp`, `src/canvas/SectionRenderer.cpp`
**Description:** SectionObject exists. Wire it as a large-canvas organizational unit: sections divide the canvas into labeled regions, navigable from the board navigator.
**Acceptance Criteria:**
- Sections created via command or context menu
- Sections have titles and optional descriptions
- Section list in minimap for quick navigation
- Sections can have distinct background colors

### Task 3: Wire KanbanController for Canvas Kanban Boards
**Files:** `src/canvas/KanbanController.cpp`, `src/canvas/KanbanObjects.cpp`
**Description:** KanbanController (299 lines) and KanbanObjects (464 lines) exist. Wire them so users can create kanban layouts on the canvas with columns and draggable cards.
**Acceptance Criteria:**
- "Insert Kanban" command creates a kanban layout
- Columns addable/removable/renamable
- Cards draggable between columns
- Card content editable inline
- `AVKanbanCardMovedEvent` emitted on card move

### Task 4: Wire TableObject for Canvas Data Tables
**Files:** `src/canvas/TableObject.cpp`, `src/canvas/TableRenderer.cpp`, `src/canvas/TableSortFilter.cpp`
**Description:** TableObject (307 lines), TableRenderer, and TableSortFilter exist. Wire them for inline data tables on the canvas with sorting and filtering.
**Acceptance Criteria:**
- "Insert Table" command creates table object
- Columns addable/removable/resizable
- Rows addable/removable
- Sort by column header click
- Filter row for column filtering

### Task 5: Wire MindMapController
**Files:** `src/canvas/MindMapController.cpp`, `src/canvas/MindMapNode.cpp`
**Description:** MindMapController (283 lines) and MindMapNode exist. Wire for mind map creation: central node with branching children, auto-layout, and expand/collapse.
**Acceptance Criteria:**
- "Insert Mind Map" command creates central node
- Tab creates child node
- Enter creates sibling node
- Auto-layout positions nodes
- Collapse/expand branches

### Task 6: Wire DiagramShapeObject for Technical Diagrams
**Files:** `src/canvas/DiagramShapeObject.cpp`, `src/canvas/DiagramLibraryPanel.cpp`
**Description:** DiagramShapeObject and DiagramLibraryPanel exist. Wire a shape library panel with standard diagram shapes: rectangles, diamonds, circles, arrows, cylinders, etc.
**Acceptance Criteria:**
- Shape library panel shows categorized shapes
- Drag shapes onto canvas to create
- Standard shapes: rectangle, rounded-rect, diamond, circle, triangle, arrow, cylinder
- Shapes support text labels
- Connectors attach to shape ports

### Task 7: Wire ShapeRecognizer for Freehand-to-Shape
**Files:** `src/canvas/ShapeRecognizer.cpp`
**Description:** ShapeRecognizer (458 lines) exists. Wire it so freehand drawings are recognized and optionally converted to clean shapes.
**Acceptance Criteria:**
- After freehand draw, system suggests shape recognition
- Recognized shapes: line, rectangle, circle, triangle, arrow
- User can accept or keep freehand
- Recognition confidence shown
- Toggle recognition on/off in settings

### Task 8: Wire ConnectorData for Advanced Routing
**Files:** `src/canvas/ConnectorData.cpp`, `src/canvas/ConnectorRenderer.cpp`
**Description:** ConnectorData (364 lines) and ConnectorRenderer exist. Wire advanced connector routing: orthogonal, curved, and straight styles with midpoint handles.
**Acceptance Criteria:**
- Three connector styles: straight, curved, orthogonal
- Connectors attach to object ports (top, bottom, left, right, center)
- Midpoint handles for manual routing
- Labels on connectors
- Style switchable after creation

### Task 9: Wire GroupObject for Manual Grouping
**Files:** `src/canvas/GroupObject.cpp`, `src/canvas/GroupingService.cpp`
**Description:** GroupObject and GroupingService exist. Wire group/ungroup operations: Cmd+G to group, Cmd+Shift+G to ungroup, double-click to enter group.
**Acceptance Criteria:**
- Select multiple objects, Cmd+G groups them
- Cmd+Shift+G ungroups
- Double-click group enters group editing mode
- Click outside exits group editing mode
- Groups can be nested

### Task 10: Wire LayeringService for Z-Order Management
**Files:** `src/canvas/LayeringService.cpp`
**Description:** LayeringService exists. Wire bring-to-front, send-to-back, bring-forward, send-backward operations.
**Acceptance Criteria:**
- Context menu: Bring to Front, Send to Back, Bring Forward, Send Backward
- Keyboard shortcuts for each operation
- Z-order preserved in serialization
- Group z-order respected (all group members above/below)

### Task 11: Wire LockingService for Object Protection
**Files:** `src/canvas/LockingService.cpp`
**Description:** LockingService exists. Wire lock/unlock: locked objects cannot be moved, resized, or edited. Visual lock indicator.
**Acceptance Criteria:**
- Lock/unlock via context menu and shortcut
- Locked objects show lock icon
- Locked objects cannot be moved/resized/edited
- Locked objects can be selected (for inspection)
- Unlock requires explicit action

### Task 12: Wire ImageObject for Image Embedding
**Files:** `src/canvas/ImageObject.cpp`, `src/canvas/ImageObjectRenderer.cpp`
**Description:** ImageObject (412 lines) and ImageObjectRenderer exist. Wire image drag-and-drop onto canvas, clipboard paste, and file picker insertion.
**Acceptance Criteria:**
- Drag image file onto canvas creates ImageObject
- Paste image from clipboard creates ImageObject
- "Insert Image" command shows file picker
- Images resizable with aspect ratio lock
- Image stored as reference path or embedded

### Task 13: Wire VideoEmbedObject for Video Embedding
**Files:** `src/canvas/VideoEmbedObject.cpp`
**Description:** VideoEmbedObject exists. Wire video URL embedding (YouTube, Vimeo, etc.) on the canvas with preview thumbnail.
**Acceptance Criteria:**
- Paste video URL creates embed object
- Thumbnail preview shown
- Click to play (opens external or inline player)
- Resize supported

### Task 14: Wire BookmarkCardObject for Link Cards
**Files:** `src/canvas/BookmarkCardObject.cpp`
**Description:** BookmarkCardObject exists for rich link previews. Wire URL paste to create bookmark cards with title, description, and favicon.
**Acceptance Criteria:**
- Paste URL creates bookmark card
- Card shows title, description, favicon (from MetadataScraper)
- Click opens URL in browser
- Editable title and description

### Task 15: Wire IconLibrary for Canvas Icons
**Files:** `src/canvas/IconLibrary.cpp`, `src/canvas/IconObject.cpp`
**Description:** IconLibrary and IconObject exist. Wire icon insertion from a searchable library panel.
**Acceptance Criteria:**
- "Insert Icon" opens icon library panel
- Search to filter icons
- Click inserts icon at cursor position on canvas
- Icons scalable and colorable
- Icon color from theme or custom

### Task 16: Wire Canvas Search
**Files:** `src/canvas/CanvasSearch.cpp`, `src/canvas/SearchBar.cpp`
**Description:** CanvasSearch and SearchBar exist. Wire in-canvas search: Cmd+F in canvas mode searches object labels/text, results highlight matching objects.
**Acceptance Criteria:**
- Cmd+F in canvas mode opens search bar
- Search matches object labels, text content, metadata
- Matching objects highlighted
- Navigate between matches with arrows
- Viewport pans to focused match

### Task 17: Wire Canvas Outline Panel
**Files:** `src/canvas/OutlinePanel.cpp`
**Description:** OutlinePanel (320 lines) exists. Wire it as a navigable tree of all canvas objects, organized by frames/sections.
**Acceptance Criteria:**
- Outline shows hierarchical object list
- Frames and sections as top-level nodes
- Click outlines item selects and centers object
- Drag items in outline reorders z-order
- Search to filter objects

### Task 18: Add Canvas Object Style Presets
**Files:** `src/canvas/CanvasObject.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Add style presets for canvas objects: color palettes, border styles, shadow options. User can save custom presets.
**Acceptance Criteria:**
- Style presets in inspector: fill color, border, shadow, font
- Preset palette matches current theme
- Apply preset to selected objects
- Save custom presets

### Task 19: Add Canvas Clipboard Operations
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Wire full clipboard: Cut, Copy, Paste, Paste in Place, Duplicate (Cmd+D). Copy creates internal clipboard format; paste from external sources (text, images) creates appropriate objects.
**Acceptance Criteria:**
- Cut/Copy/Paste work within canvas
- Copy preserves object relationships (connectors)
- Paste from external: text creates TextBox, image creates ImageObject
- Duplicate creates copy offset by 10px
- Clipboard operations are undoable

### Task 20: Add Canvas Object Tests
**Files:** `tests/unit/test_board_model.cpp`, `tests/unit/test_kanban.cpp`, `tests/unit/test_mind_map.cpp`, `tests/unit/test_table_object.cpp`
**Description:** Test all advanced object types: creation, editing, serialization, and interaction.
**Acceptance Criteria:**
- Frame: contains objects, moves contained objects
- Kanban: columns, cards, drag between columns
- Table: sort, filter, resize columns
- Mind map: create, expand, collapse, auto-layout
- All objects serialize/deserialize correctly

## Testing Requirements
- Each object type: create, edit, transform, serialize, deserialize
- Connector routing: all three styles, object port attachment
- Group: create, enter, exit, ungroup, nested groups
- Performance: 200 objects with mixed types at 60fps

## Phase Completion Criteria
- All canvas object types fully functional
- Frames, sections, groups for organization
- Kanban, tables, mind maps for structured content
- Connectors with advanced routing
- Full clipboard operations
- All tests pass
