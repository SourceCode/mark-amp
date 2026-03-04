#include "ThemedToggleSwitch.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// ── ToggleSwitchModel ──────────────────────────────────────────────

void ToggleSwitchModel::set_on(bool on)
{
    on_ = on;
}
auto ToggleSwitchModel::is_on() const -> bool
{
    return on_;
}
void ToggleSwitchModel::toggle()
{
    on_ = !on_;
}
void ToggleSwitchModel::set_label(const std::string& label)
{
    label_ = label;
}
auto ToggleSwitchModel::label() const -> const std::string&
{
    return label_;
}

// ── ThemedToggleSwitch ─────────────────────────────────────────────

ThemedToggleSwitch::ThemedToggleSwitch(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       const std::string& label)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    model_.set_label(label);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(FromDIP(kTrackWidth + kGap) + 100, FromDIP(kTrackHeight + 4)));

    Bind(wxEVT_PAINT, &ThemedToggleSwitch::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedToggleSwitch::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ThemedToggleSwitch::OnMouseUp, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemedToggleSwitch::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedToggleSwitch::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &ThemedToggleSwitch::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &ThemedToggleSwitch::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &ThemedToggleSwitch::OnKillFocus, this);
}

void ThemedToggleSwitch::set_on(bool on)
{
    model_.set_on(on);
    knob_position_ = on ? 1.0F : 0.0F;
    Refresh();
}
auto ThemedToggleSwitch::is_on() const -> bool
{
    return model_.is_on();
}
void ThemedToggleSwitch::set_label(const std::string& label)
{
    model_.set_label(label);
    Refresh();
}
void ThemedToggleSwitch::set_on_change(ChangeCallback callback)
{
    on_change_ = std::move(callback);
}
void ThemedToggleSwitch::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}
auto ThemedToggleSwitch::model() const -> const ToggleSwitchModel&
{
    return model_;
}

void ThemedToggleSwitch::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedToggleSwitch::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    FillBackground(pdc, core::ThemeColorToken::BgPanel);

    const int tw = FromDIP(kTrackWidth);
    const int th = FromDIP(kTrackHeight);
    const int knob = FromDIP(kKnobSize);
    const int gap = FromDIP(kGap);
    const int track_y = (sz.y - th) / 2;
    const double track_r = th / 2.0;

    // Track
    auto track_token = model_.is_on() ? core::ThemeColorToken::AccentPrimary
                                      : core::ThemeColorToken::ControlBgPressed;
    if (state_.is_disabled())
        track_token = core::ThemeColorToken::ControlBgDisabled;

    gc->SetBrush(wxBrush(theme_engine().color(track_token)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto track_path = gc->CreatePath();
    track_path.AddRoundedRectangle(0, track_y, tw, th, track_r);
    gc->FillPath(track_path);

    // Knob
    const int knob_padding = (th - knob) / 2;
    const float knob_x_off = model_.is_on() ? static_cast<float>(tw - knob - knob_padding)
                                            : static_cast<float>(knob_padding);
    const int knob_y = track_y + knob_padding;

    gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentSecondary)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawEllipse(static_cast<double>(knob_x_off), knob_y, knob, knob);

    // Focus ring
    if (state_.is_focused())
    {
        gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::FocusRingColor), FromDIP(2)));
        gc->SetBrush(*wxTRANSPARENT_BRUSH);
        auto focus_path = gc->CreatePath();
        focus_path.AddRoundedRectangle(-1, track_y - 1, tw + 2, th + 2, track_r + 1);
        gc->StrokePath(focus_path);
    }

    // Label
    auto fg = state_.is_disabled() ? core::ThemeColorToken::ControlFgDisabled
                                   : core::ThemeColorToken::TextMain;
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel), theme_engine().color(fg));
    double ltw = 0, lth = 0;
    gc->GetTextExtent(wxString::FromUTF8(model_.label()), &ltw, &lth);
    gc->DrawText(wxString::FromUTF8(model_.label()), tw + gap, (sz.y - lth) / 2.0);

    state_.acknowledge_change();
}

void ThemedToggleSwitch::OnMouseDown(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_down();
        CaptureMouse();
        Refresh();
    }
}

void ThemedToggleSwitch::OnMouseUp(wxMouseEvent& event)
{
    if (HasCapture())
        ReleaseMouse();
    const bool inside = GetClientRect().Contains(event.GetPosition());
    state_.on_mouse_up(inside);
    if (inside && !state_.is_disabled())
    {
        model_.toggle();
        knob_position_ = model_.is_on() ? 1.0F : 0.0F;
        if (on_change_)
            on_change_(model_.is_on());
    }
    Refresh();
}

void ThemedToggleSwitch::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_enter();
        SetControlCursor(ControlCursorType::kHand);
        Refresh();
    }
}

void ThemedToggleSwitch::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

void ThemedToggleSwitch::OnKeyDown(wxKeyEvent& event)
{
    if ((event.GetKeyCode() == WXK_SPACE || event.GetKeyCode() == WXK_RETURN) &&
        !state_.is_disabled())
    {
        model_.toggle();
        knob_position_ = model_.is_on() ? 1.0F : 0.0F;
        if (on_change_)
            on_change_(model_.is_on());
        Refresh();
    }
    else
        event.Skip();
}

void ThemedToggleSwitch::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}
void ThemedToggleSwitch::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

} // namespace markamp::ui
