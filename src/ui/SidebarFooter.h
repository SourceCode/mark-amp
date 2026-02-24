#pragma once

#include "ui/ThemeAwareWindow.h"

#include <string>

namespace markamp::core
{
class EventBus;
}

namespace markamp::ui
{
struct DesignSystemContext;

/// Optional footer at the bottom of a sidebar panel showing contextual status information.
class SidebarFooter : public ThemeAwareWindow
{
public:
    SidebarFooter(wxWindow* parent, DesignSystemContext& ds, core::EventBus& event_bus);

    void set_text(const std::string& text);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    std::string text_;

    int footer_height_{24};

    void UpdateMetrics();

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
