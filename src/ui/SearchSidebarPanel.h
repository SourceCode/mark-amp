#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "ui/ThemeAwareWindow.h"

#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

#include <string>
#include <vector>

namespace markamp::ui
{

struct DesignSystemContext;
class IconManager;
class EmptyPanelState;
class EmptyPanelState;
class MatchBadge;
class ThemedScrollbar;
class SidebarFooter;

/// Search sidebar panel with input field, results list, and replace support.
/// Delegates to existing search infrastructure via events.
class SearchSidebarPanel : public ThemeAwareWindow
{
public:
    SearchSidebarPanel(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus,
                       DesignSystemContext& ds,
                       IconManager& icon_manager);

    /// Clear search results and input.
    void ClearResults();

    /// Set the search query programmatically.
    void SetQuery(const std::string& query);

    /// Get the current match count.
    [[nodiscard]] auto match_count() const -> int
    {
        return match_count_;
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    DesignSystemContext& ds_;
    IconManager& icon_manager_;

    wxSearchCtrl* search_input_{nullptr};
    wxTextCtrl* replace_input_{nullptr};
    wxListBox* results_list_{nullptr};
    EmptyPanelState* empty_state_{nullptr};
    MatchBadge* match_badge_{nullptr};
    ThemedScrollbar* scrollbar_{nullptr};
    SidebarFooter* footer_{nullptr};
    wxTimer scroll_sync_timer_;

    wxTextCtrl* files_include_input_{nullptr};
    wxTextCtrl* files_exclude_input_{nullptr};

    wxCheckBox* regex_cb_{nullptr};
    wxCheckBox* case_cb_{nullptr};
    wxCheckBox* word_cb_{nullptr};

    core::Subscription action_sub_;

    int match_count_{0};

    void OnSearch(wxCommandEvent& event);
    void OnResultSelected(wxCommandEvent& event);

    void OnScrollSyncTimer(wxTimerEvent& event);
    void OnScrollbarDrag(wxScrollWinEvent& event);
    void UpdateScrollbar();
};

} // namespace markamp::ui
