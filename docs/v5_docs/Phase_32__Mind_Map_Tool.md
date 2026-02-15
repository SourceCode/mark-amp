# Phase 32 -- Mind Map Tool

## Objective

Implement a mind map creation tool that generates hierarchical tree structures on the canvas. A central topic node branches into child nodes, auto-laid out in a radial or tree arrangement. Tab creates siblings, Enter creates children. Nodes are connected by auto-routed connectors.

## Prerequisites

- Phase 07 (StickyNoteObject or TextBoxObject for node content)
- Phase 10 (ConnectorObject for links)
- Phase 31 (LayoutEngine for tree/radial layout)
- Phase 05 (UndoRedoStack)

## Feature References (PRD)

- PRD #69: Mind Map Tool

## Data Structures to Implement

### File: `src/canvas/MindMapNode.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

class MindMapNode : public CanvasObject
{
public:
    MindMapNode();

    [[nodiscard]] auto text() const -> const std::string&;
    auto set_text(const std::string& text) -> void;

    [[nodiscard]] auto parent_node_id() const -> ObjectId;
    auto set_parent_node_id(ObjectId id) -> void;

    [[nodiscard]] auto child_node_ids() const -> const std::vector<ObjectId>&;
    auto add_child_node_id(ObjectId id) -> void;
    auto remove_child_node_id(ObjectId id) -> void;

    [[nodiscard]] auto connector_id() const -> ObjectId; // Connector to parent
    auto set_connector_id(ObjectId id) -> void;

    [[nodiscard]] auto node_color() const -> const CanvasColor&;
    auto set_node_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto depth() const -> int; // 0 = root
    auto set_depth(int d) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string text_{"Topic"};
    ObjectId parent_node_id_{kInvalidObjectId};
    std::vector<ObjectId> child_node_ids_;
    ObjectId connector_id_{kInvalidObjectId};
    CanvasColor node_color_{66, 133, 244, 255};
    int depth_{0};
    double width_{120.0};
    double height_{40.0};
};

} // namespace markamp::canvas
```

### File: `src/canvas/MindMapController.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <string>

namespace markamp::canvas
{

class Board;
class UndoRedoStack;

class MindMapController
{
public:
    MindMapController(Board& board, UndoRedoStack& undo_stack);

    /// Create a new mind map with a root node at the given position.
    auto create_mind_map(const Point2D& position, const std::string& root_text) -> ObjectId;

    /// Add a child node to a parent.
    auto add_child(ObjectId parent_id, const std::string& text) -> ObjectId;

    /// Add a sibling node (same parent as the given node).
    auto add_sibling(ObjectId sibling_id, const std::string& text) -> ObjectId;

    /// Remove a node and all its descendants.
    auto remove_branch(ObjectId node_id) -> void;

    /// Re-layout the entire mind map tree starting from the root.
    auto relayout(ObjectId root_id) -> void;

    /// Get the root node of a mind map that contains the given node.
    [[nodiscard]] auto find_root(ObjectId node_id) const -> ObjectId;

private:
    Board& board_;
    UndoRedoStack& undo_stack_;

    auto layout_subtree(ObjectId node_id, double x, double y,
                         double angle_start, double angle_end, double radius) -> void;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `MindMapController::create_mind_map()` -- Create root MindMapNode. Add to board. Return root ID.

2. `MindMapController::add_child()` -- Create child node. Create ConnectorObject linking parent to child. Update parent's child list. Relayout the tree.

3. `MindMapController::relayout()` -- Traverse the tree. Use radial layout: root at center, children spread in arcs. Deeper levels have larger radius. Update connector paths.

4. `MindMapNodeRenderer::render()` -- Draw rounded rectangle with text. Color varies by depth level.

5. Keyboard shortcuts during mind map editing: Tab = add child, Enter = add sibling, Delete = remove branch.

## Test Cases

File: `tests/unit/test_mind_map.cpp`

1. **Create mind map** -- Create root, verify node exists.
2. **Add child** -- Add child, verify parent-child link.
3. **Add sibling** -- Add sibling, verify shares parent.
4. **Remove branch** -- Remove subtree, verify nodes and connectors deleted.
5. **Layout positions** -- Create 3-level tree, relayout, verify no overlaps.
6. **Find root** -- Deep node, find_root returns root.
7. **Connector creation** -- Add child, verify connector exists.
8. **JSON round-trip** -- Serialize mind map nodes, verify.

## Acceptance Criteria

- [ ] MindMapNode with parent/child references and connector IDs
- [ ] MindMapController manages tree CRUD with auto-relayout
- [ ] Radial layout with depth-based radius
- [ ] Tab/Enter/Delete keyboard shortcuts for tree editing
- [ ] Auto-created connectors between parent and child nodes
- [ ] Color varies by depth level
- [ ] All 8 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/MindMapNode.h` | Mind map node data model |
| CREATE | `src/canvas/MindMapNode.cpp` | Implementation |
| CREATE | `src/canvas/MindMapController.h` | Tree management + layout |
| CREATE | `src/canvas/MindMapController.cpp` | Controller implementation |
| CREATE | `src/canvas/MindMapNodeRenderer.h` | Node rendering |
| CREATE | `src/canvas/MindMapNodeRenderer.cpp` | Render implementation |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_mind_map.cpp` | 8 Catch2 tests |

## Architecture Notes

- MindMapNodes are individual CanvasObjects connected by ConnectorObjects. This reuses the existing connector infrastructure rather than creating a separate rendering system.
- The MindMapController is the only place that manages the tree invariants (parent-child consistency, connector sync). Other code interacts through the controller.
- Relayout runs after every structural change (add, remove, move). For large mind maps, consider incremental layout.

## Estimated Complexity

**L** -- Tree data structure, radial layout algorithm, connector auto-creation, keyboard-driven editing, 8 tests.
