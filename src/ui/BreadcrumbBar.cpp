#include "BreadcrumbBar.h"

#include "TypographyScale.h"
#include "core/Events.h"
#include "ui/FileTypeIconResolver.h"
#include "ui/FocusManager.h"
#include "ui/FocusRingRenderer.h"
#include "ui/IconManager.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/settings.h>
#include <wx/sizer.h>

namespace markamp::ui
{

BreadcrumbBar::BreadcrumbBar(wxWindow* parent, DesignSystemContext& ds)
    : ThemeAwareWindow(parent,
                       ds.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(-1, ds.metrics.row_height()),
                       wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE)
    , ds_(ds)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    OnThemeChanged(ds_.theme.current_theme());

    SetCanFocus(true);

    Bind(wxEVT_PAINT, &BreadcrumbBar::OnPaint, this);
    Bind(wxEVT_MOTION, &BreadcrumbBar::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &BreadcrumbBar::OnMouseDown, this);
    Bind(wxEVT_LEAVE_WINDOW, &BreadcrumbBar::OnLeaveWindow, this);

    Bind(wxEVT_KEY_DOWN,
         [this](wxKeyEvent& event)
         {
             if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_SPACE)
             {
                 if (is_focused_ && !file_segments_.empty())
                 {
                     if (segment_click_callback_)
                     {
                         // Since we don't have fine-grained keyboard nav yet, just invoke on full
                         // path
                         std::string full_path;
                         for (size_t idx = 0; idx < file_segments_.size(); ++idx)
                         {
                             if (idx > 0)
                                 full_path += "/";
                             full_path += file_segments_[idx];
                         }
                         segment_click_callback_(full_path);
                     }
                 }
             }
             event.Skip();
         });

    Bind(wxEVT_SET_FOCUS,
         [this](wxFocusEvent& event)
         {
             is_focused_ = true;
             FocusManager::get().set_focus(FocusZoneId::kBreadcrumb, 0);
             Refresh();
             event.Skip();
         });

    Bind(wxEVT_KILL_FOCUS,
         [this](wxFocusEvent& event)
         {
             is_focused_ = false;
             if (FocusManager::get().current_zone() == FocusZoneId::kBreadcrumb)
             {
                 FocusManager::get().set_item(-1);
             }
             Refresh();
             event.Skip();
         });
}

void BreadcrumbBar::SetSegmentClickCallback(SegmentClickCallback callback)
{
    segment_click_callback_ = std::move(callback);
}

void BreadcrumbBar::SetFilePath(const std::vector<std::string>& segments)
{
    file_segments_ = segments;
    Rebuild();
    Refresh();
}

void BreadcrumbBar::SetHeadingPath(const std::vector<std::string>& headings)
{
    heading_segments_ = headings;
    Rebuild();
    Refresh();
}

void BreadcrumbBar::SetTraversalSegments(const std::vector<TraversalSegment>& segments)
{
    traversal_segments_ = segments;
    Rebuild();
    Refresh();
}

auto BreadcrumbBar::GetTraversalSegments() const -> const std::vector<TraversalSegment>&
{
    return traversal_segments_;
}

void BreadcrumbBar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

void BreadcrumbBar::Rebuild()
{
    drawn_segments_.clear();
    hovered_segment_ = -1;
}

void BreadcrumbBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);

    auto client_size = GetClientSize();
    int width = client_size.GetWidth();
    int height = client_size.GetHeight();

    auto bg_color = theme_engine().color(core::ThemeColorToken::BgPanel);
    dc.SetBrush(wxBrush(bg_color));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, width, height);

    if (file_segments_.empty() && heading_segments_.empty() && traversal_segments_.empty())
    {
        return;
    }

    auto font = ds_.typography.font(TypeSlot::kCaption);
    auto bold_font = font;
    bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);

    int current_x = ds_.spacing.scaled(SpacingToken::kMd);
    const int text_y = (height - dc.GetCharHeight()) / 2 + 1;
    const int icon_size = 14;
    const int icon_y = (height - icon_size) / 2;

    auto text_color_normal = theme_engine().color(core::ThemeColorToken::TextMuted);
    auto text_color_hover = theme_engine().color(core::ThemeColorToken::TextMain);
    auto separator_color = theme_engine().color(core::ThemeColorToken::BorderLight);

    bool rebuild_bounds = drawn_segments_.empty();
    int segment_index = 0;
    std::string full_path = "";

    auto draw_separator = [&]()
    {
        IconManager::get().draw_icon(dc,
                                     "chevron-right",
                                     current_x + 2,
                                     icon_y,
                                     wxSize(icon_size, icon_size),
                                     separator_color);
        current_x += icon_size + 4;
    };

    auto draw_segment = [&](const std::string& text,
                            const std::string& icon_name,
                            bool is_last_file,
                            const std::string& segment_path)
    {
        int start_x = current_x;

        wxColour fg_color = text_color_normal;
        if (hovered_segment_ == segment_index)
        {
            fg_color = text_color_hover;
        }

        if (!icon_name.empty())
        {
            IconManager::get().draw_icon(dc,
                                         icon_name,
                                         current_x,
                                         icon_y,
                                         wxSize(icon_size, icon_size),
                                         fg_color,
                                         GetDPIScaleFactor());
            current_x += icon_size + 4;
        }

        dc.SetFont(is_last_file ? bold_font : font);
        dc.SetTextForeground(fg_color);
        wxString wx_text = wxString::FromUTF8(text);
        dc.DrawText(wx_text, current_x, text_y);
        current_x += dc.GetTextExtent(wx_text).GetWidth();

        if (rebuild_bounds)
        {
            DrawnSegment ds;
            ds.bounds = wxRect(start_x, 0, current_x - start_x, height);
            ds.full_path = segment_path;
            drawn_segments_.push_back(ds);
        }

        segment_index++;
    };

    // 1. File Segments
    for (size_t idx = 0; idx < file_segments_.size(); ++idx)
    {
        if (idx > 0)
        {
            draw_separator();
            full_path += "/";
        }
        full_path += file_segments_[idx];

        std::string icon = "";
        if (idx == file_segments_.size() - 1)
        {
            icon = FileTypeIconResolver::GetFileIcon(file_segments_[idx]);
        }

        draw_segment(file_segments_[idx], icon, idx == file_segments_.size() - 1, full_path);
    }

    // 2. Heading Segments
    for (size_t idx = 0; idx < heading_segments_.size(); ++idx)
    {
        if (!file_segments_.empty() || idx > 0)
        {
            draw_separator();
        }

        std::string heading_path = full_path;
        if (!heading_path.empty())
            heading_path += " -> ";
        heading_path += heading_segments_[idx];

        draw_segment(heading_segments_[idx], "symbol-misc", false, heading_path);
    }

    // 3. Traversal Segments (V8 Phase 12)
    for (size_t idx = 0; idx < traversal_segments_.size(); ++idx)
    {
        if (!file_segments_.empty() || !heading_segments_.empty() || idx > 0)
        {
            draw_separator();
        }

        std::string label =
            traversal_segments_[idx].surface_label + ": " + traversal_segments_[idx].anchor_label;
        draw_segment(label, "link", false, label);
    }

    // Phase 06 Task 6: Register bounds and draw animated focus ring
    FocusRingRenderer::get().register_item_bounds(
        FocusZoneId::kBreadcrumb, 0, this, GetClientRect());
    FocusRingRenderer::get().draw(dc, this, theme_engine());
}

void BreadcrumbBar::OnMouseMove(wxMouseEvent& event)
{
    if (drawn_segments_.empty())
    {
        event.Skip();
        return;
    }

    int previous_hover = hovered_segment_;
    hovered_segment_ = -1;

    wxPoint pos = event.GetPosition();
    for (size_t i = 0; i < drawn_segments_.size(); ++i)
    {
        if (drawn_segments_[i].bounds.Contains(pos))
        {
            hovered_segment_ = static_cast<int>(i);
            break;
        }
    }

    if (hovered_segment_ != previous_hover)
    {
        if (hovered_segment_ >= 0)
        {
            SetCursor(wxCursor(wxCURSOR_HAND));
            SetToolTip(wxString::FromUTF8(
                drawn_segments_[static_cast<size_t>(hovered_segment_)].full_path));
        }
        else
        {
            SetCursor(wxNullCursor);
            SetToolTip("");
        }
        Refresh();
    }

    event.Skip();
}

void BreadcrumbBar::OnLeaveWindow(wxMouseEvent& event)
{
    if (hovered_segment_ != -1)
    {
        hovered_segment_ = -1;
        SetCursor(wxNullCursor);
        SetToolTip("");
        Refresh();
    }
    event.Skip();
}

void BreadcrumbBar::OnMouseDown(wxMouseEvent& event)
{
    if (hovered_segment_ >= 0 && hovered_segment_ < static_cast<int>(drawn_segments_.size()))
    {
        if (segment_click_callback_)
        {
            segment_click_callback_(
                drawn_segments_[static_cast<size_t>(hovered_segment_)].full_path);
        }
    }
    else if (segment_click_callback_ && !file_segments_.empty())
    {
        // Fallback: click empty area -> navigate to full path
        std::string full_path;
        for (size_t idx = 0; idx < file_segments_.size(); ++idx)
        {
            if (idx > 0)
                full_path += "/";
            full_path += file_segments_[idx];
        }
        segment_click_callback_(full_path);
    }
    SetFocus();
    event.Skip();
}

} // namespace markamp::ui
