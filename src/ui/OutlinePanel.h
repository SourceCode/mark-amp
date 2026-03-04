#pragma once

/// @file OutlinePanel.h
/// @brief Phase 33 – Document Outline Panel sidebar view.

#include "../core/EventBus.h"
#include "../core/Events.h"
#include "../core/Outline.h"
#include "../core/OutlineService.h"
#include "../core/ThemeEngine.h"
#include "OutlineIconProvider.h"

#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/treectrl.h>

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Sort modes for outline entries.
enum class OutlineSortMode : int
{
    kByPosition = 0, ///< Document order (default).
    kByName          ///< Alphabetical (case-insensitive).
};

/// Data attached to each tree item.
class OutlineItemData : public wxTreeItemData
{
public:
    OutlineItemData(core::OutlineEntry entry_data)
        : entry(std::move(entry_data))
    {
    }

    core::OutlineEntry entry;
};

/// A sidebar panel showing the hierarchical outline of the current document.
class OutlinePanel : public wxPanel
{
public:
    OutlinePanel(wxWindow* parent,
                 core::ThemeEngine& theme_engine,
                 core::EventBus& event_bus,
                 core::OutlineService& outline_service);

    /// Apply theme colors to all panel elements.
    void ApplyTheme();

    /// Set the active document whose outline to display.
    void SetActiveDocument(const std::string& root_id);

    /// Get current sort mode.
    [[nodiscard]] auto sort_mode() const -> OutlineSortMode
    {
        return sort_mode_;
    }

    /// Set sort mode and rebuild.
    void set_sort_mode(OutlineSortMode mode);

    /// Toggle follow-cursor mode.
    void set_follow_cursor(bool enabled);
    [[nodiscard]] auto follow_cursor() const -> bool
    {
        return follow_cursor_;
    }

    /// Get the heading count stats string.
    [[nodiscard]] auto heading_stats_text() const -> std::string;

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::OutlineService& outline_service_;

    wxTreeCtrl* tree_ctrl_{nullptr};
    wxSearchCtrl* search_ctrl_{nullptr};
    wxStaticText* stats_label_{nullptr};
    wxStaticText* empty_label_{nullptr};

    wxTimer search_debounce_timer_;
    wxTimer cursor_throttle_timer_;

    std::string active_root_id_;
    OutlineSortMode sort_mode_{OutlineSortMode::kByPosition};
    bool follow_cursor_{true};
    bool is_stale_{false};
    std::string pending_search_query_;

    // Per-document collapse state: document_id -> set of collapsed heading texts.
    std::unordered_map<std::string, std::set<std::string>> collapse_state_;

    core::Subscription outline_changed_sub_;
    core::Subscription cursor_moved_sub_;

    /// Build/rebuild the tree from current outline data.
    void RebuildTree();
    void RebuildTree(const std::string& filter_query);

    /// Populate tree items from outline nodes.
    void PopulateTree(const std::vector<core::OutlineEntry>& entries,
                      const std::string& filter_query);

    /// Show or hide the empty state.
    void ShowEmptyState(const std::string& message);
    void HideEmptyState();

    /// Save and restore collapse state.
    void SaveCollapseState();
    void RestoreCollapseState();

    /// Handle tree selection change (click-to-navigate).
    void OnTreeSelectionChanged(wxTreeEvent& event);

    /// Handle search input change.
    void OnSearchTextChanged(wxCommandEvent& event);

    /// Handle search debounce timer.
    void OnSearchDebounceTimer(wxTimerEvent& event);

    /// Handle cursor throttle timer.
    void OnCursorThrottleTimer(wxTimerEvent& event);
};

} // namespace markamp::ui
