#include "SegmentedControl.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

void SegmentedControlModel::set_items(std::vector<SegmentItem> items)
{
    items_ = std::move(items);
    selected_index_ = 0;
}
auto SegmentedControlModel::items() const -> const std::vector<SegmentItem>&
{
    return items_;
}
void SegmentedControlModel::set_selected_index(int index)
{
    if (index >= 0 && index < static_cast<int>(items_.size()))
        selected_index_ = index;
}
auto SegmentedControlModel::selected_index() const -> int
{
    return selected_index_;
}
auto SegmentedControlModel::selected_item() const -> const SegmentItem*
{
    return selected_index_ >= 0 && selected_index_ < static_cast<int>(items_.size())
               ? &items_[static_cast<size_t>(selected_index_)]
               : nullptr;
}
auto SegmentedControlModel::item_count() const -> int
{
    return static_cast<int>(items_.size());
}

SegmentedControl::SegmentedControl(wxWindow* parent,
                                   core::ThemeEngine& theme_engine,
                                   std::vector<SegmentItem> items)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    model_.set_items(std::move(items));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(FromDIP(kPaddingH * 2) * std::max(1, model_.item_count()), FromDIP(kHeight)));

    Bind(wxEVT_PAINT, &SegmentedControl::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &SegmentedControl::OnMouseDown, this);
    Bind(wxEVT_MOTION, &SegmentedControl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, &SegmentedControl::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &SegmentedControl::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &SegmentedControl::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &SegmentedControl::OnKillFocus, this);
}

void SegmentedControl::set_items(std::vector<SegmentItem> items)
{
    model_.set_items(std::move(items));
    Refresh();
}
void SegmentedControl::set_selected_index(int index)
{
    model_.set_selected_index(index);
    Refresh();
}
void SegmentedControl::set_on_change(ChangeCallback callback)
{
    on_change_ = std::move(callback);
}
void SegmentedControl::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}
auto SegmentedControl::model() const -> const SegmentedControlModel&
{
    return model_;
}

void SegmentedControl::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void SegmentedControl::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const double r = FromDIP(kBorderRadius);
    const int seg_w = segment_width();

    // Container background
    gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::ControlBgPressed)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto container = gc->CreatePath();
    container.AddRoundedRectangle(0, 0, sz.x, sz.y, r);
    gc->FillPath(container);

    for (int i = 0; i < model_.item_count(); ++i)
    {
        const int x = i * seg_w;
        const bool selected = (i == model_.selected_index());
        const bool hovered = (i == indexed_state_.hover_index());

        if (selected)
        {
            gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::BgPanel)));
            gc->SetPen(*wxTRANSPARENT_PEN);
            auto seg_path = gc->CreatePath();
            seg_path.AddRoundedRectangle(x + 2, 2, seg_w - 4, sz.y - 4, r - 1);
            gc->FillPath(seg_path);
        }
        else if (hovered)
        {
            gc->SetBrush(wxBrush(theme_engine().color(core::ThemeColorToken::HoverBg)));
            gc->SetPen(*wxTRANSPARENT_PEN);
            auto seg_path = gc->CreatePath();
            seg_path.AddRoundedRectangle(x + 2, 2, seg_w - 4, sz.y - 4, r - 1);
            gc->FillPath(seg_path);
        }

        // Label
        auto fg = selected ? core::ThemeColorToken::TextMain : core::ThemeColorToken::TextMuted;
        if (state_.is_disabled())
            fg = core::ThemeColorToken::ControlFgDisabled;
        gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel),
                    theme_engine().color(fg));
        double tw = 0, th = 0;
        gc->GetTextExtent(
            wxString::FromUTF8(model_.items()[static_cast<size_t>(i)].label), &tw, &th);
        gc->DrawText(wxString::FromUTF8(model_.items()[static_cast<size_t>(i)].label),
                     x + (seg_w - tw) / 2.0,
                     (sz.y - th) / 2.0);
    }

    if (state_.is_focused())
        DrawFocusRing(pdc);
    state_.acknowledge_change();
}

void SegmentedControl::OnMouseDown(wxMouseEvent& event)
{
    if (state_.is_disabled())
        return;
    int idx = item_at_x(event.GetX());
    if (idx >= 0)
    {
        model_.set_selected_index(idx);
        if (on_change_ && model_.selected_item())
            on_change_(model_.selected_index(), *model_.selected_item());
        Refresh();
    }
}

void SegmentedControl::OnMouseMove(wxMouseEvent& event)
{
    indexed_state_.set_hover(item_at_x(event.GetX()));
    if (indexed_state_.changed())
        Refresh();
}
void SegmentedControl::OnMouseLeave(wxMouseEvent& /*event*/)
{
    indexed_state_.set_hover(-1);
    Refresh();
}

void SegmentedControl::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_LEFT)
    {
        model_.set_selected_index(std::max(0, model_.selected_index() - 1));
        if (on_change_ && model_.selected_item())
            on_change_(model_.selected_index(), *model_.selected_item());
        Refresh();
    }
    else if (event.GetKeyCode() == WXK_RIGHT)
    {
        model_.set_selected_index(std::min(model_.item_count() - 1, model_.selected_index() + 1));
        if (on_change_ && model_.selected_item())
            on_change_(model_.selected_index(), *model_.selected_item());
        Refresh();
    }
    else
        event.Skip();
}

void SegmentedControl::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}
void SegmentedControl::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

auto SegmentedControl::item_at_x(int x) const -> int
{
    if (model_.item_count() == 0)
        return -1;
    int idx = x / segment_width();
    return (idx >= 0 && idx < model_.item_count()) ? idx : -1;
}

auto SegmentedControl::segment_width() const -> int
{
    return model_.item_count() > 0 ? GetClientSize().x / model_.item_count() : GetClientSize().x;
}

} // namespace markamp::ui
