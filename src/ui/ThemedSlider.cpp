#include "ThemedSlider.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

// ── SliderModel ────────────────────────────────────────────────────

void SliderModel::set_range(double min_val, double max_val)
{
    min_ = min_val;
    max_ = max_val;
    value_ = std::clamp(value_, min_, max_);
}
auto SliderModel::min_value() const -> double
{
    return min_;
}
auto SliderModel::max_value() const -> double
{
    return max_;
}

void SliderModel::set_value(double value)
{
    value_ = std::clamp(value, min_, max_);
}
auto SliderModel::value() const -> double
{
    return value_;
}

void SliderModel::set_step(double step)
{
    step_ = step > 0.0 ? step : 1.0;
}
auto SliderModel::step() const -> double
{
    return step_;
}

auto SliderModel::normalized() const -> double
{
    if (max_ <= min_)
        return 0.0;
    return (value_ - min_) / (max_ - min_);
}

void SliderModel::snap_to_step()
{
    if (step_ > 0.0)
    {
        value_ = std::round((value_ - min_) / step_) * step_ + min_;
        value_ = std::clamp(value_, min_, max_);
    }
}

void SliderModel::increment()
{
    set_value(value_ + step_);
}
void SliderModel::decrement()
{
    set_value(value_ - step_);
}

// ── ThemedSlider ───────────────────────────────────────────────────

ThemedSlider::ThemedSlider(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           double min_val,
                           double max_val)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    model_.set_range(min_val, max_val);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(100, FromDIP(kHeight)));

    Bind(wxEVT_PAINT, &ThemedSlider::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedSlider::OnMouseDown, this);
    Bind(wxEVT_MOTION, &ThemedSlider::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &ThemedSlider::OnMouseUp, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemedSlider::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedSlider::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &ThemedSlider::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &ThemedSlider::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &ThemedSlider::OnKillFocus, this);
}

void ThemedSlider::set_value(double value)
{
    model_.set_value(value);
    Refresh();
}
auto ThemedSlider::value() const -> double
{
    return model_.value();
}
void ThemedSlider::set_range(double min_val, double max_val)
{
    model_.set_range(min_val, max_val);
    Refresh();
}
void ThemedSlider::set_step(double step)
{
    model_.set_step(step);
}
void ThemedSlider::set_on_change(ChangeCallback callback)
{
    on_change_ = std::move(callback);
}
void ThemedSlider::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}
auto ThemedSlider::model() const -> const SliderModel&
{
    return model_;
}

void ThemedSlider::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedSlider::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    FillBackground(pdc, core::ThemeColorToken::BgPanel);

    const int thumb = FromDIP(kThumbSize);
    const int track_h = FromDIP(kTrackHeight);
    const int track_y = (sz.y - track_h) / 2;
    const int usable_w = sz.x - thumb;
    const double norm = model_.normalized();

    // Track background
    gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::ControlBgPressed)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto tp = gc->CreatePath();
    tp.AddRoundedRectangle(thumb / 2.0, track_y, usable_w, track_h, track_h / 2.0);
    gc->FillPath(tp);

    // Track fill
    auto fill_token = state_.is_disabled() ? core::ThemeColorToken::ControlFgDisabled
                                           : core::ThemeColorToken::AccentPrimary;
    gc->SetBrush(wxBrush(theme_engine().color(fill_token)));
    auto fp = gc->CreatePath();
    fp.AddRoundedRectangle(thumb / 2.0, track_y, usable_w * norm, track_h, track_h / 2.0);
    gc->FillPath(fp);

    // Thumb
    const double thumb_x = usable_w * norm;
    const double thumb_y = (sz.y - thumb) / 2.0;
    gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentSecondary)));
    gc->SetPen(wxPen(theme_engine().color(fill_token), 2));
    gc->DrawEllipse(thumb_x, thumb_y, thumb, thumb);

    // Focus ring
    if (state_.is_focused())
    {
        gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::FocusRingColor), 2));
        gc->SetBrush(*wxTRANSPARENT_BRUSH);
        gc->DrawEllipse(thumb_x - 2, thumb_y - 2, thumb + 4, thumb + 4);
    }

    state_.acknowledge_change();
}

void ThemedSlider::OnMouseDown(wxMouseEvent& event)
{
    if (state_.is_disabled())
        return;
    is_dragging_ = true;
    CaptureMouse();
    model_.set_value(pixel_to_value(event.GetX()));
    model_.snap_to_step();
    if (on_change_)
        on_change_(model_.value());
    Refresh();
}

void ThemedSlider::OnMouseMove(wxMouseEvent& event)
{
    if (is_dragging_)
    {
        model_.set_value(pixel_to_value(event.GetX()));
        model_.snap_to_step();
        if (on_change_)
            on_change_(model_.value());
        Refresh();
    }
}

void ThemedSlider::OnMouseUp(wxMouseEvent& /*event*/)
{
    if (HasCapture())
        ReleaseMouse();
    is_dragging_ = false;
}

void ThemedSlider::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_enter();
        SetControlCursor(ControlCursorType::kHand);
        Refresh();
    }
}

void ThemedSlider::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

void ThemedSlider::OnKeyDown(wxKeyEvent& event)
{
    if (state_.is_disabled())
    {
        event.Skip();
        return;
    }
    if (event.GetKeyCode() == WXK_RIGHT || event.GetKeyCode() == WXK_UP)
    {
        model_.increment();
        if (on_change_)
            on_change_(model_.value());
        Refresh();
    }
    else if (event.GetKeyCode() == WXK_LEFT || event.GetKeyCode() == WXK_DOWN)
    {
        model_.decrement();
        if (on_change_)
            on_change_(model_.value());
        Refresh();
    }
    else
        event.Skip();
}

void ThemedSlider::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}
void ThemedSlider::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

auto ThemedSlider::pixel_to_value(int x) const -> double
{
    const int thumb = FromDIP(kThumbSize);
    const int usable = GetClientSize().x - thumb;
    if (usable <= 0)
        return model_.min_value();
    double norm = static_cast<double>(x - thumb / 2) / static_cast<double>(usable);
    norm = std::clamp(norm, 0.0, 1.0);
    return model_.min_value() + norm * (model_.max_value() - model_.min_value());
}

auto ThemedSlider::value_to_pixel(double val) const -> int
{
    const int thumb = FromDIP(kThumbSize);
    const int usable = GetClientSize().x - thumb;
    double norm = (val - model_.min_value()) / (model_.max_value() - model_.min_value());
    return static_cast<int>(norm * usable) + thumb / 2;
}

} // namespace markamp::ui
