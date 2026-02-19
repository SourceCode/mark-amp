#include "GraphControlModel.h"

#include <algorithm>

namespace markamp::ui
{

void GraphControlModel::set_presets(std::vector<GraphFilterPreset> presets)
{
    presets_ = std::move(presets);
}

auto GraphControlModel::presets() const -> const std::vector<GraphFilterPreset>&
{
    return presets_;
}

void GraphControlModel::add_preset(GraphFilterPreset preset)
{
    presets_.push_back(std::move(preset));
}

void GraphControlModel::remove_preset(const std::string& preset_id)
{
    presets_.erase(std::remove_if(presets_.begin(),
                                  presets_.end(),
                                  [&](const GraphFilterPreset& preset)
                                  { return preset.preset_id == preset_id; }),
                   presets_.end());
}

auto GraphControlModel::preset_by_id(const std::string& preset_id) const -> const GraphFilterPreset*
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

void GraphControlModel::set_layout(GraphLayout layout)
{
    layout_ = layout;
}
auto GraphControlModel::layout() const -> GraphLayout
{
    return layout_;
}

void GraphControlModel::set_depth(int depth)
{
    depth_ = (depth < 1) ? 1 : depth;
}
auto GraphControlModel::depth() const -> int
{
    return depth_;
}

void GraphControlModel::set_nodes(std::vector<GraphNode> nodes)
{
    nodes_ = std::move(nodes);
    selected_index_ = -1;
}

void GraphControlModel::select_node(const std::string& node_id)
{
    for (int i = 0; i < static_cast<int>(nodes_.size()); ++i)
    {
        nodes_[static_cast<size_t>(i)].is_selected =
            (nodes_[static_cast<size_t>(i)].node_id == node_id);
        if (nodes_[static_cast<size_t>(i)].is_selected)
        {
            selected_index_ = i;
        }
    }
}

auto GraphControlModel::selected_node() const -> const GraphNode*
{
    if (selected_index_ < 0 || selected_index_ >= static_cast<int>(nodes_.size()))
    {
        return nullptr;
    }
    return &nodes_[static_cast<size_t>(selected_index_)];
}

void GraphControlModel::select_next()
{
    if (nodes_.empty())
    {
        return;
    }
    if (selected_index_ >= 0)
    {
        nodes_[static_cast<size_t>(selected_index_)].is_selected = false;
    }
    selected_index_ = (selected_index_ + 1) % static_cast<int>(nodes_.size());
    nodes_[static_cast<size_t>(selected_index_)].is_selected = true;
}

void GraphControlModel::select_prev()
{
    if (nodes_.empty())
    {
        return;
    }
    if (selected_index_ >= 0)
    {
        nodes_[static_cast<size_t>(selected_index_)].is_selected = false;
    }
    selected_index_ =
        (selected_index_ - 1 + static_cast<int>(nodes_.size())) % static_cast<int>(nodes_.size());
    nodes_[static_cast<size_t>(selected_index_)].is_selected = true;
}

void GraphControlModel::set_backlinks(std::vector<BacklinkEntry> backlinks)
{
    backlinks_ = std::move(backlinks);
}

auto GraphControlModel::sorted_backlinks() const -> std::vector<BacklinkEntry>
{
    auto sorted = backlinks_;
    std::sort(sorted.begin(),
              sorted.end(),
              [](const BacklinkEntry& lhs, const BacklinkEntry& rhs)
              {
                  if (lhs.source_file != rhs.source_file)
                  {
                      return lhs.source_file < rhs.source_file;
                  }
                  return lhs.line_number < rhs.line_number;
              });
    return sorted;
}

auto GraphControlModel::backlink_count() const -> int
{
    return static_cast<int>(backlinks_.size());
}

} // namespace markamp::ui
