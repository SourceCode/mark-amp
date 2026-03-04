#pragma once

/**
 * @file ContextMenuBuilder.h
 * @brief Phase 28: Fluent builder for constructing context menus.
 *
 * ContextMenuBuilder provides a type-safe, readable API for constructing
 * ContextMenuModel instances with groups, separators, sub-menus, icons,
 * checked state, dynamic generation, and dynamic enablement.
 */

#include "ui/ContextMenuModel.h"

#include <functional>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Context keys for dynamic enablement (e.g., "hasSelection", "isFile").
using ContextKeys = std::unordered_map<std::string, bool>;

/// Predicate that determines whether a menu item is enabled.
using EnablementPredicate = std::function<bool(const ContextKeys&)>;

/// Callback invoked when a menu action is triggered.
using ActionCallback = std::function<void()>;

/// Generator that produces dynamic menu items at build time.
using DynamicItemGenerator = std::function<std::vector<ContextMenuItem>(const ContextKeys&)>;

/**
 * @brief Fluent builder for constructing context menus.
 *
 * Usage:
 * @code
 *   auto model = ContextMenuBuilder()
 *       .item("Edit", "cut", "Cut", "Ctrl+X")
 *       .icon("cut", "scissors")
 *       .item("Edit", "copy", "Copy", "Ctrl+C")
 *       .separator()
 *       .begin_submenu("Refactor")
 *           .item("Refactor", "extract_method", "Extract Method")
 *           .item("Refactor", "rename_symbol", "Rename Symbol")
 *       .end_submenu()
 *       .when("paste", [](auto& keys) { return keys.count("hasClipboard"); })
 *       .build();
 * @endcode
 */
class ContextMenuBuilder
{
public:
    ContextMenuBuilder() = default;

    /// Add a menu item with category, action_id, label, and optional shortcut.
    auto item(MenuActionCategory category,
              const std::string& action_id,
              const std::string& label,
              const std::string& shortcut = "") -> ContextMenuBuilder&;

    /// Add a separator line.
    auto separator() -> ContextMenuBuilder&;

    /// Add an enablement predicate for a specific action_id.
    auto when(const std::string& action_id, EnablementPredicate predicate) -> ContextMenuBuilder&;

    // ── Phase 28: Extended builder API ──────────────────────────────

    /// Begin a sub-menu group with the given display label.
    auto begin_submenu(const std::string& label) -> ContextMenuBuilder&;

    /// End the current sub-menu group and return to the parent level.
    auto end_submenu() -> ContextMenuBuilder&;

    /// Set the icon name for the most recently added item.
    auto icon(const std::string& action_id, const std::string& icon_name) -> ContextMenuBuilder&;

    /// Set the checked state for the most recently added item.
    auto checked(const std::string& action_id, bool is_checked) -> ContextMenuBuilder&;

    /// Set a disabled tooltip for a specific action_id.
    auto disabled_tooltip(const std::string& action_id, const std::string& tooltip)
        -> ContextMenuBuilder&;

    /// Register a dynamic item generator invoked during build().
    auto dynamic(DynamicItemGenerator generator) -> ContextMenuBuilder&;

    /// Register an action callback for a specific action_id.
    auto on_action(const std::string& action_id, ActionCallback callback) -> ContextMenuBuilder&;

    // ── Build ───────────────────────────────────────────────────────

    /// Build the final ContextMenuModel.
    [[nodiscard]] auto build() const -> ContextMenuModel;

    /// Build the final model with dynamic enablement applied.
    [[nodiscard]] auto build(const ContextKeys& context) const -> ContextMenuModel;

    /// Get registered action callbacks (for wiring to ThemedContextMenu).
    [[nodiscard]] auto build_callbacks() const -> std::unordered_map<std::string, ActionCallback>;

    // ── Introspection ───────────────────────────────────────────────

    /// Get the number of items added (excluding separators).
    [[nodiscard]] auto item_count() const -> int;

    /// Get the number of separators added.
    [[nodiscard]] auto separator_count() const -> int;

    /// Check if any enablement predicates are registered.
    [[nodiscard]] auto has_predicates() const -> bool;

    /// Get the current sub-menu nesting depth.
    [[nodiscard]] auto submenu_depth() const -> int;

    /// Reset the builder to empty state.
    void clear();

private:
    /// Active item insertion target (top-level or sub-menu children).
    [[nodiscard]] auto current_items() -> std::vector<ContextMenuItem>&;

    std::vector<ContextMenuItem> items_;
    std::unordered_map<std::string, EnablementPredicate> predicates_;

    // Phase 28: Extended state
    std::stack<ContextMenuItem*> submenu_stack_; ///< Stack of open sub-menu parents
    std::unordered_map<std::string, ActionCallback> callbacks_;
    std::vector<DynamicItemGenerator> generators_;
};

} // namespace markamp::ui
