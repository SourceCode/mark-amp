#include "PanelHeaderBar.h"

#include "SpacingGrid.h"
#include "core/Events.h"
#include "ui/DesignSystemContext.h"
#include "ui/IconManager.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

namespace markamp::ui
{

PanelHeaderBar::PanelHeaderBar(wxWindow* parent,
                               DesignSystemContext& design_system,
                               core::EventBus& event_bus)
    : ThemeAwareWindow(
          parent, design_system.theme, wxID_ANY, wxDefaultPosition, wxSize(-1, kBarHeight))
    , ds_(design_system)
    , event_bus_(event_bus)
{
    SetMinSize(wxSize(-1, kBarHeight));
    SetMaxSize(wxSize(-1, kBarHeight));
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &PanelHeaderBar::OnPaint, this);
    Bind(wxEVT_MOTION, &PanelHeaderBar::OnMouseMove, this);
    Bind(wxEVT_LEFT_DOWN, &PanelHeaderBar::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &PanelHeaderBar::OnMouseUp, this);
    Bind(wxEVT_LEAVE_WINDOW, &PanelHeaderBar::OnMouseLeave, this);
}

void PanelHeaderBar::set_actions(const std::vector<PanelHeaderAction>& actions)
{
    actions_ = actions;
    RecalculateLayout();
    Refresh();
}

void PanelHeaderBar::update_action(const std::string& action_id,
                                   bool is_enabled,
                                   bool is_toggled_on)
{
    for (auto& action : actions_)
    {
        if (action.action_id == action_id)
        {
            action.is_enabled = is_enabled;
            action.is_toggled_on = is_toggled_on;
            Refresh();
            return;
        }
    }
}

void PanelHeaderBar::set_on_action(PanelActionCallback callback)
{
    on_action_ = std::move(callback);
}

auto PanelHeaderBar::actions() const -> const std::vector<PanelHeaderAction>&
{
    return actions_;
}

auto PanelHeaderBar::preferred_width() const -> int
{
    int total_width = 0;
    for (const auto& action : actions_)
    {
        if (action.separator_before)
        {
            total_width += kSeparatorWidth;
        }
        total_width += kActionSize + kActionGap;
    }
    return total_width;
}

auto PanelHeaderBar::has_overflow() const -> bool
{
    return overflow_;
}

void PanelHeaderBar::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    Refresh();
}

void PanelHeaderBar::RecalculateLayout()
{
    action_rects_.clear();
    const int available_width = GetClientSize().GetWidth();
    int current_x = kActionGap;

    visible_action_count_ = 0;
    overflow_ = false;

    for (const auto& action : actions_)
    {
        if (action.separator_before)
        {
            current_x += kSeparatorWidth;
        }

        const int next_end = current_x + kActionSize;
        if (next_end > available_width && available_width > 0)
        {
            overflow_ = true;
            break;
        }

        ActionRect action_rect;
        action_rect.action_id = action.action_id;
        const int vertical_offset = (kBarHeight - kActionSize) / 2;
        action_rect.rect = wxRect(current_x, vertical_offset, kActionSize, kActionSize);
        action_rects_.push_back(action_rect);

        current_x += kActionSize + kActionGap;
        ++visible_action_count_;
    }
}

void PanelHeaderBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(this);
    const auto& current_theme = theme();

    // Transparent background (inherits from parent)
    paint_dc.SetBackground(wxBrush(wxColour(current_theme.colors.bg_panel.to_rgba_string())));
    paint_dc.Clear();

    std::unique_ptr<wxGraphicsContext> gc_owner(wxGraphicsContext::Create(paint_dc));
    if (gc_owner == nullptr)
    {
        return;
    }
    auto& graphics_ctx = *gc_owner;

    // Draw each visible action
    for (size_t idx = 0; idx < action_rects_.size() && idx < actions_.size(); ++idx)
    {
        const auto& action = actions_[idx];
        const auto& action_rect = action_rects_[idx];

        // Draw separator if needed
        if (action.separator_before && action_rect.rect.GetX() > kActionGap + kSeparatorWidth)
        {
            const auto sep_color = wxColour(current_theme.colors.border_light.to_rgba_string());
            graphics_ctx.SetPen(graphics_ctx.CreatePen(wxGraphicsPenInfo(sep_color).Width(1.0)));
            const int sep_x = action_rect.rect.GetX() - kSeparatorWidth / 2;
            graphics_ctx.StrokeLine(sep_x, 4, sep_x, kBarHeight - 4);
        }

        DrawActionButton(graphics_ctx,
                         action,
                         action_rect.rect,
                         action_rect.is_hovered,
                         action_rect.is_pressed,
                         current_theme);
    }

    // Overflow indicator
    if (overflow_)
    {
        const auto muted = wxColour(current_theme.colors.text_muted.to_rgba_string());
        graphics_ctx.SetPen(graphics_ctx.CreatePen(wxGraphicsPenInfo(muted).Width(1.5)));
        graphics_ctx.SetBrush(*wxTRANSPARENT_BRUSH);

        const int overflow_x = GetClientSize().GetWidth() - 12;
        const int overflow_y = kBarHeight / 2;
        // Three dots
        for (int dot_idx = 0; dot_idx < 3; ++dot_idx)
        {
            const double dot_x = overflow_x + dot_idx * 3.0;
            graphics_ctx.DrawEllipse(dot_x, overflow_y - 1, 2, 2);
        }
    }
}

void PanelHeaderBar::DrawActionButton(wxGraphicsContext& graphics_ctx,
                                      const PanelHeaderAction& action,
                                      const wxRect& rect,
                                      bool is_hovered,
                                      bool is_pressed,
                                      const core::Theme& current_theme) const
{
    const auto& colors = current_theme.colors;

    // Background
    if (is_pressed && action.is_enabled)
    {
        const auto bg_str = colors.accent_primary.with_alpha(0.15F).to_rgba_string();
        graphics_ctx.SetBrush(graphics_ctx.CreateBrush(wxBrush(wxColour(bg_str))));
        graphics_ctx.SetPen(*wxTRANSPARENT_PEN);
        graphics_ctx.DrawRoundedRectangle(
            rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), 3.0);
    }
    else if (is_hovered && action.is_enabled)
    {
        const auto bg_str = colors.text_main.with_alpha(0.08F).to_rgba_string();
        graphics_ctx.SetBrush(graphics_ctx.CreateBrush(wxBrush(wxColour(bg_str))));
        graphics_ctx.SetPen(*wxTRANSPARENT_PEN);
        graphics_ctx.DrawRoundedRectangle(
            rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), 3.0);
    }
    else if (action.is_toggle && action.is_toggled_on)
    {
        const auto bg_str = colors.accent_primary.with_alpha(0.12F).to_rgba_string();
        graphics_ctx.SetBrush(graphics_ctx.CreateBrush(wxBrush(wxColour(bg_str))));
        graphics_ctx.SetPen(*wxTRANSPARENT_PEN);
        graphics_ctx.DrawRoundedRectangle(
            rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), 3.0);
    }

    // Icon color
    wxColour icon_color;
    if (!action.is_enabled)
    {
        const auto muted = colors.text_muted;
        icon_color = wxColour(muted.r, muted.g, muted.b, 80);
    }
    else if (action.is_toggle && action.is_toggled_on)
    {
        icon_color = wxColour(colors.accent_primary.to_rgba_string());
    }
    else if (is_hovered)
    {
        icon_color = wxColour(colors.text_main.to_rgba_string());
    }
    else
    {
        icon_color = wxColour(colors.text_muted.to_rgba_string());
    }

    // Draw icon
    constexpr int kIconPad = 3;
    const int icon_size = kActionSize - 2 * kIconPad;
    if (!action.icon_name.empty() && icon_size > 0)
    {
        auto bmp = IconManager::get().get_icon_bitmap(
            action.icon_name, wxSize(icon_size, icon_size), icon_color);
        graphics_ctx.DrawBitmap(
            bmp, rect.GetX() + kIconPad, rect.GetY() + kIconPad, icon_size, icon_size);
    }
}

void PanelHeaderBar::OnMouseMove(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();
    bool changed = false;

    for (auto& action_rect : action_rects_)
    {
        const bool was_hovered = action_rect.is_hovered;
        action_rect.is_hovered = action_rect.rect.Contains(pos);
        if (was_hovered != action_rect.is_hovered)
        {
            changed = true;
        }
    }

    SetCursor(changed ? wxCursor(wxCURSOR_HAND) : wxNullCursor);
    if (changed)
    {
        Refresh();
    }
}

void PanelHeaderBar::OnMouseDown(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();

    for (auto& action_rect : action_rects_)
    {
        if (action_rect.rect.Contains(pos))
        {
            action_rect.is_pressed = true;
            Refresh();
            return;
        }
    }
}

void PanelHeaderBar::OnMouseUp(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();

    for (auto& action_rect : action_rects_)
    {
        if (action_rect.is_pressed && action_rect.rect.Contains(pos))
        {
            // Find the action to check if it's enabled
            for (auto& action : actions_)
            {
                if (action.action_id == action_rect.action_id && action.is_enabled)
                {
                    // Toggle if applicable
                    if (action.is_toggle)
                    {
                        action.is_toggled_on = !action.is_toggled_on;
                    }

                    if (on_action_)
                    {
                        on_action_(action.action_id);
                    }
                    break;
                }
            }
        }
        action_rect.is_pressed = false;
    }
    Refresh();
}

void PanelHeaderBar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    for (auto& action_rect : action_rects_)
    {
        action_rect.is_hovered = false;
        action_rect.is_pressed = false;
    }
    SetCursor(wxNullCursor);
    Refresh();
}

} // namespace markamp::ui
