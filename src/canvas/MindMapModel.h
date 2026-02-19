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
};

} // namespace markamp::canvas
