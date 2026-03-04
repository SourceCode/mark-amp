#pragma once

/**
 * @file ThemedContextMenu.h
 * @brief Phase 28: Custom-drawn, theme-aware popup context menu.
 *
 * ThemedContextMenu renders a ContextMenuModel as a themed popup window
 * with icons, shortcut hints, sub-menu arrows, checked indicators,
 * disabled item tooltips, and keyboard navigation.
 *
 * Usage:
 * @code
 *   auto model = builder.build(context_keys);
 *   auto callbacks = builder.build_callbacks();
 *   ThemedContextMenu::Show(this, pos, model, callbacks, design_system);
 * @endcode
 */

#include "ui/ContextMenuBuilder.h"
#include "ui/ContextMenuModel.h"

#include <wx/popupwin.h>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class ThemeEngine;
struct Theme;
} // namespace markamp::core

namespace markamp::ui
{
struct DesignSystemContext;

/// A single menu row computed for rendering.
struct MenuRow
{
    ContextMenuItem item;
    wxRect rect;
    bool is_hovered{false};
};

/**
 * @brief Theme-aware popup context menu renderer.
 *
 * Replaces native wxMenu with a custom-drawn popup that matches
 * the application's theme and supports icons, sub-menus, checked
 * items, and keyboard navigation.
 */
class ThemedContextMenu : public wxPopupTransientWindow
{
public:
    ThemedContextMenu(wxWindow* parent,
                      DesignSystemContext& design_system,
                      core::ThemeEngine& theme_engine);

    /// Show a context menu at the given screen position.
    static void Show(wxWindow* parent,
                     const wxPoint& screen_pos,
                     const ContextMenuModel& model,
                     const std::unordered_map<std::string, ActionCallback>& callbacks,
                     DesignSystemContext& design_system,
                     core::ThemeEngine& theme_engine);

    /// Set the menu model to display.
    void SetModel(const ContextMenuModel& model);

    /// Set action callbacks for each menu item.
    void SetCallbacks(const std::unordered_map<std::string, ActionCallback>& callbacks);

    /// Layout and show at the given screen position.
    void ShowAt(const wxPoint& screen_pos);

    // Dimensions
    static constexpr int kItemHeight = 28;
    static constexpr int kSeparatorHeight = 9;
    static constexpr int kIconColumnWidth = 24;
    static constexpr int kShortcutGap = 32;
    static constexpr int kSubmenuArrowWidth = 16;
    static constexpr int kPadH = 8;
    static constexpr int kPadV = 4;
    static constexpr int kMinWidth = 180;
    static constexpr int kMaxWidth = 360;
    static constexpr int kCornerRadius = 6;

private:
    void OnPaint(wxPaintEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    void RecalculateLayout();
    void DrawMenuItem(wxGraphicsContext& graphics_ctx,
                      const MenuRow& row,
                      const core::Theme& current_theme) const;
    void DrawSeparator(wxGraphicsContext& graphics_ctx,
                       const wxRect& rect,
                       const core::Theme& current_theme) const;
    void DrawCheckmark(wxGraphicsContext& graphics_ctx,
                       double pos_x,
                       double pos_y,
                       double size,
                       const wxColour& color) const;
    void DrawSubmenuArrow(wxGraphicsContext& graphics_ctx,
                          double pos_x,
                          double pos_y,
                          double size,
                          const wxColour& color) const;

    void InvokeAction(const std::string& action_id);
    void ShowSubmenuAt(const ContextMenuItem& parent_item, const wxRect& parent_rect);

    DesignSystemContext& design_system_;
    core::ThemeEngine& theme_engine_;

    std::vector<MenuRow> rows_;
    std::unordered_map<std::string, ActionCallback> callbacks_;
    int hovered_index_{-1};
    int menu_width_{kMinWidth};
};

} // namespace markamp::ui
