#include "SearchResultsPanel.h"

#include "../core/Events.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

// ---- SearchResultsModel ----

void SearchResultsModel::set_results(std::vector<SearchResultFileGroup> groups)
{
    groups_ = std::move(groups);
}

auto SearchResultsModel::total_matches() const -> std::size_t
{
    std::size_t total = 0;
    for (const auto& group : groups_)
    {
        total += group.match_count();
    }
    return total;
}

auto SearchResultsModel::file_count() const -> std::size_t
{
    return groups_.size();
}

auto SearchResultsModel::summary_text() const -> std::string
{
    if (groups_.empty())
    {
        return "No results";
    }
    return std::to_string(total_matches()) + " results in " + std::to_string(file_count()) +
           " files";
}

void SearchResultsModel::toggle_match_inclusion(std::size_t group_index, std::size_t match_index)
{
    if (group_index < groups_.size() && match_index < groups_[group_index].matches.size())
    {
        groups_[group_index].matches[match_index].included =
            !groups_[group_index].matches[match_index].included;
    }
}

void SearchResultsModel::toggle_group_collapse(std::size_t group_index)
{
    if (group_index < groups_.size())
    {
        groups_[group_index].collapsed = !groups_[group_index].collapsed;
    }
}

void SearchResultsModel::clear()
{
    groups_.clear();
}

auto SearchResultsModel::included_matches() const -> std::vector<SearchMatchResult>
{
    std::vector<SearchMatchResult> result;
    for (const auto& group : groups_)
    {
        for (const auto& match : group.matches)
        {
            if (match.included)
            {
                result.push_back(match);
            }
        }
    }
    return result;
}

// ---- SearchResultsPanel ----

SearchResultsPanel::SearchResultsPanel(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    summary_label_ = new wxStaticText(this, wxID_ANY, "No results");
    main_sizer->Add(summary_label_, 0, wxEXPAND | wxALL, 4);

    results_list_ = new wxDataViewListCtrl(this, wxID_ANY);
    results_list_->AppendTextColumn("File", wxDATAVIEW_CELL_INERT, 200);
    results_list_->AppendTextColumn("Line", wxDATAVIEW_CELL_INERT, 60);
    results_list_->AppendTextColumn("Match", wxDATAVIEW_CELL_INERT, 400);
    main_sizer->Add(results_list_, 1, wxEXPAND | wxALL, 2);

    SetSizer(main_sizer);
    ApplyTheme();

    // Improvement 13: Wire double-click on search result to navigate to file+line
    results_list_->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED,
                        [this](wxDataViewEvent& evt)
                        {
                            const int row = results_list_->ItemToRow(evt.GetItem());
                            if (row < 0)
                            {
                                return;
                            }

                            // Find the corresponding match in the model
                            int current_row = 0;
                            for (const auto& group : model_.groups())
                            {
                                if (group.collapsed)
                                {
                                    if (current_row == row)
                                    {
                                        // Clicked on a collapsed group header — expand it
                                        return;
                                    }
                                    ++current_row;
                                    continue;
                                }

                                for (const auto& match : group.matches)
                                {
                                    if (current_row == row)
                                    {
                                        // Navigate to this match
                                        core::events::FileOpenRequestEvent open_evt;
                                        open_evt.file_path = match.file_path;
                                        open_evt.line_number = match.line_number;
                                        event_bus_.publish(open_evt);
                                        return;
                                    }
                                    ++current_row;
                                }
                            }
                        });
}

void SearchResultsPanel::ApplyTheme()
{
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);

    SetBackgroundColour(bg_color);
    SetForegroundColour(fg_color);

    if (results_list_ != nullptr)
    {
        results_list_->SetBackgroundColour(bg_color);
        results_list_->SetForegroundColour(fg_color);
    }
    if (summary_label_ != nullptr)
    {
        summary_label_->SetForegroundColour(fg_color);
    }

    Refresh();
}

void SearchResultsPanel::SetResults(std::vector<SearchResultFileGroup> groups)
{
    model_.set_results(std::move(groups));
    RebuildResultsList();
}

void SearchResultsPanel::ClearResults()
{
    model_.clear();
    RebuildResultsList();
}

void SearchResultsPanel::RebuildResultsList()
{
    if (results_list_ == nullptr)
    {
        return;
    }

    results_list_->DeleteAllItems();

    for (const auto& group : model_.groups())
    {
        if (group.collapsed)
        {
            wxVector<wxVariant> row;
            row.push_back(wxVariant(group.display_name + " (" +
                                    std::to_string(group.match_count()) + " matches)"));
            row.push_back(wxVariant(""));
            row.push_back(wxVariant(""));
            results_list_->AppendItem(row);
            continue;
        }

        for (const auto& match : group.matches)
        {
            wxVector<wxVariant> row;
            row.push_back(wxVariant(group.display_name));
            row.push_back(wxVariant(std::to_string(match.line_number)));
            row.push_back(wxVariant(match.match_text));
            results_list_->AppendItem(row);
        }
    }

    if (summary_label_ != nullptr)
    {
        summary_label_->SetLabel(model_.summary_text());
    }
}

} // namespace markamp::ui
