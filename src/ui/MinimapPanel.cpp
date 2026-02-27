#include "MinimapPanel.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/stc/stc.h>

#include <algorithm>
#include <string_view>

namespace markamp::ui
{

namespace
{
wxColour GetLineColor(core::ThemeEngine& theme_engine,
                      const std::vector<SyntaxToken>& tokens,
                      size_t start_pos,
                      size_t end_pos)
{
    wxColour color = theme_engine.color(core::ThemeColorToken::TextMuted);

    // Map Scintilla Styles to ThemeTokens (simplified for MVP)
    for (const auto& token : tokens)
    {
        if (static_cast<size_t>(token.position) >= start_pos &&
            static_cast<size_t>(token.position) < end_pos)
        {
            if (token.style == wxSTC_MARKDOWN_CODE || token.style == wxSTC_MARKDOWN_CODE2)
            {
                color = theme_engine.color(core::ThemeColorToken::SyntaxType);
            }
            else if (token.style == wxSTC_MARKDOWN_STRONG1)
            {
                color = theme_engine.color(core::ThemeColorToken::SyntaxKeyword);
            }
            else if (token.style == wxSTC_MARKDOWN_EM1)
            {
                color = theme_engine.color(core::ThemeColorToken::SyntaxString);
            }
            else if (token.style == wxSTC_MARKDOWN_HEADER1)
            {
                color = theme_engine.color(core::ThemeColorToken::RenderHeading);
            }
            break;
        }
    }
    return color;
}
} // namespace

MinimapPanel::MinimapPanel(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           wxStyledTextCtrl* editor)
    : ThemeAwareWindow(parent, theme_engine, wxID_ANY)
    , editor_(editor)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &MinimapPanel::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &MinimapPanel::OnEraseBackground, this);
    Bind(wxEVT_SIZE, &MinimapPanel::OnSize, this);

    // Mouse interactions (Click and Drag)
    Bind(wxEVT_LEFT_DOWN, &MinimapPanel::OnMouseEvent, this);
    Bind(wxEVT_LEFT_UP, &MinimapPanel::OnMouseEvent, this);
    Bind(wxEVT_MOTION, &MinimapPanel::OnMouseEvent, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &MinimapPanel::OnMouseCaptureLost, this);
}

void MinimapPanel::SetContent(const std::string& content, const std::vector<SyntaxToken>& tokens)
{
    content_ = content;
    tokens_ = tokens;
    InvalidateCache();
}

void MinimapPanel::SetViewportRange(int first_line, int lines_on_screen, int total_lines)
{
    first_visible_line_ = first_line;
    lines_on_screen_ = lines_on_screen;
    total_lines_ = total_lines;
    Refresh(); // Trigger a paint event, but cache might not need refreshing
}

void MinimapPanel::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    InvalidateCache();
}

void MinimapPanel::OnEraseBackground(wxEraseEvent& /*event*/)
{
    // Do nothing to prevent flicker
}

void MinimapPanel::OnSize(wxSizeEvent& event)
{
    InvalidateCache();
    event.Skip();
}

void MinimapPanel::InvalidateCache()
{
    cache_dirty_ = true;
    Refresh();
}

void MinimapPanel::RenderToBuffer()
{
    wxSize size = GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
    {
        return;
    }

    // Create a new bitmap matching the client area
    render_buffer_ = wxBitmap(size.GetWidth(), size.GetHeight());

    wxMemoryDC memDC;
    memDC.SelectObject(render_buffer_);

    // Fill background
    const auto bg = theme_engine().color(core::ThemeColorToken::MinimapBg);
    memDC.SetBackground(wxBrush(bg));
    memDC.Clear();

    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(memDC));
    if (gc)
    {
        // Simple "block" representation: Render each line as a tiny bar
        int block_y = 0;
        size_t start_pos = 0;
        const int kCharWidth = 1;
        const int kLineHeight = 2; // 2px per line to fit thousands of lines compactly

        while (start_pos < content_.length())
        {
            size_t end_pos = content_.find('\n', start_pos);
            if (end_pos == std::string::npos)
            {
                end_pos = content_.length();
            }

            const int kLengthChars = static_cast<int>(end_pos - start_pos);
            if (kLengthChars > 0)
            {
                // Find dominant token color for this line
                const wxColour kLineColor =
                    GetLineColor(theme_engine(), tokens_, start_pos, end_pos);

                gc->SetBrush(wxBrush(kLineColor));
                gc->SetPen(*wxTRANSPARENT_PEN);

                // Draw a rectangle representing the line length (capped at 100px width)
                const int kBlockW = std::min(kLengthChars * kCharWidth, size.GetWidth() - 2);
                gc->DrawRectangle(1, block_y, kBlockW, 1);
            }

            block_y += kLineHeight;
            start_pos = end_pos + 1;
        }
    }

    memDC.SelectObject(wxNullBitmap);
    cache_dirty_ = false;
}

void MinimapPanel::OnPaint(wxPaintEvent& /*event*/)
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

    // Phase 15 Task 3: Draw Viewport Slider
    if (total_lines_ > 0)
    {
        const int kMinimapHeight = GetClientSize().GetHeight();
        const int kMinimapWidth = GetClientSize().GetWidth();

        const double kVisibleFraction = static_cast<double>(lines_on_screen_) / total_lines_;
        const int kSliderHeight = std::max(20, static_cast<int>(kMinimapHeight * kVisibleFraction));

        const double kScrollFraction = static_cast<double>(first_visible_line_) / total_lines_;
        const int kSliderY = static_cast<int>(kMinimapHeight * kScrollFraction);

        std::unique_ptr<wxGraphicsContext> graphics_context(wxGraphicsContext::Create(paint_dc));
        if (graphics_context)
        {
            auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
            const wxColour kSliderColor(
                accent.Red(), accent.Green(), accent.Blue(), 40); // Semi-transparent
            const wxColour kBorderColor(accent.Red(), accent.Green(), accent.Blue(), 80);

            graphics_context->SetBrush(wxBrush(kSliderColor));
            graphics_context->SetPen(wxPen(kBorderColor, 1));
            graphics_context->DrawRoundedRectangle(
                1, kSliderY, kMinimapWidth - 2, kSliderHeight, 2);
        }
    }
}

void MinimapPanel::OnMouseEvent(wxMouseEvent& event)
{
    if (total_lines_ == 0 || editor_ == nullptr)
    {
        event.Skip();
        return;
    }

    if (event.LeftDown())
    {
        is_dragging_ = true;
        if (!HasCapture())
        {
            CaptureMouse();
        }
        ScrollEditorToCoordinate(event.GetPosition().y);
    }
    else if (event.LeftUp() || event.GetEventType() == wxEVT_MOUSE_CAPTURE_LOST)
    {
        is_dragging_ = false;
        if (HasCapture())
        {
            ReleaseMouse();
        }
    }
    else if (event.Dragging() && is_dragging_)
    {
        ScrollEditorToCoordinate(event.GetPosition().y);
    }
}

void MinimapPanel::OnMouseCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    is_dragging_ = false;
}

void MinimapPanel::ScrollEditorToCoordinate(int y_pos)
{
    if (editor_ == nullptr || total_lines_ <= 0)
    {
        return;
    }

    const int kMinimapHeight = GetClientSize().GetHeight();
    if (kMinimapHeight <= 0)
    {
        return;
    }

    // Determine the target absolute line fraction from Y pos
    const double kFraction = static_cast<double>(y_pos) / static_cast<double>(kMinimapHeight);

    const int kTargetLine =
        std::clamp(static_cast<int>(kFraction * total_lines_), 0, std::max(0, total_lines_ - 1));

    // Center the target line on screen
    const int kFirstLine = std::max(0, kTargetLine - (lines_on_screen_ / 2));

    editor_->SetFirstVisibleLine(kFirstLine);
}

} // namespace markamp::ui
