#include "ThemedProgressBar.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

// ── ProgressBarModel ───────────────────────────────────────────────

void ProgressBarModel::set_value(double value)
{
    value_ = std::clamp(value, 0.0, max_);
}
auto ProgressBarModel::value() const -> double
{
    return value_;
}
void ProgressBarModel::set_max(double max)
{
    max_ = max > 0.0 ? max : 100.0;
    value_ = std::clamp(value_, 0.0, max_);
}
auto ProgressBarModel::max() const -> double
{
    return max_;
}
auto ProgressBarModel::normalized() const -> double
{
    return max_ > 0.0 ? value_ / max_ : 0.0;
}
auto ProgressBarModel::percentage() const -> int
{
    return static_cast<int>(std::round(normalized() * 100.0));
}
void ProgressBarModel::set_style(ProgressBarStyle style)
{
    style_ = style;
}
auto ProgressBarModel::style() const -> ProgressBarStyle
{
    return style_;
}
void ProgressBarModel::set_label(const std::string& label)
{
    label_ = label;
}
auto ProgressBarModel::label() const -> const std::string&
{
    return label_;
}
auto ProgressBarModel::is_complete() const -> bool
{
    return value_ >= max_;
}

// ── ThemedProgressBar ──────────────────────────────────────────────

ThemedProgressBar::ThemedProgressBar(wxWindow* parent,
                                     core::ThemeEngine& theme_engine,
                                     ProgressBarStyle style)
    : ThemeAwareWindow(parent, theme_engine)
{
    model_.set_style(style);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    if (style == ProgressBarStyle::kCircular)
        SetMinSize(wxSize(FromDIP(kCircularSize), FromDIP(kCircularSize)));
    else
        SetMinSize(wxSize(-1, FromDIP(kLinearHeight)));

    Bind(wxEVT_PAINT, &ThemedProgressBar::OnPaint, this);
}

void ThemedProgressBar::set_value(double value)
{
    model_.set_value(value);
    Refresh();
}
void ThemedProgressBar::set_max(double max)
{
    model_.set_max(max);
    Refresh();
}
void ThemedProgressBar::set_style(ProgressBarStyle style)
{
    model_.set_style(style);
    Refresh();
}
void ThemedProgressBar::set_label(const std::string& label)
{
    model_.set_label(label);
    Refresh();
}
auto ThemedProgressBar::model() const -> const ProgressBarModel&
{
    return model_;
}

void ThemedProgressBar::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedProgressBar::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();

    if (model_.style() == ProgressBarStyle::kCircular)
    {
        const double cx = sz.x / 2.0;
        const double cy = sz.y / 2.0;
        const double radius = std::min(cx, cy) - 2;

        // Background circle
        gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::ControlBgPressed), FromDIP(3)));
        gc->SetBrush(*wxTRANSPARENT_BRUSH);
        gc->DrawEllipse(cx - radius, cy - radius, radius * 2, radius * 2);

        // Progress arc
        gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::AccentPrimary), FromDIP(3)));
        auto path = gc->CreatePath();
        const double start_angle = -M_PI / 2.0;
        const double sweep = model_.normalized() * 2.0 * M_PI;
        path.AddArc(cx, cy, radius, start_angle, start_angle + sweep, true);
        gc->StrokePath(path);
    }
    else
    {
        // Linear bar
        const double r = FromDIP(kBorderRadius);

        // Background track
        gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::ControlBgPressed)));
        gc->SetPen(*wxTRANSPARENT_PEN);
        auto bg_path = gc->CreatePath();
        bg_path.AddRoundedRectangle(0, 0, sz.x, sz.y, r);
        gc->FillPath(bg_path);

        // Fill
        const double fill_w = sz.x * model_.normalized();
        if (fill_w > 0)
        {
            gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
            auto fill_path = gc->CreatePath();
            fill_path.AddRoundedRectangle(0, 0, fill_w, sz.y, r);
            gc->FillPath(fill_path);
        }
    }
}

} // namespace markamp::ui
