#pragma once

#include "WorkbenchZone.h"

#include <nlohmann/json_fwd.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui::layout
{

struct WorkbenchLayoutPreset
{
    std::string id;
    std::string name;
    std::unordered_map<WorkbenchZoneId, WorkbenchZoneState> zone_states;
};

class WorkbenchLayoutModel
{
public:
    WorkbenchLayoutModel();

    /// Update the total dimensions available to the layout model
    void update_window_size(int width, int height);

    /// Show or hide a specific zone
    void set_zone_visible(WorkbenchZoneId zone_id, bool visible);
    [[nodiscard]] auto is_zone_visible(WorkbenchZoneId zone_id) const -> bool;

    /// Called by ResizeHandles to request a new size for a zone
    void resize_zone(WorkbenchZoneId zone_id, int new_width, int new_height);

    /// Use to bypass constraints during animation
    void set_zone_size_override(WorkbenchZoneId zone_id, int width, int height);

    /// Access state/constraints for a zone
    [[nodiscard]] auto get_state(WorkbenchZoneId zone_id) const -> const WorkbenchZoneState&;
    [[nodiscard]] auto get_constraints(WorkbenchZoneId zone_id) const
        -> const WorkbenchZoneConstraints&;

    /// Forces a recalculation of all zone bounds based on current sizes and visibility
    void recalculate_layout();

    /// Presets & State Persistence (Tasks 5 & 6)
    [[nodiscard]] auto save_state_to_json() const -> nlohmann::json;
    void load_state_from_json(const nlohmann::json& json_state);

    void add_preset(WorkbenchLayoutPreset preset);
    void apply_preset(const std::string& preset_id);
    [[nodiscard]] auto get_available_presets() const -> const std::vector<WorkbenchLayoutPreset>&;

private:
    int window_width_{1024};
    int window_height_{768};

    std::unordered_map<WorkbenchZoneId, WorkbenchZoneState> states_;
    std::unordered_map<WorkbenchZoneId, WorkbenchZoneConstraints> constraints_;
    std::vector<WorkbenchLayoutPreset> presets_;

    void init_defaults();
};

} // namespace markamp::ui::layout
