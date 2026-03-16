#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Mind map layout mode.
enum class MindMapLayout : uint8_t
{
    kRadial,
    kRightward,
    kDownward,
};

/// Mind map node.
struct MindMapNode
{
    std::string node_id;
    std::string label;
    std::string parent_id; ///< Empty for root
    bool collapsed{false};
    int depth{0};
    std::string branch_color;

    // ── Round 6 Batch 6 (#54-57) ────────────────────────────────

    /// (#54) Whether this is the root node.
    [[nodiscard]] auto is_root() const noexcept -> bool
    {
        return parent_id.empty();
    }

    /// (#55) Whether this node is collapsed.
    [[nodiscard]] auto is_collapsed() const noexcept -> bool
    {
        return collapsed;
    }

    /// (#56) Whether a label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }

    /// (#57) Whether a branch color is set.
    [[nodiscard]] auto has_branch_color() const noexcept -> bool
    {
        return !branch_color.empty();
    }
};

/// Testable model for Mind Map Core Editing (Phase 58).
///
/// Encapsulates:
/// - Node tree with parent-child relationships
/// - Sibling/child creation shortcuts
/// - Collapse/expand branches
/// - Layout mode (radial/directional)
/// - Branch style by depth
class MindMapModel
{
public:
    // ── Nodes ───────────────────────────────────────────────────────

    void set_nodes(std::vector<MindMapNode> nodes);
    [[nodiscard]] auto nodes() const -> const std::vector<MindMapNode>&;
    [[nodiscard]] auto node_count() const -> int;

    void
    add_child(const std::string& parent_id, const std::string& node_id, const std::string& label);
    void add_sibling(const std::string& sibling_id,
                     const std::string& node_id,
                     const std::string& label);
    void remove_node(const std::string& node_id);

    [[nodiscard]] auto children_of(const std::string& parent_id) const -> std::vector<MindMapNode>;

    // ── Collapse/Expand ─────────────────────────────────────────────

    void toggle_collapse(const std::string& node_id);
    [[nodiscard]] auto is_collapsed(const std::string& node_id) const -> bool;

    // ── Layout ──────────────────────────────────────────────────────

    void set_layout(MindMapLayout layout);
    [[nodiscard]] auto layout() const -> MindMapLayout;

    // ── Branch color ────────────────────────────────────────────────

    void set_depth_colors(std::vector<std::string> colors);
    [[nodiscard]] auto color_for_depth(int depth) const -> std::string;

private:
    std::vector<MindMapNode> nodes_;
    MindMapLayout layout_{MindMapLayout::kRadial};
    std::vector<std::string> depth_colors_;

    auto find_index(const std::string& node_id) -> int;

    // ── Round 6 Batch 6-7 (#58-62) ──────────────────────────────

    /// (#58) Whether layout is radial.
    [[nodiscard]] auto is_radial() const noexcept -> bool
    {
        return layout_ == MindMapLayout::kRadial;
    }

    /// (#59) Whether layout is rightward.
    [[nodiscard]] auto is_rightward() const noexcept -> bool
    {
        return layout_ == MindMapLayout::kRightward;
    }

    /// (#60) Whether layout is downward.
    [[nodiscard]] auto is_downward() const noexcept -> bool
    {
        return layout_ == MindMapLayout::kDownward;
    }

    /// (#61) Whether depth colors are configured.
    [[nodiscard]] auto has_depth_colors() const noexcept -> bool
    {
        return !depth_colors_.empty();
    }

    /// (#62) Whether nodes exist.
    [[nodiscard]] auto has_nodes() const noexcept -> bool
    {
        return !nodes_.empty();
    }
};

} // namespace markamp::canvas
