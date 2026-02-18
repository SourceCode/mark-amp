// ============================================================================
// File: src/canvas/CanvasToolHost.cpp
// Phase 14: Canvas Extensibility — unified canvas tool hosting
// ============================================================================
#include "canvas/CanvasToolHost.h"

#include "core/Events.h"

#include <algorithm>
#include <ranges>
#include <set>

namespace markamp::canvas
{

CanvasToolHost::CanvasToolHost(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ── Registration ──────────────────────────────────────────────────

auto CanvasToolHost::register_tool(const HostedTool& tool) -> bool
{
    if (tool.tool_id.empty())
    {
        return false;
    }

    if (tools_.contains(tool.tool_id))
    {
        return false;
    }

    tools_.emplace(tool.tool_id, tool);
    return true;
}

auto CanvasToolHost::unregister_tool(const std::string& tool_id) -> bool
{
    if (active_tool_id_ == tool_id)
    {
        deactivate_current();
    }

    return tools_.erase(tool_id) > 0;
}

auto CanvasToolHost::unregister_extension_tools(const std::string& extension_id) -> size_t
{
    // First deactivate if the active tool belongs to this extension
    if (!active_tool_id_.empty())
    {
        auto iter = tools_.find(active_tool_id_);
        if (iter != tools_.end() && iter->second.extension_id == extension_id)
        {
            deactivate_current();
        }
    }

    size_t removed = 0;
    for (auto iter = tools_.begin(); iter != tools_.end();)
    {
        if (iter->second.extension_id == extension_id && !iter->second.is_built_in)
        {
            iter = tools_.erase(iter);
            ++removed;
        }
        else
        {
            ++iter;
        }
    }
    return removed;
}

// ── Activation ────────────────────────────────────────────────────

auto CanvasToolHost::activate_tool(const std::string& tool_id) -> ToolActivationResult
{
    auto iter = tools_.find(tool_id);
    if (iter == tools_.end())
    {
        return {false, "Tool not found: " + tool_id, ""};
    }

    std::string previous_tool_id;

    // Deactivate current tool
    if (!active_tool_id_.empty() && active_tool_id_ != tool_id)
    {
        previous_tool_id = active_tool_id_;
        auto current_iter = tools_.find(active_tool_id_);
        if (current_iter != tools_.end() && current_iter->second.on_activation)
        {
            current_iter->second.on_activation(false);
        }
    }

    // Activate new tool
    active_tool_id_ = tool_id;
    if (iter->second.on_activation)
    {
        iter->second.on_activation(true);
    }

    // Publish activation event
    core::events::CanvasAppRegisteredEvent evt;
    evt.app_id = tool_id;
    evt.app_name = iter->second.label;
    evt.extension_id = iter->second.extension_id;
    event_bus_.publish(evt);

    return {true, "", previous_tool_id};
}

auto CanvasToolHost::deactivate_current() -> void
{
    if (active_tool_id_.empty())
    {
        return;
    }

    auto iter = tools_.find(active_tool_id_);
    if (iter != tools_.end() && iter->second.on_activation)
    {
        iter->second.on_activation(false);
    }

    active_tool_id_.clear();
}

auto CanvasToolHost::active_tool_id() const -> const std::string&
{
    return active_tool_id_;
}

auto CanvasToolHost::active_tool() const -> const HostedTool*
{
    if (active_tool_id_.empty())
    {
        return nullptr;
    }

    auto iter = tools_.find(active_tool_id_);
    return iter != tools_.end() ? &iter->second : nullptr;
}

// ── Query ─────────────────────────────────────────────────────────

auto CanvasToolHost::find_tool(const std::string& tool_id) const -> const HostedTool*
{
    auto iter = tools_.find(tool_id);
    return iter != tools_.end() ? &iter->second : nullptr;
}

auto CanvasToolHost::registered_tools() const -> const std::unordered_map<std::string, HostedTool>&
{
    return tools_;
}

auto CanvasToolHost::tools_in_category(const std::string& category) const
    -> std::vector<const HostedTool*>
{
    std::vector<const HostedTool*> result;
    for (const auto& [tid, tool] : tools_)
    {
        if (tool.category == category)
        {
            result.push_back(&tool);
        }
    }

    // Sort by priority (lower first)
    std::ranges::sort(result,
                      [](const HostedTool* lhs, const HostedTool* rhs)
                      { return lhs->priority < rhs->priority; });

    return result;
}

auto CanvasToolHost::tool_categories() const -> std::vector<std::string>
{
    std::set<std::string> categories;
    for (const auto& [tid, tool] : tools_)
    {
        if (!tool.category.empty())
        {
            categories.insert(tool.category);
        }
    }
    return {categories.begin(), categories.end()};
}

auto CanvasToolHost::available_tools() const -> std::vector<const HostedTool*>
{
    std::vector<const HostedTool*> result;
    result.reserve(tools_.size());
    for (const auto& [tid, tool] : tools_)
    {
        result.push_back(&tool);
    }

    // Sort by category, then by priority within category
    std::ranges::sort(result,
                      [](const HostedTool* lhs, const HostedTool* rhs)
                      {
                          if (lhs->category != rhs->category)
                          {
                              return lhs->category < rhs->category;
                          }
                          return lhs->priority < rhs->priority;
                      });

    return result;
}

auto CanvasToolHost::tool_count() const -> size_t
{
    return tools_.size();
}

auto CanvasToolHost::extension_tool_count() const -> size_t
{
    size_t count = 0;
    for (const auto& [tid, tool] : tools_)
    {
        if (!tool.is_built_in)
        {
            ++count;
        }
    }
    return count;
}

// ── Cleanup ───────────────────────────────────────────────────────

auto CanvasToolHost::clear() -> void
{
    deactivate_current();
    tools_.clear();
}

} // namespace markamp::canvas
