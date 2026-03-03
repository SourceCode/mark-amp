#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 21 Task 7: Terminal tab bar for multiple session management.
/// Custom-painted tab bar at the top of the terminal area.
class TerminalTabBar : public ThemeAwareWindow
{
public:
    TerminalTabBar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    struct TerminalTab
    {
        int terminal_id{0};
        std::string name;
        std::string shell_type;
        bool is_active{false};
        wxRect rect;
        wxRect close_rect;
    };

    void AddTab(int terminal_id, const std::string& name, const std::string& shell_type);
    void RemoveTab(int terminal_id);
    void SetActiveTab(int terminal_id);
    void RenameTab(int terminal_id, const std::string& new_name);
    [[nodiscard]] auto tab_count() const -> int;

    using TabSelectedCallback = std::function<void(int terminal_id)>;
    using NewTerminalCallback = std::function<void()>;
    using CloseTerminalCallback = std::function<void(int terminal_id)>;

    void set_on_tab_selected(TabSelectedCallback callback);
    void set_on_new_terminal(NewTerminalCallback callback);
    void set_on_close_terminal(CloseTerminalCallback callback);

    static constexpr int kHeight = 28;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    std::vector<TerminalTab> tabs_;

    TabSelectedCallback on_tab_selected_;
    NewTerminalCallback on_new_terminal_;
    CloseTerminalCallback on_close_terminal_;

    int hovered_tab_{-1};
    wxRect plus_button_rect_;

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void RecalculateTabRects();
};

} // namespace markamp::ui
