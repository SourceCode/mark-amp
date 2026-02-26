#include "OverviewRulerPanel.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/stc/stc.h>

#include <algorithm>

namespace markamp::ui
{

OverviewRulerPanel::OverviewRulerPanel(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       wxStyledTextCtrl* editor)
    : ThemeAwareWindow(parent, theme_engine, wxID_ANY)
    , editor_(editor)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Narrow ruler size
    SetMinSize(wxSize(14, -1));
    SetMaxSize(wxSize(14, -1));

    SetControlCursor(ControlCursorType::kHand);

    SetToolTip("Overview Ruler Legend\n"
               "─────────────────────\n"
               "Red      = Error\n"
               "Orange   = Warning\n"
               "Blue     = Info\n"
               "Yellow   = Find Match\n"
               "Green    = Git Added\n"
               "Cyan     = Git Modified\n"
               "Red      = Git Deleted\n"
               "Accent   = Bookmark\n"
               "Bright   = Breakpoint\n"
               "Gray     = Folded Region");

    Bind(wxEVT_PAINT, &OverviewRulerPanel::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &OverviewRulerPanel::OnEraseBackground, this);
    Bind(wxEVT_SIZE, &OverviewRulerPanel::OnSize, this);

    Bind(wxEVT_LEFT_DOWN, &OverviewRulerPanel::OnMouseEvent, this);
}

void OverviewRulerPanel::SetMarkers(const std::vector<OverviewMarker>& markers)
{
    markers_ = markers;
    InvalidateCache();
}

void OverviewRulerPanel::SetViewportRange(int first_line, int lines_on_screen, int total_lines)
{
    if (first_visible_line_ != first_line || lines_on_screen_ != lines_on_screen ||
        total_lines_ != total_lines)
    {
        first_visible_line_ = first_line;
        lines_on_screen_ = lines_on_screen;
        total_lines_ = total_lines;
        InvalidateCache();
    }
}

void OverviewRulerPanel::SetCursorLine(int line)
{
    if (cursor_line_ != line)
    {
        cursor_line_ = line;
        InvalidateCache();
    }
}

void OverviewRulerPanel::SetSelectionRange(int start_line, int end_line)
{
    if (sel_start_line_ != start_line || sel_end_line_ != end_line)
    {
        sel_start_line_ = start_line;
        sel_end_line_ = end_line;
        InvalidateCache();
    }
}

void OverviewRulerPanel::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    InvalidateCache();
}

void OverviewRulerPanel::OnEraseBackground(wxEraseEvent& /*event*/) {}

void OverviewRulerPanel::OnSize(wxSizeEvent& event)
{
    InvalidateCache();
    event.Skip();
}

void OverviewRulerPanel::InvalidateCache()
{
    cache_dirty_ = true;
    Refresh();
}

auto OverviewRulerPanel::GetColorForMarkerType(OverviewMarkerType type) const -> wxColour
{
    switch (type)
    {
        case OverviewMarkerType::kError:
            return theme_engine().color(core::ThemeColorToken::EditorGutterError);
        case OverviewMarkerType::kWarning:
            return theme_engine().color(core::ThemeColorToken::EditorGutterWarn);
        case OverviewMarkerType::kInfo:
            return theme_engine().color(core::ThemeColorToken::EditorGutterInfo);
        case OverviewMarkerType::kFindMatch:
            return theme_engine().color(core::ThemeColorToken::EditorFindHit);
        case OverviewMarkerType::kSelection:
            return theme_engine().color(core::ThemeColorToken::SelectionBg);
        case OverviewMarkerType::kGitAdded:
            return theme_engine().color(core::ThemeColorToken::DiffInsertedBg);
        case OverviewMarkerType::kGitModified:
            return theme_engine().color(core::ThemeColorToken::AccentSecondary);
        case OverviewMarkerType::kGitDeleted:
            return theme_engine().color(core::ThemeColorToken::DiffRemovedBg);
        case OverviewMarkerType::kBookmark:
            return theme_engine().color(core::ThemeColorToken::AccentPrimary);
        case OverviewMarkerType::kBreakpoint:
            return wxColour(255, 60, 60); // Bright red for breakpoints
        case OverviewMarkerType::kFoldCollapsed:
            return theme_engine().color(
                core::ThemeColorToken::TextMuted); // Subtle indicator for folding
        case OverviewMarkerType::kCustom:
        default:
            return theme_engine().color(core::ThemeColorToken::TextMuted);
    }
}

void OverviewRulerPanel::RenderToBuffer()
{
    const wxSize kSize = GetClientSize();
    if (kSize.GetWidth() <= 0 || kSize.GetHeight() <= 0)
    {
        return;
    }

    render_buffer_ = wxBitmap(kSize.GetWidth(), kSize.GetHeight());

    wxMemoryDC memDC;
    memDC.SelectObject(render_buffer_);

    const auto kBg = theme_engine().color(core::ThemeColorToken::BgApp);
    memDC.SetBackground(wxBrush(kBg));
    memDC.Clear();

    std::unique_ptr<wxGraphicsContext> graphics_context(wxGraphicsContext::Create(memDC));
    if (graphics_context && total_lines_ > 0)
    {
        const double kScaleY =
            static_cast<double>(kSize.GetHeight()) / static_cast<double>(total_lines_);
        const int kMarkerHeight = std::max(2, static_cast<int>(std::ceil(kScaleY)));

        // Phase 15 Task 9: Draw current selection range block
        if (sel_start_line_ >= 0 && sel_end_line_ >= sel_start_line_ &&
            sel_start_line_ < total_lines_)
        {
            const int kSelStart = static_cast<int>(sel_start_line_ * kScaleY);
            const int kSelEnd = static_cast<int>((sel_end_line_ + 1) * kScaleY);
            const int kSelHeight = std::max(2, kSelEnd - kSelStart);

            auto sel_color = theme_engine().color(core::ThemeColorToken::SelectionBg);
            graphics_context->SetBrush(wxBrush(sel_color));
            graphics_context->SetPen(*wxTRANSPARENT_PEN);
            graphics_context->DrawRectangle(0, kSelStart, kSize.GetWidth(), kSelHeight);
        }

        // Phase 15 Task 17: Support dense overlapping markers. Draw in predictable Z-order
        for (const auto& marker : markers_)
        {
            const int kMarkerY = static_cast<int>(marker.line * kScaleY);

            wxColour fill_color = (marker.type == OverviewMarkerType::kCustom)
                                      ? marker.color
                                      : GetColorForMarkerType(marker.type);

            // Draw marker spanning the full width of the ruler
            graphics_context->SetBrush(wxBrush(fill_color));
            graphics_context->SetPen(*wxTRANSPARENT_PEN);
            graphics_context->DrawRectangle(0, kMarkerY, kSize.GetWidth(), kMarkerHeight);
        }

        // Draw Viewport Slider Overlay representing current editor viewport range.
        const double kVisibleFraction = static_cast<double>(lines_on_screen_) / total_lines_;
        const int kSliderHeight =
            std::max(5, static_cast<int>(kSize.GetHeight() * kVisibleFraction));
        const double kScrollFraction = static_cast<double>(first_visible_line_) / total_lines_;
        const int kSliderY = static_cast<int>(kSize.GetHeight() * kScrollFraction);

        auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
        const wxColour kSliderColor(accent.Red(), accent.Green(), accent.Blue(), 50); // Alpha 50

        graphics_context->SetBrush(wxBrush(kSliderColor));
        graphics_context->SetPen(*wxTRANSPARENT_PEN);
        graphics_context->DrawRectangle(0, kSliderY, kSize.GetWidth(), kSliderHeight);

        // Phase 15 Task 8: Draw current cursor line indicator
        if (cursor_line_ >= 0 && cursor_line_ < total_lines_)
        {
            const int kCursorY = static_cast<int>(cursor_line_ * kScaleY);
            // Draw a subtle, distinct band over the slider
            const wxColour kCursorColor(accent.Red(), accent.Green(), accent.Blue(), 180);
            graphics_context->SetBrush(wxBrush(kCursorColor));
            graphics_context->DrawRectangle(0, kCursorY, kSize.GetWidth(), 2);
        }
    }

    memDC.SelectObject(wxNullBitmap);
    cache_dirty_ = false;
}

void OverviewRulerPanel::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);

    if (cache_dirty_ || !render_buffer_.IsOk())
    {
        RenderToBuffer();
    }

    if (render_buffer_.IsOk())
    {
        paint_dc.DrawBitmap(render_buffer_, 0, 0);
    }
}

void OverviewRulerPanel::OnMouseEvent(wxMouseEvent& event)
{
    if (total_lines_ == 0 || editor_ == nullptr)
    {
        event.Skip();
        return;
    }

    if (event.LeftDown())
    {
        ScrollEditorToCoordinate(event.GetPosition().y);
    }
    else
    {
        event.Skip();
    }
}

void OverviewRulerPanel::ScrollEditorToCoordinate(int y_pos)
{
    if (editor_ == nullptr || total_lines_ <= 0)
    {
        return;
    }

    const int ruler_height = GetClientSize().GetHeight();
    if (ruler_height <= 0)
    {
        return;
    }

    const double fraction = static_cast<double>(y_pos) / static_cast<double>(ruler_height);
    const int target_line =
        std::clamp(static_cast<int>(fraction * total_lines_), 0, std::max(0, total_lines_ - 1));

    // Phase 15 Task 18: Map marker clicks to immediate scroll jumps targeting the specific
    // error/match
    const int first_line = std::max(0, target_line - (lines_on_screen_ / 2));
    editor_->SetFirstVisibleLine(first_line);
}

} // namespace markamp::ui
