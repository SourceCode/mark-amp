#pragma once

#include "DragController.h"
#include "ThemeAwareWindow.h"

#include <string>

namespace markamp::ui
{

/// Phase 45: Floating preview window that follows cursor during drag.
class DragGhostWindow : public ThemeAwareWindow
{
public:
    DragGhostWindow(wxWindow* parent, core::ThemeEngine& theme_engine);

    /// Configure the ghost for a tab drag.
    void configure_for_tab(const std::string& tab_title);

    /// Configure the ghost for a file drag.
    void configure_for_file(const std::string& file_name, const std::string& icon_name);

    /// Configure the ghost for a panel drag.
    void configure_for_panel(const std::string& panel_title);

    /// Update position to follow cursor.
    void move_to(int screen_x, int screen_y);

    /// Show/hide the ghost.
    void show_ghost();
    void hide_ghost();

    [[nodiscard]] auto is_visible() const -> bool
    {
        return is_visible_;
    }

private:
    void on_paint(wxPaintEvent& event);

    std::string label_;
    std::string icon_name_;
    DragPayloadType type_{DragPayloadType::kTab};
    bool is_visible_{false};
    static constexpr int kGhostWidth = 200;
    static constexpr int kGhostHeight = 32;
    static constexpr int kCursorOffset = 8;
};

} // namespace markamp::ui
