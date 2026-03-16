#include "FindBar.h"

#include "../core/Events.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

FindBar::FindBar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // ---- Search row ----
    auto* search_row = new wxBoxSizer(wxHORIZONTAL);
    search_input_ = new wxTextCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    search_input_->SetHint("Find");
    search_row->Add(search_input_, 1, wxEXPAND | wxRIGHT, 4);

    match_counter_ = new wxStaticText(this, wxID_ANY, "");
    search_row->Add(match_counter_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    main_sizer->Add(search_row, 0, wxEXPAND | wxALL, 4);

    // ---- Replace row (hidden by default) ----
    auto* replace_row = new wxBoxSizer(wxHORIZONTAL);
    replace_input_ = new wxTextCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    replace_input_->SetHint("Replace");
    replace_row->Add(replace_input_, 1, wxEXPAND | wxRIGHT, 4);
    main_sizer->Add(replace_row, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);

    SetSizer(main_sizer);

    // Initially hidden
    replace_input_->Hide();

    // Bind events
    search_input_->Bind(wxEVT_TEXT, &FindBar::OnSearchTextChanged, this);
    replace_input_->Bind(wxEVT_TEXT, &FindBar::OnReplaceTextChanged, this);

    ApplyTheme();
    Hide(); // Start hidden
}

void FindBar::ApplyTheme()
{
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);

    SetBackgroundColour(bg_color);
    SetForegroundColour(fg_color);

    if (search_input_ != nullptr)
    {
        search_input_->SetBackgroundColour(bg_color);
        search_input_->SetForegroundColour(fg_color);
    }
    if (replace_input_ != nullptr)
    {
        replace_input_->SetBackgroundColour(bg_color);
        replace_input_->SetForegroundColour(fg_color);
    }
    if (match_counter_ != nullptr)
    {
        match_counter_->SetForegroundColour(fg_color);
    }

    Refresh();
}

void FindBar::ShowFind(bool with_replace)
{
    state_.replace_visible = with_replace;
    if (replace_input_ != nullptr)
    {
        replace_input_->Show(with_replace);
    }
    Show();
    search_input_->SetFocus();
    GetParent()->Layout();
}

void FindBar::HideFind()
{
    state_.clear_matches();
    Hide();
    GetParent()->Layout();
}

void FindBar::SetSearchQuery(const std::string& query)
{
    state_.search_query = query;
    if (search_input_ != nullptr)
    {
        search_input_->SetValue(query);
    }
}

void FindBar::SetMatchCount(int total, int current)
{
    state_.total_matches = total;
    state_.current_match = current;
    if (match_counter_ != nullptr)
    {
        match_counter_->SetLabel(state_.match_counter_text());
    }
}

void FindBar::OnSearchTextChanged(wxCommandEvent& /*event*/)
{
    state_.search_query = search_input_->GetValue().ToStdString();

    // Improvement 18-19: Trigger real search via EventBus when text changes
    if (!state_.search_query.empty())
    {
        // FindRequestEvent is fieldless — the editor reads the query from FindBarState.
        // Publishing this event tells the editor to execute the find.
        event_bus_.publish(core::events::FindRequestEvent{});
    }
    else
    {
        state_.clear_matches();
        if (match_counter_ != nullptr)
        {
            match_counter_->SetLabel("");
        }
    }
}

void FindBar::OnReplaceTextChanged(wxCommandEvent& /*event*/)
{
    state_.replace_text = replace_input_->GetValue().ToStdString();
}

} // namespace markamp::ui
