#pragma once

#include "core/EventBus.h"
#include "core/ShortcutManager.h"
#include "core/ThemeEngine.h"

#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Modal dialog for viewing and editing keyboard shortcuts.
/// Displays a table of all registered commands with their current bindings,
/// supports search/filter by command name or key, and click-to-record rebinding.
///
/// Batch 5C Task 12: Provides a dedicated shortcut editor inspired by
/// VS Code's keyboard shortcuts view.
class ShortcutEditor : public wxDialog
{
public:
    ShortcutEditor(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   core::EventBus& event_bus,
                   core::ShortcutManager& shortcut_manager);

    /// Reload the shortcut list from ShortcutManager.
    void ReloadShortcuts();

    /// Filter the list by a search query.
    void FilterByQuery(const std::string& query);

private:
    core::ThemeEngine& theme_engine_;
    [[maybe_unused]] core::EventBus& event_bus_;
    core::ShortcutManager& shortcut_manager_;

    wxSearchCtrl* search_ctrl_{nullptr};
    wxListCtrl* shortcut_list_{nullptr};
    wxStaticText* status_label_{nullptr};

    /// Cached list of shortcuts for display/filtering.
    std::vector<core::Shortcut> all_shortcuts_;

    void CreateLayout();
    void PopulateList(const std::string& filter = "");
    void OnSearchChanged(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    void OnResetAll(wxCommandEvent& event);
    void ApplyTheme();

    /// Start recording a new key binding for the shortcut at the given index.
    void RecordNewBinding(long item_index);
};

} // namespace markamp::ui
