#pragma once

/// @file TabSwitcherOverlay.h
/// @brief Phase 35 – Ctrl+Tab overlay for cycling between open tabs.

#include "../core/EventBus.h"
#include "../core/ThemeEngine.h"

#include <wx/popupwin.h>

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// An entry in the tab switcher overlay.
struct TabSwitcherEntry
{
    std::string file_path;
    std::string display_name;  ///< Filename for display.
    std::string relative_path; ///< Relative path (dimmed).
    bool is_modified{false};   ///< Whether the file has unsaved changes.
    bool is_preview{false};    ///< Whether this is a preview tab.
};

/// Model for the tab switcher overlay.
class TabSwitcherModel
{
public:
    /// Set the list of tabs in MRU order.
    void set_tabs(std::vector<TabSwitcherEntry> tabs);

    /// Get all tabs.
    [[nodiscard]] auto tabs() const -> const std::vector<TabSwitcherEntry>&
    {
        return tabs_;
    }

    /// Currently selected index.
    [[nodiscard]] auto selected_index() const -> std::size_t
    {
        return selected_index_;
    }

    /// Move selection to next tab (wraps around).
    void select_next();

    /// Move selection to previous tab (wraps around).
    void select_previous();

    /// Set selection directly.
    void set_selected_index(std::size_t index);

    /// Get the selected entry.
    [[nodiscard]] auto selected_entry() const -> const TabSwitcherEntry&;

    /// Whether there are tabs to switch between.
    [[nodiscard]] auto has_tabs() const -> bool
    {
        return tabs_.size() > 1;
    }

    /// Number of tabs.
    [[nodiscard]] auto tab_count() const -> std::size_t
    {
        return tabs_.size();
    }

private:
    std::vector<TabSwitcherEntry> tabs_;
    std::size_t selected_index_{0};
};

/// Floating overlay showing open tabs in MRU order for Ctrl+Tab switching.
class TabSwitcherOverlay : public wxPopupWindow
{
public:
    TabSwitcherOverlay(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus);

    /// Show the overlay with the given tabs.
    void ShowWithTabs(std::vector<TabSwitcherEntry> tabs);

    /// Move selection to next tab.
    void SelectNext();

    /// Move selection to previous tab.
    void SelectPrevious();

    /// Confirm selection and switch to the selected tab.
    void ConfirmSelection();

    /// Dismiss the overlay without switching.
    void Dismiss();

    /// Apply theme colors.
    void ApplyTheme();

    /// Get the model.
    [[nodiscard]] auto model() -> TabSwitcherModel&
    {
        return model_;
    }

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    TabSwitcherModel model_;

    void OnPaint(wxPaintEvent& event);
};

} // namespace markamp::ui
