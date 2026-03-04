#include "NumberStepper.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace markamp::ui
{

void NumberStepperModel::set_value(double value)
{
    value_ = std::clamp(value, min_, max_);
}
auto NumberStepperModel::value() const -> double
{
    return value_;
}
void NumberStepperModel::set_range(double min_val, double max_val)
{
    min_ = min_val;
    max_ = max_val;
    value_ = std::clamp(value_, min_, max_);
}
auto NumberStepperModel::min_value() const -> double
{
    return min_;
}
auto NumberStepperModel::max_value() const -> double
{
    return max_;
}
void NumberStepperModel::set_step(double step)
{
    step_ = step > 0.0 ? step : 1.0;
}
auto NumberStepperModel::step() const -> double
{
    return step_;
}
void NumberStepperModel::increment()
{
    set_value(value_ + step_);
}
void NumberStepperModel::decrement()
{
    set_value(value_ - step_);
}
auto NumberStepperModel::can_increment() const -> bool
{
    return value_ < max_;
}
auto NumberStepperModel::can_decrement() const -> bool
{
    return value_ > min_;
}

auto NumberStepperModel::display_text() const -> std::string
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision_) << value_;
    return oss.str();
}

void NumberStepperModel::set_precision(int decimal_places)
{
    precision_ = std::max(0, decimal_places);
}
auto NumberStepperModel::precision() const -> int
{
    return precision_;
}

NumberStepper::NumberStepper(
    wxWindow* parent, core::ThemeEngine& theme_engine, double min_val, double max_val, double step)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    model_.set_range(min_val, max_val);
    model_.set_step(step);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(FromDIP(kButtonWidth * 2 + kPaddingH * 2 + 40), FromDIP(kHeight)));

    Bind(wxEVT_PAINT, &NumberStepper::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &NumberStepper::OnMouseDown, this);
    Bind(wxEVT_ENTER_WINDOW, &NumberStepper::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &NumberStepper::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &NumberStepper::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &NumberStepper::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &NumberStepper::OnKillFocus, this);
}

void NumberStepper::set_value(double value)
{
    model_.set_value(value);
    Refresh();
}
auto NumberStepper::value() const -> double
{
    return model_.value();
}
void NumberStepper::set_range(double min_val, double max_val)
{
    model_.set_range(min_val, max_val);
    Refresh();
}
void NumberStepper::set_step(double step)
{
    model_.set_step(step);
}
void NumberStepper::set_precision(int decimal_places)
{
    model_.set_precision(decimal_places);
    Refresh();
}
void NumberStepper::set_on_change(ChangeCallback callback)
{
    on_change_ = std::move(callback);
}
void NumberStepper::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}
auto NumberStepper::model() const -> const NumberStepperModel&
{
    return model_;
}

void NumberStepper::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void NumberStepper::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const double r = FromDIP(kBorderRadius);
    // const int btn_w = FromDIP(kButtonWidth); // reserved for future layout
    const int padH = FromDIP(kPaddingH);

    // Background
    gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::BgInput)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto bg = gc->CreatePath();
    bg.AddRoundedRectangle(0, 0, sz.x, sz.y, r);
    gc->FillPath(bg);

    // Border
    auto border_token = state_.is_focused() ? core::ThemeColorToken::FocusRingColor
                                            : core::ThemeColorToken::BorderLight;
    gc->SetPen(wxPen(theme_engine().color(border_token), 1));
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    auto bp = gc->CreatePath();
    bp.AddRoundedRectangle(0.5, 0.5, sz.x - 1, sz.y - 1, r);
    gc->StrokePath(bp);

    // Value text
    auto fg = state_.is_disabled() ? core::ThemeColorToken::ControlFgDisabled
                                   : core::ThemeColorToken::TextMain;
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel), theme_engine().color(fg));
    double tw = 0, th = 0;
    auto text = wxString::FromUTF8(model_.display_text());
    gc->GetTextExtent(text, &tw, &th);
    gc->DrawText(text, (sz.x - tw) / 2.0, (sz.y - th) / 2.0);

    // Minus button (left)
    auto btn_fg = model_.can_decrement() && !state_.is_disabled()
                      ? core::ThemeColorToken::TextMain
                      : core::ThemeColorToken::ControlFgDisabled;
    gc->SetPen(wxPen(theme_engine().color(btn_fg), FromDIP(2)));
    gc->StrokeLine(padH, sz.y / 2.0, padH + 8, sz.y / 2.0);

    // Plus button (right)
    btn_fg = model_.can_increment() && !state_.is_disabled()
                 ? core::ThemeColorToken::TextMain
                 : core::ThemeColorToken::ControlFgDisabled;
    gc->SetPen(wxPen(theme_engine().color(btn_fg), FromDIP(2)));
    const double px = sz.x - padH - 4;
    gc->StrokeLine(px - 4, sz.y / 2.0, px + 4, sz.y / 2.0);
    gc->StrokeLine(px, sz.y / 2.0 - 4, px, sz.y / 2.0 + 4);

    state_.acknowledge_change();
}

void NumberStepper::OnMouseDown(wxMouseEvent& event)
{
    if (state_.is_disabled())
        return;
    SetFocus();
    const int btn_w = FromDIP(kButtonWidth);
    if (event.GetX() < btn_w)
    {
        model_.decrement();
        if (on_change_)
            on_change_(model_.value());
    }
    else if (event.GetX() > GetClientSize().x - btn_w)
    {
        model_.increment();
        if (on_change_)
            on_change_(model_.value());
    }
    Refresh();
}

void NumberStepper::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_enter();
        SetControlCursor(ControlCursorType::kHand);
        Refresh();
    }
}
void NumberStepper::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

void NumberStepper::OnKeyDown(wxKeyEvent& event)
{
    if (state_.is_disabled())
    {
        event.Skip();
        return;
    }
    if (event.GetKeyCode() == WXK_UP)
    {
        model_.increment();
        if (on_change_)
            on_change_(model_.value());
        Refresh();
    }
    else if (event.GetKeyCode() == WXK_DOWN)
    {
        model_.decrement();
        if (on_change_)
            on_change_(model_.value());
        Refresh();
    }
    else
        event.Skip();
}

void NumberStepper::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}
void NumberStepper::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

} // namespace markamp::ui
