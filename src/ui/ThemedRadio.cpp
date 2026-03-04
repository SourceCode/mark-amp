#include "ThemedRadio.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

// ── RadioGroupModel ────────────────────────────────────────────────

void RadioGroupModel::set_items(std::vector<RadioItem> items)
{
    items_ = std::move(items);
    selected_index_ = 0;
}
auto RadioGroupModel::items() const -> const std::vector<RadioItem>&
{
    return items_;
}

void RadioGroupModel::set_selected_index(int index)
{
    if (index >= 0 && index < static_cast<int>(items_.size()) &&
        !items_[static_cast<size_t>(index)].is_disabled)
        selected_index_ = index;
}

auto RadioGroupModel::selected_index() const -> int
{
    return selected_index_;
}

auto RadioGroupModel::selected_item() const -> const RadioItem*
{
    if (selected_index_ >= 0 && selected_index_ < static_cast<int>(items_.size()))
        return &items_[static_cast<size_t>(selected_index_)];
    return nullptr;
}

void RadioGroupModel::select_next()
{
    for (int i = selected_index_ + 1; i < static_cast<int>(items_.size()); ++i)
    {
        if (!items_[static_cast<size_t>(i)].is_disabled)
        {
            selected_index_ = i;
            return;
        }
    }
}

void RadioGroupModel::select_previous()
{
    for (int i = selected_index_ - 1; i >= 0; --i)
    {
        if (!items_[static_cast<size_t>(i)].is_disabled)
        {
            selected_index_ = i;
            return;
        }
    }
}

auto RadioGroupModel::item_count() const -> int
{
    return static_cast<int>(items_.size());
}

// ── ThemedRadio ────────────────────────────────────────────────────

ThemedRadio::ThemedRadio(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         std::vector<RadioItem> items)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    model_.set_items(std::move(items));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, FromDIP(kItemHeight) * std::max(1, model_.item_count())));

    Bind(wxEVT_PAINT, &ThemedRadio::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedRadio::OnMouseDown, this);
    Bind(wxEVT_MOTION, &ThemedRadio::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedRadio::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &ThemedRadio::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &ThemedRadio::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &ThemedRadio::OnKillFocus, this);
}

void ThemedRadio::set_items(std::vector<RadioItem> items)
{
    model_.set_items(std::move(items));
    Refresh();
}
void ThemedRadio::set_selected_index(int index)
{
    model_.set_selected_index(index);
    Refresh();
}
void ThemedRadio::set_on_change(ChangeCallback callback)
{
    on_change_ = std::move(callback);
}
void ThemedRadio::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}
auto ThemedRadio::model() const -> const RadioGroupModel&
{
    return model_;
}

void ThemedRadio::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedRadio::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    FillBackground(pdc, core::ThemeColorToken::BgPanel);

    const int item_h = FromDIP(kItemHeight);
    const int circle_r = FromDIP(kCircleRadius);
    const int dot_r = FromDIP(kDotRadius);
    const int gap = FromDIP(kGap);

    for (int i = 0; i < model_.item_count(); ++i)
    {
        const auto& item = model_.items()[static_cast<size_t>(i)];
        const int y = i * item_h;
        const int cy = y + item_h / 2;

        const bool selected = (i == model_.selected_index());
        const bool hovered = (i == indexed_state_.hover_index());
        const bool disabled = item.is_disabled || state_.is_disabled();

        // Outer circle
        auto circle_color =
            selected ? core::ThemeColorToken::AccentPrimary : core::ThemeColorToken::BorderLight;
        if (disabled)
            circle_color = core::ThemeColorToken::ControlFgDisabled;

        gc->SetPen(wxPen(theme_engine().color(circle_color), FromDIP(2)));
        gc->SetBrush(*wxTRANSPARENT_BRUSH);
        gc->DrawEllipse(circle_r - circle_r, cy - circle_r, circle_r * 2, circle_r * 2);

        // Inner dot
        if (selected)
        {
            gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawEllipse(circle_r - dot_r, cy - dot_r, dot_r * 2, dot_r * 2);
        }

        // Hover highlight
        if (hovered && !disabled)
        {
            gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::HoverBg)));
            gc->SetPen(*wxTRANSPARENT_PEN);
            gc->DrawRectangle(0, y, sz.x, item_h);
        }

        // Label
        auto fg =
            disabled ? core::ThemeColorToken::ControlFgDisabled : core::ThemeColorToken::TextMain;
        gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel), theme_engine().color(fg));
        double tw = 0, th = 0;
        gc->GetTextExtent(wxString::FromUTF8(item.label), &tw, &th);
        gc->DrawText(wxString::FromUTF8(item.label), circle_r * 2 + gap, cy - th / 2.0);
    }

    state_.acknowledge_change();
}

void ThemedRadio::OnMouseDown(wxMouseEvent& event)
{
    if (state_.is_disabled())
        return;
    int idx = item_at_y(event.GetY());
    if (idx >= 0)
    {
        model_.set_selected_index(idx);
        if (on_change_ && model_.selected_item())
            on_change_(model_.selected_index(), *model_.selected_item());
        Refresh();
    }
}

void ThemedRadio::OnMouseMove(wxMouseEvent& event)
{
    indexed_state_.set_hover(item_at_y(event.GetY()));
    if (indexed_state_.changed())
        Refresh();
}

void ThemedRadio::OnMouseLeave(wxMouseEvent& /*event*/)
{
    indexed_state_.set_hover(-1);
    Refresh();
}

void ThemedRadio::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DOWN)
    {
        model_.select_next();
        if (on_change_ && model_.selected_item())
            on_change_(model_.selected_index(), *model_.selected_item());
        Refresh();
    }
    else if (event.GetKeyCode() == WXK_UP)
    {
        model_.select_previous();
        if (on_change_ && model_.selected_item())
            on_change_(model_.selected_index(), *model_.selected_item());
        Refresh();
    }
    else
        event.Skip();
}

void ThemedRadio::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}
void ThemedRadio::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

auto ThemedRadio::item_at_y(int y) const -> int
{
    const int item_h = FromDIP(kItemHeight);
    int idx = y / item_h;
    if (idx >= 0 && idx < model_.item_count())
        return idx;
    return -1;
}

} // namespace markamp::ui
