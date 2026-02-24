#pragma once

#include "ui/ThemeAwareWindow.h"

#include <string>
#include <vector>

class wxBoxSizer;

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::ui
{
struct DesignSystemContext;
class IconManager;
class BreadcrumbBar;

enum class PanelHeaderMode
{
    kTitle,
    kBreadcrumb
};

class PanelHeader : public ThemeAwareWindow
{
public:
    PanelHeader(wxWindow* parent,
                DesignSystemContext& ds,
                IconManager& icon_manager,
                core::EventBus& event_bus);

    void set_title(const std::string& title);

    void set_panel_id(const std::string& panel_id);
    void set_display_mode(PanelHeaderMode mode);
    void set_breadcrumb(const std::vector<std::string>& path);

    struct ActionIcon
    {
        std::string id;
        std::string icon_name;
        std::string tooltip;
    };

    void set_actions(const std::vector<ActionIcon>& actions);

    void set_toolbar(wxWindow* toolbar);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnMouseRightUp(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    IconManager& icon_manager_;
    core::EventBus& event_bus_;

    std::string title_;
    std::string panel_id_;
    PanelHeaderMode mode_{PanelHeaderMode::kTitle};
    std::vector<ActionIcon> actions_;

    BreadcrumbBar* breadcrumb_{nullptr};

    // Breadcrumb updates subscription
    core::Subscription breadcrumb_sub_;

    // Layout and interaction metrics
    int header_height_{28};
    wxWindow* toolbar_{nullptr};
    ::wxBoxSizer* main_sizer_{nullptr};

    struct ActionRect
    {
        ActionIcon action;
        wxRect rect;
        bool is_hovered{false};
        bool is_pressed{false};
    };

    std::vector<ActionRect> action_rects_;

    wxPoint drag_start_pos_{wxDefaultPosition};
    bool is_dragging_{false};

    void UpdateMetrics();
    void LayoutActions();

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
