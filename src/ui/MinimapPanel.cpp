#include "MinimapPanel.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/stc/stc.h>

#include <algorithm>
#include <string_view>

namespace markamp::ui
{

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
        size_t startPos = 0;
        const int kCharWidth = 1;
        const int kLineHeight = 2; // 2px per line to fit thousands of lines compactly

        while (startPos < content_.length())
        {
            size_t endPos = content_.find('\n', startPos);
            if (endPos == std::string::npos)
            {
                endPos = content_.length();
            }

            const int kLengthChars = static_cast<int>(endPos - startPos);
            if (kLengthChars > 0)
            {
                // Find dominant token color for this line
                wxColour line_color = theme_engine().color(core::ThemeColorToken::TextMuted);

                // Map Scintilla Styles to ThemeTokens (simplified for MVP)
                for (const auto& token : tokens_)
                {
                    if (static_cast<size_t>(token.position) >= startPos &&
                        static_cast<size_t>(token.position) < endPos)
                    {
                        if (token.style == wxSTC_MARKDOWN_CODE ||
                            token.style == wxSTC_MARKDOWN_CODE2)
                        {
                            line_color = theme_engine().color(core::ThemeColorToken::SyntaxType);
                        }
                        else if (token.style == wxSTC_MARKDOWN_STRONG1)
                        {
                            line_color = theme_engine().color(core::ThemeColorToken::SyntaxKeyword);
                        }
                        else if (token.style == wxSTC_MARKDOWN_EM1)
                        {
                            line_color = theme_engine().color(core::ThemeColorToken::SyntaxString);
                        }
                        else if (token.style == wxSTC_MARKDOWN_HEADER1)
                        {
                            line_color = theme_engine().color(core::ThemeColorToken::RenderHeading);
                        }
                        break;
                    }
                }

                gc->SetBrush(wxBrush(line_color));
                gc->SetPen(*wxTRANSPARENT_PEN);

                // Draw a rectangle representing the line length (capped at 100px width)
                const int kBlockW = std::min(kLengthChars * kCharWidth, size.GetWidth() - 2);
                gc->DrawRectangle(1, block_y, kBlockW, 1);
            }

            block_y += kLineHeight;
            startPos = endPos + 1;
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
        const int minimap_height = GetClientSize().GetHeight();
        const int minimap_width = GetClientSize().GetWidth();

        const double visible_fraction = static_cast<double>(lines_on_screen_) / total_lines_;
        const int slider_height = std::max(20, static_cast<int>(minimap_height * visible_fraction));

        const double scroll_fraction = static_cast<double>(first_visible_line_) / total_lines_;
        const int slider_y = static_cast<int>(minimap_height * scroll_fraction);

        std::unique_ptr<wxGraphicsContext> graphics_context(wxGraphicsContext::Create(paint_dc));
        if (graphics_context)
        {
            auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
            const wxColour slider_color(
                accent.Red(), accent.Green(), accent.Blue(), 40); // Semi-transparent
            const wxColour border_color(accent.Red(), accent.Green(), accent.Blue(), 80);

            graphics_context->SetBrush(wxBrush(slider_color));
            graphics_context->SetPen(wxPen(border_color, 1));
            graphics_context->DrawRoundedRectangle(
                1, slider_y, minimap_width - 2, slider_height, 2);
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

    const int minimap_height = GetClientSize().GetHeight();
    if (minimap_height <= 0)
    {
        return;
    }

    // Determine the target absolute line fraction from Y pos
    const double fraction = static_cast<double>(y_pos) / static_cast<double>(minimap_height);

    const int target_line =
        std::clamp(static_cast<int>(fraction * total_lines_), 0, std::max(0, total_lines_ - 1));

    // Center the target line on screen
    const int first_line = std::max(0, target_line - (lines_on_screen_ / 2));

    editor_->SetFirstVisibleLine(first_line);
}

} // namespace markamp::ui
