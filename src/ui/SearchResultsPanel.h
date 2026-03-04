#pragma once

/// @file SearchResultsPanel.h
/// @brief Phase 34 – Bottom panel for project-wide search results.

#include "../core/EventBus.h"
#include "../core/ThemeEngine.h"

#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/stattext.h>

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A single search match result.
struct SearchMatchResult
{
    std::string file_path;
    int line_number{0};
    int column{0};
    std::string match_text;
    std::string context_before;  ///< 1-2 lines above match.
    std::string context_after;   ///< 1-2 lines below match.
    std::string replace_preview; ///< Preview of replacement text.
    bool included{true};         ///< Whether to include in batch replace.
};

/// A group of search results for a single file.
struct SearchResultFileGroup
{
    std::string file_path;
    std::string display_name; ///< Filename for display.
    std::vector<SearchMatchResult> matches;
    bool collapsed{false};

    [[nodiscard]] auto match_count() const -> std::size_t
    {
        return matches.size();
    }

    [[nodiscard]] auto included_count() const -> std::size_t
    {
        std::size_t count = 0;
        for (const auto& match : matches)
        {
            if (match.included)
            {
                ++count;
            }
        }
        return count;
    }
};

/// Model for project-wide search results.
class SearchResultsModel
{
public:
    /// Set all results, grouped by file.
    void set_results(std::vector<SearchResultFileGroup> groups);

    /// Get all result groups.
    [[nodiscard]] auto groups() const -> const std::vector<SearchResultFileGroup>&
    {
        return groups_;
    }

    /// Total number of matches across all files.
    [[nodiscard]] auto total_matches() const -> std::size_t;

    /// Number of files with matches.
    [[nodiscard]] auto file_count() const -> std::size_t;

    /// Summary text: "N results in M files".
    [[nodiscard]] auto summary_text() const -> std::string;

    /// Toggle inclusion of a specific match for batch replace.
    void toggle_match_inclusion(std::size_t group_index, std::size_t match_index);

    /// Toggle collapse state of a file group.
    void toggle_group_collapse(std::size_t group_index);

    /// Clear all results.
    void clear();

    /// Get all included matches for batch replace.
    [[nodiscard]] auto included_matches() const -> std::vector<SearchMatchResult>;

private:
    std::vector<SearchResultFileGroup> groups_;
};

/// Bottom panel showing search results in a tree/table format.
class SearchResultsPanel : public wxPanel
{
public:
    SearchResultsPanel(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus);

    /// Apply theme colors.
    void ApplyTheme();

    /// Set results to display.
    void SetResults(std::vector<SearchResultFileGroup> groups);

    /// Clear results.
    void ClearResults();

    /// Get the underlying model.
    [[nodiscard]] auto model() -> SearchResultsModel&
    {
        return model_;
    }

private:
    core::ThemeEngine& theme_engine_;
    [[maybe_unused]] core::EventBus& event_bus_;
    SearchResultsModel model_;

    wxDataViewListCtrl* results_list_{nullptr};
    [[maybe_unused]] wxStaticText* summary_label_{nullptr};

    void RebuildResultsList();
};

} // namespace markamp::ui
