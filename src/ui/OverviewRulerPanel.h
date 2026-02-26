#pragma once

#include "ui/ThemeAwareWindow.h"

#include <wx/bitmap.h>
#include <wx/panel.h>

#include <vector>

class wxStyledTextCtrl;

namespace markamp::ui
{

enum class OverviewMarkerType
{
    kError,
    kWarning,
    kInfo,
    kFindMatch,
    kSelection,
    kGitAdded,
    kGitModified,
    kGitDeleted,
    kBookmark,
    kBreakpoint,
    kFoldCollapsed,
    kCustom
};

struct OverviewMarker
{
    int line{0};
    OverviewMarkerType type{OverviewMarkerType::kCustom};
    wxColour color; // Optional override color for custom markers
};

class OverviewRulerPanel : public ThemeAwareWindow
{
public:
    OverviewRulerPanel(wxWindow* parent, core::ThemeEngine& theme_engine, wxStyledTextCtrl* editor);
    ~OverviewRulerPanel() override = default;

    OverviewRulerPanel(const OverviewRulerPanel&) = delete;
    auto operator=(const OverviewRulerPanel&) -> OverviewRulerPanel& = delete;
    OverviewRulerPanel(OverviewRulerPanel&&) = delete;
    auto operator=(OverviewRulerPanel&&) -> OverviewRulerPanel& = delete;

    void SetMarkers(const std::vector<OverviewMarker>& markers);
    void SetViewportRange(int first_line, int lines_on_screen, int total_lines);
    void SetCursorLine(int line);
    void SetSelectionRange(int start_line, int end_line);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseEvent(wxMouseEvent& event);

    void RenderToBuffer();
    void InvalidateCache();
    void ScrollEditorToCoordinate(int y_pos);
    [[nodiscard]] auto GetColorForMarkerType(OverviewMarkerType type) const -> wxColour;

    wxStyledTextCtrl* editor_;

    std::vector<OverviewMarker> markers_;
    int first_visible_line_{0};
    int lines_on_screen_{0};
    int total_lines_{0};
    int cursor_line_{-1};
    int sel_start_line_{-1};
    int sel_end_line_{-1};

    wxBitmap render_buffer_;
    bool cache_dirty_{true};
};

} // namespace markamp::ui
