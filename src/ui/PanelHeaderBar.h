#pragma once

/**
 * @file PanelHeaderBar.h
 * @brief Phase 27: Reusable action bar widget for panel headers.
 *
 * PanelHeaderBar provides a compact, horizontal strip of icon buttons
 * with themed rendering, hover/press states, overflow handling, and
 * accessibility support. Each panel configures it with its own set
 * of PanelHeaderAction entries.
 */

#include "ui/ThemeAwareWindow.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::ui
{
struct DesignSystemContext;

/// Defines a single action button in a panel header bar.
struct PanelHeaderAction
{
    std::string action_id;        ///< Unique identifier (e.g., "explorer.new_file")
    std::string icon_name;        ///< Icon key for IconManager
    std::string tooltip;          ///< Tooltip text
    std::string shortcut;         ///< Keyboard shortcut display text
    bool is_toggle{false};        ///< If true, renders as a toggle button
    bool is_toggled_on{false};    ///< Current toggle state (only if is_toggle)
    bool is_enabled{true};        ///< Whether the action is enabled
    bool separator_before{false}; ///< Draw a separator before this action
};

/// Callback type for action button clicks.
using PanelActionCallback = std::function<void(const std::string& action_id)>;

/**
 * @brief Compact action bar widget for panel headers.
 *
 * Renders a row of icon buttons with consistent spacing, themed colors,
 * hover/press visual states, separator rendering, and overflow support.
 * Each panel creates and configures one with its own actions.
 */
class PanelHeaderBar : public ThemeAwareWindow
{
public:
    PanelHeaderBar(wxWindow* parent, DesignSystemContext& design_system, core::EventBus& event_bus);

    /// Set the actions displayed in this bar.
    void set_actions(const std::vector<PanelHeaderAction>& actions);

    /// Update a single action's properties.
    void update_action(const std::string& action_id, bool is_enabled, bool is_toggled_on = false);

    /// Register a callback for action clicks.
    void set_on_action(PanelActionCallback callback);

    /// Get all current actions.
    [[nodiscard]] auto actions() const -> const std::vector<PanelHeaderAction>&;

    /// Get the preferred width for all actions laid out.
    [[nodiscard]] auto preferred_width() const -> int;

    /// Returns true if some actions are hidden due to overflow.
    [[nodiscard]] auto has_overflow() const -> bool;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    void OnPaint(wxPaintEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    void RecalculateLayout();
    void DrawActionButton(wxGraphicsContext& graphics_ctx,
                          const PanelHeaderAction& action,
                          const wxRect& rect,
                          bool is_hovered,
                          bool is_pressed,
                          const core::Theme& current_theme) const;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
    DesignSystemContext& ds_;
    core::EventBus& event_bus_;
#pragma clang diagnostic pop

    std::vector<PanelHeaderAction> actions_;
    PanelActionCallback on_action_;

    // Layout state
    struct ActionRect
    {
        std::string action_id;
        wxRect rect;
        bool is_hovered{false};
        bool is_pressed{false};
    };

    std::vector<ActionRect> action_rects_;
    int visible_action_count_{0};
    bool overflow_{false};

    static constexpr int kActionSize = 22;
    static constexpr int kActionGap = 2;
    static constexpr int kSeparatorWidth = 8;
    static constexpr int kBarHeight = 24;
};

} // namespace markamp::ui
