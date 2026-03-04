#include "CodeActionMenu.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(CodeActionMenu, wxPopupTransientWindow) // NOLINT
    EVT_PAINT(CodeActionMenu::OnPaint) EVT_KEY_DOWN(CodeActionMenu::OnKeyDown)
        EVT_MOTION(CodeActionMenu::OnMouseMove) EVT_LEFT_UP(CodeActionMenu::OnMouseClick)
            EVT_LEAVE_WINDOW(CodeActionMenu::OnMouseLeave) wxEND_EVENT_TABLE() // NOLINT

    CodeActionMenu::CodeActionMenu(wxWindow* parent,
                                   core::ThemeEngine& theme_engine,
                                   CodeActionCallback on_select,
                                   CodeActionHoverCallback on_hover)
    : wxPopupTransientWindow(parent, wxBORDER_NONE)
    , theme_engine_(theme_engine)
    , theme_sub_(theme_engine_.subscribe_theme_change([this](const std::string& /*theme_id*/)
                                                      { ApplyTheme(); }))
    , on_select_(std::move(on_select))
    , on_hover_(std::move(on_hover))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    ApplyTheme();
}

void CodeActionMenu::ShowActions(const core::CodeActionSet& action_set, const wxPoint& position)
{
    items_.clear();
    selected_index_ = -1;
    hover_index_ = -1;

    auto add_category =
        [this](const std::string& header, const std::vector<core::CodeActionInfo>& actions)
    {
        if (actions.empty())
        {
            return;
        }

        MenuItem header_item;
        header_item.type = MenuItemType::kHeader;
        header_item.label = header;
        items_.push_back(std::move(header_item));

        for (const auto& action : actions)
        {
            MenuItem item;
            item.type = MenuItemType::kAction;
            item.label = action.title;
            item.is_preferred = action.is_preferred;
            item.action = &action;
            items_.push_back(std::move(item));
        }
    };

    add_category("Quick Fix", action_set.quick_fixes);
    add_category("Refactor", action_set.refactors);
    add_category("Source Action", action_set.source_actions);

    if (items_.empty())
    {
        MenuItem empty_item;
        empty_item.type = MenuItemType::kHeader;
        empty_item.label = "No code actions available";
        items_.push_back(std::move(empty_item));
    }

    // Select first action (skip header)
    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        if (items_[static_cast<size_t>(idx)].type == MenuItemType::kAction)
        {
            selected_index_ = idx;
            break;
        }
    }

    // Calculate size
    int total_height = 0;
    for (const auto& item : items_)
    {
        total_height += (item.type == MenuItemType::kHeader) ? header_height_ : item_height_;
    }
    total_height += padding_ * 2;

    SetSize(wxSize(menu_width_, total_height));
    SetPosition(position);
    Popup();
    SetFocus();
}

auto CodeActionMenu::GetItemCount() const -> int
{
    return static_cast<int>(items_.size());
}

auto CodeActionMenu::GetSelectedIndex() const -> int
{
    return selected_index_;
}

void CodeActionMenu::OnPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC device_context(this);
    const auto menu_size = GetSize();

    const auto& bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto& border_color = theme_engine_.color(core::ThemeColorToken::BorderLight);
    const auto& text_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    const auto& header_fg_color = theme_engine_.color(core::ThemeColorToken::TextMuted);
    const auto& selected_bg = theme_engine_.color(core::ThemeColorToken::SelectionBg);
    const auto& accent_color = theme_engine_.color(core::ThemeColorToken::AccentPrimary);

    device_context.SetBrush(wxBrush(bg_color));
    device_context.SetPen(wxPen(border_color));
    device_context.DrawRoundedRectangle(0, 0, menu_size.x, menu_size.y, 4);

    int current_y = padding_;
    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        const auto& item = items_[static_cast<size_t>(idx)];

        if (item.type == MenuItemType::kHeader)
        {
            device_context.SetFont(
                wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
            device_context.SetTextForeground(header_fg_color);
            device_context.DrawText(item.label, padding_ + 4, current_y + 4);
            current_y += header_height_;
        }
        else
        {
            if (idx == selected_index_ || idx == hover_index_)
            {
                device_context.SetBrush(wxBrush(selected_bg));
                device_context.SetPen(*wxTRANSPARENT_PEN);
                device_context.DrawRectangle(2, current_y, menu_size.x - 4, item_height_);
            }

            device_context.SetFont(
                wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
            device_context.SetTextForeground(text_color);

            int text_x = padding_ + 4;
            if (item.is_preferred)
            {
                device_context.SetTextForeground(accent_color);
                device_context.DrawText(wxString::FromUTF8("\u2605"), text_x, current_y + 4);
                text_x += 18;
                device_context.SetTextForeground(text_color);
            }

            device_context.DrawText(item.label, text_x, current_y + 4);
            current_y += item_height_;
        }
    }
}

void CodeActionMenu::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
        case WXK_UP:
        {
            int new_index = selected_index_ - 1;
            while (new_index >= 0)
            {
                if (items_[static_cast<size_t>(new_index)].type == MenuItemType::kAction)
                {
                    SelectItem(new_index);
                    break;
                }
                --new_index;
            }
            break;
        }
        case WXK_DOWN:
        {
            int new_index = selected_index_ + 1;
            while (new_index < static_cast<int>(items_.size()))
            {
                if (items_[static_cast<size_t>(new_index)].type == MenuItemType::kAction)
                {
                    SelectItem(new_index);
                    break;
                }
                ++new_index;
            }
            break;
        }
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
        {
            ExecuteSelected();
            break;
        }
        case WXK_ESCAPE:
        {
            Dismiss();
            break;
        }
        default:
        {
            event.Skip();
            break;
        }
    }
}

void CodeActionMenu::OnMouseMove(wxMouseEvent& event)
{
    const int mouse_y = event.GetY();
    int current_y = padding_;

    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        const auto& item = items_[static_cast<size_t>(idx)];
        const int row_height = (item.type == MenuItemType::kHeader) ? header_height_ : item_height_;

        if (mouse_y >= current_y && mouse_y < current_y + row_height)
        {
            if (item.type == MenuItemType::kAction && hover_index_ != idx)
            {
                hover_index_ = idx;
                Refresh();

                if (on_hover_ && item.action != nullptr)
                {
                    on_hover_(*item.action);
                }
            }
            return;
        }
        current_y += row_height;
    }

    if (hover_index_ != -1)
    {
        hover_index_ = -1;
        Refresh();
    }
}

void CodeActionMenu::OnMouseClick(wxMouseEvent& event)
{
    const int mouse_y = event.GetY();
    int current_y = padding_;

    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        const auto& item = items_[static_cast<size_t>(idx)];
        const int row_height = (item.type == MenuItemType::kHeader) ? header_height_ : item_height_;

        if (mouse_y >= current_y && mouse_y < current_y + row_height)
        {
            if (item.type == MenuItemType::kAction)
            {
                selected_index_ = idx;
                ExecuteSelected();
            }
            return;
        }
        current_y += row_height;
    }
}

void CodeActionMenu::OnMouseLeave(wxMouseEvent& /*event*/)
{
    if (hover_index_ != -1)
    {
        hover_index_ = -1;
        Refresh();
    }
}

void CodeActionMenu::ApplyTheme()
{
    Refresh();
}

void CodeActionMenu::SelectItem(int index)
{
    if (index >= 0 && index < static_cast<int>(items_.size()))
    {
        selected_index_ = index;
        Refresh();
    }
}

void CodeActionMenu::ExecuteSelected()
{
    if (selected_index_ >= 0 && selected_index_ < static_cast<int>(items_.size()))
    {
        const auto& item = items_[static_cast<size_t>(selected_index_)];
        if (item.type == MenuItemType::kAction && item.action != nullptr && on_select_)
        {
            on_select_(*item.action);
        }
    }
    Dismiss();
}

} // namespace markamp::ui
