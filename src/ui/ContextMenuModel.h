#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Standard action category for context menus (Phase 16 Task 1).
enum class MenuActionCategory : uint8_t
{
    kNavigation, ///< Open, Go To, Reveal
    kEdit,       ///< Cut, Copy, Paste, Rename, Delete
    kView,       ///< Toggle, Show/Hide, Zoom
    kCreate,     ///< New File, New Folder
    kRefactor,   ///< Extract, Move, Rename Symbol
    kDebug,      ///< Debug actions
    kOther,      ///< Uncategorized
};

/// A context menu item model.
struct ContextMenuItem
{
    std::string action_id; ///< Unique action identifier
    std::string label;     ///< Display label (normalized)
    std::string shortcut;  ///< Shortcut display string
    MenuActionCategory category{MenuActionCategory::kOther};
    bool is_enabled{true};    ///< Dynamic enablement state
    bool is_visible{true};    ///< Whether to show the item at all
    bool is_separator{false}; ///< Separator line (label/action ignored)
};

/// Testable model for Context Menus (Phase 16).
///
/// Encapsulates:
/// - Menu taxonomy: items sorted by category order
/// - Label normalization (no duplicate verb variants)
/// - Dynamic enablement from context keys
/// - Empty-state fallback menus
class ContextMenuModel
{
public:
    /// Set the full menu item list.
    void set_items(std::vector<ContextMenuItem> items);

    /// Get items sorted by category (preserving insertion order within category).
    [[nodiscard]] auto sorted_items() const -> std::vector<ContextMenuItem>;

    /// Get only visible items (filtered by enablement rules).
    [[nodiscard]] auto visible_items() const -> std::vector<ContextMenuItem>;

    // ── Enablement ──────────────────────────────────────────────────

    /// Set enablement state by action_id.
    void set_enabled(const std::string& action_id, bool enabled);

    /// Set visibility by action_id.
    void set_visible(const std::string& action_id, bool visible);

    // ── Empty state ─────────────────────────────────────────────────

    /// Check if all items are disabled or hidden.
    [[nodiscard]] auto is_empty_state() const -> bool;

    /// Get fallback items for empty-area context menus.
    [[nodiscard]] static auto empty_area_fallbacks() -> std::vector<ContextMenuItem>;

    /// Total item count (excluding separators).
    [[nodiscard]] auto action_count() const -> int;

private:
    std::vector<ContextMenuItem> items_;

    [[nodiscard]] auto find_item(const std::string& action_id) -> ContextMenuItem*;
};

} // namespace markamp::ui
