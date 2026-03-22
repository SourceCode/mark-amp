/// @file MenuCommandBinder.h
/// @brief V21 Phase 02 — Menu-to-manifest command binding layer.
///
/// Bridges the main menu system with the canonical ControlActionManifest.
/// Provides:
///   - Menu item registration from manifest entries
///   - Accelerator table generation from canonical shortcut metadata
///   - Stub/dead menu item detection
///   - Settings deep-link routing for preferences menu items
///   - Menu item enablement/visibility driven by context keys
#pragma once

#include "core/ControlActionManifest.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// MenuItemBinding — a single menu-to-action binding
// ============================================================================

/// Represents a menu item bound to a canonical action.
struct MenuItemBinding
{
    std::string action_id;        ///< Canonical action ID
    std::string menu_path;        ///< Menu hierarchy path, e.g. "File/Save"
    std::string label;            ///< Display label from manifest
    std::string shortcut_display; ///< Shortcut display string, e.g. "⌘S"
    int menu_item_id{0};          ///< Platform menu item ID (wxWidgets ID)
    bool is_separator{false};     ///< Whether this is a separator entry
    bool is_submenu{false};       ///< Whether this opens a submenu
    bool is_bound{false};         ///< Whether the action has a real handler
    bool is_enabled{true};        ///< Current enablement state
    bool is_visible{true};        ///< Current visibility state
    bool is_checked{false};       ///< For toggle/check menu items

    /// Whether this binding represents a live, useful menu item.
    [[nodiscard]] auto is_live() const noexcept -> bool
    {
        return is_bound && is_visible;
    }
};

// ============================================================================
// AcceleratorBinding — shortcut-to-action mapping
// ============================================================================

/// Maps a keyboard shortcut to a canonical action for accelerator table generation.
struct AcceleratorBinding
{
    std::string action_id;  ///< Canonical action ID
    int key_code{0};        ///< wxWidgets key code
    int modifiers{0};       ///< wxWidgets modifier flags
    std::string display;    ///< Human-readable display, e.g. "⌘N"
};

// ============================================================================
// MenuBindingDiagnostic — audit entry for a menu binding
// ============================================================================

/// Diagnostic information for a menu binding.
struct MenuBindingDiagnostic
{
    std::string action_id;
    std::string menu_path;
    std::string issue;         ///< Description of the problem
    bool is_dead{false};       ///< No handler at all
    bool is_stub{false};       ///< Handler exists but is a no-op
    bool is_duplicate{false};  ///< Same action appears multiple times
    bool is_orphaned{false};   ///< Menu item with no manifest entry
};

// ============================================================================
// DeepLinkTarget — settings deep-link resolution
// ============================================================================

/// Describes the target of a settings deep-link action.
struct DeepLinkTarget
{
    std::string setting_id;    ///< Target setting ID (e.g. "editor.fontSize")
    std::string query;         ///< Search query to focus
    std::string scope;         ///< "user", "workspace", or "project"
    std::string category;      ///< Setting category to navigate to
};

// ============================================================================
// MenuCommandBinder — the binding engine
// ============================================================================

/// Bridges the main menu system with the canonical ControlActionManifest.
/// Replaces direct `Bind(wxEVT_MENU, ...)` lambdas in MainFrame with
/// manifest-driven dispatch.
class MenuCommandBinder
{
public:
    MenuCommandBinder() = default;

    // ── Binding Registration ──

    /// Bind a menu item to a canonical action.
    void bind(MenuItemBinding binding);

    /// Bind multiple menu items at once.
    void bind_all(std::vector<MenuItemBinding> bindings);

    /// Remove a binding by action ID.
    auto unbind(const std::string& action_id) -> bool;

    // ── Lookup ──

    /// Get a binding by action ID.
    [[nodiscard]] auto get_binding(const std::string& action_id) const
        -> const MenuItemBinding*;

    /// Get a binding by menu item ID (wxWidgets ID).
    [[nodiscard]] auto get_binding_by_menu_id(int menu_item_id) const
        -> const MenuItemBinding*;

    /// Get all bindings.
    [[nodiscard]] auto all_bindings() const -> std::vector<const MenuItemBinding*>;

    /// Get bindings under a specific menu path prefix.
    [[nodiscard]] auto bindings_for_menu(const std::string& menu_prefix) const
        -> std::vector<const MenuItemBinding*>;

    /// Total binding count.
    [[nodiscard]] auto binding_count() const -> std::size_t;

    // ── Execution ──

    /// Handle a menu event by ID. Dispatches to the canonical manifest.
    /// Returns true if the action was found and executed.
    auto dispatch_menu_event(int menu_item_id, ControlActionManifest& manifest) -> bool;

    // ── Enablement Sync ──

    /// Update enablement/visibility state for all bindings using current context.
    void refresh_enablement(const ControlActionManifest& manifest,
                            const ContextKeyService& context);

    /// Get bindings that are currently disabled.
    [[nodiscard]] auto disabled_bindings() const -> std::vector<const MenuItemBinding*>;

    /// Get bindings that are currently hidden.
    [[nodiscard]] auto hidden_bindings() const -> std::vector<const MenuItemBinding*>;

    // ── Accelerator Generation ──

    /// Generate accelerator bindings from all registered bindings + manifest shortcut hints.
    [[nodiscard]] auto generate_accelerators() const -> std::vector<AcceleratorBinding>;

    // ── Manifest Synchronization ──

    /// Populate bindings from a manifest, creating MenuItemBinding entries for every
    /// action that targets ControlSurface::kMenu. Returns the number of bindings created.
    auto sync_from_manifest(const ControlActionManifest& manifest) -> int;

    // ── Diagnostics ──

    /// Detect dead, stub, duplicate, and orphaned menu bindings.
    [[nodiscard]] auto diagnose(const ControlActionManifest& manifest) const
        -> std::vector<MenuBindingDiagnostic>;

    /// Count of live (bound + visible) bindings.
    [[nodiscard]] auto live_binding_count() const -> std::size_t;

    /// Count of dead (unbound) bindings.
    [[nodiscard]] auto dead_binding_count() const -> std::size_t;

    // ── Settings Deep-Link ──

    /// Register a deep-link target for a settings-related menu action.
    void register_deep_link(const std::string& action_id, DeepLinkTarget target);

    /// Resolve a deep-link target for a settings action. Returns nullptr if not a deep-link action.
    [[nodiscard]] auto get_deep_link(const std::string& action_id) const
        -> const DeepLinkTarget*;

    /// Check if an action is a settings deep-link.
    [[nodiscard]] auto is_deep_link(const std::string& action_id) const -> bool;

private:
    /// All registered bindings, keyed by action_id.
    std::unordered_map<std::string, MenuItemBinding> bindings_;

    /// Menu item ID → action ID reverse lookup.
    std::unordered_map<int, std::string> menu_id_to_action_;

    /// Insertion order for deterministic iteration.
    std::vector<std::string> insertion_order_;

    /// Settings deep-link targets.
    std::unordered_map<std::string, DeepLinkTarget> deep_links_;
};

} // namespace markamp::core
