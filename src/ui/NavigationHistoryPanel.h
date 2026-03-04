/// @file NavigationHistoryPanel.h
/// @brief V13 Phase 32 Task 17 — Sidebar panel showing navigation history.
#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/NavigationService.h"
#include "core/ThemeEngine.h"

#include <wx/listbox.h>
#include <wx/panel.h>

namespace markamp::ui
{

/// A sidebar panel that displays the navigation history stack.
///
/// Shows all navigation entries chronologically. Current location highlighted.
/// Double-clicking an entry navigates there.
class NavigationHistoryPanel : public wxPanel
{
public:
    NavigationHistoryPanel(wxWindow* parent,
                           core::NavigationService& navigation,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus);

    /// Refresh the list from NavigationService.
    void RefreshHistory();

    /// Clear all history.
    void ClearHistory();

    /// Get the number of displayed entries.
    [[nodiscard]] auto entry_count() const -> int;

private:
    void OnNavigationChanged(const core::events::NavigationChangedEvent& event);
    void OnItemDoubleClick(wxCommandEvent& event);
    void ApplyTheme();

    core::NavigationService& navigation_;
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Subscription nav_sub_;
    core::Subscription theme_sub_;

    wxListBox* list_{nullptr};
};

} // namespace markamp::ui
