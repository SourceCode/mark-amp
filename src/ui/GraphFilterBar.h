#pragma once

#include "../core/GraphTypes.h"

#include <functional>
#include <set>
#include <string>

namespace markamp::ui
{

/// Filter controls sidebar for the graph visualization.
/// Provides controls for block type, notebook, min refs, and layout parameters.
class GraphFilterBar
{
public:
    using OnFilterChangedCallback = std::function<void(const core::GraphConfig& config)>;

    GraphFilterBar() = default;

    // ── Panel lifecycle ──

    void initialize();
    void layout(float pos_x, float pos_y, float width, float height);
    void render();

    // ── Filter state ──

    void set_config(const core::GraphConfig& config);
    [[nodiscard]] auto config() const -> const core::GraphConfig&;

    // ── Individual filter controls ──

    void set_min_refs(int min_refs);
    void set_show_daily_notes(bool show);
    void set_show_tag_nodes(bool show);
    void set_show_orphan_nodes(bool show);
    void set_include_types(const std::set<core::GraphNodeType>& types);

    // ── Layout parameters ──

    void set_force_strength(double strength);
    void set_link_distance(double distance);
    void set_layout_algorithm(const std::string& algorithm);

    // ── Callbacks ──

    void set_on_filter_changed(OnFilterChangedCallback callback);

private:
    float pos_x_{0}, pos_y_{0}, width_{0}, height_{0};
    core::GraphConfig config_;
    OnFilterChangedCallback on_filter_changed_;

    void notify_change();
};

} // namespace markamp::ui
