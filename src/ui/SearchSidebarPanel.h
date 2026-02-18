#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Search sidebar panel with input field, results list, and replace support.
/// Delegates to existing search infrastructure via events.
class SearchSidebarPanel : public wxPanel
{
public:
    SearchSidebarPanel(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus);

    /// Clear search results and input.
    void ClearResults();

    /// Set the search query programmatically.
    void SetQuery(const std::string& query);

    /// Get the current match count.
    [[nodiscard]] auto match_count() const -> int
    {
        return match_count_;
    }

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;

    wxSearchCtrl* search_input_{nullptr};
    wxTextCtrl* replace_input_{nullptr};
    wxListBox* results_list_{nullptr};
    wxStaticText* match_label_{nullptr};

    int match_count_{0};

    core::Subscription theme_sub_;

    void ApplyTheme();
    void OnSearch(wxCommandEvent& event);
    void OnResultSelected(wxCommandEvent& event);
};

} // namespace markamp::ui
