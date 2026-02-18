/// @file WidgetContainer.cpp
/// @brief V9 Phase 41 — WidgetContainer implementation.

#include "WidgetContainer.h"

#include <algorithm>

namespace markamp::core
{

WidgetContainer::WidgetContainer(std::string container_id)
    : container_id_(std::move(container_id))
{
}

void WidgetContainer::add_widget(ContainedWidget widget)
{
    if (widget.widget_id.empty())
    {
        widget.widget_id = "cw_" + std::to_string(widgets_.size());
    }
    widget.position = static_cast<int>(widgets_.size());
    widgets_.push_back(std::move(widget));
}

auto WidgetContainer::remove_widget(const std::string& widget_id) -> bool
{
    auto iter = std::remove_if(widgets_.begin(),
                               widgets_.end(),
                               [&](const ContainedWidget& widget)
                               { return widget.widget_id == widget_id; });
    if (iter == widgets_.end())
    {
        return false;
    }
    widgets_.erase(iter, widgets_.end());
    reindex_positions();
    return true;
}

auto WidgetContainer::find_widget(const std::string& widget_id) const -> const ContainedWidget*
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

auto WidgetContainer::widget_count() const -> int
{
    return static_cast<int>(widgets_.size());
}

auto WidgetContainer::move_widget(const std::string& widget_id, int new_position) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    new_position = std::clamp(new_position, 0, static_cast<int>(widgets_.size()) - 1);
    widget->position = new_position;
    reindex_positions();
    return true;
}

auto WidgetContainer::swap_widgets(const std::string& widget_a, const std::string& widget_b) -> bool
{
    auto* w_a = find_mut(widget_a);
    auto* w_b = find_mut(widget_b);
    if (w_a == nullptr || w_b == nullptr)
    {
        return false;
    }
    std::swap(w_a->position, w_b->position);
    return true;
}

auto WidgetContainer::set_widget_state(const std::string& widget_id, WidgetState state) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->state = state;
    return true;
}

auto WidgetContainer::set_widget_visible(const std::string& widget_id, bool visible) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->visible = visible;
    return true;
}

auto WidgetContainer::set_widget_height(const std::string& widget_id, int height) -> bool
{
    auto* widget = find_mut(widget_id);
    if (widget == nullptr)
    {
        return false;
    }
    widget->height = height;
    return true;
}

auto WidgetContainer::all_widgets() const -> const std::vector<ContainedWidget>&
{
    return widgets_;
}

auto WidgetContainer::visible_widgets() const -> std::vector<const ContainedWidget*>
{
    std::vector<const ContainedWidget*> result;
    for (const auto& widget : widgets_)
    {
        if (widget.visible)
        {
            result.push_back(&widget);
        }
    }
    return result;
}

auto WidgetContainer::ordered_widgets() const -> std::vector<const ContainedWidget*>
{
    std::vector<const ContainedWidget*> result;
    result.reserve(widgets_.size());
    for (const auto& widget : widgets_)
    {
        result.push_back(&widget);
    }
    std::sort(result.begin(),
              result.end(),
              [](const ContainedWidget* lhs, const ContainedWidget* rhs)
              { return lhs->position < rhs->position; });
    return result;
}

auto WidgetContainer::container_id() const -> const std::string&
{
    return container_id_;
}

auto WidgetContainer::total_height() const -> int
{
    int total = 0;
    for (const auto& widget : widgets_)
    {
        if (widget.visible)
        {
            total += widget.height;
        }
    }
    return total;
}

void WidgetContainer::clear_all()
{
    widgets_.clear();
}

auto WidgetContainer::find_mut(const std::string& widget_id) -> ContainedWidget*
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

void WidgetContainer::reindex_positions()
{
    for (int idx = 0; idx < static_cast<int>(widgets_.size()); ++idx)
    {
        widgets_[static_cast<size_t>(idx)].position = idx;
    }
}

} // namespace markamp::core
