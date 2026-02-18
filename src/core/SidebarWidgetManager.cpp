/// @file SidebarWidgetManager.cpp
/// @brief V9 Phase 41 — SidebarWidgetManager implementation.

#include "SidebarWidgetManager.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

auto sidebar_slot_name(SidebarSlot slot) -> std::string
{
    switch (slot)
    {
        case SidebarSlot::kTop:
            return "top";
        case SidebarSlot::kPrimary:
            return "primary";
        case SidebarSlot::kSecondary:
            return "secondary";
        case SidebarSlot::kBottom:
            return "bottom";
    }
    return "unknown";
}

void SidebarWidgetManager::register_widget(SidebarWidget widget)
{
    if (widget.widget_id.empty())
    {
        widget.widget_id = "widget_" + std::to_string(widgets_.size());
    }
    widgets_.push_back(std::move(widget));
}

auto SidebarWidgetManager::unregister_widget(const std::string& widget_id) -> bool
{
    auto iter =
        std::remove_if(widgets_.begin(),
                       widgets_.end(),
                       [&](const SidebarWidget& widget) { return widget.widget_id == widget_id; });
    if (iter == widgets_.end())
    {
        return false;
    }
    widgets_.erase(iter, widgets_.end());
    return true;
}

auto SidebarWidgetManager::find_widget(const std::string& widget_id) const -> const SidebarWidget*
{
    for (const auto& widget : widgets_)
    {
        if (widget.widget_id == widget_id)
        {
            return &widget;
        }
    }
    return nullptr;
}

auto SidebarWidgetManager::widget_count() const -> int
{
    return static_cast<int>(widgets_.size());
}

auto SidebarWidgetManager::widgets_in_slot(SidebarSlot slot) const
    -> std::vector<const SidebarWidget*>
{
    std::vector<const SidebarWidget*> result;
    for (const auto& widget : widgets_)
    {
        if (widget.slot == slot && widget.display_mode != WidgetDisplayMode::kHidden)
        {
            result.push_back(&widget);
        }
    }
    // Sort by sort_order
    std::sort(result.begin(),
              result.end(),
              [](const SidebarWidget* lhs, const SidebarWidget* rhs)
              { return lhs->sort_order < rhs->sort_order; });
    return result;
}

auto SidebarWidgetManager::move_to_slot(const std::string& widget_id, SidebarSlot slot) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->slot = slot;
    return true;
}

auto SidebarWidgetManager::set_sort_order(const std::string& widget_id, int order) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->sort_order = order;
    return true;
}

auto SidebarWidgetManager::set_display_mode(const std::string& widget_id, WidgetDisplayMode mode)
    -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->display_mode = mode;
    return true;
}

auto SidebarWidgetManager::toggle_collapsed(const std::string& widget_id) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->display_mode = (widget->display_mode == WidgetDisplayMode::kCollapsed)
                               ? WidgetDisplayMode::kExpanded
                               : WidgetDisplayMode::kCollapsed;
    return true;
}

auto SidebarWidgetManager::show_widget(const std::string& widget_id) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->display_mode = WidgetDisplayMode::kExpanded;
    return true;
}

auto SidebarWidgetManager::hide_widget(const std::string& widget_id) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->display_mode = WidgetDisplayMode::kHidden;
    return true;
}

auto SidebarWidgetManager::all_widgets() const -> std::vector<const SidebarWidget*>
{
    std::vector<const SidebarWidget*> result;
    result.reserve(widgets_.size());
    for (const auto& widget : widgets_)
    {
        result.push_back(&widget);
    }
    return result;
}

auto SidebarWidgetManager::visible_widgets() const -> std::vector<const SidebarWidget*>
{
    std::vector<const SidebarWidget*> result;
    for (const auto& widget : widgets_)
    {
        if (widget.display_mode != WidgetDisplayMode::kHidden)
        {
            result.push_back(&widget);
        }
    }
    return result;
}

auto SidebarWidgetManager::active_slots() const -> std::vector<SidebarSlot>
{
    std::set<SidebarSlot> seen;
    for (const auto& widget : widgets_)
    {
        if (widget.display_mode != WidgetDisplayMode::kHidden)
        {
            seen.insert(widget.slot);
        }
    }
    return {seen.begin(), seen.end()};
}

auto SidebarWidgetManager::widgets_by_extension(const std::string& extension_id) const
    -> std::vector<const SidebarWidget*>
{
    std::vector<const SidebarWidget*> result;
    for (const auto& widget : widgets_)
    {
        if (widget.extension_id == extension_id)
        {
            result.push_back(&widget);
        }
    }
    return result;
}

void SidebarWidgetManager::clear_all()
{
    widgets_.clear();
}

auto SidebarWidgetManager::find_mut(const std::string& widget_id) -> SidebarWidget*
{
    for (auto& widget : widgets_)
    {
        if (widget.widget_id == widget_id)
        {
            return &widget;
        }
    }
    return nullptr;
}

} // namespace markamp::core
