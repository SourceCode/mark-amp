/// @file NavigationHistoryPanel.cpp
/// @brief V13 Phase 32 Task 17 — Navigation history panel implementation.

#include "ui/NavigationHistoryPanel.h"

#include "core/Events.h"

#include <wx/sizer.h>

#include <filesystem>

namespace markamp::ui
{

NavigationHistoryPanel::NavigationHistoryPanel(wxWindow* parent,
                                               core::NavigationService& navigation,
                                               core::ThemeEngine& theme_engine,
                                               core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , navigation_(navigation)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    list_ =
        new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE);

    sizer->Add(list_, 1, wxEXPAND | wxALL, 2);
    SetSizer(sizer);

    // Subscribe to navigation changes
    nav_sub_ = event_bus_.subscribe<core::events::NavigationChangedEvent>(
        [this](const core::events::NavigationChangedEvent& evt) { OnNavigationChanged(evt); });

    // Subscribe to theme changes
    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });

    list_->Bind(wxEVT_LISTBOX_DCLICK, [this](wxCommandEvent& evt) { OnItemDoubleClick(evt); });

    ApplyTheme();
    RefreshHistory();
}

void NavigationHistoryPanel::RefreshHistory()
{
    list_->Clear();

    const auto& entries = navigation_.history();
    const int current_idx = navigation_.current_index();

    for (size_t entry_idx = 0; entry_idx < entries.size(); ++entry_idx)
    {
        const auto& entry = entries[entry_idx];
        std::filesystem::path file_path(entry.document_id);
        std::string display = file_path.filename().string() + ":" + std::to_string(entry.line);

        if (static_cast<int>(entry_idx) == current_idx)
        {
            display = "▸ " + display;
        }
        else
        {
            display = "  " + display;
        }

        list_->Append(display);
    }

    // Select current entry
    if (current_idx >= 0 && current_idx < static_cast<int>(list_->GetCount()))
    {
        list_->SetSelection(current_idx);
    }
}

void NavigationHistoryPanel::ClearHistory()
{
    navigation_.clear();
    RefreshHistory();
}

auto NavigationHistoryPanel::entry_count() const -> int
{
    return static_cast<int>(list_->GetCount());
}

void NavigationHistoryPanel::OnNavigationChanged(
    const core::events::NavigationChangedEvent& /*event*/)
{
    RefreshHistory();
}

void NavigationHistoryPanel::OnItemDoubleClick(wxCommandEvent& /*event*/)
{
    int selection = list_->GetSelection();
    if (selection == wxNOT_FOUND)
    {
        return;
    }

    const auto& entries = navigation_.history();
    if (static_cast<size_t>(selection) < entries.size())
    {
        const auto& entry = entries[static_cast<size_t>(selection)];
        navigation_.go_to(entry.document_id, entry.line, entry.column);
    }
}

void NavigationHistoryPanel::ApplyTheme()
{
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgPanel));
    list_->SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgPanel));
    list_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
    Refresh();
}

} // namespace markamp::ui
