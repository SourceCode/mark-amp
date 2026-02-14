#pragma once

#include "ExtensionManifest.h"
#include "IPlugin.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class ShortcutManager;

namespace ui
{
class CommandPalette;
} // namespace ui

/// Manages the plugin lifecycle: registration, activation, deactivation,
/// and contribution-point processing.
///
/// Modeled after VS Code's extension host. Phase 4 enhancements:
///   - Lazy activation via activation events (plugins only activate when needed)
///   - Dependency resolution with topological sort
///   - Extension pack expansion
///
/// On `activate_all()`:
///   1. Each plugin's manifest is read
///   2. Plugins with `*` activation event are activated immediately
///   3. Other plugins are deferred until their activation event fires
///   4. Dependencies are resolved before activation
///
/// Thread safety: all methods must be called from the main (UI) thread.
class PluginManager
{
public:
    PluginManager(EventBus& event_bus, Config& config);

    // ── Registration ──

    /// Register a plugin. The plugin is not activated until its activation
    /// event fires (or immediately if activation event is `*`).
    /// Returns false if a plugin with the same ID is already registered.
    auto register_plugin(std::unique_ptr<IPlugin> plugin) -> bool;

    /// Register a plugin with an associated ExtensionManifest for
    /// lazy activation and dependency resolution.
    auto register_plugin(std::unique_ptr<IPlugin> plugin, ExtensionManifest ext_manifest) -> bool;

    /// Unregister a plugin by ID. Deactivates it first if active.
    void unregister_plugin(const std::string& plugin_id);

    // ── Lifecycle ──

    /// Activate all registered plugins (respecting activation events).
    /// Plugins with `*` activation event are activated immediately.
    /// Others are registered for lazy activation.
    void activate_all();

    /// Deactivate all active plugins. Call during app shutdown.
    void deactivate_all();

    /// Activate a single plugin by ID.
    auto activate_plugin(const std::string& plugin_id) -> bool;

    /// Deactivate a single plugin by ID.
    auto deactivate_plugin(const std::string& plugin_id) -> bool;

    /// Trigger activation for all plugins waiting on the given event.
    /// For example, call `trigger_activation_event("onLanguage:markdown")`
    /// when a markdown file is opened.
    void trigger_activation_event(const std::string& event_string);

    // ── Dependency Resolution ──

    /// Resolve dependencies for a plugin. Returns the topologically-sorted
    /// list of plugin IDs that must be activated before this one.
    /// Throws std::runtime_error if circular dependencies are detected.
    [[nodiscard]] auto resolve_dependencies(const std::string& plugin_id) const
        -> std::vector<std::string>;

    /// Expand an extension pack: return all member extension IDs.
    [[nodiscard]] auto expand_extension_pack(const std::string& plugin_id) const
        -> std::vector<std::string>;

    // ── Dependency injection for command wiring ──

    /// Set the shortcut manager for keybinding contributions.
    void set_shortcut_manager(ShortcutManager* sm)
    {
        shortcut_manager_ = sm;
    }

    /// Set the callback for registering palette commands.
    using PaletteRegistrar = std::function<void(const std::string& label,
                                                const std::string& category,
                                                const std::string& shortcut,
                                                std::function<void()> action)>;
    void set_palette_registrar(PaletteRegistrar registrar)
    {
        palette_registrar_ = std::move(registrar);
    }

    // ── Queries ──

    [[nodiscard]] auto get_plugin(const std::string& plugin_id) const -> const IPlugin*;
    [[nodiscard]] auto get_all_plugins() const -> std::vector<const IPlugin*>;
    [[nodiscard]] auto plugin_count() const -> std::size_t;
    [[nodiscard]] auto is_plugin_active(const std::string& plugin_id) const -> bool;

    /// Check if a plugin is pending activation (waiting for activation event).
    [[nodiscard]] auto is_pending_activation(const std::string& plugin_id) const -> bool;

    /// Get all contributed settings across all plugins.
    [[nodiscard]] auto get_all_setting_contributions() const -> std::vector<SettingContribution>;

    /// Get the extension manifest for a registered plugin (if available).
    [[nodiscard]] auto get_extension_manifest(const std::string& plugin_id) const
        -> const ExtensionManifest*;

private:
    EventBus& event_bus_;
    Config& config_;
    ShortcutManager* shortcut_manager_{nullptr};
    PaletteRegistrar palette_registrar_;

    struct PluginEntry
    {
        std::unique_ptr<IPlugin> plugin;
        std::unordered_map<std::string, std::function<void()>> command_handlers;
        std::optional<ExtensionManifest> ext_manifest; // Phase 4: optional manifest
    };

    std::vector<PluginEntry> plugins_;

    /// Map from activation event string → list of plugin IDs waiting on it.
    std::unordered_map<std::string, std::vector<std::string>> pending_activations_;

    /// Set of plugin IDs that are pending (not yet activated).
    std::unordered_set<std::string> pending_ids_;

    /// Process a plugin's manifest contribution points.
    void process_contributions(PluginEntry& entry);

    /// Apply default values for plugin settings to Config.
    void apply_setting_defaults(const std::vector<SettingContribution>& settings);

    /// Register a plugin's activation events for lazy activation.
    void register_activation_events(const std::string& plugin_id,
                                    const std::vector<ActivationEvent>& events);

    /// Internal: find PluginEntry by ID, returns iterator.
    auto find_entry(const std::string& plugin_id) -> std::vector<PluginEntry>::iterator;
    auto find_entry(const std::string& plugin_id) const -> std::vector<PluginEntry>::const_iterator;

    /// DFS helper for topological sort dependency resolution.
    void resolve_deps_dfs(const std::string& plugin_id,
                          std::unordered_set<std::string>& visited,
                          std::unordered_set<std::string>& in_stack,
                          std::vector<std::string>& order) const;
};

} // namespace markamp::core
