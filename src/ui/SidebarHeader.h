#pragma once

#include "ui/ThemeAwareWindow.h"

#include <string>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::ui
{
struct DesignSystemContext;
class IconManager;
class BreadcrumbBar;

enum class SidebarHeaderMode
{
    kTitle,
    kBreadcrumb
};

class SidebarHeader : public ThemeAwareWindow
{
public:
    SidebarHeader(wxWindow* parent,
                  DesignSystemContext& ds,
                  IconManager& icon_manager,
                  core::EventBus& event_bus);

    void set_title(const std::string& title);

    void set_display_mode(SidebarHeaderMode mode);
    void set_breadcrumb(const std::vector<std::string>& path);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    IconManager& icon_manager_;
    core::EventBus& event_bus_;
    std::string title_{"EXPLORER"};
    SidebarHeaderMode mode_{SidebarHeaderMode::kTitle};

    BreadcrumbBar* breadcrumb_{nullptr};

    // Metrics
    int height_{36}; // Defaults to be overridden by DesignSystem

    // Action button state
    bool is_hovering_collapse_all_{false};
    bool is_pressed_collapse_all_{false};
    wxRect collapse_all_rect_;

    void UpdateMetrics();

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
