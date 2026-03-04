#include "ThemedContextMenu.h"

#include "core/ThemeEngine.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"
#include "ui/TypographyScale.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

ThemedContextMenu::ThemedContextMenu(wxWindow* parent,
                                     DesignSystemContext& ds,
                                     core::ThemeEngine& te)
    : wxPopupTransientWindow(parent)
    , design_system_(ds)
    , theme_engine_(te)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &ThemedContextMenu::OnPaint, this);
    Bind(wxEVT_MOTION, &ThemedContextMenu::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedContextMenu::OnMouseDown, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedContextMenu::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &ThemedContextMenu::OnKeyDown, this);
}

// ═══════════════════════════════════════════════════════
// Static entry point
// ═══════════════════════════════════════════════════════

void ThemedContextMenu::Show(wxWindow* parent,
                             const wxPoint& screen_pos,
                             const ContextMenuModel& model,
                             const std::unordered_map<std::string, ActionCallback>& callbacks,
                             DesignSystemContext& ds,
                             core::ThemeEngine& te)
{
    auto* menu = new ThemedContextMenu(parent, ds, te);
    menu->SetModel(model);
    menu->SetCallbacks(callbacks);
    menu->ShowAt(screen_pos);
}

// ═══════════════════════════════════════════════════════
// Model / Callbacks
// ═══════════════════════════════════════════════════════

void ThemedContextMenu::SetModel(const ContextMenuModel& model)
{
    rows_.clear();
    auto items = model.visible_items();
    rows_.reserve(items.size());
    for (auto& menu_item : items)
    {
        MenuRow row;
        row.item = std::move(menu_item);
        rows_.push_back(std::move(row));
    }
    RecalculateLayout();
}

void ThemedContextMenu::SetCallbacks(
    const std::unordered_map<std::string, ActionCallback>& callbacks)
{
    callbacks_ = callbacks;
}

// ═══════════════════════════════════════════════════════
// Layout
// ═══════════════════════════════════════════════════════

void ThemedContextMenu::RecalculateLayout()
{
    // Measure text widths to determine menu width
    wxClientDC temp_dc(this);
    auto label_font = design_system_.typography.font(TypeSlot::kBody);
    auto shortcut_font = design_system_.typography.font(TypeSlot::kCaption);
    temp_dc.SetFont(label_font);

    int max_label_width = 0;
    int max_shortcut_width = 0;

    for (const auto& row : rows_)
    {
        if (row.item.is_separator)
        {
            continue;
        }
        auto label_extent = temp_dc.GetTextExtent(row.item.label);
        max_label_width = std::max(max_label_width, label_extent.GetWidth());

        if (!row.item.shortcut.empty())
        {
            temp_dc.SetFont(shortcut_font);
            auto sc_extent = temp_dc.GetTextExtent(row.item.shortcut);
            max_shortcut_width = std::max(max_shortcut_width, sc_extent.GetWidth());
            temp_dc.SetFont(label_font);
        }
    }

    menu_width_ = kPadH + kIconColumnWidth + max_label_width + kShortcutGap + max_shortcut_width +
                  kSubmenuArrowWidth + kPadH;
    menu_width_ = std::clamp(menu_width_, kMinWidth, kMaxWidth);

    // Lay out row rects
    int current_y = kPadV;
    for (auto& row : rows_)
    {
        int item_height = row.item.is_separator ? kSeparatorHeight : kItemHeight;
        row.rect = wxRect(0, current_y, menu_width_, item_height);
        current_y += item_height;
    }

    int total_height = current_y + kPadV;
    SetSize(wxSize(menu_width_, total_height));
}

void ThemedContextMenu::ShowAt(const wxPoint& screen_pos)
{
    Position(screen_pos, wxSize(0, 0));
    Popup();
    SetFocus();
}

// ═══════════════════════════════════════════════════════
// Painting
// ═══════════════════════════════════════════════════════

void ThemedContextMenu::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    const auto& current_theme = theme_engine_.current_theme();

    auto bg_color = wxColour(current_theme.colors.bg_panel.to_rgba_string());
    paint_dc.SetBackground(wxBrush(bg_color));
    paint_dc.Clear();

    std::unique_ptr<wxGraphicsContext> graphics_ctx(wxGraphicsContext::Create(paint_dc));
    if (graphics_ctx == nullptr)
    {
        return;
    }

    // Draw rounded background
    auto border_color = wxColour(current_theme.colors.border_light.to_rgba_string());
    graphics_ctx->SetBrush(graphics_ctx->CreateBrush(wxBrush(bg_color)));
    graphics_ctx->SetPen(graphics_ctx->CreatePen(wxGraphicsPenInfo(border_color).Width(1.0)));
    graphics_ctx->DrawRoundedRectangle(
        0, 0, GetClientSize().GetWidth(), GetClientSize().GetHeight(), kCornerRadius);

    // Draw each row
    for (const auto& row : rows_)
    {
        if (row.item.is_separator)
        {
            DrawSeparator(*graphics_ctx, row.rect, current_theme);
        }
        else
        {
            DrawMenuItem(*graphics_ctx, row, current_theme);
        }
    }
}

void ThemedContextMenu::DrawMenuItem(wxGraphicsContext& gc,
                                     const MenuRow& row,
                                     const core::Theme& current_theme) const
{
    const auto& colors = current_theme.colors;
    const auto& menu_item = row.item;

    // Hover highlight
    if (row.is_hovered && menu_item.is_enabled)
    {
        auto highlight_color = wxColour(colors.accent_primary.with_alpha(0.12F).to_rgba_string());
        gc.SetBrush(gc.CreateBrush(wxBrush(highlight_color)));
        gc.SetPen(*wxTRANSPARENT_PEN);
        gc.DrawRoundedRectangle(
            row.rect.GetX() + 4, row.rect.GetY(), row.rect.GetWidth() - 8, row.rect.GetHeight(), 4);
    }

    // Text color
    wxColour text_col;
    if (!menu_item.is_enabled)
    {
        text_col = wxColour(colors.text_muted.to_rgba_string());
    }
    else
    {
        text_col = wxColour(colors.text_main.to_rgba_string());
    }

    double text_y = row.rect.GetY() + (row.rect.GetHeight() - 14) / 2.0;

    // Checkmark or icon column
    double icon_col_x = row.rect.GetX() + kPadH;
    if (menu_item.is_checked)
    {
        DrawCheckmark(gc, icon_col_x + 4, text_y + 2, 10.0, text_col);
    }
    else if (!menu_item.icon_name.empty())
    {
        auto bmp =
            IconManager::get().get_icon_bitmap(menu_item.icon_name, wxSize(14, 14), text_col);
        gc.DrawBitmap(bmp, icon_col_x + 2, text_y, 14, 14);
    }

    // Label
    auto label_font = design_system_.typography.font(TypeSlot::kBody);
    gc.SetFont(label_font, text_col);
    double label_x = icon_col_x + kIconColumnWidth;
    gc.DrawText(menu_item.label, label_x, text_y);

    // Shortcut (right-aligned)
    if (!menu_item.shortcut.empty())
    {
        auto shortcut_col = wxColour(colors.text_muted.to_rgba_string());
        auto shortcut_font = design_system_.typography.font(TypeSlot::kCaption);
        gc.SetFont(shortcut_font, shortcut_col);
        wxDouble shortcut_width = 0;
        wxDouble shortcut_height = 0;
        gc.GetTextExtent(menu_item.shortcut, &shortcut_width, &shortcut_height);
        double shortcut_x = row.rect.GetRight() - kPadH - kSubmenuArrowWidth - shortcut_width;
        gc.DrawText(menu_item.shortcut, shortcut_x, text_y + 1);
    }

    // Sub-menu arrow
    if (menu_item.has_submenu)
    {
        double arrow_x = row.rect.GetRight() - kPadH - 8;
        double arrow_y = row.rect.GetY() + (row.rect.GetHeight() - 8) / 2.0;
        DrawSubmenuArrow(gc, arrow_x, arrow_y, 8.0, text_col);
    }
}

void ThemedContextMenu::DrawSeparator(wxGraphicsContext& gc,
                                      const wxRect& rect,
                                      const core::Theme& current_theme) const
{
    auto sep_color = wxColour(current_theme.colors.border_light.to_rgba_string());
    gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(sep_color).Width(1.0)));
    int sep_y = rect.GetY() + rect.GetHeight() / 2;
    gc.StrokeLine(rect.GetX() + kPadH + kIconColumnWidth, sep_y, rect.GetRight() - kPadH, sep_y);
}

void ThemedContextMenu::DrawCheckmark(
    wxGraphicsContext& gc, double pos_x, double pos_y, double size, const wxColour& color) const
{
    gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(color).Width(1.5)));
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
    auto path = gc.CreatePath();
    path.MoveToPoint(pos_x, pos_y + size * 0.5);
    path.AddLineToPoint(pos_x + size * 0.35, pos_y + size * 0.8);
    path.AddLineToPoint(pos_x + size, pos_y + size * 0.15);
    gc.StrokePath(path);
}

void ThemedContextMenu::DrawSubmenuArrow(
    wxGraphicsContext& gc, double pos_x, double pos_y, double size, const wxColour& color) const
{
    gc.SetPen(gc.CreatePen(wxGraphicsPenInfo(color).Width(1.5)));
    gc.SetBrush(*wxTRANSPARENT_BRUSH);
    auto path = gc.CreatePath();
    path.MoveToPoint(pos_x, pos_y);
    path.AddLineToPoint(pos_x + size * 0.6, pos_y + size * 0.5);
    path.AddLineToPoint(pos_x, pos_y + size);
    gc.StrokePath(path);
}

// ═══════════════════════════════════════════════════════
// Mouse interaction
// ═══════════════════════════════════════════════════════

void ThemedContextMenu::OnMouseMove(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();
    bool changed = false;

    for (int idx = 0; idx < static_cast<int>(rows_.size()); ++idx)
    {
        bool was_hovered = rows_[static_cast<size_t>(idx)].is_hovered;
        bool now_hovered = rows_[static_cast<size_t>(idx)].rect.Contains(pos) &&
                           !rows_[static_cast<size_t>(idx)].item.is_separator;
        rows_[static_cast<size_t>(idx)].is_hovered = now_hovered;
        if (was_hovered != now_hovered)
        {
            changed = true;
            if (now_hovered)
            {
                hovered_index_ = idx;
            }
        }
    }

    SetCursor(hovered_index_ >= 0 ? wxCursor(wxCURSOR_HAND) : wxNullCursor);
    if (changed)
    {
        Refresh();
    }
}

void ThemedContextMenu::OnMouseDown(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();

    for (const auto& row : rows_)
    {
        if (row.rect.Contains(pos) && !row.item.is_separator && row.item.is_enabled)
        {
            if (row.item.has_submenu)
            {
                ShowSubmenuAt(row.item, row.rect);
            }
            else
            {
                InvokeAction(row.item.action_id);
                Dismiss();
            }
            return;
        }
    }
}

void ThemedContextMenu::OnMouseLeave(wxMouseEvent& /*event*/)
{
    for (auto& row : rows_)
    {
        row.is_hovered = false;
    }
    hovered_index_ = -1;
    SetCursor(wxNullCursor);
    Refresh();
}

// ═══════════════════════════════════════════════════════
// Keyboard navigation
// ═══════════════════════════════════════════════════════

void ThemedContextMenu::OnKeyDown(wxKeyEvent& event)
{
    int key = event.GetKeyCode();

    auto move_selection = [this](int direction)
    {
        if (rows_.empty())
        {
            return;
        }

        // Clear old hover
        if (hovered_index_ >= 0 && hovered_index_ < static_cast<int>(rows_.size()))
        {
            rows_[static_cast<size_t>(hovered_index_)].is_hovered = false;
        }

        // Find next non-separator item
        int start = hovered_index_;
        int next = start;
        for (int attempt = 0; attempt < static_cast<int>(rows_.size()); ++attempt)
        {
            next += direction;
            if (next < 0)
            {
                next = static_cast<int>(rows_.size()) - 1;
            }
            else if (next >= static_cast<int>(rows_.size()))
            {
                next = 0;
            }
            if (!rows_[static_cast<size_t>(next)].item.is_separator)
            {
                break;
            }
        }

        hovered_index_ = next;
        rows_[static_cast<size_t>(hovered_index_)].is_hovered = true;
        Refresh();
    };

    switch (key)
    {
        case WXK_UP:
            move_selection(-1);
            break;
        case WXK_DOWN:
            move_selection(1);
            break;
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
            if (hovered_index_ >= 0 && hovered_index_ < static_cast<int>(rows_.size()))
            {
                const auto& row = rows_[static_cast<size_t>(hovered_index_)];
                if (row.item.is_enabled && !row.item.is_separator)
                {
                    if (row.item.has_submenu)
                    {
                        ShowSubmenuAt(row.item, row.rect);
                    }
                    else
                    {
                        InvokeAction(row.item.action_id);
                        Dismiss();
                    }
                }
            }
            break;
        case WXK_RIGHT:
            if (hovered_index_ >= 0 && hovered_index_ < static_cast<int>(rows_.size()))
            {
                const auto& row = rows_[static_cast<size_t>(hovered_index_)];
                if (row.item.has_submenu)
                {
                    ShowSubmenuAt(row.item, row.rect);
                }
            }
            break;
        case WXK_ESCAPE:
        case WXK_LEFT:
            Dismiss();
            break;
        default:
            event.Skip();
            break;
    }
}

// ═══════════════════════════════════════════════════════
// Action dispatch
// ═══════════════════════════════════════════════════════

void ThemedContextMenu::InvokeAction(const std::string& action_id)
{
    auto cb_iter = callbacks_.find(action_id);
    if (cb_iter != callbacks_.end())
    {
        cb_iter->second();
    }
}

void ThemedContextMenu::ShowSubmenuAt(const ContextMenuItem& parent_item, const wxRect& parent_rect)
{
    if (parent_item.children.empty())
    {
        return;
    }

    // Build a model from children
    ContextMenuModel sub_model;
    sub_model.set_items(parent_item.children);

    wxPoint sub_pos = ClientToScreen(wxPoint(parent_rect.GetRight(), parent_rect.GetY()));
    Show(GetParent(), sub_pos, sub_model, callbacks_, design_system_, theme_engine_);
}

} // namespace markamp::ui
