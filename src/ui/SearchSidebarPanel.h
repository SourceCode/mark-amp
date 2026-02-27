#pragma once

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "ui/SearchResultsTree.h"
#include "ui/ThemeAwareWindow.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

#include <future>
#include <string>
#include <vector>

namespace markamp::core
{
class ThemeEngine;
class EventBus;
class Config;
struct Theme;
class Subscription;
class WorkspaceSearchEngine;
} // namespace markamp::core

namespace markamp::ui
{

struct DesignSystemContext;
class IconManager;
class EmptyPanelState;
class MatchBadge;
class ThemedScrollbar;
class ToggleActionIconButton;
class SidebarFooter;

/// Search sidebar panel with input field, results list, and replace support.
/// Delegates to existing search infrastructure via events.
class SearchSidebarPanel : public ThemeAwareWindow
{
public:
    SearchSidebarPanel(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus,
                       core::Config* config,
                       DesignSystemContext& ds,
                       IconManager& icon_manager,
                       const std::string& persistence_id = "");
    ~SearchSidebarPanel() override;

    /// Clear search results and input.
    void ClearResults();

    /// Set the search query programmatically.
    void SetQuery(const std::string& query);

    /// Get the current match count.
    [[nodiscard]] auto match_count() const -> int
    {
        return match_count_;
    }

    void SelectNextMatch();
    void SelectPreviousMatch();

    // Callbacks for inline toggles
    void OnSearchOptionsChanged();

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::Config* config_{nullptr};
    DesignSystemContext& ds_;
    IconManager& icon_manager_;
    std::string persistence_id_;

    wxTextCtrl* search_input_{nullptr};
    wxTextCtrl* replace_input_{nullptr};
    SearchResultsTree* results_list_{nullptr};
    EmptyPanelState* empty_state_{nullptr};
    MatchBadge* match_badge_{nullptr};
    ThemedScrollbar* scrollbar_{nullptr};
    SidebarFooter* footer_{nullptr};
    wxTimer scroll_sync_timer_;

    // UI elements for Search/Replace rows
    wxPanel* replace_row_{nullptr};
    wxPanel* filters_panel_{nullptr};
    wxTextCtrl* files_include_input_{nullptr};
    wxTextCtrl* files_exclude_input_{nullptr};

    // State for toggles
    bool use_regex_{false};
    bool match_case_{false};
    bool match_word_{false};
    bool preserve_case_{false};

    core::Subscription action_sub_;
    core::Subscription progress_sub_;
    core::Subscription search_next_sub_;
    core::Subscription search_prev_sub_;

    int match_count_{0};

    // Engine
    std::unique_ptr<core::WorkspaceSearchEngine> search_engine_;
    std::future<void> search_future_;
    wxTimer search_debounce_timer_;

    void ExecuteSearch();
    void OnSearch(wxCommandEvent& event);
    void OnDebouncedSearch(wxTimerEvent& event);
    void OnResultSelected(wxCommandEvent& event);

    void OnScrollSyncTimer(wxTimerEvent& event);
    void OnScrollbarDrag(wxScrollWinEvent& event);
    void UpdateScrollbar();
};

} // namespace markamp::ui
