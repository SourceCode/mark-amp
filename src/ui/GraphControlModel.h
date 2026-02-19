#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Graph layout mode.
enum class GraphLayout : uint8_t
{
    kForceDirected,
    kHierarchical,
    kRadial,
};

/// Graph filter preset.
struct GraphFilterPreset
{
    std::string preset_id;
    std::string name;
    int depth{2};
    bool show_orphans{false};
    std::string tag_filter; ///< Empty = show all
};

/// Graph node for selection/navigation.
struct GraphNode
{
    std::string node_id;
    std::string label;
    int link_count{0};
    bool is_selected{false};
};

/// Backlink entry.
struct BacklinkEntry
{
    std::string source_file;
    std::string context_line; ///< Surrounding text
    int line_number{0};
};

/// Testable model for Graph and Backlink Controls (Phase 26).
///
/// Encapsulates:
/// - Graph filter presets (save/load/apply)
/// - Layout mode management
/// - Node selection and keyboard traversal
/// - Backlink sorting and grouping
class GraphControlModel
{
public:
    // ── Filter presets ──────────────────────────────────────────────

    void set_presets(std::vector<GraphFilterPreset> presets);
    [[nodiscard]] auto presets() const -> const std::vector<GraphFilterPreset>&;
    void add_preset(GraphFilterPreset preset);
    void remove_preset(const std::string& preset_id);
    [[nodiscard]] auto preset_by_id(const std::string& preset_id) const -> const GraphFilterPreset*;

    // ── Layout ──────────────────────────────────────────────────────

    void set_layout(GraphLayout layout);
    [[nodiscard]] auto layout() const -> GraphLayout;

    // ── Depth ───────────────────────────────────────────────────────

    void set_depth(int depth);
    [[nodiscard]] auto depth() const -> int;

    // ── Node selection ──────────────────────────────────────────────

    void set_nodes(std::vector<GraphNode> nodes);
    void select_node(const std::string& node_id);
    [[nodiscard]] auto selected_node() const -> const GraphNode*;

    /// Navigate to next/prev node in list (for keyboard traversal).
    void select_next();
    void select_prev();

    // ── Backlinks ───────────────────────────────────────────────────

    void set_backlinks(std::vector<BacklinkEntry> backlinks);

    /// Backlinks sorted by file then line number.
    [[nodiscard]] auto sorted_backlinks() const -> std::vector<BacklinkEntry>;
    [[nodiscard]] auto backlink_count() const -> int;

private:
    std::vector<GraphFilterPreset> presets_;
    GraphLayout layout_{GraphLayout::kForceDirected};
    int depth_{2};
    std::vector<GraphNode> nodes_;
    int selected_index_{-1};
    std::vector<BacklinkEntry> backlinks_;
};

} // namespace markamp::ui
