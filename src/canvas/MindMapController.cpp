#include "MindMapController.h"

#include "canvas/Board.h"
#include "canvas/MindMapNode.h"
#include "canvas/UndoRedoStack.h"

#include <cmath>

namespace markamp::canvas
{

MindMapController::MindMapController(Board& board, UndoRedoStack& undo_stack)
    : board_(board)
    , undo_stack_(undo_stack)
{
}

auto MindMapController::create_mind_map(const Point2D& position, const std::string& root_text)
    -> ObjectId
{
    auto root = std::make_unique<MindMapNode>();
    root->set_text(root_text);
    root->set_depth(0);
    auto xform = Transform2D::identity();
    xform.tx = position.x;
    xform.ty = position.y;
    root->set_transform(xform);
    return board_.add_object(std::move(root));
}

auto MindMapController::add_child(ObjectId parent_id, const std::string& text) -> ObjectId
{
    auto* parent = dynamic_cast<MindMapNode*>(board_.get_object_mut(parent_id));
    if (parent == nullptr)
    {
        return kInvalidObjectId;
    }

    auto child = std::make_unique<MindMapNode>();
    child->set_text(text);
    child->set_parent_node_id(parent_id);
    child->set_depth(parent->depth() + 1);

    // Depth-based color shift.
    const auto base = parent->node_color();
    child->set_node_color({static_cast<uint8_t>(std::min(255, base.r + 30)),
                           static_cast<uint8_t>(std::min(255, base.g + 15)),
                           base.b,
                           base.a});

    const ObjectId child_id = board_.add_object(std::move(child));
    parent->add_child_node_id(child_id);

    return child_id;
}

auto MindMapController::add_sibling(ObjectId sibling_id, const std::string& text) -> ObjectId
{
    const auto* sibling = dynamic_cast<const MindMapNode*>(board_.get_object_mut(sibling_id));
    if (sibling == nullptr)
    {
        return kInvalidObjectId;
    }

    const ObjectId parent_id = sibling->parent_node_id();
    if (parent_id == kInvalidObjectId)
    {
        return kInvalidObjectId;
    } // Root has no siblings.

    return add_child(parent_id, text);
}

auto MindMapController::remove_branch(ObjectId node_id) -> void
{
    auto* node = dynamic_cast<MindMapNode*>(board_.get_object_mut(node_id));
    if (node == nullptr)
    {
        return;
    }

    // Recursively remove children first.
    auto children = node->child_node_ids(); // Copy because we'll mutate.
    for (const auto& child_id : children)
    {
        remove_branch(child_id);
    }

    // Remove from parent's child list.
    if (node->parent_node_id() != kInvalidObjectId)
    {
        auto* parent = dynamic_cast<MindMapNode*>(board_.get_object_mut(node->parent_node_id()));
        if (parent != nullptr)
        {
            parent->remove_child_node_id(node_id);
        }
    }

    // Remove the connector if present.
    if (node->connector_id() != kInvalidObjectId)
    {
        board_.remove_object(node->connector_id());
    }

    board_.remove_object(node_id);
}

auto MindMapController::relayout(ObjectId root_id) -> void
{
    const auto* root = dynamic_cast<const MindMapNode*>(board_.get_object_mut(root_id));
    if (root == nullptr)
    {
        return;
    }

    const auto& xform = root->transform();
    layout_subtree(root_id, xform.tx, xform.ty, 0.0, 2.0 * M_PI, 150.0);
}

auto MindMapController::find_root(ObjectId node_id) const -> ObjectId
{
    const auto* node = dynamic_cast<const MindMapNode*>(board_.get_object_mut(node_id));
    if (node == nullptr)
    {
        return kInvalidObjectId;
    }

    while (node->parent_node_id() != kInvalidObjectId)
    {
        node = dynamic_cast<const MindMapNode*>(board_.get_object_mut(node->parent_node_id()));
        if (node == nullptr)
        {
            return kInvalidObjectId;
        }
    }
    return node->id();
}

auto MindMapController::layout_subtree(
    ObjectId node_id, double x, double y, double angle_start, double angle_end, double radius)
    -> void
{
    auto* node = dynamic_cast<MindMapNode*>(board_.get_object_mut(node_id));
    if (node == nullptr)
    {
        return;
    }

    auto node_xform = Transform2D::identity();
    node_xform.tx = x;
    node_xform.ty = y;
    node->set_transform(node_xform);

    const auto& children = node->child_node_ids();
    if (children.empty())
    {
        return;
    }

    const double angle_range = angle_end - angle_start;
    const double angle_step = angle_range / static_cast<double>(children.size());

    for (size_t idx = 0; idx < children.size(); ++idx)
    {
        const double child_angle = angle_start + angle_step * (static_cast<double>(idx) + 0.5);
        const double child_x = x + radius * std::cos(child_angle);
        const double child_y = y + radius * std::sin(child_angle);

        layout_subtree(children[idx],
                       child_x,
                       child_y,
                       child_angle - angle_step / 2.0,
                       child_angle + angle_step / 2.0,
                       radius * 0.8);
    }
}

// ── Expansion (#24-26) ───────────────────────────────────────

auto MindMapController::toggle_collapse(ObjectId node_id) -> void
{
    auto* node = dynamic_cast<MindMapNode*>(board_.get_object_mut(node_id));
    if (node == nullptr)
    {
        return;
    }
    node->set_collapsed(!node->is_collapsed());

    // Re-layout from root so the tree adjusts.
    const ObjectId root_id = find_root(node_id);
    if (root_id != kInvalidObjectId)
    {
        relayout(root_id);
    }
}

auto MindMapController::node_count(ObjectId root_id) const -> size_t
{
    const auto* node = dynamic_cast<const MindMapNode*>(board_.get_object(root_id));
    if (node == nullptr)
    {
        return 0;
    }

    size_t total = 1;
    for (const auto& child_id : node->child_node_ids())
    {
        total += node_count(child_id);
    }
    return total;
}

auto MindMapController::reparent_node(ObjectId node_id, ObjectId new_parent_id) -> void
{
    auto* node = dynamic_cast<MindMapNode*>(board_.get_object_mut(node_id));
    if (node == nullptr)
    {
        return;
    }

    // Remove from old parent.
    if (node->parent_node_id() != kInvalidObjectId)
    {
        auto* old_parent =
            dynamic_cast<MindMapNode*>(board_.get_object_mut(node->parent_node_id()));
        if (old_parent != nullptr)
        {
            old_parent->remove_child_node_id(node_id);
        }
    }

    // Add to new parent.
    auto* new_parent = dynamic_cast<MindMapNode*>(board_.get_object_mut(new_parent_id));
    if (new_parent != nullptr)
    {
        new_parent->add_child_node_id(node_id);
        node->set_parent_node_id(new_parent_id);
        node->set_depth(new_parent->depth() + 1);
    }
}

// ── Depth & Search (#36-37) ──────────────────────────────

auto MindMapController::max_depth(ObjectId root_id) const -> size_t
{
    const auto* node = dynamic_cast<const MindMapNode*>(board_.get_object(root_id));
    if (node == nullptr)
    {
        return 0;
    }

    size_t deepest = 0;
    for (const auto& child_id : node->child_node_ids())
    {
        deepest = std::max(deepest, max_depth(child_id));
    }
    return deepest + 1;
}

auto MindMapController::find_by_text(ObjectId root_id, const std::string& query) const
    -> std::vector<ObjectId>
{
    std::vector<ObjectId> matches;
    const auto* node = dynamic_cast<const MindMapNode*>(board_.get_object(root_id));
    if (node == nullptr)
    {
        return matches;
    }

    if (node->text().find(query) != std::string::npos)
    {
        matches.push_back(root_id);
    }

    for (const auto& child_id : node->child_node_ids())
    {
        auto child_matches = find_by_text(child_id, query);
        matches.insert(matches.end(), child_matches.begin(), child_matches.end());
    }
    return matches;
}

} // namespace markamp::canvas
