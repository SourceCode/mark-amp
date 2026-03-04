#include "ThemedDropdown.h"

#include <wx/dcclient.h>
#include <wx/graphics.h>

#include <algorithm>
#include <set>

namespace markamp::ui
{

// ── DropdownModel ──────────────────────────────────────────────────

void DropdownModel::set_items(std::vector<DropdownItem> items)
{
    items_ = std::move(items);
    if (selected_index_ >= static_cast<int>(items_.size()))
    {
        selected_index_ = -1;
    }
    highlight_index_ = 0;
}

auto DropdownModel::items() const -> const std::vector<DropdownItem>&
{
    return items_;
}

void DropdownModel::set_selected_index(int index)
{
    if (index >= -1 && index < static_cast<int>(items_.size()))
    {
        selected_index_ = index;
    }
}

auto DropdownModel::selected_index() const -> int
{
    return selected_index_;
}

auto DropdownModel::selected_item() const -> const DropdownItem*
{
    if (selected_index_ >= 0 && selected_index_ < static_cast<int>(items_.size()))
    {
        return &items_[static_cast<size_t>(selected_index_)];
    }
    return nullptr;
}

void DropdownModel::set_search_query(const std::string& query)
{
    search_query_ = query;
    highlight_index_ = 0;
}

auto DropdownModel::search_query() const -> const std::string&
{
    return search_query_;
}

auto DropdownModel::filtered_items() const -> std::vector<DropdownItem>
{
    if (search_query_.empty())
        return items_;

    std::vector<DropdownItem> result;
    auto query_lower = search_query_;
    std::transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);

    for (const auto& item : items_)
    {
        auto label_lower = item.label;
        std::transform(label_lower.begin(), label_lower.end(), label_lower.begin(), ::tolower);
        if (label_lower.find(query_lower) != std::string::npos)
        {
            result.push_back(item);
        }
    }
    return result;
}

auto DropdownModel::groups() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::set<std::string> seen;
    for (const auto& item : items_)
    {
        if (!item.group.empty() && seen.insert(item.group).second)
        {
            result.push_back(item.group);
        }
    }
    return result;
}

void DropdownModel::set_open(bool open)
{
    is_open_ = open;
    if (open)
    {
        search_query_.clear();
        highlight_index_ = selected_index_ >= 0 ? selected_index_ : 0;
    }
}

auto DropdownModel::is_open() const -> bool
{
    return is_open_;
}

void DropdownModel::set_highlight_index(int index)
{
    const auto filtered = filtered_items();
    if (index >= 0 && index < static_cast<int>(filtered.size()))
    {
        highlight_index_ = index;
    }
}

auto DropdownModel::highlight_index() const -> int
{
    return highlight_index_;
}

void DropdownModel::move_highlight(int delta)
{
    const auto filtered = filtered_items();
    if (filtered.empty())
        return;

    int new_index = highlight_index_ + delta;
    if (new_index < 0)
        new_index = 0;
    if (new_index >= static_cast<int>(filtered.size()))
        new_index = static_cast<int>(filtered.size()) - 1;

    // Skip separators and disabled items
    while (new_index >= 0 && new_index < static_cast<int>(filtered.size()))
    {
        if (!filtered[static_cast<size_t>(new_index)].is_separator &&
            !filtered[static_cast<size_t>(new_index)].is_disabled)
        {
            break;
        }
        new_index += (delta > 0) ? 1 : -1;
    }

    if (new_index >= 0 && new_index < static_cast<int>(filtered.size()))
    {
        highlight_index_ = new_index;
    }
}

void DropdownModel::select_highlighted()
{
    const auto filtered = filtered_items();
    if (highlight_index_ >= 0 && highlight_index_ < static_cast<int>(filtered.size()))
    {
        // Find original index
        const auto& highlighted = filtered[static_cast<size_t>(highlight_index_)];
        for (int i = 0; i < static_cast<int>(items_.size()); ++i)
        {
            if (items_[static_cast<size_t>(i)].id == highlighted.id)
            {
                selected_index_ = i;
                break;
            }
        }
        is_open_ = false;
    }
}

auto DropdownModel::item_count() const -> int
{
    return static_cast<int>(items_.size());
}

// ── ThemedDropdown ─────────────────────────────────────────────────

ThemedDropdown::ThemedDropdown(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               std::vector<DropdownItem> items)
    : ThemeAwareWindow(parent,
                       theme_engine,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL | wxWANTS_CHARS)
{
    model_.set_items(std::move(items));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetMinSize(wxSize(-1, FromDIP(kHeight)));

    Bind(wxEVT_PAINT, &ThemedDropdown::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &ThemedDropdown::OnMouseDown, this);
    Bind(wxEVT_ENTER_WINDOW, &ThemedDropdown::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ThemedDropdown::OnMouseLeave, this);
    Bind(wxEVT_KEY_DOWN, &ThemedDropdown::OnKeyDown, this);
    Bind(wxEVT_SET_FOCUS, &ThemedDropdown::OnSetFocus, this);
    Bind(wxEVT_KILL_FOCUS, &ThemedDropdown::OnKillFocus, this);
}

void ThemedDropdown::set_items(std::vector<DropdownItem> items)
{
    model_.set_items(std::move(items));
    Refresh();
}

void ThemedDropdown::set_selected_index(int index)
{
    model_.set_selected_index(index);
    Refresh();
}

auto ThemedDropdown::selected_index() const -> int
{
    return model_.selected_index();
}

auto ThemedDropdown::selected_item() const -> const DropdownItem*
{
    return model_.selected_item();
}

void ThemedDropdown::set_on_select(SelectCallback callback)
{
    on_select_ = std::move(callback);
}

void ThemedDropdown::set_enabled(bool enabled)
{
    state_.set_disabled(!enabled);
    Refresh();
}

auto ThemedDropdown::model() const -> const DropdownModel&
{
    return model_;
}

void ThemedDropdown::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    Refresh();
}

void ThemedDropdown::OnPaint(wxPaintEvent& /*event*/)
{
    wxPaintDC pdc(this);
    auto gc = std::unique_ptr<wxGraphicsContext>(wxGraphicsContext::Create(pdc));
    if (!gc)
        return;

    const auto sz = GetClientSize();
    const double r = FromDIP(kBorderRadius);
    const int padH = FromDIP(kPaddingH);

    // Background
    auto bg_token =
        state_.is_hover() ? core::ThemeColorToken::HoverBg : core::ThemeColorToken::BgInput;
    gc->SetBrush(wxBrush(theme_engine().color(bg_token)));
    gc->SetPen(*wxTRANSPARENT_PEN);
    auto path = gc->CreatePath();
    path.AddRoundedRectangle(0, 0, sz.x, sz.y, r);
    gc->FillPath(path);

    // Border
    auto border_token = state_.is_focused() ? core::ThemeColorToken::FocusRingColor
                                            : core::ThemeColorToken::BorderLight;
    gc->SetPen(wxPen(theme_engine().color(border_token), 1));
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    auto border_path = gc->CreatePath();
    border_path.AddRoundedRectangle(0.5, 0.5, sz.x - 1, sz.y - 1, r);
    gc->StrokePath(border_path);

    // Label
    auto fg_token = state_.is_disabled() ? core::ThemeColorToken::ControlFgDisabled
                                         : core::ThemeColorToken::TextMain;
    gc->SetFont(theme_engine().font(core::ThemeFontToken::UILabel),
                theme_engine().color(fg_token));

    const auto* selected = model_.selected_item();
    const auto display = selected ? selected->label : std::string("Select...");
    double tw = 0, th = 0;
    gc->GetTextExtent(wxString::FromUTF8(display), &tw, &th);
    gc->DrawText(wxString::FromUTF8(display), padH, (sz.y - th) / 2.0);

    // Chevron
    const double chevron_x = sz.x - padH - 8;
    const double chevron_y = sz.y / 2.0;
    gc->SetPen(wxPen(theme_engine().color(core::ThemeColorToken::TextMuted), 1));
    gc->StrokeLine(chevron_x - 3, chevron_y - 2, chevron_x, chevron_y + 2);
    gc->StrokeLine(chevron_x, chevron_y + 2, chevron_x + 3, chevron_y - 2);

    state_.acknowledge_change();
}

void ThemedDropdown::OnMouseDown(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        model_.set_open(!model_.is_open());
        SetFocus();
        Refresh();
    }
}

void ThemedDropdown::OnMouseEnter(wxMouseEvent& /*event*/)
{
    if (!state_.is_disabled())
    {
        state_.on_mouse_enter();
        SetControlCursor(ControlCursorType::kHand);
        Refresh();
    }
}

void ThemedDropdown::OnMouseLeave(wxMouseEvent& /*event*/)
{
    state_.on_mouse_leave();
    Refresh();
}

void ThemedDropdown::OnKeyDown(wxKeyEvent& event)
{
    if (state_.is_disabled())
    {
        event.Skip();
        return;
    }

    switch (event.GetKeyCode())
    {
        case WXK_DOWN:
            if (!model_.is_open())
            {
                model_.set_open(true);
            }
            else
            {
                model_.move_highlight(1);
            }
            Refresh();
            break;
        case WXK_UP:
            model_.move_highlight(-1);
            Refresh();
            break;
        case WXK_RETURN:
        case WXK_SPACE:
            if (model_.is_open())
            {
                model_.select_highlighted();
                if (on_select_ && model_.selected_item())
                {
                    on_select_(model_.selected_index(), *model_.selected_item());
                }
            }
            else
            {
                model_.set_open(true);
            }
            Refresh();
            break;
        case WXK_ESCAPE:
            model_.set_open(false);
            Refresh();
            break;
        default:
            event.Skip();
            break;
    }
}

void ThemedDropdown::OnSetFocus(wxFocusEvent& /*event*/)
{
    state_.on_focus();
    Refresh();
}

void ThemedDropdown::OnKillFocus(wxFocusEvent& /*event*/)
{
    state_.on_blur();
    model_.set_open(false);
    Refresh();
}

} // namespace markamp::ui
