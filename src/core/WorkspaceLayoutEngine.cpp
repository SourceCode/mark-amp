/// @file WorkspaceLayoutEngine.cpp
/// @brief V9 Phase 46 — WorkspaceLayoutEngine implementation.

#include "WorkspaceLayoutEngine.h"

namespace markamp::core
{

void WorkspaceLayoutEngine::set_root(LayoutNode node)
{
    root_ = std::move(node);
    has_root_ = true;
}

auto WorkspaceLayoutEngine::get_root() const -> const LayoutNode&
{
    return root_;
}

auto WorkspaceLayoutEngine::has_root() const -> bool
{
    return has_root_;
}

auto WorkspaceLayoutEngine::save_layout(const std::string& name, const std::string& description)
    -> std::string
{
    WorkspaceLayoutPreset preset;
    preset.preset_id = "preset_" + std::to_string(next_id_++);
    preset.name = name;
    preset.description = description;
    preset.root_node = root_;
    presets_.push_back(std::move(preset));
    return presets_.back().preset_id;
}

auto WorkspaceLayoutEngine::restore_layout(const std::string& preset_id) -> bool
{
    const auto* preset = get_preset(preset_id);
    if (preset == nullptr)
    {
        return false;
    }
    root_ = preset->root_node;
    has_root_ = true;
    return true;
}

void WorkspaceLayoutEngine::add_preset(WorkspaceLayoutPreset preset)
{
    if (preset.preset_id.empty())
    {
        preset.preset_id = "preset_" + std::to_string(next_id_++);
    }
    presets_.push_back(std::move(preset));
}

auto WorkspaceLayoutEngine::get_preset(const std::string& preset_id) const
    -> const WorkspaceLayoutPreset*
{
    for (const auto& preset : presets_)
    {
        if (preset.preset_id == preset_id)
        {
            return &preset;
        }
    }
    return nullptr;
}

auto WorkspaceLayoutEngine::all_presets() const -> std::vector<const WorkspaceLayoutPreset*>
{
    std::vector<const WorkspaceLayoutPreset*> result;
    result.reserve(presets_.size());
    for (const auto& preset : presets_)
    {
        result.push_back(&preset);
    }
    return result;
}

void WorkspaceLayoutEngine::load_defaults()
{
    // Single column
    {
        WorkspaceLayoutPreset preset;
        preset.preset_id = "builtin_single";
        preset.name = "Single Column";
        preset.description = "One editor filling the full width";
        preset.root_node.node_id = "root";
        preset.root_node.type = LayoutNodeType::kGroup;
        preset.root_node.group_id = "main";
        presets_.push_back(std::move(preset));
    }

    // Two column
    {
        WorkspaceLayoutPreset preset;
        preset.preset_id = "builtin_two_col";
        preset.name = "Two Column";
        preset.description = "Side-by-side editors";

        preset.root_node.node_id = "root";
        preset.root_node.type = LayoutNodeType::kSplit;
        preset.root_node.direction = LayoutSplitDirection::kHorizontal;
        preset.root_node.ratio = 0.5;

        LayoutNode left;
        left.node_id = "left";
        left.type = LayoutNodeType::kGroup;
        left.group_id = "left";

        LayoutNode right;
        right.node_id = "right";
        right.type = LayoutNodeType::kGroup;
        right.group_id = "right";

        preset.root_node.children.push_back(std::move(left));
        preset.root_node.children.push_back(std::move(right));
        presets_.push_back(std::move(preset));
    }

    // Three panel
    {
        WorkspaceLayoutPreset preset;
        preset.preset_id = "builtin_three_panel";
        preset.name = "Three Panel";
        preset.description = "Large editor with two side panels";

        preset.root_node.node_id = "root";
        preset.root_node.type = LayoutNodeType::kSplit;
        preset.root_node.direction = LayoutSplitDirection::kHorizontal;
        preset.root_node.ratio = 0.6;

        LayoutNode main_group;
        main_group.node_id = "main";
        main_group.type = LayoutNodeType::kGroup;
        main_group.group_id = "main";

        LayoutNode right_split;
        right_split.node_id = "right_split";
        right_split.type = LayoutNodeType::kSplit;
        right_split.direction = LayoutSplitDirection::kVertical;
        right_split.ratio = 0.5;

        LayoutNode top_right;
        top_right.node_id = "top_right";
        top_right.type = LayoutNodeType::kGroup;
        top_right.group_id = "top_right";

        LayoutNode bottom_right;
        bottom_right.node_id = "bottom_right";
        bottom_right.type = LayoutNodeType::kGroup;
        bottom_right.group_id = "bottom_right";

        right_split.children.push_back(std::move(top_right));
        right_split.children.push_back(std::move(bottom_right));

        preset.root_node.children.push_back(std::move(main_group));
        preset.root_node.children.push_back(std::move(right_split));
        presets_.push_back(std::move(preset));
    }
}

auto WorkspaceLayoutEngine::preset_count() const -> int
{
    return static_cast<int>(presets_.size());
}

void WorkspaceLayoutEngine::clear_presets()
{
    presets_.clear();
}

} // namespace markamp::core
