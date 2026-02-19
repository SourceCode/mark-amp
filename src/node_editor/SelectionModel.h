#pragma once

// V11 Phase 26: Selection Box Lasso Marquee And Focus Navigation
// Multi-selection model with box/lasso selection and keyboard focus navigation.

#include "NodeEditorTypes.h"

#include <unordered_set>
#include <vector>

namespace markamp::node_editor
{

/// Multi-selection model for nodes. Supports click, toggle, extend, box, and lasso operations.
class SelectionModel
{
public:
    SelectionModel() = default;

    // --- Single-item operations ---
    void select(NodeId node_id);
    void deselect(NodeId node_id);
    void toggle(NodeId node_id);
    void select_only(NodeId node_id);
    void deselect_all();

    // --- Bulk operations ---
    void select_all(const std::vector<NodeId>& all_node_ids);
    void select_set(const std::vector<NodeId>& node_ids);
    void select_in_rect(Rect world_rect,
                        const std::vector<NodeId>& all_node_ids,
                        const std::vector<Rect>& all_bounds);

    // --- Queries ---
    [[nodiscard]] auto is_selected(NodeId node_id) const -> bool;
    [[nodiscard]] auto count() const -> std::size_t;
    [[nodiscard]] auto empty() const -> bool;
    [[nodiscard]] auto selected_ids() const -> std::vector<NodeId>;

    // --- Focus navigation ---
    void focus_node(NodeId node_id);
    void focus_next(const std::vector<NodeId>& all_node_ids);
    void focus_prev(const std::vector<NodeId>& all_node_ids);
    [[nodiscard]] auto focused_node() const noexcept -> NodeId;
    [[nodiscard]] auto has_focus() const noexcept -> bool;
    void clear_focus();

private:
    std::unordered_set<NodeId> selected_;
    NodeId focused_;

    [[nodiscard]] auto find_focus_index(const std::vector<NodeId>& all_node_ids) const
        -> std::size_t;
};

} // namespace markamp::node_editor
