#include "Badge.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// ── BadgeModel ─────────────────────────────────────────────────────

void BadgeModel::set_count(int count)
{
    count_ = count;
}
auto BadgeModel::count() const -> int
{
    return count_;
}
void BadgeModel::set_max_count(int max_count)
{
    max_count_ = max_count > 0 ? max_count : 99;
}
auto BadgeModel::max_count() const -> int
{
    return max_count_;
}

auto BadgeModel::display_text() const -> std::string
{
    if (dot_only_)
        return {};
    if (count_ > max_count_)
        return std::to_string(max_count_) + "+";
    return std::to_string(count_);
}

void BadgeModel::set_dot_only(bool dot_only)
{
    dot_only_ = dot_only;
}
auto BadgeModel::is_dot_only() const -> bool
{
    return dot_only_;
}
auto BadgeModel::is_visible() const -> bool
{
    return count_ > 0;
}

// ── Badge ──────────────────────────────────────────────────────────

Badge::Badge(wxWindow* parent, core::ThemeEngine& theme_engine)
    : ThemeAwareWindow(parent, theme_engine)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(FromDIP(kMinWidth), FromDIP(kHeight)));
    Bind(wxEVT_PAINT, &Badge::OnPaint, this);
}

void Badge::set_count(int count)
{
    model_.set_count(count);
    Refresh();
}
void Badge::set_max_count(int max_count)
{
    model_.set_max_count(max_count);
    Refresh();
}
void Badge::set_dot_only(bool dot_only)
{
    model_.set_dot_only(dot_only);
    Refresh();
}
auto Badge::model() const -> const BadgeModel&
{
    return model_;
}

void Badge::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void Badge::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    if (!model_.is_visible())
        return;

    const auto sz = GetClientSize();

    if (model_.is_dot_only())
    {
        const int dot = FromDIP(kDotSize);
        gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->DrawEllipse((sz.x - dot) / 2.0, (sz.y - dot) / 2.0, dot, dot);
    }
    else
    {
        // Pill badge
        const double h = FromDIP(kHeight);
        gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
        gc->SetPen(*wxTRANSPARENT_PEN);
        auto path = gc->CreatePath();
        path.AddRoundedRectangle(0, (sz.y - h) / 2.0, sz.x, h, h / 2.0);
        gc->FillPath(path);

        // Text
        gc->SetFont(theme_engine().font(core::ThemeFontToken::UISmall),
                    theme_engine().color(core::ThemeColorToken::AccentSecondary));
        double tw = 0, th = 0;
        const auto text = wxString::FromUTF8(model_.display_text());
        gc->GetTextExtent(text, &tw, &th);
        gc->DrawText(text, (sz.x - tw) / 2.0, (sz.y - th) / 2.0);
    }
}

} // namespace markamp::ui
