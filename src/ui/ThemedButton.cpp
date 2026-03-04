#include "ThemedButton.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

ThemedButton::ThemedButton(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           const std::string& label,
                           ControlVariant variant)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
    , label_(label)
    , variant_(variant)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &ThemedButton::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemedButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedButton::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedButton::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ThemedButton::OnMouseUp, this);
    Bind(wxEVT_KEY_DOWN, &ThemedButton::OnKeyDown, this);
    Bind(wxEVT_KEY_UP, &ThemedButton::OnKeyUp, this);
    Bind(wxEVT_SET_FOCUS, &ThemedButton::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &ThemedButton::OnKillFocus, this);

    auto best = compute_best_size();
    SetMinSize(best);
    SetSize(best);
}

void ThemedButton::set_label(const std::string& label)
{
    label_ = label;
    auto best = compute_best_size();
    SetMinSize(best);
    Refresh();
}

auto ThemedButton::label() const -> const std::string&
{
    return label_;
}

void ThemedButton::set_variant(ControlVariant variant)
{
    variant_ = variant;
    Refresh();
}

auto ThemedButton::variant() const -> ControlVariant
{
    return variant_;
}

void ThemedButton::set_icon_name(const std::string& icon_name)
{
    icon_name_ = icon_name;
    auto best = compute_best_size();
    SetMinSize(best);
    Refresh();
}

auto ThemedButton::icon_name() const -> const std::string&
{
    return icon_name_;
}

void ThemedButton::set_icon_only(bool icon_only)
{
    icon_only_ = icon_only;
    auto best = compute_best_size();
    SetMinSize(best);
    Refresh();
}

auto ThemedButton::is_icon_only() const -> bool
{
    return icon_only_;
}

void ThemedButton::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    if (!enabled)
    {
        SetControlCursor(ControlCursorType::kArrow);
    }
    else
    {
        SetControlCursor(ControlCursorType::kHand);
    }
    Refresh();
}

auto ThemedButton::is_enabled() const -> bool
{
    return !state_.is_disabled();
}

void ThemedButton::set_on_click(ClickCallback callback)
{
    on_click_ = std::move(callback);
}

auto ThemedButton::state_tracker() const -> const ControlStateTracker&
{
    return state_;
}

void ThemedButton::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedButton::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const auto tokens = VariantTokenResolver::resolve(variant_, state_.flags());

    // Background
    const auto bg = theme_engine().color(tokens.background);
    gc->SetBrush(wxBrush(bg));
    gc->SetPen(*wxTRANSPARENT_PEN);

    if (variant_ != ControlVariant::kGhost && variant_ != ControlVariant::kLink)
    {
        auto path = gc->CreatePath();
        const double r = FromDIP(kBorderRadius);
        path.AddRoundedRectangle(0, 0, sz.x, sz.y, r);
        gc->FillPath(path);
    }

    // Border
    if (variant_ == ControlVariant::kSecondary || state_.is_focused())
    {
        const auto border_color = theme_engine().color(tokens.border);
        gc->SetPen(wxPen(border_color, 1));
        gc->SetBrush(*wxTRANSPARENT_BRUSH);
        auto path = gc->CreatePath();
        const double r = FromDIP(kBorderRadius);
        path.AddRoundedRectangle(0.5, 0.5, sz.x - 1, sz.y - 1, r);
        gc->StrokePath(path);
    }

    // Focus ring
    if (state_.is_focused())
    {
        DrawFocusRing(pdc);
    }

    // Text
    const auto fg = theme_engine().color(tokens.foreground);
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel), fg);

    if (!icon_only_ && !label_.empty())
    {
        double tw = 0, th = 0;
        gc->GetTextExtent(wxString::FromUTF8(label_), &tw, &th);

        const double tx = (sz.x - tw) / 2.0;
        const double ty = (sz.y - th) / 2.0;
        gc->DrawText(wxString::FromUTF8(label_), tx, ty);
    }

    // Link underline
    if (variant_ == ControlVariant::kLink && !label_.empty())
    {
        double tw = 0, th = 0;
        gc->GetTextExtent(wxString::FromUTF8(label_), &tw, &th);
        const double tx = (sz.x - tw) / 2.0;
        const double ty = (sz.y - th) / 2.0;
        gc->SetPen(wxPen(fg, 1));
        gc->StrokeLine(tx, ty + th, tx + tw, ty + th);
    }

    state_.acknowledge_change();
}

void ThemedButton::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_enter();
        SetControlCursor(ControlCursorType::kHand);
        Refresh();
    }
}

void ThemedButton::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

void ThemedButton::OnMouseDown(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_down();
        CaptureMouse();
        Refresh();
    }
}

void ThemedButton::OnMouseUp(wxMouseEvent& event)
{
    if (HasCapture())
        ReleaseMouse();

    const bool inside = GetClientRect().Contains(event.GetPosition());
    state_.on_mouse_up(inside);

    if (inside && !state_.is_disabled() && on_click_)
    {
        on_click_();
    }
    Refresh();
}

void ThemedButton::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_SPACE || event.GetKeyCode() == WXK_RETURN)
    {
        if (!state_.is_disabled())
        {
            state_.on_mouse_down();
            Refresh();
        }
    }
    else
    {
        event.Skip();
    }
}

void ThemedButton::OnKeyUp(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_SPACE || event.GetKeyCode() == WXK_RETURN)
    {
        state_.on_mouse_up(true);
        if (!state_.is_disabled() && on_click_)
        {
            on_click_();
        }
        Refresh();
    }
    else
    {
        event.Skip();
    }
}

void ThemedButton::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}

void ThemedButton::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

auto ThemedButton::compute_best_size() const -> wxSize
{
    if (icon_only_)
    {
        const int s = FromDIP(kIconOnlySize + kPaddingV * 2);
        return {s, s};
    }

    wxClientDC dc(const_cast<ThemedButton*>(this));
    dc.SetFont(theme_engine().font(core::ThemeFontToken::UILabel));
    auto text_size = dc.GetTextExtent(wxString::FromUTF8(label_));

    int w = text_size.x + FromDIP(kPaddingH * 2);
    int h = text_size.y + FromDIP(kPaddingV * 2);

    if (!icon_name_.empty())
    {
        w += FromDIP(kIconSize + kIconLabelGap);
    }

    return {w, h};
}

} // namespace markamp::ui
