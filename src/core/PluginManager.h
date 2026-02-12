#pragma once

#include "IPlugin.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
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
/// Modeled after VS Code's extension host. On `activate_all()`:
///   1. Each plugin's manifest is read
///   2. Contribution points are processed:
///      - Commands → registered in CommandPalette
///      - Keybindings → registered in ShortcutManager
///      - Settings → defaults applied to Config
///   3. Plugin's `activate(ctx)` is called with a PluginContext
///
/// Thread safety: all methods must be called from the main (UI) thread.
class PluginManager
{
public:
    PluginManager(EventBus& event_bus, Config& config);

    // ── Registration ──

    /// Register a plugin. The plugin is not activated until `activate_all()`.
    /// Returns false if a plugin with the same ID is already registered.
    auto register_plugin(std::unique_ptr<IPlugin> plugin) -> bool;

    /// Unregister a plugin by ID. Deactivates it first if active.
    void unregister_plugin(const std::string& plugin_id);

    // ── Lifecycle ──

    /// Activate all registered plugins. Call once during app startup.
    /// Requires that ShortcutManager and palette command handler are set.
    void activate_all();

    /// Deactivate all active plugins. Call during app shutdown.
    void deactivate_all();

    /// Activate a single plugin by ID.
    auto activate_plugin(const std::string& plugin_id) -> bool;

    /// Deactivate a single plugin by ID.
    auto deactivate_plugin(const std::string& plugin_id) -> bool;

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

    /// Get all contributed settings across all plugins.
    [[nodiscard]] auto get_all_setting_contributions() const -> std::vector<SettingContribution>;

private:
    EventBus& event_bus_;
    Config& config_;
    ShortcutManager* shortcut_manager_{nullptr};
    PaletteRegistrar palette_registrar_;

    struct PluginEntry
    {
        std::unique_ptr<IPlugin> plugin;
        std::unordered_map<std::string, std::function<void()>> command_handlers;
    };

    std::vector<PluginEntry> plugins_;

    /// Process a plugin's manifest contribution points.
    void process_contributions(PluginEntry& entry);

    /// Apply default values for plugin settings to Config.
    void apply_setting_defaults(const std::vector<SettingContribution>& settings);
};

} // namespace markamp::core
