#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/TerminalService.h"

#include <wx/timer.h>

#include <array>
#include <string>
#include <utility>
#include <vector>

namespace markamp::ui
{

/// Phase 21 Task 10: Terminal ANSI color → theme color mapping.
struct TerminalColorScheme
{
    wxColour background;
    wxColour foreground;
    wxColour cursor;
    wxColour selection_bg;
    std::array<wxColour, 16> ansi_palette; // 0-7 standard, 8-15 bright
};

/// Build a terminal color scheme from the active theme.
auto BuildTerminalColorScheme(const core::Theme& theme) -> TerminalColorScheme;

/// Phase 21 Task 6: Terminal panel renders buffer contents using custom painting.
/// Handles keyboard input, mouse selection, scrollback, and cursor blink.
class TerminalPanel : public ThemeAwareWindow
{
public:
    TerminalPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::TerminalService& terminal_service);

    void SetActiveTerminal(int terminal_id);
    [[nodiscard]] auto active_terminal_id() const -> int;

    void ScrollToBottom();
    void ScrollUp(int lines);
    void ScrollDown(int lines);

    void SelectAll();
    void CopySelection();
    void PasteClipboard();
    void ClearTerminal();
    void SoftClear();
    void ClearScrollback();

    void FindInTerminal(const std::string& query);
    void FindNext();
    void FindPrevious();

    void ShowContextMenu();

    static constexpr int kDefaultFontSize = 13;
    static constexpr int kCursorBlinkMs = 500;
    static constexpr int kMinCols = 20;
    static constexpr int kMinRows = 4;
    static constexpr int kResizeDebounceMs = 50;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::TerminalService& terminal_service_;
    int active_terminal_id_{0};

    // Rendering
    wxFont terminal_font_;
    int char_width_{0};
    int char_height_{0};
    int scroll_offset_{0};

    // Selection
    bool selecting_{false};
    int sel_start_row_{0};
    int sel_start_col_{0};
    int sel_end_row_{0};
    int sel_end_col_{0};

    // Cursor blink
    wxTimer cursor_blink_timer_;
    bool cursor_visible_{true};

    // Search
    std::string find_query_;
    std::vector<std::pair<int, int>> find_matches_;
    int find_current_index_{-1};

    // Color scheme
    TerminalColorScheme color_scheme_;

    // Resize debounce
    wxTimer resize_debounce_timer_;
    int pending_cols_{80};
    int pending_rows_{24};

    // Event subscriptions
    core::Subscription data_sub_;
    core::Subscription destroyed_sub_;

    void OnPaint(wxPaintEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnCursorBlink(wxTimerEvent& event);
    void OnResizeDebounce(wxTimerEvent& event);

    void CalculateCellDimensions();
    void CalculateTerminalSize();
    auto ScreenToCell(const wxPoint& point) -> std::pair<int, int>;
    auto MapAnsiColorToWx(const core::AnsiColor& color, bool is_foreground) -> wxColour;
    void BuildColorScheme();
};

} // namespace markamp::ui
