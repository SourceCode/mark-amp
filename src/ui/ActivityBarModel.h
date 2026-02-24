#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Badge style for an activity bar item.
enum class BadgeStyle : uint8_t
{
    kNone,   ///< No badge
    kCount,  ///< Numeric count (capped at 99+)
    kDot,    ///< Small dot indicator
    kUrgent, ///< Pulsing/attention badge
};

/// An activity bar item's model (decoupled from wxWidgets rendering).
struct ActivityBarItemModel
{
    std::string item_id;         ///< Unique identifier
    std::string label;           ///< Display/tooltip label
    std::string shortcut_hint;   ///< Keyboard shortcut (e.g., "Cmd+Shift+E")
    std::string accessible_name; ///< Screen reader label
    std::string icon_name;       ///< Name of the SVG icon to use for rendering
    bool is_bottom_item{false};  ///< Whether to render this item at the bottom of the rail
    BadgeStyle badge_style{BadgeStyle::kNone};
    int badge_count{0}; ///< Count value (only meaningful when badge_style == kCount)
    bool visible{true}; ///< Whether item is shown
    int order_index{0}; ///< Position in the rail
};

/// Testable model for the Activity Bar control surface (Phase 06).
///
/// Encapsulates:
/// - Keyboard navigation (arrow up/down, Enter activation)
/// - Drag reorder with persistence
/// - Badge normalization (count capping, dot mode, urgency)
/// - Context menu actions (hide/show/reset order)
/// - Accessibility names and announcements
class ActivityBarModel
{
public:
    /// Add an item to the bar.
    void add_item(ActivityBarItemModel item);

    /// Remove an item from the bar.
    void remove_item(const std::string& item_id);

    /// Get all items (in current order, filtered by visibility).
    [[nodiscard]] auto visible_items() const -> std::vector<ActivityBarItemModel>;

    /// Get all items (including hidden), in order.
    [[nodiscard]] auto all_items() const -> const std::vector<ActivityBarItemModel>&;

    /// Total number of items.
    [[nodiscard]] auto item_count() const -> int;

    // ── Keyboard focus ──────────────────────────────────────────────

    /// Current keyboard-focused index among visible items (-1 if none).
    [[nodiscard]] auto focus_index() const -> int;

    /// Move focus (wraps around).
    void focus_next();
    void focus_previous();

    /// Set focus to specific index.
    void set_focus(int index);

    // ── Active item ─────────────────────────────────────────────────

    /// Active (selected) item ID.
    [[nodiscard]] auto active_item_id() const -> std::string;

    /// Activate the currently focused item.
    void activate_focused();

    /// Activate by ID.
    void set_active(const std::string& item_id);

    // ── Drag reorder ────────────────────────────────────────────────

    /// Move item from src_index to dst_index (among visible items).
    void reorder(int src_index, int dst_index);

    // ── Badge management ────────────────────────────────────────────

    /// Set badge on an item.
    void set_badge(const std::string& item_id, BadgeStyle style, int count = 0);

    /// Get the display string for a badge (e.g., "5", "99+", "●", "").
    [[nodiscard]] static auto badge_display(BadgeStyle style, int count) -> std::string;

    // ── Visibility/context menu ─────────────────────────────────────

    /// Hide/show an item.
    void set_item_visible(const std::string& item_id, bool visible);

    /// Reset order to initial registration order.
    void reset_order();

    /// Apply serialized layout (order and visibility).
    void apply_layout(const std::vector<std::pair<std::string, bool>>& layout);

    /// Get current layout for serialization.
    [[nodiscard]] auto get_layout() const -> std::vector<std::pair<std::string, bool>>;

    // ── Accessibility ───────────────────────────────────────────────

    /// Get accessible announcement for the focused item (label + badge state).
    [[nodiscard]] auto focused_announcement() const -> std::string;

private:
    std::vector<ActivityBarItemModel> items_;
    int focus_index_{-1};
    std::string active_item_id_;
};

} // namespace markamp::ui
