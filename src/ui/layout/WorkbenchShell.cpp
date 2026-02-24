#include "WorkbenchShell.h"

#include "../LayoutMetrics.h"
#include "core/Theme.h"
#include "core/ThemeTokens.h"

#include <nlohmann/json.hpp>

namespace markamp::ui::layout
{

WorkbenchShell::WorkbenchShell(wxWindow* parent, DesignSystemContext& ds)
    : ThemeAwareWindow(parent,
                       ds.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxNO_BORDER | wxCLIP_CHILDREN)
    , ds_(ds)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(ds_.theme.color(core::ThemeColorToken::BgApp));

    // Create a container panel for each zone
    for (int i = 0; i < static_cast<int>(WorkbenchZoneId::kCount); ++i)
    {
        auto id = static_cast<WorkbenchZoneId>(i);
        auto* panel = new wxPanel(
            this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTAB_TRAVERSAL);
        panel->SetBackgroundColour(ds_.theme.color(core::ThemeColorToken::BgApp));
        zone_panels_[id] = panel;
    }

    // Create ResizeHandles
    primary_sidebar_handle_ =
        new ResizeHandle(this,
                         ds_,
                         ResizeOrientation::kVertical,
                         [this](int delta)
                         {
                             auto& state =
                                 layout_model_.get_state(WorkbenchZoneId::kPrimarySidebar);
                             layout_model_.resize_zone(WorkbenchZoneId::kPrimarySidebar,
                                                       state.current_width + delta,
                                                       state.current_height);
                             trigger_layout();
                         });

    secondary_sidebar_handle_ =
        new ResizeHandle(this,
                         ds_,
                         ResizeOrientation::kVertical,
                         [this](int delta)
                         {
                             auto& state =
                                 layout_model_.get_state(WorkbenchZoneId::kSecondarySidebar);
                             // Dragging left increases secondary sidebar width since it's on the
                             // right
                             layout_model_.resize_zone(WorkbenchZoneId::kSecondarySidebar,
                                                       state.current_width - delta,
                                                       state.current_height);
                             trigger_layout();
                         });

    panel_area_handle_ =
        new ResizeHandle(this,
                         ds_,
                         ResizeOrientation::kHorizontal,
                         [this](int delta)
                         {
                             auto& state = layout_model_.get_state(WorkbenchZoneId::kPanelArea);
                             // Dragging up increases panel area height since it's on the bottom
                             layout_model_.resize_zone(WorkbenchZoneId::kPanelArea,
                                                       state.current_width,
                                                       state.current_height - delta);
                             trigger_layout();
                         });

    Bind(wxEVT_SIZE, &WorkbenchShell::OnSize, this);
}

auto WorkbenchShell::get_zone_container(WorkbenchZoneId id) -> wxWindow*
{
    return zone_panels_[id];
}

void WorkbenchShell::set_zone_visible(WorkbenchZoneId id, bool visible)
{
    if (layout_model_.is_zone_visible(id) == visible)
    {
        return;
    }

    if (visible)
    {
        layout_model_.set_zone_visible(id, true);
        const auto& state = layout_model_.get_state(id);

        int target_width = state.current_width;
        int target_height = state.current_height;

        layout_model_.set_zone_size_override(id,
                                             id == WorkbenchZoneId::kPanelArea ? target_width : 0,
                                             id == WorkbenchZoneId::kPanelArea ? 0 : target_height);
        trigger_layout();

        animation::AnimationConfig cfg;
        cfg.duration = std::chrono::milliseconds(200);
        cfg.easing_type = animation::EasingType::EaseOutCubic;

        std::string anim_name = "zone_toggle_" + std::to_string(static_cast<int>(id));
        transition_manager_.register_transition(anim_name, cfg);

        if (id == WorkbenchZoneId::kPanelArea)
        {
            transition_manager_.start<int>(anim_name,
                                           0,
                                           target_height,
                                           [this, id, target_width](int h)
                                           {
                                               layout_model_.set_zone_size_override(
                                                   id, target_width, h);
                                               trigger_layout();
                                           });
        }
        else
        {
            transition_manager_.start<int>(anim_name,
                                           0,
                                           target_width,
                                           [this, id, target_height](int w)
                                           {
                                               layout_model_.set_zone_size_override(
                                                   id, w, target_height);
                                               trigger_layout();
                                           });
        }
    }
    else
    {
        const auto& state = layout_model_.get_state(id);
        int start_width = state.current_width;
        int start_height = state.current_height;

        animation::AnimationConfig cfg;
        cfg.duration = std::chrono::milliseconds(200);
        cfg.easing_type = animation::EasingType::EaseOutCubic;

        std::string anim_name = "zone_toggle_" + std::to_string(static_cast<int>(id));
        transition_manager_.register_transition(anim_name, cfg);

        if (id == WorkbenchZoneId::kPanelArea)
        {
            transition_manager_.start<int>(anim_name,
                                           start_height,
                                           0,
                                           [this, id, start_width](int h)
                                           {
                                               layout_model_.set_zone_size_override(
                                                   id, start_width, h);
                                               trigger_layout();
                                               if (h == 0)
                                               {
                                                   layout_model_.set_zone_visible(id, false);
                                                   trigger_layout();
                                               }
                                           });
        }
        else
        {
            transition_manager_.start<int>(anim_name,
                                           start_width,
                                           0,
                                           [this, id, start_height](int w)
                                           {
                                               layout_model_.set_zone_size_override(
                                                   id, w, start_height);
                                               trigger_layout();
                                               if (w == 0)
                                               {
                                                   layout_model_.set_zone_visible(id, false);
                                                   trigger_layout();
                                               }
                                           });
        }
    }
}

auto WorkbenchShell::is_zone_visible(WorkbenchZoneId id) const -> bool
{
    return layout_model_.is_zone_visible(id);
}

void WorkbenchShell::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    SetBackgroundColour(ds_.theme.color(core::ThemeColorToken::BgApp));
    for (auto& [id, panel] : zone_panels_)
    {
        panel->SetBackgroundColour(ds_.theme.color(core::ThemeColorToken::BgApp));
        panel->Refresh();
    }
}

void WorkbenchShell::OnSize(wxSizeEvent& event)
{
    auto sz = event.GetSize();
    layout_model_.update_window_size(sz.GetWidth(), sz.GetHeight());
    trigger_layout();
    event.Skip();
}

void WorkbenchShell::trigger_layout()
{
    layout_model_.recalculate_layout();

    const int kHitWidth = ds_.metrics.splitter_hit_width();
    const int kHalfHit = kHitWidth / 2;

    for (int i = 0; i < static_cast<int>(WorkbenchZoneId::kCount); ++i)
    {
        auto id = static_cast<WorkbenchZoneId>(i);
        auto* panel = zone_panels_[id];
        const auto& state = layout_model_.get_state(id);

        if (state.visible)
        {
            panel->SetSize(state.bounds);
            if (!panel->IsShown())
                panel->Show();
        }
        else
        {
            if (panel->IsShown())
                panel->Hide();
        }
    }

    // Position resize handles
    auto& primary = layout_model_.get_state(WorkbenchZoneId::kPrimarySidebar);
    if (primary.visible && is_zone_visible(WorkbenchZoneId::kEditorArea))
    {
        primary_sidebar_handle_->SetSize(primary.bounds.GetRight() - kHalfHit,
                                         primary.bounds.GetTop(),
                                         kHitWidth,
                                         primary.bounds.GetHeight());
        if (!primary_sidebar_handle_->IsShown())
            primary_sidebar_handle_->Show();
        primary_sidebar_handle_->Raise();
    }
    else
    {
        if (primary_sidebar_handle_->IsShown())
            primary_sidebar_handle_->Hide();
    }

    auto& secondary = layout_model_.get_state(WorkbenchZoneId::kSecondarySidebar);
    if (secondary.visible && is_zone_visible(WorkbenchZoneId::kEditorArea))
    {
        secondary_sidebar_handle_->SetSize(secondary.bounds.GetLeft() - kHalfHit,
                                           secondary.bounds.GetTop(),
                                           kHitWidth,
                                           secondary.bounds.GetHeight());
        if (!secondary_sidebar_handle_->IsShown())
            secondary_sidebar_handle_->Show();
        secondary_sidebar_handle_->Raise();
    }
    else
    {
        if (secondary_sidebar_handle_->IsShown())
            secondary_sidebar_handle_->Hide();
    }

    auto& panel_area = layout_model_.get_state(WorkbenchZoneId::kPanelArea);
    if (panel_area.visible && is_zone_visible(WorkbenchZoneId::kEditorArea))
    {
        panel_area_handle_->SetSize(panel_area.bounds.GetLeft(),
                                    panel_area.bounds.GetTop() - kHalfHit,
                                    panel_area.bounds.GetWidth(),
                                    kHitWidth);
        if (!panel_area_handle_->IsShown())
            panel_area_handle_->Show();
        panel_area_handle_->Raise();
    }
    else
    {
        if (panel_area_handle_->IsShown())
            panel_area_handle_->Hide();
    }

    Refresh();
}

auto WorkbenchShell::save_state_to_json() const -> nlohmann::json
{
    return layout_model_.save_state_to_json();
}

void WorkbenchShell::load_state_from_json(const nlohmann::json& json_state)
{
    layout_model_.load_state_from_json(json_state);
    trigger_layout();
}

void WorkbenchShell::apply_preset(const std::string& preset_id)
{
    layout_model_.apply_preset(preset_id);
    trigger_layout();
}

} // namespace markamp::ui::layout
