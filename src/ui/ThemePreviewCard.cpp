#include "ThemePreviewCard.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <cmath>

namespace markamp::ui
{

namespace
{
constexpr int kPadding = 12;
constexpr int kExportBtnSize = 24;
} // namespace

// ═══════════════════════════════════════════════════════
// Construction
// ═══════════════════════════════════════════════════════

ThemePreviewCard::ThemePreviewCard(wxWindow* parent,
                                   const core::Theme& theme,
                                   bool is_active,
                                   bool is_builtin)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(kCardWidth, kCardHeight))
    , theme_(theme)
    , is_active_(is_active)
    , is_builtin_(is_builtin)
{
    SetMinSize(wxSize(kCardWidth, kCardHeight));
    SetMaxSize(wxSize(kCardWidth, kCardHeight));
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &ThemePreviewCard::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemePreviewCard::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemePreviewCard::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &ThemePreviewCard::OnMouseDown, this);
}

// ═══════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════

void ThemePreviewCard::SetActive(bool active)
{
    is_active_ = active;
    Refresh();
}

auto ThemePreviewCard::IsActive() const -> bool
{
    return is_active_;
}

auto ThemePreviewCard::GetThemeId() const -> std::string
{
    return theme_.id;
}

auto ThemePreviewCard::GetThemeName() const -> std::string
{
    return theme_.name;
}

auto ThemePreviewCard::IsBuiltin() const -> bool
{
    return is_builtin_;
}

void ThemePreviewCard::SetOnClick(ClickCallback callback)
{
    on_click_ = std::move(callback);
}

void ThemePreviewCard::SetOnExport(ExportCallback callback)
{
    on_export_ = std::move(callback);
}

void ThemePreviewCard::SetOnDelete(DeleteCallback callback)
{
    on_delete_ = std::move(callback);
}

// ═══════════════════════════════════════════════════════
// Mouse handlers
// ═══════════════════════════════════════════════════════

void ThemePreviewCard::OnMouseEnter(wxMouseEvent& /*event*/)
{
    is_hovered_ = true;
    SetCursor(wxCursor(wxCURSOR_HAND));
    Refresh();
}

void ThemePreviewCard::OnMouseLeave(wxMouseEvent& /*event*/)
{
    is_hovered_ = false;
    SetCursor(wxNullCursor);
    Refresh();
}

void ThemePreviewCard::OnMouseDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();

    // Check delete button first (only for non-builtin themes)
    if (!is_builtin_ && delete_btn_rect_.Contains(pos))
    {
        if (on_delete_)
        {
            on_delete_(theme_.id);
        }
        return;
    }

    // Check export button
    if (export_btn_rect_.Contains(pos))
    {
        if (on_export_)
        {
            on_export_(theme_.id);
        }
        return;
    }

    // Card click — apply theme
    if (on_click_)
    {
        on_click_(theme_.id);
    }
}

// ═══════════════════════════════════════════════════════
// Painting
// ═══════════════════════════════════════════════════════

void ThemePreviewCard::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();

    // Clear with transparent
    dc.SetBackground(wxBrush(GetParent()->GetBackgroundColour()));
    dc.Clear();

    const auto& c = theme_.colors;

    // Card border
    wxColour border_color;
    if (is_active_)
    {
        border_color = wxColour(c.accent_primary.to_rgba_string());
    }
    else if (is_hovered_)
    {
        border_color = wxColour(c.border_light.to_rgba_string());
    }
    else
    {
        border_color = wxColour(c.border_dark.to_rgba_string());
    }

    // Draw card background with rounded rect
    dc.SetPen(wxPen(border_color, kBorderWidth));
    dc.SetBrush(wxBrush(wxColour(c.bg_app.to_rgba_string())));
    dc.DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), kCornerRadius);

    // Mini-preview area
    wxRect preview_area(kBorderWidth,
                        kBorderWidth,
                        size.GetWidth() - kBorderWidth * 2,
                        kPreviewHeight - kBorderWidth);
    DrawMiniPreview(dc, preview_area);

    // Footer area
    wxRect footer_area(kBorderWidth,
                       kPreviewHeight,
                       size.GetWidth() - kBorderWidth * 2,
                       kFooterHeight - kBorderWidth);
    DrawFooter(dc, footer_area);

    // Active indicator badge
    if (is_active_)
    {
        DrawActiveIndicator(dc);
    }
}

void ThemePreviewCard::DrawMiniPreview(wxDC& dc, const wxRect& area)
{
    const auto& c = theme_.colors;

    // Background
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxColour(c.bg_app.to_rgba_string())));
    dc.DrawRectangle(area);

    // ---- Header bar (top 18px) ----
    wxRect header_rect(area.GetLeft(), area.GetTop(), area.GetWidth(), 18);
    dc.SetBrush(wxBrush(wxColour(c.bg_header.to_rgba_string())));
    dc.DrawRectangle(header_rect);

    // Two colored dots in header
    int dot_y = header_rect.GetTop() + 7;
    dc.SetBrush(wxBrush(wxColour(c.accent_primary.to_rgba_string())));
    dc.DrawCircle(area.GetLeft() + 10, dot_y, 3);
    dc.SetBrush(wxBrush(wxColour(c.accent_secondary.to_rgba_string())));
    dc.DrawCircle(area.GetLeft() + 20, dot_y, 3);

    // Small text bar in header
    dc.SetBrush(wxBrush(wxColour(c.text_muted.to_rgba_string())));
    dc.DrawRectangle(area.GetLeft() + 32, dot_y - 2, 40, 4);

    // ---- Body (below header) ----
    int body_top = header_rect.GetBottom();
    int body_height = area.GetBottom() - body_top;

    // Sidebar (left 60px)
    int sidebar_width = 60;
    wxRect sidebar_rect(area.GetLeft(), body_top, sidebar_width, body_height);
    dc.SetBrush(wxBrush(wxColour(c.bg_panel.to_rgba_string())));
    dc.DrawRectangle(sidebar_rect);

    // Sidebar text bars
    dc.SetBrush(wxBrush(wxColour(c.text_muted.to_rgba_string())));
    dc.DrawRectangle(sidebar_rect.GetLeft() + 6, body_top + 8, 36, 3);
    dc.DrawRectangle(sidebar_rect.GetLeft() + 6, body_top + 16, 28, 3);
    dc.DrawRectangle(sidebar_rect.GetLeft() + 6, body_top + 24, 42, 3);

    // ---- Content area (right of sidebar) ----
    int content_left = area.GetLeft() + sidebar_width + 1;
    int content_width = area.GetWidth() - sidebar_width - 1;

    // Content background
    dc.SetBrush(wxBrush(wxColour(c.bg_input.to_rgba_string())));
    dc.DrawRectangle(content_left, body_top, content_width, body_height);

    // Heading line (accent_primary)
    dc.SetBrush(wxBrush(wxColour(c.accent_primary.to_rgba_string())));
    dc.DrawRectangle(content_left + 8, body_top + 8, 80, 4);

    // Body text lines (text_main)
    dc.SetBrush(wxBrush(wxColour(c.text_main.to_rgba_string())));
    dc.DrawRectangle(content_left + 8, body_top + 18, 120, 3);
    dc.DrawRectangle(content_left + 8, body_top + 26, 100, 3);
    dc.DrawRectangle(content_left + 8, body_top + 34, 110, 3);
    dc.DrawRectangle(content_left + 8, body_top + 42, 60, 3);

    // Second heading
    dc.SetBrush(wxBrush(wxColour(c.accent_primary.to_rgba_string())));
    dc.DrawRectangle(content_left + 8, body_top + 52, 70, 4);

    // More body lines
    dc.SetBrush(wxBrush(wxColour(c.text_main.to_rgba_string())));
    dc.DrawRectangle(content_left + 8, body_top + 62, 115, 3);
    dc.DrawRectangle(content_left + 8, body_top + 70, 90, 3);
}

void ThemePreviewCard::DrawFooter(wxDC& dc, const wxRect& area)
{
    const auto& c = theme_.colors;

    // Footer background
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxColour(c.bg_panel.to_rgba_string())));
    dc.DrawRectangle(area);

    // Top border line
    dc.SetPen(wxPen(wxColour(c.border_light.to_rgba_string()), 1));
    dc.DrawLine(area.GetLeft(), area.GetTop(), area.GetRight(), area.GetTop());

    // Theme name
    wxFont name_font(wxFontInfo(11).Family(wxFONTFAMILY_SWISS).Bold());
    dc.SetFont(name_font);
    if (is_active_)
    {
        dc.SetTextForeground(wxColour(c.accent_primary.to_rgba_string()));
    }
    else
    {
        dc.SetTextForeground(wxColour(c.text_main.to_rgba_string()));
    }
    dc.DrawText(theme_.name, area.GetLeft() + kPadding, area.GetTop() + 14);

    // Export button (download icon area)
    export_btn_rect_ = wxRect(area.GetRight() - kExportBtnSize - kPadding,
                              area.GetTop() + (area.GetHeight() - kExportBtnSize) / 2,
                              kExportBtnSize,
                              kExportBtnSize);
    DrawExportButton(dc, export_btn_rect_);

    // Delete button (trash icon, only visible for non-builtin themes on hover)
    if (!is_builtin_ && is_hovered_)
    {
        delete_btn_rect_ = wxRect(export_btn_rect_.GetLeft() - kExportBtnSize - 4,
                                  area.GetTop() + (area.GetHeight() - kExportBtnSize) / 2,
                                  kExportBtnSize,
                                  kExportBtnSize);
        DrawDeleteButton(dc, delete_btn_rect_);
    }
    else
    {
        delete_btn_rect_ = wxRect(0, 0, 0, 0);
    }
}

void ThemePreviewCard::DrawActiveIndicator(wxDC& dc)
{
    const auto& c = theme_.colors;

    // Badge in top-right corner
    int badge_x = GetClientSize().GetWidth() - kBadgeSize - 4;
    int badge_y = 4;

    // Filled accent circle
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxColour(c.accent_primary.to_rgba_string())));
    dc.DrawCircle(badge_x + kBadgeSize / 2, badge_y + kBadgeSize / 2, kBadgeSize / 2);

    // Checkmark (white lines)
    dc.SetPen(wxPen(wxColour(c.bg_app.to_rgba_string()), 2));
    int cx = badge_x + kBadgeSize / 2;
    int cy = badge_y + kBadgeSize / 2;
    // Checkmark: short line down-left, then long line down-right
    dc.DrawLine(cx - 4, cy, cx - 1, cy + 3);
    dc.DrawLine(cx - 1, cy + 3, cx + 4, cy - 3);
}

void ThemePreviewCard::DrawExportButton(wxDC& dc, const wxRect& area)
{
    const auto& c = theme_.colors;

    // Download arrow icon
    dc.SetPen(wxPen(wxColour(c.text_muted.to_rgba_string()), 1));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    int cx = area.GetLeft() + area.GetWidth() / 2;
    int cy = area.GetTop() + area.GetHeight() / 2;

    // Down arrow
    dc.DrawLine(cx, cy - 5, cx, cy + 3);         // Vertical line
    dc.DrawLine(cx - 3, cy, cx, cy + 3);         // Left arrowhead
    dc.DrawLine(cx + 3, cy, cx, cy + 3);         // Right arrowhead
    dc.DrawLine(cx - 5, cy + 5, cx + 5, cy + 5); // Base line
}

void ThemePreviewCard::DrawDeleteButton(wxDC& dc, const wxRect& area)
{
    const auto& c = theme_.colors;

    // Trash icon
    dc.SetPen(wxPen(wxColour(c.text_muted.to_rgba_string()), 1));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    int cx = area.GetLeft() + area.GetWidth() / 2;
    int cy = area.GetTop() + area.GetHeight() / 2;

    // Trash can body
    dc.DrawRectangle(cx - 4, cy - 2, 8, 9);
    // Lid
    dc.DrawLine(cx - 5, cy - 2, cx + 5, cy - 2);
    // Handle on lid
    dc.DrawLine(cx - 2, cy - 4, cx + 2, cy - 4);
    dc.DrawLine(cx - 2, cy - 4, cx - 2, cy - 2);
    dc.DrawLine(cx + 2, cy - 4, cx + 2, cy - 2);
    // Vertical lines inside
    dc.DrawLine(cx - 1, cy, cx - 1, cy + 5);
    dc.DrawLine(cx + 1, cy, cx + 1, cy + 5);
}

} // namespace markamp::ui
