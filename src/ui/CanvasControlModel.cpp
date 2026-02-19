#include "CanvasControlModel.h"

#include <set>

namespace markamp::ui
{

void CanvasControlModel::set_tools(std::vector<CanvasToolItem> tools)
{
    tools_ = std::move(tools);
}

auto CanvasControlModel::tools() const -> const std::vector<CanvasToolItem>&
{
    return tools_;
}

auto CanvasControlModel::tools_by_group(const std::string& group) const
    -> std::vector<CanvasToolItem>
{
    std::vector<CanvasToolItem> result;
    for (const auto& tool : tools_)
    {
        if (tool.group == group)
        {
            result.push_back(tool);
        }
    }
    return result;
}

void CanvasControlModel::set_active_tool(CanvasTool tool)
{
    active_tool_ = tool;
}
auto CanvasControlModel::active_tool() const -> CanvasTool
{
    return active_tool_;
}

auto CanvasControlModel::active_tool_label() const -> std::string
{
    for (const auto& tool : tools_)
    {
        if (tool.tool == active_tool_)
        {
            return tool.label;
        }
    }
    return "Unknown";
}

void CanvasControlModel::set_properties(std::vector<InspectorProperty> props)
{
    properties_ = std::move(props);
}

auto CanvasControlModel::properties() const -> const std::vector<InspectorProperty>&
{
    return properties_;
}

void CanvasControlModel::set_selection_count(int count)
{
    selection_count_ = count;
}
auto CanvasControlModel::selection_count() const -> int
{
    return selection_count_;
}

auto CanvasControlModel::is_multi_select() const -> bool
{
    return selection_count_ >= 2;
}

auto CanvasControlModel::selection_actions() const -> std::vector<std::string>
{
    if (selection_count_ == 0)
    {
        return {};
    }
    if (selection_count_ == 1)
    {
        return {"Delete", "Duplicate", "Lock", "Bring to Front", "Send to Back"};
    }
    // Multi-select: add group/align actions
    return {"Group", "Align Left", "Align Center", "Align Right", "Distribute Evenly", "Delete"};
}

} // namespace markamp::ui
