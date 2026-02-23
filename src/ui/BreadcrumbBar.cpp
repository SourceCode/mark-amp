#include "BreadcrumbBar.h"

#include "LayoutMetrics.h"
#include "TypographyScale.h"
#include "accessibility/AccessibilityController.h"
#include "core/Logger.h"
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

BreadcrumbBar::BreadcrumbBar(wxWindow* parent, DesignSystemContext& design_system)
    : ThemeAwareWindow(parent,
                       design_system.theme,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxSize(-1, design_system.metrics.row_height()),
                       wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE)
    , ds_(design_system)
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
                             {
                                 full_path += "/";
                             }
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
    wxAutoBufferedPaintDC paint_dc(this);

    const auto kClientSize = GetClientSize();
    const int kWidth = kClientSize.GetWidth();
    const int kHeight = kClientSize.GetHeight();

    const auto kBgColor = theme_engine().color(core::ThemeColorToken::BgPanel);
    paint_dc.SetBrush(wxBrush(kBgColor));
    paint_dc.SetPen(*wxTRANSPARENT_PEN);
    paint_dc.DrawRectangle(0, 0, kWidth, kHeight);

    if (file_segments_.empty() && heading_segments_.empty() && traversal_segments_.empty())
    {
        return;
    }

    auto font = ds_.typography.font(TypeSlot::kCaption);
    auto bold_font = font;
    bold_font.SetWeight(wxFONTWEIGHT_SEMIBOLD);

    int current_x = ds_.spacing.scaled(SpacingToken::kMd);
    const int kTextY = (kHeight - paint_dc.GetCharHeight()) / 2 + 1;
    const int kIconSize = 14;
    const int kIconY = (kHeight - kIconSize) / 2;

    const auto kTextColorNormal = theme_engine().color(core::ThemeColorToken::TextMuted);
    const auto kTextColorHover = theme_engine().color(core::ThemeColorToken::TextMain);
    const auto kSeparatorColor = theme_engine().color(core::ThemeColorToken::BorderLight);

    const bool kRebuildBounds = drawn_segments_.empty();
    int segment_index = 0;
    std::string full_path;

    auto draw_separator = [&]()
    {
        IconManager::get().draw_icon(paint_dc,
                                     "chevron-right",
                                     current_x + 2,
                                     kIconY,
                                     wxSize(kIconSize, kIconSize),
                                     kSeparatorColor);
        current_x += kIconSize + 4;
    };

    auto draw_segment = [&](const std::string& text,
                            const std::string& icon_name,
                            bool is_last_file,
                            const std::string& segment_path)
    {
        const int kStartX = current_x;

        wxColour fg_color = kTextColorNormal;
        if (hovered_segment_ == segment_index)
        {
            fg_color = kTextColorHover;
        }

        if (!icon_name.empty())
        {
            IconManager::get().draw_icon(paint_dc,
                                         icon_name,
                                         current_x,
                                         kIconY,
                                         wxSize(kIconSize, kIconSize),
                                         fg_color,
                                         GetDPIScaleFactor());
            current_x += kIconSize + 4;
        }

        paint_dc.SetFont(is_last_file ? bold_font : font);
        paint_dc.SetTextForeground(fg_color);
        const wxString kWxText = wxString::FromUTF8(text);
        paint_dc.DrawText(kWxText, current_x, kTextY);
        current_x += paint_dc.GetTextExtent(kWxText).GetWidth();

        if (kRebuildBounds)
        {
            DrawnSegment segment_info;
            segment_info.bounds = wxRect(kStartX, 0, current_x - kStartX, kHeight);
            segment_info.full_path = segment_path;
            drawn_segments_.push_back(segment_info);
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

        std::string icon;
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
        {
            heading_path += " -> ";
        }
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

        const std::string kLabel =
            traversal_segments_[idx].surface_label + ": " + traversal_segments_[idx].anchor_label;
        draw_segment(kLabel, "link", false, kLabel);
    }

    for (size_t i = 0; i < drawn_segments_.size(); ++i)
    {
        FocusRingRenderer::get().register_item_bounds(
            FocusZoneId::kBreadcrumb, static_cast<int>(i), this, drawn_segments_[i].bounds);
    }

    FocusRingRenderer::get().draw(paint_dc, this, theme_engine());
}

void BreadcrumbBar::OnMouseMove(wxMouseEvent& event)
{
    if (drawn_segments_.empty())
    {
        event.Skip();
        return;
    }

    const int kPreviousHover = hovered_segment_;
    hovered_segment_ = -1;

    const wxPoint kPos = event.GetPosition();
    for (size_t i = 0; i < drawn_segments_.size(); ++i)
    {
        if (drawn_segments_[i].bounds.Contains(kPos))
        {
            hovered_segment_ = static_cast<int>(i);
            break;
        }
    }

    if (hovered_segment_ != kPreviousHover)
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

void BreadcrumbBar::OnSetFocus(wxFocusEvent& event)
{
    is_focused_ = true;
    if (focused_segment_index_ < 0 && !drawn_segments_.empty())
    {
        focused_segment_index_ =
            static_cast<int>(drawn_segments_.size() - 1); // default to last item (file name)
    }

    if (focused_segment_index_ >= 0 &&
        focused_segment_index_ < static_cast<int>(drawn_segments_.size()))
    {
        accessibility::AccessibilityController::get().announce_focus(
            drawn_segments_[static_cast<std::size_t>(focused_segment_index_)].full_path,
            "Breadcrumb Segment",
            "Selected");
    }

    Refresh();
    event.Skip();
}

void BreadcrumbBar::OnKillFocus(wxFocusEvent& event)
{
    is_focused_ = false;
    Refresh();
    event.Skip();
}

void BreadcrumbBar::OnKeyDown(wxKeyEvent& event)
{
    if (!is_focused_ || drawn_segments_.empty())
    {
        event.Skip();
        return;
    }

    const int kKeyCode = event.GetKeyCode();
    const int kCount = static_cast<int>(drawn_segments_.size());

    bool handled = false;

    if (kKeyCode == WXK_LEFT)
    {
        focused_segment_index_--;
        if (focused_segment_index_ < 0)
        {
            focused_segment_index_ = kCount - 1;
        }
        handled = true;
    }
    else if (kKeyCode == WXK_RIGHT)
    {
        focused_segment_index_++;
        if (focused_segment_index_ >= kCount)
        {
            focused_segment_index_ = 0;
        }
        handled = true;
    }
    else if (kKeyCode == WXK_RETURN || kKeyCode == WXK_SPACE)
    {
        if (focused_segment_index_ >= 0 && focused_segment_index_ < kCount)
        {
            const auto& seg = drawn_segments_[static_cast<std::size_t>(focused_segment_index_)];
            MARKAMP_LOG_DEBUG("Breadcrumb keyboard activated: {}", seg.full_path);

            if (segment_click_callback_)
            {
                segment_click_callback_(seg.full_path);
            }
        }
        handled = true;
    }

    if (handled)
    {
        if (kKeyCode == WXK_LEFT || kKeyCode == WXK_RIGHT)
        {
            accessibility::AccessibilityController::get().announce_focus(
                drawn_segments_[static_cast<std::size_t>(focused_segment_index_)].full_path,
                "Breadcrumb Segment",
                "Selected");
        }
        Refresh();
    }
    else
    {
        event.Skip();
    }
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
            {
                full_path += "/";
            }
            full_path += file_segments_[idx];
        }
        segment_click_callback_(full_path);
    }
    SetFocus();
    event.Skip();
}

} // namespace markamp::ui
