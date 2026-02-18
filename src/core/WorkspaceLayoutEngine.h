/// @file WorkspaceLayoutEngine.h
/// @brief V9 Phase 46 — Workspace layout tree and preset management.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Type of layout node.
enum class LayoutNodeType : uint8_t
{
    kGroup = 0,
    kSplit = 1,
    kPanel = 2,
};

/// Direction for split nodes.
enum class LayoutSplitDirection : uint8_t
{
    kHorizontal = 0,
    kVertical = 1,
};

/// A node in the layout tree.
struct LayoutNode
{
    std::string node_id;
    LayoutNodeType type{LayoutNodeType::kGroup};
    LayoutSplitDirection direction{LayoutSplitDirection::kHorizontal};
    double ratio{0.5};
    std::vector<LayoutNode> children;
    std::string group_id; ///< For kGroup nodes
};

/// A saved layout preset.
struct WorkspaceLayoutPreset
{
    std::string preset_id;
    std::string name;
    std::string description;
    LayoutNode root_node;
};

/// Manages workspace layout tree and presets.
class WorkspaceLayoutEngine
{
public:
    WorkspaceLayoutEngine() = default;

    // ── Current layout ────────────────────────────────────────────────
    void set_root(LayoutNode node);
    [[nodiscard]] auto get_root() const -> const LayoutNode&;
    [[nodiscard]] auto has_root() const -> bool;

    // ── Presets ───────────────────────────────────────────────────────
    auto save_layout(const std::string& name, const std::string& description = "") -> std::string;
    auto restore_layout(const std::string& preset_id) -> bool;
    void add_preset(WorkspaceLayoutPreset preset);
    [[nodiscard]] auto get_preset(const std::string& preset_id) const
        -> const WorkspaceLayoutPreset*;
    [[nodiscard]] auto all_presets() const -> std::vector<const WorkspaceLayoutPreset*>;
    void load_defaults();

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto preset_count() const -> int;
    void clear_presets();

private:
    LayoutNode root_;
    bool has_root_{false};
    std::vector<WorkspaceLayoutPreset> presets_;
    int next_id_{1};
};

} // namespace markamp::core
