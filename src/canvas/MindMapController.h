#pragma once

#include "canvas/CanvasTypes.h"

#include <string>

namespace markamp::canvas
{

class Board;
class UndoRedoStack;

/// Controller for mind map tree operations: creating, adding children/siblings,
/// removing branches, and re-laying out the tree.
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
    [[maybe_unused]] UndoRedoStack& undo_stack_;

    auto layout_subtree(
        ObjectId node_id, double x, double y, double angle_start, double angle_end, double radius)
        -> void;
};

} // namespace markamp::canvas
