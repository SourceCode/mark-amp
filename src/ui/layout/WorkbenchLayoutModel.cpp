#include "WorkbenchLayoutModel.h"

#include <nlohmann/json.hpp>

#include <algorithm>

namespace markamp::ui::layout
{

WorkbenchLayoutModel::WorkbenchLayoutModel()
{
    init_defaults();
}

void WorkbenchLayoutModel::init_defaults()
{
    // Initialize constraints
    constraints_[WorkbenchZoneId::kActivityBar] = {
        .min_width = 48, .max_width = 48, .default_width = 48};
    constraints_[WorkbenchZoneId::kPrimarySidebar] = {
        .min_width = 170, .max_width = 800, .default_width = 256};
    constraints_[WorkbenchZoneId::kEditorArea] = {
        .min_width = 200, .max_width = 10000, .min_height = 100};
    constraints_[WorkbenchZoneId::kSecondarySidebar] = {
        .min_width = 170, .max_width = 800, .default_width = 256};
    constraints_[WorkbenchZoneId::kPanelArea] = {
        .min_height = 100, .max_height = 800, .default_height = 200};
    constraints_[WorkbenchZoneId::kStatusBar] = {
        .min_height = 24, .max_height = 24, .default_height = 24};

    // Initialize states
    for (int i = 0; i < static_cast<int>(WorkbenchZoneId::kCount); ++i)
    {
        auto id = static_cast<WorkbenchZoneId>(i);
        states_[id] = {.id = id,
                       .visible = true, // By default all visible except maybe secondary sidebar
                       .current_width = constraints_[id].default_width,
                       .current_height = constraints_[id].default_height,
                       .restored_width = constraints_[id].default_width,
                       .restored_height = constraints_[id].default_height};
    }

    states_[WorkbenchZoneId::kSecondarySidebar].visible = false;
    states_[WorkbenchZoneId::kPanelArea].visible = false;

    // Build default presets
    WorkbenchLayoutPreset standard_preset{"standard", "Standard", {}};
    for (int i = 0; i < static_cast<int>(WorkbenchZoneId::kCount); ++i)
    {
        auto zone_id = static_cast<WorkbenchZoneId>(i);
        standard_preset.zone_states[zone_id] = states_[zone_id];
    }
    presets_.push_back(standard_preset);

    WorkbenchLayoutPreset developer_preset{"developer", "Developer", {}};
    for (int i = 0; i < static_cast<int>(WorkbenchZoneId::kCount); ++i)
    {
        auto zone_id = static_cast<WorkbenchZoneId>(i);
        auto state = states_[zone_id];
        if (zone_id == WorkbenchZoneId::kPanelArea)
        {
            state.visible = true;
            state.current_height = state.restored_height > 0 ? state.restored_height : 200;
        }
        developer_preset.zone_states[zone_id] = state;
    }
    presets_.push_back(developer_preset);

    WorkbenchLayoutPreset zen_preset{"zen", "Zen Mode", {}};
    for (int i = 0; i < static_cast<int>(WorkbenchZoneId::kCount); ++i)
    {
        auto zone_id = static_cast<WorkbenchZoneId>(i);
        auto state = states_[zone_id];
        if (zone_id != WorkbenchZoneId::kEditorArea)
        {
            state.visible = false;
            state.current_width = 0;
            state.current_height = 0;
        }
        zen_preset.zone_states[zone_id] = state;
    }
    presets_.push_back(zen_preset);
}

void WorkbenchLayoutModel::update_window_size(int width, int height)
{
    if (window_width_ != width || window_height_ != height)
    {
        window_width_ = width;
        window_height_ = height;
        recalculate_layout();
    }
}

void WorkbenchLayoutModel::set_zone_visible(WorkbenchZoneId zone_id, bool visible)
{
    auto& state = states_[zone_id];
    if (state.visible != visible)
    {
        state.visible = visible;
        if (visible)
        {
            state.current_width = state.restored_width > 0 ? state.restored_width
                                                           : constraints_[zone_id].default_width;
            state.current_height = state.restored_height > 0 ? state.restored_height
                                                             : constraints_[zone_id].default_height;
        }
        else
        {
            if (state.current_width > 0)
            {
                state.restored_width = state.current_width;
            }
            if (state.current_height > 0)
            {
                state.restored_height = state.current_height;
            }
            state.current_width = 0;
            state.current_height = 0;
        }
        recalculate_layout();
    }
}

auto WorkbenchLayoutModel::is_zone_visible(WorkbenchZoneId zone_id) const -> bool
{
    return states_.at(zone_id).visible;
}

void WorkbenchLayoutModel::resize_zone(WorkbenchZoneId zone_id, int new_width, int new_height)
{
    auto& state = states_[zone_id];
    const auto& constraints = constraints_[zone_id];

    if (state.visible)
    {
        // For horizontal zones like PanelArea we care about height.
        // For vertical zones like Sidebars we care about width.
        state.current_width = std::clamp(new_width, constraints.min_width, constraints.max_width);
        state.current_height =
            std::clamp(new_height, constraints.min_height, constraints.max_height);
        recalculate_layout();
    }
}

void WorkbenchLayoutModel::set_zone_size_override(WorkbenchZoneId zone_id, int width, int height)
{
    auto& state = states_[zone_id];
    if (state.visible)
    {
        state.current_width = width;
        state.current_height = height;
        recalculate_layout();
    }
}

auto WorkbenchLayoutModel::get_state(WorkbenchZoneId zone_id) const -> const WorkbenchZoneState&
{
    return states_.at(zone_id);
}

auto WorkbenchLayoutModel::get_constraints(WorkbenchZoneId zone_id) const
    -> const WorkbenchZoneConstraints&
{
    return constraints_.at(zone_id);
}

void WorkbenchLayoutModel::recalculate_layout()
{
    int content_width = window_width_;
    int content_height = window_height_;

    // 1. Status Bar
    auto& status = states_[WorkbenchZoneId::kStatusBar];
    if (status.visible)
    {
        status.bounds =
            wxRect(0, window_height_ - status.current_height, window_width_, status.current_height);
        content_height -= status.current_height;
    }
    else
    {
        status.bounds = wxRect(0, window_height_, window_width_, 0);
    }

    // 2. Activity Bar
    auto& activity = states_[WorkbenchZoneId::kActivityBar];
    int current_x = 0;
    if (activity.visible)
    {
        activity.bounds = wxRect(current_x, 0, activity.current_width, content_height);
        current_x += activity.current_width;
        content_width -= activity.current_width;
    }
    else
    {
        activity.bounds = wxRect(current_x, 0, 0, content_height);
    }

    // 3. Primary Sidebar
    auto& primary = states_[WorkbenchZoneId::kPrimarySidebar];
    if (primary.visible)
    {
        primary.bounds = wxRect(current_x, 0, primary.current_width, content_height);
        current_x += primary.current_width;
        content_width -= primary.current_width;
    }
    else
    {
        primary.bounds = wxRect(current_x, 0, 0, content_height);
    }

    // 4. Secondary Sidebar
    auto& secondary = states_[WorkbenchZoneId::kSecondarySidebar];
    if (secondary.visible)
    {
        secondary.bounds = wxRect(
            window_width_ - secondary.current_width, 0, secondary.current_width, content_height);
        content_width -= secondary.current_width;
    }
    else
    {
        secondary.bounds = wxRect(window_width_, 0, 0, content_height);
    }

    // 5. Panel Area (Bottom)
    auto& panel = states_[WorkbenchZoneId::kPanelArea];
    if (panel.visible)
    {
        panel.bounds = wxRect(
            current_x, content_height - panel.current_height, content_width, panel.current_height);
        content_height -= panel.current_height;
    }
    else
    {
        panel.bounds = wxRect(current_x, content_height, content_width, 0);
    }

    // 6. Editor Area
    auto& editor = states_[WorkbenchZoneId::kEditorArea];
    // Enforce constraints for EditorArea just to be safe
    editor.current_width =
        std::max(constraints_[WorkbenchZoneId::kEditorArea].min_width, content_width);
    editor.current_height =
        std::max(constraints_[WorkbenchZoneId::kEditorArea].min_height, content_height);
    editor.bounds = wxRect(current_x, 0, editor.current_width, editor.current_height);
}

auto WorkbenchLayoutModel::save_state_to_json() const -> nlohmann::json
{
    nlohmann::json j_state = nlohmann::json::object();

    for (const auto& [zone_id, state] : states_)
    {
        nlohmann::json z = nlohmann::json::object();
        z["visible"] = state.visible;
        z["current_width"] = state.current_width;
        z["current_height"] = state.current_height;
        z["restored_width"] = state.restored_width;
        z["restored_height"] = state.restored_height;
        std::string key = std::to_string(static_cast<int>(zone_id));
        j_state[key] = z;
    }

    return j_state;
}

void WorkbenchLayoutModel::load_state_from_json(const nlohmann::json& json_state)
{
    if (!json_state.is_object())
    {
        return;
    }

    for (int i = 0; i < static_cast<int>(WorkbenchZoneId::kCount); ++i)
    {
        auto zone_id = static_cast<WorkbenchZoneId>(i);
        const std::string key = std::to_string(static_cast<int>(zone_id));
        if (json_state.contains(key) && json_state[key].is_object())
        {
            const auto& z = json_state[key];
            auto& state = states_[zone_id];
            state.visible = z.value("visible", state.visible);
            state.current_width = z.value("current_width", state.current_width);
            state.current_height = z.value("current_height", state.current_height);
            state.restored_width = z.value("restored_width", state.restored_width);
            state.restored_height = z.value("restored_height", state.restored_height);
        }
    }
    recalculate_layout();
}

void WorkbenchLayoutModel::add_preset(WorkbenchLayoutPreset preset)
{
    presets_.push_back(std::move(preset));
}

void WorkbenchLayoutModel::apply_preset(const std::string& preset_id)
{
    for (const auto& preset : presets_)
    {
        if (preset.id == preset_id)
        {
            for (const auto& [zone_id, state_override] : preset.zone_states)
            {
                auto& state = states_[zone_id];
                state.visible = state_override.visible;
                state.current_width = state_override.current_width;
                state.current_height = state_override.current_height;
                state.restored_width = state_override.restored_width;
                state.restored_height = state_override.restored_height;
            }
            recalculate_layout();
            return;
        }
    }
}

auto WorkbenchLayoutModel::get_available_presets() const
    -> const std::vector<WorkbenchLayoutPreset>&
{
    return presets_;
}

} // namespace markamp::ui::layout
