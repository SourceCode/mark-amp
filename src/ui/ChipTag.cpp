#include "ChipTag.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

void ChipTagModel::set_label(const std::string& label)
{
    label_ = label;
}
auto ChipTagModel::label() const -> const std::string&
{
    return label_;
}
void ChipTagModel::set_removable(bool removable)
{
    removable_ = removable;
}
auto ChipTagModel::is_removable() const -> bool
{
    return removable_;
}
void ChipTagModel::set_icon_name(const std::string& icon_name)
{
    icon_name_ = icon_name;
}
auto ChipTagModel::icon_name() const -> const std::string&
{
    return icon_name_;
}
void ChipTagModel::set_selected(bool selected)
{
    selected_ = selected;
}
auto ChipTagModel::is_selected() const -> bool
{
    return selected_;
}

ChipTag::ChipTag(wxWindow* parent, core::ThemeEngine& theme_engine, const std::string& label)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
{
    model_.set_label(label);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(FromDIP(kPaddingH * 2) + 40, FromDIP(kHeight)));

    Bind(wxEVT_PAINT, &ChipTag::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ChipTag::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &ChipTag::OnMouseUp, this);
    Bind(wxEVT_ENTER_WINDOW, &ChipTag::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ChipTag::OnMouseLeave, this);
}

void ChipTag::set_label(const std::string& label)
{
    model_.set_label(label);
    Refresh();
}
void ChipTag::set_removable(bool removable)
{
    model_.set_removable(removable);
    Refresh();
}
void ChipTag::set_icon_name(const std::string& icon_name)
{
    model_.set_icon_name(icon_name);
    Refresh();
}
void ChipTag::set_selected(bool selected)
{
    model_.set_selected(selected);
    Refresh();
}
void ChipTag::set_on_remove(RemoveCallback callback)
{
    on_remove_ = std::move(callback);
}
void ChipTag::set_on_click(ClickCallback callback)
{
    on_click_ = std::move(callback);
}
auto ChipTag::model() const -> const ChipTagModel&
{
    return model_;
}

void ChipTag::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ChipTag::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const double r = FromDIP(kBorderRadius);
    const int padH = FromDIP(kPaddingH);

    auto bg_token = model_.is_selected() ? core::ThemeColorToken::AccentPrimary
                    : state_.is_hover()  ? core::ThemeColorToken::HoverBg
                                         : core::ThemeColorToken::ControlBgPressed;
    gc->SetBrush(wxBrush(theme_engine().color(bg_token)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto path = gc->CreatePath();
    path.AddRoundedRectangle(0, 0, sz.x, sz.y, r);
    gc->FillPath(path);

    auto fg_token = model_.is_selected() ? core::ThemeColorToken::AccentSecondary
                                         : core::ThemeColorToken::TextMain;
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UISmall),
                theme_engine().color(fg_token));
    double tw = 0, th = 0;
    gc->GetTextExtent(wxString::FromUTF8(model_.label()), &tw, &th);
    gc->DrawText(wxString::FromUTF8(model_.label()), padH, (sz.y - th) / 2.0);

    // Close button
    if (model_.is_removable())
    {
        const int close = FromDIP(kCloseSize);
        const double cx = sz.x - padH - close / 2.0;
        const double cy = sz.y / 2.0;
        gc->SetPen(wxPen(theme_engine().color(fg_token), 1));
        gc->StrokeLine(cx - 3, cy - 3, cx + 3, cy + 3);
        gc->StrokeLine(cx - 3, cy + 3, cx + 3, cy - 3);
    }

    state_.acknowledge_change();
}

void ChipTag::OnMouseDown(wxMouseEvent& /*event*/)
{
    state_.on_mouse_down();
    CaptureMouse();
    Refresh();
}

void ChipTag::OnMouseUp(wxMouseEvent& event)
{
    if (HasCapture())
        ReleaseMouse();
    const bool inside = GetClientRect().Contains(event.GetPosition());
    state_.on_mouse_up(inside);
    if (inside)
    {
        // Check if click was on close button
        const int padH_dip = FromDIP(kPaddingH);
        const int close_size = FromDIP(kCloseSize);
        if (model_.is_removable() && event.GetX() > GetClientSize().x - padH_dip - close_size)
        {
            if (on_remove_)
                on_remove_();
        }
        else if (on_click_)
            on_click_();
    }
    Refresh();
}

void ChipTag::OnMouseEnter(wxMouseEvent& /*event*/)
{
    state_.on_mouse_enter();
    SetControlCursor(ControlCursorType::kHand);
    Refresh();
}
void ChipTag::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

} // namespace markamp::ui
