#include "TabSwitcherOverlay.h"

#include "../core/Events.h"

#include <wx/dcbuffer.h>
#include <wx/sizer.h>

namespace markamp::ui
{

// ---- TabSwitcherModel ----

void TabSwitcherModel::set_tabs(std::vector<TabSwitcherEntry> tabs)
{
    tabs_ = std::move(tabs);
    selected_index_ = tabs_.empty() ? 0 : 1; // Start at second (previous file)
    if (selected_index_ >= tabs_.size())
    {
        selected_index_ = 0;
    }
}

void TabSwitcherModel::select_next()
{
    if (tabs_.empty())
    {
        return;
    }
    selected_index_ = (selected_index_ + 1) % tabs_.size();
}

void TabSwitcherModel::select_previous()
{
    if (tabs_.empty())
    {
        return;
    }
    selected_index_ = selected_index_ == 0 ? tabs_.size() - 1 : selected_index_ - 1;
}

void TabSwitcherModel::set_selected_index(std::size_t index)
{
    if (index < tabs_.size())
    {
        selected_index_ = index;
    }
}

auto TabSwitcherModel::selected_entry() const -> const TabSwitcherEntry&
{
    static const TabSwitcherEntry empty_entry;
    if (tabs_.empty() || selected_index_ >= tabs_.size())
    {
        return empty_entry;
    }
    return tabs_[selected_index_];
}

// ---- TabSwitcherOverlay ----

TabSwitcherOverlay::TabSwitcherOverlay(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus)
    : wxPopupWindow(parent)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    SetSize(350, 300);
    Bind(wxEVT_PAINT, &TabSwitcherOverlay::OnPaint, this);
    ApplyTheme();
}

void TabSwitcherOverlay::ShowWithTabs(std::vector<TabSwitcherEntry> tabs)
{
    model_.set_tabs(std::move(tabs));

    if (!model_.has_tabs())
    {
        return;
    }

    // Center on parent
    auto* parent_win = GetParent();
    if (parent_win != nullptr)
    {
        auto parent_rect = parent_win->GetScreenRect();
        auto size = GetSize();
        SetPosition(wxPoint(parent_rect.x + (parent_rect.width - size.GetWidth()) / 2,
                            parent_rect.y + (parent_rect.height - size.GetHeight()) / 2));
    }

    Show();
    Refresh();
}

void TabSwitcherOverlay::SelectNext()
{
    model_.select_next();
    Refresh();
}

void TabSwitcherOverlay::SelectPrevious()
{
    model_.select_previous();
    Refresh();
}

void TabSwitcherOverlay::ConfirmSelection()
{
    if (!model_.tabs().empty())
    {
        const auto& entry = model_.selected_entry();
        core::events::FileOpenRequestEvent evt;
        evt.file_path = entry.file_path;
        event_bus_.publish(evt);
    }
    Hide();
}

void TabSwitcherOverlay::Dismiss()
{
    Hide();
}

void TabSwitcherOverlay::ApplyTheme()
{
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    SetBackgroundColour(bg_color);
}

void TabSwitcherOverlay::OnPaint(wxPaintEvent& /*event*/)
{
    wxBufferedPaintDC dc(this);
    auto size = GetClientSize();

    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    const auto selected_bg = theme_engine_.color(core::ThemeColorToken::AccentPrimary);

    dc.SetBackground(wxBrush(bg_color));
    dc.Clear();

    const int row_height = 32;
    const int padding = 8;
    int y_pos = padding;

    for (std::size_t idx = 0; idx < model_.tabs().size(); ++idx)
    {
        const auto& tab = model_.tabs()[idx];

        // Highlight selected
        if (idx == model_.selected_index())
        {
            dc.SetBrush(wxBrush(selected_bg));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(0, y_pos, size.GetWidth(), row_height);
        }

        dc.SetTextForeground(fg_color);

        // Draw filename
        std::string label = tab.display_name;
        if (tab.is_modified)
        {
            label += " •";
        }
        dc.DrawText(label, padding, y_pos + 4);

        // Draw relative path (dimmed)
        dc.SetTextForeground(wxColour(150, 150, 150));
        dc.DrawText(tab.relative_path, padding + 200, y_pos + 4);

        y_pos += row_height;
    }
}

} // namespace markamp::ui
