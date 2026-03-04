#pragma once

/// @file CodeActionMenu.h
/// @brief V13 Phase 29 — Themed popup menu for displaying code actions.
///
/// Shows categorized code actions with preferred action highlighted,
/// keyboard navigation, and preview-on-hover support.

#include "core/CodeActionTypes.h"
#include "core/ThemeEngine.h"

#include <wx/popupwin.h>
#include <wx/wx.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Callback invoked when a code action is selected from the menu.
using CodeActionCallback = std::function<void(const core::CodeActionInfo& action)>;

/// Callback invoked when hovering over a code action (for preview).
using CodeActionHoverCallback = std::function<void(const core::CodeActionInfo& action)>;

/// Themed popup menu for displaying and selecting code actions.
///
/// Displays actions grouped by category (Quick Fix, Refactor, Source Action)
/// with the preferred action shown first and highlighted with a star icon.
/// Supports keyboard navigation: Up/Down/Enter/Escape.
class CodeActionMenu : public wxPopupTransientWindow
{
public:
    CodeActionMenu(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   CodeActionCallback on_select,
                   CodeActionHoverCallback on_hover = nullptr);

    /// Show the menu with the given action set at the specified position.
    void ShowActions(const core::CodeActionSet& action_set, const wxPoint& position);

    /// Get the number of displayed items (actions + headers).
    [[nodiscard]] auto GetItemCount() const -> int;

    /// Get the currently selected index.
    [[nodiscard]] auto GetSelectedIndex() const -> int;

private:
    void OnPaint(wxPaintEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseClick(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void ApplyTheme();
    void SelectItem(int index);
    void ExecuteSelected();

    /// Item type in the display list.
    enum class MenuItemType
    {
        kHeader, ///< Non-clickable category header
        kAction  ///< Clickable code action
    };

    /// An item in the display list.
    struct MenuItem
    {
        MenuItemType type{MenuItemType::kAction};
        std::string label;
        bool is_preferred{false};
        const core::CodeActionInfo* action{nullptr}; ///< Valid if kAction
    };

    core::ThemeEngine& theme_engine_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    core::Subscription theme_sub_;
    CodeActionCallback on_select_;
    CodeActionHoverCallback on_hover_;

    std::vector<MenuItem> items_;
    int selected_index_{-1};
    int hover_index_{-1};
    int item_height_{28};
    int header_height_{24};
    int padding_{8};
    int menu_width_{280};

    wxDECLARE_EVENT_TABLE(); // NOLINT
};

} // namespace markamp::ui
