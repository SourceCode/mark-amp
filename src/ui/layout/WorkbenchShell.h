#pragma once

#include "../DesignSystemContext.h"
#include "../ThemeAwareWindow.h"
#include "ResizeHandle.h"
#include "WorkbenchLayoutModel.h"

#include <nlohmann/json_fwd.hpp>
#include <wx/panel.h>

#include <unordered_map>

namespace markamp::ui::layout
{

class WorkbenchShell : public ThemeAwareWindow
{
public:
    WorkbenchShell(wxWindow* parent, DesignSystemContext& ds);

    [[nodiscard]] auto get_zone_container(WorkbenchZoneId id) -> wxWindow*;

    void set_zone_visible(WorkbenchZoneId id, bool visible);
    [[nodiscard]] auto is_zone_visible(WorkbenchZoneId id) const -> bool;
    [[nodiscard]] auto get_zone_bounds(WorkbenchZoneId id) const -> wxRect;
    void set_zone_width(WorkbenchZoneId id, int width);

    // Forces immediate reflow based on the layout model
    void trigger_layout();

    /// Persistence & Presets
    [[nodiscard]] auto save_state_to_json() const -> nlohmann::json;
    void load_state_from_json(const nlohmann::json& json_state);
    void apply_preset(const std::string& preset_id);

    /// Callbacks
    void set_zone_resized_callback(std::function<void(WorkbenchZoneId)> cb)
    {
        on_zone_resized_ = std::move(cb);
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    WorkbenchLayoutModel layout_model_;

    std::unordered_map<WorkbenchZoneId, wxWindow*> zone_panels_;

    // Resize handles
    ResizeHandle* primary_sidebar_handle_{nullptr};
    ResizeHandle* secondary_sidebar_handle_{nullptr};
    ResizeHandle* panel_area_handle_{nullptr};

    std::function<void(WorkbenchZoneId)> on_zone_resized_;

    animation::TransitionManager transition_manager_{this};

    bool auto_hidden_secondary_{
        false}; // Phase 09 Task 14: Track auto-hidden state due to window width limits

    void OnSize(wxSizeEvent& event);
};

} // namespace markamp::ui::layout
