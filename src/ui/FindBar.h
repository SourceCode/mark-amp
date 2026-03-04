#pragma once

/// @file FindBar.h
/// @brief Phase 34 – In-file find bar with replace support.

#include "../core/EventBus.h"
#include "../core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include <cstdint>
#include <string>

namespace markamp::ui
{

/// Find scope options.
enum class FindScope : std::uint8_t
{
    kDocument = 0, ///< Entire document.
    kSelection     ///< Selected text only.
};

/// State model for the find bar.
struct FindBarState
{
    std::string search_query;
    std::string replace_text;
    bool case_sensitive{false};
    bool whole_word{false};
    bool use_regex{false};
    bool preserve_case{false};
    bool replace_visible{false};
    FindScope scope{FindScope::kDocument};

    int total_matches{0};
    int current_match{0}; ///< 1-based index of current match.

    /// Return "N of M" match counter text.
    [[nodiscard]] auto match_counter_text() const -> std::string
    {
        if (search_query.empty())
        {
            return "";
        }
        if (total_matches == 0)
        {
            return "No results";
        }
        return std::to_string(current_match) + " of " + std::to_string(total_matches);
    }

    /// Navigate to next match (wraps around).
    void next_match()
    {
        if (total_matches == 0)
        {
            return;
        }
        current_match = (current_match % total_matches) + 1;
    }

    /// Navigate to previous match (wraps around).
    void prev_match()
    {
        if (total_matches == 0)
        {
            return;
        }
        current_match = current_match <= 1 ? total_matches : current_match - 1;
    }

    /// Reset match state.
    void clear_matches()
    {
        total_matches = 0;
        current_match = 0;
    }
};

/// In-file find bar embedded at the top of the editor.
class FindBar : public wxPanel
{
public:
    FindBar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Apply theme colors.
    void ApplyTheme();

    /// Show the find bar (optionally with replace row).
    void ShowFind(bool with_replace = false);

    /// Hide the find bar.
    void HideFind();

    /// Get the current find bar state.
    [[nodiscard]] auto state() const -> const FindBarState&
    {
        return state_;
    }

    /// Set the search query programmatically.
    void SetSearchQuery(const std::string& query);

    /// Set total match count and optionally current match.
    void SetMatchCount(int total, int current = 1);

private:
    core::ThemeEngine& theme_engine_;
    [[maybe_unused]] core::EventBus& event_bus_;

    FindBarState state_;

    wxTextCtrl* search_input_{nullptr};
    wxTextCtrl* replace_input_{nullptr};
    wxStaticText* match_counter_{nullptr};

    void OnSearchTextChanged(wxCommandEvent& event);
    void OnReplaceTextChanged(wxCommandEvent& event);
};

} // namespace markamp::ui
