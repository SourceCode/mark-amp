#include "ToolRailModel.h"

#include <algorithm>

namespace markamp::canvas
{

void ToolRailModel::set_tools(std::vector<ToolEntry> tools)
{
    tools_ = std::move(tools);
}
auto ToolRailModel::tools() const -> const std::vector<ToolEntry>&
{
    return tools_;
}

auto ToolRailModel::tools_in_group(ToolGroup group) const -> std::vector<ToolEntry>
{
    std::vector<ToolEntry> result;
    for (const auto& tool : tools_)
    {
        if (tool.group == group)
        {
            result.push_back(tool);
        }
    }
    return result;
}

void ToolRailModel::select_tool(const std::string& tool_id)
{
    if (!active_tool_.empty() && active_tool_ != tool_id)
    {
        recent_stack_.erase(std::remove(recent_stack_.begin(), recent_stack_.end(), active_tool_),
                            recent_stack_.end());
        recent_stack_.insert(recent_stack_.begin(), active_tool_);
        if (static_cast<int>(recent_stack_.size()) > kMaxRecent)
        {
            recent_stack_.resize(static_cast<size_t>(kMaxRecent));
        }
    }
    active_tool_ = tool_id;
}

auto ToolRailModel::active_tool() const -> const std::string&
{
    return active_tool_;
}

void ToolRailModel::quick_switch()
{
    if (!recent_stack_.empty())
    {
        const auto prev = recent_stack_.front();
        select_tool(prev);
    }
}

auto ToolRailModel::recent_stack() const -> const std::vector<std::string>&
{
    return recent_stack_;
}

void ToolRailModel::set_context_actions(std::vector<std::string> actions)
{
    context_actions_ = std::move(actions);
}
auto ToolRailModel::context_actions() const -> const std::vector<std::string>&
{
    return context_actions_;
}

void ToolRailModel::set_tool_visible(const std::string& tool_id, bool visible)
{
    for (auto& tool : tools_)
    {
        if (tool.tool_id == tool_id)
        {
            tool.visible = visible;
            return;
        }
    }
}

auto ToolRailModel::visible_tools() const -> std::vector<ToolEntry>
{
    std::vector<ToolEntry> result;
    for (const auto& tool : tools_)
    {
        if (tool.visible)
        {
            result.push_back(tool);
        }
    }
    return result;
}

} // namespace markamp::canvas
