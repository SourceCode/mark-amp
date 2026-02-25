#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/DesignSystemContext.h"
#include "ui/ThemeAwareWindow.h"

#include <wx/panel.h>

#include <string>
#include <vector>

namespace markamp::core
{
class Config;
}

namespace markamp::ui
{

class SecondarySidebarTabStrip : public ThemeAwareWindow
{
public:
    SecondarySidebarTabStrip(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             DesignSystemContext& ds,
                             core::EventBus& event_bus,
                             core::Config* config = nullptr);

    void AddTab(const std::string& mode, const std::string& icon, const std::string& tooltip);
    void RemoveTab(const std::string& mode);
    [[nodiscard]] auto HasTab(const std::string& mode) const -> bool;
    void SetActiveMode(const std::string& mode);
    [[nodiscard]] auto GetActiveMode() const -> std::string;

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnChar(wxKeyEvent& event);

    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;

    struct Tab
    {
        std::string mode;
        std::string icon;
        std::string tooltip;
        wxRect rect;
        wxRect close_rect; // Phase 09 Task 18: Close button hit area
        bool is_hovered{false};
        bool is_pressed{false};
        bool is_close_hovered{false};
    };

    std::vector<Tab> tabs_;
    std::string active_mode_;

    // Accessibility
    int focus_index_{-1};
    void UpdateAccessibilityState();

    // Drag and Drop (Task 17)
    int drag_target_index_{-1};

    int hover_index_{-1};
    int pressed_index_{-1};

    wxPoint drag_start_pos_{wxDefaultPosition};
    bool is_dragging_{false};

    void UpdateLayoutMetrics();
    int HitTest(const wxPoint& pos) const;

    core::Subscription search_count_sub_;
    core::Subscription diagnostics_sub_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
