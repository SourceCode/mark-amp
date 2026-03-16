#include "GraphFilterBar.h"

namespace markamp::ui
{

void GraphFilterBar::initialize()
{
    config_ = core::GraphConfig{};
}

void GraphFilterBar::layout(float pos_x, float pos_y, float width, float height)
{
    pos_x_ = pos_x;
    pos_y_ = pos_y;
    width_ = width;
    height_ = height;
}

void GraphFilterBar::render()
{
    // Improvement 8: Prepare filter control state for the host wxPanel.
    // The host panel reads our config() to render checkboxes and sliders.
    // Each time a filter changes, notify_change() fires the callback.
    // This method validates the current filter state is consistent.
    if (config_.min_refs < 0)
    {
        config_.min_refs = 0;
    }
    if (config_.force_strength <= 0.0)
    {
        config_.force_strength = 0.5;
    }
    if (config_.link_distance <= 0.0)
    {
        config_.link_distance = 100.0;
    }
}

void GraphFilterBar::set_config(const core::GraphConfig& config)
{
    config_ = config;
}

auto GraphFilterBar::config() const -> const core::GraphConfig&
{
    return config_;
}

void GraphFilterBar::set_min_refs(int min_refs)
{
    config_.min_refs = min_refs;
    notify_change();
}

void GraphFilterBar::set_show_daily_notes(bool show)
{
    config_.show_daily_notes = show;
    notify_change();
}

void GraphFilterBar::set_show_tag_nodes(bool show)
{
    config_.show_tag_nodes = show;
    notify_change();
}

void GraphFilterBar::set_show_orphan_nodes(bool show)
{
    config_.show_orphan_nodes = show;
    notify_change();
}

void GraphFilterBar::set_include_types(const std::set<core::GraphNodeType>& types)
{
    config_.include_types = types;
    notify_change();
}

void GraphFilterBar::set_force_strength(double strength)
{
    config_.force_strength = strength;
    notify_change();
}

void GraphFilterBar::set_link_distance(double distance)
{
    config_.link_distance = distance;
    notify_change();
}

void GraphFilterBar::set_layout_algorithm(const std::string& algorithm)
{
    config_.layout_algorithm = algorithm;
    notify_change();
}

void GraphFilterBar::set_on_filter_changed(OnFilterChangedCallback callback)
{
    on_filter_changed_ = std::move(callback);
}

void GraphFilterBar::notify_change()
{
    if (on_filter_changed_)
    {
        on_filter_changed_(config_);
    }
}

} // namespace markamp::ui
