#include "TreeViewItem.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

namespace markamp::ui
{

void TreeViewItemModel::set_label(const std::string& label)
{
    label_ = label;
}
auto TreeViewItemModel::label() const -> const std::string&
{
    return label_;
}
void TreeViewItemModel::set_icon_name(const std::string& icon_name)
{
    icon_name_ = icon_name;
}
auto TreeViewItemModel::icon_name() const -> const std::string&
{
    return icon_name_;
}
void TreeViewItemModel::set_expandable(bool expandable)
{
    expandable_ = expandable;
}
auto TreeViewItemModel::is_expandable() const -> bool
{
    return expandable_;
}
void TreeViewItemModel::set_expanded(bool expanded)
{
    expanded_ = expanded;
}
auto TreeViewItemModel::is_expanded() const -> bool
{
    return expanded_;
}
void TreeViewItemModel::toggle_expanded()
{
    if (expandable_)
        expanded_ = !expanded_;
}
void TreeViewItemModel::set_depth(int depth)
{
    depth_ = depth;
}
auto TreeViewItemModel::depth() const -> int
{
    return depth_;
}
void TreeViewItemModel::set_selected(bool selected)
{
    selected_ = selected;
}
auto TreeViewItemModel::is_selected() const -> bool
{
    return selected_;
}
void TreeViewItemModel::set_hover_actions(std::vector<TreeViewAction> actions)
{
    hover_actions_ = std::move(actions);
}
auto TreeViewItemModel::hover_actions() const -> const std::vector<TreeViewAction>&
{
    return hover_actions_;
}
auto TreeViewItemModel::indent_pixels() const -> int
{
    return depth_ * kIndentPerLevel;
}

TreeViewItem::TreeViewItem(wxWindow* parent, core::ThemeEngine& theme_engine)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, FromDIP(kRowHeight)));

    Bind(wxEVT_PAINT, &TreeViewItem::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &TreeViewItem::OnMouseDown, this);
    Bind(wxEVT_ENTER_WINDOW, &TreeViewItem::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &TreeViewItem::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &TreeViewItem::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &TreeViewItem::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &TreeViewItem::OnKillFocus, this);
}

void TreeViewItem::set_label(const std::string& label)
{
    model_.set_label(label);
    Refresh();
}
void TreeViewItem::set_icon_name(const std::string& icon_name)
{
    model_.set_icon_name(icon_name);
    Refresh();
}
void TreeViewItem::set_expandable(bool expandable)
{
    model_.set_expandable(expandable);
    Refresh();
}
void TreeViewItem::set_expanded(bool expanded)
{
    model_.set_expanded(expanded);
    Refresh();
}
void TreeViewItem::set_depth(int depth)
{
    model_.set_depth(depth);
    Refresh();
}
void TreeViewItem::set_selected(bool selected)
{
    model_.set_selected(selected);
    Refresh();
}
void TreeViewItem::set_hover_actions(std::vector<TreeViewAction> actions)
{
    model_.set_hover_actions(std::move(actions));
    Refresh();
}
void TreeViewItem::set_on_expand(ExpandCallback callback)
{
    on_expand_ = std::move(callback);
}
void TreeViewItem::set_on_action(ActionCallback callback)
{
    on_action_ = std::move(callback);
}
void TreeViewItem::set_on_select(SelectCallback callback)
{
    on_select_ = std::move(callback);
}
auto TreeViewItem::model() const -> const TreeViewItemModel&
{
    return model_;
}

void TreeViewItem::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void TreeViewItem::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const int indent = FromDIP(model_.indent_pixels());
    const int chevron_sz = FromDIP(kChevronSize);
    const int icon_sz = FromDIP(kIconSize);
    const int gap = FromDIP(kGap);

    // Row background
    auto bg = model_.is_selected() ? core::ThemeColorToken::ControlBgPressed
              : state_.is_hover()  ? core::ThemeColorToken::HoverBg
                                   : core::ThemeColorToken::BgPanel;
    gc->SetBrush(wxBrush(theme_engine().color(bg)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawRectangle(0, 0, sz.x, sz.y);

    int x = indent;

    // Chevron
    if (model_.is_expandable())
    {
        gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::TextMuted), 1));
        const double cy = sz.y / 2.0;
        if (model_.is_expanded())
        {
            // Down chevron
            gc->StrokeLine(x + 2, cy - 2, x + chevron_sz / 2, cy + 3);
            gc->StrokeLine(x + chevron_sz / 2, cy + 3, x + chevron_sz - 2, cy - 2);
        }
        else
        {
            // Right chevron
            gc->StrokeLine(x + 3, cy - chevron_sz / 2 + 2, x + chevron_sz - 1, cy);
            gc->StrokeLine(x + chevron_sz - 1, cy, x + 3, cy + chevron_sz / 2 - 2);
        }
    }
    x += chevron_sz + gap;

    // Label
    auto fg = core::ThemeColorToken::TextMain;
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel), theme_engine().color(fg));
    double tw = 0, th = 0;
    gc->GetTextExtent(wxString::FromUTF8(model_.label()), &tw, &th);
    gc->DrawText(wxString::FromUTF8(model_.label()), x + icon_sz + gap, (sz.y - th) / 2.0);

    // Hover actions (shown only on hover)
    if (state_.is_hover() && !model_.hover_actions().empty())
    {
        const int action_sz = FromDIP(kActionButtonSize);
        int ax = sz.x - FromDIP(4);
        for (auto it = model_.hover_actions().rbegin(); it != model_.hover_actions().rend(); ++it)
        {
            ax -= action_sz + gap;
            // Draw action button placeholder
            gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::TextMuted), 1));
            gc->SetBrush(*wxTRANSPARENT_BRUSH);
            gc->DrawRectangle(ax, (sz.y - action_sz) / 2, action_sz, action_sz);
        }
    }

    // Focus indicator
    if (state_.is_focused())
        DrawFocusRing(pdc);
    state_.acknowledge_change();
}

void TreeViewItem::OnMouseDown(wxMouseEvent& event)
{
    SetFocus();
    const int indent = FromDIP(model_.indent_pixels());
    const int chevron_sz = FromDIP(kChevronSize);

    // Check if click was on chevron
    if (model_.is_expandable() && event.GetX() >= indent && event.GetX() < indent + chevron_sz)
    {
        model_.toggle_expanded();
        if (on_expand_)
            on_expand_(model_.is_expanded());
    }
    else
    {
        // Check if click was on an action button
        const int action_sz = FromDIP(kActionButtonSize);
        const int gap = FromDIP(kGap);
        int ax = GetClientSize().x - FromDIP(4);
        for (auto it = model_.hover_actions().rbegin(); it != model_.hover_actions().rend(); ++it)
        {
            ax -= action_sz + gap;
            if (event.GetX() >= ax && event.GetX() < ax + action_sz)
            {
                if (on_action_)
                    on_action_(it->id);
                Refresh();
                return;
            }
        }

        if (on_select_)
            on_select_();
    }
    Refresh();
}

void TreeViewItem::OnMouseEnter(wxMouseEvent& /*event*/)
{
    state_.on_mouse_enter();
    SetControlCursor(ControlCursorType::kHand);
    Refresh();
}
void TreeViewItem::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

void TreeViewItem::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_LEFT:
            if (model_.is_expanded())
            {
                model_.set_expanded(false);
                if (on_expand_)
                    on_expand_(false);
                Refresh();
            }
            break;
        case WXK_RIGHT:
            if (model_.is_expandable() && !model_.is_expanded())
            {
                model_.set_expanded(true);
                if (on_expand_)
                    on_expand_(true);
                Refresh();
            }
            break;
        case WXK_RETURN:
        case WXK_SPACE:
            if (on_select_)
                on_select_();
            break;
        default:
            event.Skip();
            break;
    }
}

void TreeViewItem::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}
void TreeViewItem::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    Refresh();
}

} // namespace markamp::ui
