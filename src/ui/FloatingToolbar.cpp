#include "FloatingToolbar.h"

#include "core/Events.h"

#include <wx/dcbuffer.h>

#include <algorithm>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(FloatingToolbar, wxPopupTransientWindow) // NOLINT
    EVT_PAINT(FloatingToolbar::OnPaint) EVT_LEFT_DOWN(FloatingToolbar::OnMouseDown)
        EVT_LEFT_UP(FloatingToolbar::OnMouseUp) EVT_MOTION(FloatingToolbar::OnMouseMove)
            EVT_LEAVE_WINDOW(FloatingToolbar::OnMouseLeave) EVT_KEY_DOWN(FloatingToolbar::OnKeyDown)
                wxEND_EVENT_TABLE() // NOLINT

    FloatingToolbar::FloatingToolbar(wxWindow* parent,
                                     core::ThemeEngine& theme_engine,
                                     core::EventBus& event_bus,
                                     const std::string& toolbar_id)
    : wxPopupTransientWindow(parent, wxBORDER_NONE)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , toolbar_id_(toolbar_id)
    , theme_sub_(theme_engine_.subscribe_theme_change([this](const std::string& /*theme_id*/)
                                                      { ApplyTheme(); }))
    , auto_hide_timer_(this)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(
        wxEVT_TIMER, [this](wxTimerEvent& evt) { OnAutoHideTimer(evt); }, auto_hide_timer_.GetId());
}

void FloatingToolbar::ShowAt(const wxPoint& position)
{
    last_position_ = position;
    SetSize(CalculateSize());
    SetPosition(position);
    Popup();
    visible_ = true;
    ResetAutoHideTimer();
    SetFocus();
}

void FloatingToolbar::ShowAtScreenCenter()
{
    const auto parent_size = GetParent()->GetClientSize();
    const auto toolbar_size = CalculateSize();
    const int center_x = (parent_size.x - toolbar_size.x) / 2;
    ShowAt(GetParent()->ClientToScreen(wxPoint(center_x, 8)));
}

void FloatingToolbar::HideToolbar()
{
    auto_hide_timer_.Stop();
    visible_ = false;
    Dismiss();
}

void FloatingToolbar::SetAutoHideMs(int delay_ms)
{
    auto_hide_ms_ = delay_ms;
}

void FloatingToolbar::SetDraggable(bool draggable)
{
    draggable_ = draggable;
}

void FloatingToolbar::SetSnapToEdges(bool snap)
{
    snap_to_edges_ = snap;
}

auto FloatingToolbar::toolbar_id() const -> const std::string&
{
    return toolbar_id_;
}

auto FloatingToolbar::is_visible() const -> bool
{
    return visible_;
}

auto FloatingToolbar::last_position() const -> wxPoint
{
    return last_position_;
}

auto FloatingToolbar::button_count() const -> int
{
    return static_cast<int>(buttons_.size());
}

auto FloatingToolbar::is_draggable() const -> bool
{
    return draggable_;
}

auto FloatingToolbar::has_separator() const -> bool
{
    return std::any_of(
        buttons_.begin(), buttons_.end(), [](const FloatingToolbarButton& btn) { return btn.is_separator; });
}

auto FloatingToolbar::enabled_count() const -> int
{
    int count = 0;
    for (const auto& btn : buttons_)
    {
        if (!btn.is_separator && (!btn.is_enabled || btn.is_enabled()))
        {
            ++count;
        }
    }
    return count;
}

auto FloatingToolbar::toggled_count() const -> int
{
    int count = 0;
    for (const auto& btn : buttons_)
    {
        if (btn.is_toggled && btn.is_toggled())
        {
            ++count;
        }
    }
    return count;
}

void FloatingToolbar::SetButtons(std::vector<FloatingToolbarButton> buttons)
{
    buttons_ = std::move(buttons);
    hovered_button_ = -1;
    pressed_button_ = -1;
    focused_button_ = -1;
    SetSize(CalculateSize());
    Refresh();
}

void FloatingToolbar::RefreshButtonStates()
{
    Refresh();
}

// ── Rendering ────────────────────────────────────────────────────────────────

void FloatingToolbar::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC dc(this);
    const auto size = GetSize();

    const auto& bg = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto& border = theme_engine_.color(core::ThemeColorToken::BorderLight);
    const auto& text_fg = theme_engine_.color(core::ThemeColorToken::TextMain);
    const auto& muted_fg = theme_engine_.color(core::ThemeColorToken::TextMuted);
    const auto& hover_bg = theme_engine_.color(core::ThemeColorToken::HoverBg);
    const auto& selected_bg = theme_engine_.color(core::ThemeColorToken::SelectionBg);
    const auto& accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
    const auto& disabled_fg = theme_engine_.color(core::ThemeColorToken::ControlFgDisabled);

    // Shadow
    dc.SetBrush(wxBrush(wxColour(0, 0, 0, 30)));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRoundedRectangle(kShadowOffset, kShadowOffset, size.x, size.y, kBorderRadius);

    // Background
    dc.SetBrush(wxBrush(bg));
    dc.SetPen(wxPen(border));
    dc.DrawRoundedRectangle(0, 0, size.x - kShadowOffset, size.y - kShadowOffset, kBorderRadius);

    // Drag handle
    if (draggable_)
    {
        const int handle_x = 2;
        const int handle_h = size.y - kShadowOffset - 4;
        dc.SetBrush(wxBrush(muted_fg));
        dc.SetPen(*wxTRANSPARENT_PEN);

        const int dot_size = 2;
        const int dot_spacing = 5;
        const int dot_x = handle_x + kDragHandleWidth / 2;

        for (int dy = 8; dy < handle_h - 4; dy += dot_spacing)
        {
            dc.DrawCircle(dot_x - 2, 2 + dy, dot_size / 2);
            dc.DrawCircle(dot_x + 2, 2 + dy, dot_size / 2);
        }
    }

    // Buttons
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    for (int idx = 0; idx < static_cast<int>(buttons_.size()); ++idx)
    {
        const auto& btn = buttons_[static_cast<size_t>(idx)];
        const auto rect = ButtonRectAt(idx);

        if (btn.is_separator)
        {
            // Draw vertical separator line
            dc.SetPen(wxPen(border));
            const int sep_x = rect.x + rect.width / 2;
            dc.DrawLine(sep_x, rect.y + 4, sep_x, rect.y + rect.height - 4);
            continue;
        }

        // Check enabled state
        const bool enabled = !btn.is_enabled || btn.is_enabled();
        const bool toggled = btn.is_toggled && btn.is_toggled();

        // Background: hover / press / toggled / focus
        if (idx == pressed_button_ && enabled)
        {
            dc.SetBrush(wxBrush(selected_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);
        }
        else if (idx == hovered_button_ && enabled)
        {
            dc.SetBrush(wxBrush(hover_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);
        }
        else if (toggled)
        {
            dc.SetBrush(wxBrush(selected_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);
        }

        // Focus ring
        if (idx == focused_button_)
        {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(wxPen(accent, 1, wxPENSTYLE_DOT));
            dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, 4);
        }

        // Text
        if (enabled)
        {
            dc.SetTextForeground(toggled ? accent : text_fg);
        }
        else
        {
            dc.SetTextForeground(disabled_fg);
        }

        const auto label_size = dc.GetTextExtent(btn.label);
        const int text_x = rect.x + (rect.width - label_size.x) / 2;
        const int text_y = rect.y + (rect.height - label_size.y) / 2;
        dc.DrawText(btn.label, text_x, text_y);
    }
}

void FloatingToolbar::ApplyTheme()
{
    Refresh();
}

// ── Interaction ──────────────────────────────────────────────────────────────

void FloatingToolbar::OnMouseDown(wxMouseEvent& event)
{
    const auto pos = event.GetPosition();

    // Check drag handle first
    if (draggable_ && IsDragHandleHit(pos))
    {
        is_dragging_ = true;
        drag_start_ = ClientToScreen(pos);
        window_start_ = GetPosition();
        CaptureMouse();
        return;
    }

    // Check button press
    const int btn_idx = HitTestButton(pos);
    if (btn_idx >= 0)
    {
        const auto& btn = buttons_[static_cast<size_t>(btn_idx)];
        const bool enabled = !btn.is_enabled || btn.is_enabled();
        if (enabled)
        {
            pressed_button_ = btn_idx;
            Refresh();
        }
    }
}

void FloatingToolbar::OnMouseUp(wxMouseEvent& event)
{
    if (is_dragging_)
    {
        is_dragging_ = false;
        if (HasCapture())
        {
            ReleaseMouse();
        }
        if (snap_to_edges_)
        {
            SnapToNearestEdge();
        }
        last_position_ = GetPosition();
        return;
    }

    const auto pos = event.GetPosition();
    const int btn_idx = HitTestButton(pos);

    if (btn_idx >= 0 && btn_idx == pressed_button_)
    {
        const auto& btn = buttons_[static_cast<size_t>(btn_idx)];
        const bool enabled = !btn.is_enabled || btn.is_enabled();
        if (enabled && btn.callback)
        {
            btn.callback();
        }
    }

    pressed_button_ = -1;
    Refresh();
}

void FloatingToolbar::OnMouseMove(wxMouseEvent& event)
{
    if (is_dragging_)
    {
        const auto screen_pos = ClientToScreen(event.GetPosition());
        const wxPoint delta(screen_pos.x - drag_start_.x, screen_pos.y - drag_start_.y);
        SetPosition(wxPoint(window_start_.x + delta.x, window_start_.y + delta.y));
        return;
    }

    const auto pos = event.GetPosition();
    const int new_hover = HitTestButton(pos);
    if (new_hover != hovered_button_)
    {
        hovered_button_ = new_hover;
        Refresh();
    }

    // Reset auto-hide on mouse activity
    ResetAutoHideTimer();
}

void FloatingToolbar::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (hovered_button_ != -1)
    {
        hovered_button_ = -1;
        Refresh();
    }
    // Start auto-hide countdown
    if (auto_hide_ms_ > 0 && !is_dragging_)
    {
        auto_hide_timer_.StartOnce(auto_hide_ms_);
    }
}

void FloatingToolbar::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_LEFT:
        {
            if (focused_button_ > 0)
            {
                --focused_button_;
                // Skip separators
                while (focused_button_ > 0 &&
                       buttons_[static_cast<size_t>(focused_button_)].is_separator)
                {
                    --focused_button_;
                }
                Refresh();
            }
            break;
        }
        case WXK_RIGHT:
        {
            if (focused_button_ < static_cast<int>(buttons_.size()) - 1)
            {
                ++focused_button_;
                while (focused_button_ < static_cast<int>(buttons_.size()) - 1 &&
                       buttons_[static_cast<size_t>(focused_button_)].is_separator)
                {
                    ++focused_button_;
                }
                Refresh();
            }
            break;
        }
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
        {
            if (focused_button_ >= 0 && focused_button_ < static_cast<int>(buttons_.size()))
            {
                const auto& btn = buttons_[static_cast<size_t>(focused_button_)];
                const bool enabled = !btn.is_enabled || btn.is_enabled();
                if (enabled && btn.callback)
                {
                    btn.callback();
                }
            }
            break;
        }
        case WXK_ESCAPE:
        {
            HideToolbar();
            break;
        }
        default:
        {
            event.Skip();
            break;
        }
    }
}

// ── Layout ───────────────────────────────────────────────────────────────────

auto FloatingToolbar::CalculateSize() const -> wxSize
{
    const int drag_w = draggable_ ? kDragHandleWidth : 0;
    int total_w = drag_w + kButtonPadding;

    for (const auto& btn : buttons_)
    {
        if (btn.is_separator)
        {
            total_w += 8; // separator width
        }
        else
        {
            total_w += kButtonSize + kButtonPadding;
        }
    }
    total_w += kButtonPadding;

    const int total_h = kButtonSize + kButtonPadding * 2;

    return {total_w + kShadowOffset, total_h + kShadowOffset};
}

auto FloatingToolbar::ButtonRectAt(int index) const -> wxRect
{
    const int drag_w = draggable_ ? kDragHandleWidth : 0;
    int current_x = drag_w + kButtonPadding;

    for (int idx = 0; idx < index && idx < static_cast<int>(buttons_.size()); ++idx)
    {
        if (buttons_[static_cast<size_t>(idx)].is_separator)
        {
            current_x += 8;
        }
        else
        {
            current_x += kButtonSize + kButtonPadding;
        }
    }

    if (index >= 0 && index < static_cast<int>(buttons_.size()) &&
        buttons_[static_cast<size_t>(index)].is_separator)
    {
        return {current_x, kButtonPadding, 8, kButtonSize};
    }

    return {current_x, kButtonPadding, kButtonSize, kButtonSize};
}

auto FloatingToolbar::HitTestButton(const wxPoint& point) const -> int
{
    for (int idx = 0; idx < static_cast<int>(buttons_.size()); ++idx)
    {
        if (buttons_[static_cast<size_t>(idx)].is_separator)
        {
            continue;
        }
        const auto rect = ButtonRectAt(idx);
        if (rect.Contains(point))
        {
            return idx;
        }
    }
    return -1;
}

auto FloatingToolbar::IsDragHandleHit(const wxPoint& point) const -> bool
{
    if (!draggable_)
    {
        return false;
    }
    return point.x < kDragHandleWidth && point.y < (GetSize().y - kShadowOffset);
}

// ── Snapping ─────────────────────────────────────────────────────────────────

void FloatingToolbar::SnapToNearestEdge()
{
    auto* parent_win = GetParent();
    if (parent_win == nullptr)
    {
        return;
    }

    const auto parent_rect = parent_win->GetClientRect();
    const auto parent_screen_origin = parent_win->ClientToScreen(wxPoint(0, 0));
    auto pos = GetPosition();
    const auto toolbar_size = GetSize();

    // Convert to parent-relative coordinates
    const int rel_x = pos.x - parent_screen_origin.x;
    const int rel_y = pos.y - parent_screen_origin.y;

    int snapped_x = rel_x;
    int snapped_y = rel_y;

    // Snap to top
    if (rel_y < kSnapDistance)
    {
        snapped_y = 0;
    }
    // Snap to bottom
    if (parent_rect.GetHeight() - (rel_y + toolbar_size.GetHeight()) < kSnapDistance)
    {
        snapped_y = parent_rect.GetHeight() - toolbar_size.GetHeight();
    }
    // Snap to left
    if (rel_x < kSnapDistance)
    {
        snapped_x = 0;
    }
    // Snap to right
    if (parent_rect.GetWidth() - (rel_x + toolbar_size.GetWidth()) < kSnapDistance)
    {
        snapped_x = parent_rect.GetWidth() - toolbar_size.GetWidth();
    }

    const auto new_pos = parent_win->ClientToScreen(wxPoint(snapped_x, snapped_y));
    SetPosition(new_pos);
}

// ── Auto-hide ────────────────────────────────────────────────────────────────

void FloatingToolbar::OnAutoHideTimer(wxTimerEvent& /*event*/)
{
    HideToolbar();
}

void FloatingToolbar::ResetAutoHideTimer()
{
    auto_hide_timer_.Stop();
    // Timer will restart on mouse leave
}

} // namespace markamp::ui
