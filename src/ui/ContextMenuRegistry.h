#pragma once

/**
 * @file ContextMenuRegistry.h
 * @brief Phase 28: Centralized registry mapping context types to menu builders.
 *
 * ContextMenuRegistry maps string context identifiers (e.g., "editor",
 * "file_tree", "tab_bar") to factory functions that produce a
 * ContextMenuBuilder. This allows any UI component to request a context
 * menu by name and receive a correctly-configured builder.
 */

#include "ui/ContextMenuBuilder.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Factory function that produces a ContextMenuBuilder for a given context.
using MenuFactory = std::function<ContextMenuBuilder()>;

/**
 * @brief Centralized registry for context menu definitions.
 *
 * Usage:
 * @code
 *   auto& registry = ContextMenuRegistry::instance();
 *   registry.register_menu("editor", editor_context_menu);
 *   registry.register_menu("file_tree", file_tree_context_menu);
 *
 *   auto builder = registry.get("editor");
 *   auto model = builder.build(context_keys);
 * @endcode
 */
class ContextMenuRegistry
{
public:
    /// Get the singleton instance.
    static auto instance() -> ContextMenuRegistry&;

    /// Register a menu factory for a context type.
    void register_menu(const std::string& context_type, MenuFactory factory);

    /// Unregister a menu factory.
    void unregister_menu(const std::string& context_type);

    /// Get a builder for the given context type.
    /// Returns empty builder if context not registered.
    [[nodiscard]] auto get(const std::string& context_type) const -> ContextMenuBuilder;

    /// Check if a context type is registered.
    [[nodiscard]] auto has(const std::string& context_type) const -> bool;

    /// Get all registered context type names.
    [[nodiscard]] auto registered_types() const -> std::vector<std::string>;

    /// Get the number of registered menu types.
    [[nodiscard]] auto count() const -> int;

    /// Clear all registrations (primarily for testing).
    void clear();

private:
    ContextMenuRegistry() = default;

    std::unordered_map<std::string, MenuFactory> factories_;
};

// ═══════════════════════════════════════════════════════
// Built-in context menu factories
// ═══════════════════════════════════════════════════════

namespace context_menus
{

/// Editor text area context menu.
auto editor_menu() -> ContextMenuBuilder;

/// File tree context menu (file selected).
auto file_tree_file_menu() -> ContextMenuBuilder;

/// File tree context menu (folder selected).
auto file_tree_folder_menu() -> ContextMenuBuilder;

/// File tree context menu (empty area).
auto file_tree_empty_menu() -> ContextMenuBuilder;

/// Tab bar context menu.
auto tab_bar_menu() -> ContextMenuBuilder;

/// Terminal context menu.
auto terminal_menu() -> ContextMenuBuilder;

/// Output panel context menu.
auto output_panel_menu() -> ContextMenuBuilder;

/// Problems panel context menu.
auto problems_panel_menu() -> ContextMenuBuilder;

/// Breadcrumb bar context menu.
auto breadcrumb_menu() -> ContextMenuBuilder;

/// Register all built-in context menus with the registry.
void register_all_defaults();

} // namespace context_menus

} // namespace markamp::ui
