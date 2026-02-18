#include "SearchSidebarPanel.h"

#include "core/Logger.h"

#include <wx/button.h>
#include <wx/sizer.h>

namespace markamp::ui
{

SearchSidebarPanel::SearchSidebarPanel(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Search input with magnifying glass
    search_input_ = new wxSearchCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    search_input_->SetDescriptiveText("Search");
    main_sizer->Add(search_input_, 0, wxEXPAND | wxALL, 8);

    // Replace input
    replace_input_ = new wxTextCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    replace_input_->SetHint("Replace");
    main_sizer->Add(replace_input_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Match count label
    match_label_ = new wxStaticText(this, wxID_ANY, "0 results");
    main_sizer->Add(match_label_, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Results list
    results_list_ = new wxListBox(this, wxID_ANY);
    main_sizer->Add(results_list_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    SetSizer(main_sizer);

    // Bind events
    search_input_->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &SearchSidebarPanel::OnSearch, this);
    search_input_->Bind(wxEVT_TEXT_ENTER, &SearchSidebarPanel::OnSearch, this);
    results_list_->Bind(wxEVT_LISTBOX, &SearchSidebarPanel::OnResultSelected, this);

    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void SearchSidebarPanel::ClearResults()
{
    if (results_list_ != nullptr)
    {
        results_list_->Clear();
    }
    match_count_ = 0;
    if (match_label_ != nullptr)
    {
        match_label_->SetLabel("0 results");
    }
}

void SearchSidebarPanel::SetQuery(const std::string& query)
{
    if (search_input_ != nullptr)
    {
        search_input_->SetValue(query);
    }
}

void SearchSidebarPanel::ApplyTheme()
{
    SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::BgPanel));

    if (match_label_ != nullptr)
    {
        match_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    }

    Refresh();
}

void SearchSidebarPanel::OnSearch(wxCommandEvent& /*event*/)
{
    const auto query = search_input_->GetValue().ToStdString();
    if (query.empty())
    {
        ClearResults();
        return;
    }

    MARKAMP_LOG_INFO("SearchSidebarPanel: Searching for '{}'", query);

    // Publish search request event — FindRequestEvent triggers editor search
    core::events::FindRequestEvent find_evt;
    event_bus_.publish(find_evt);

    // Publish search result count for badge
    core::events::SearchResultCountEvent count_evt;
    count_evt.count = 0; // Will be updated by search engine response
    event_bus_.publish(count_evt);
}

void SearchSidebarPanel::OnResultSelected(wxCommandEvent& /*event*/)
{
    const int sel = results_list_->GetSelection();
    if (sel == wxNOT_FOUND)
    {
        return;
    }

    MARKAMP_LOG_INFO("SearchSidebarPanel: Result selected at index {}", sel);
}

} // namespace markamp::ui
