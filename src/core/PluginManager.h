#pragma once

#include "ExtensionManifest.h"
#include "IPlugin.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::ui
{
class WalkthroughPanel;
} // namespace markamp::ui

namespace markamp::core
{

class EventBus;
class Config;
class ShortcutManager;
class StatusBarItemService;
class ThemeRegistry;
class TreeDataProviderRegistry;

// Forward declarations for ExtensionServices
class ContextKeyService;
class OutputChannelService;
class DiagnosticsService;
class DecorationService;
class WebviewService;
class FileSystemProviderRegistry;
class LanguageProviderRegistry;
class SnippetEngine;
class WorkspaceService;
class TextEditorService;
class ProgressService;
class ExtensionEventBus;
class EnvironmentService;
class NotificationService;
class InputBoxService;
class QuickPickService;
class GrammarEngine;
class TerminalService;
class TaskRunnerService;

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

    // ── Tier 3: Contribution point wiring dependency injection ──

    /// Set the status bar item service for status bar contributions.
    void set_status_bar_service(StatusBarItemService* svc)
    {
        status_bar_service_ = svc;
    }

    /// Set the walkthrough panel for walkthrough contributions.
    void set_walkthrough_panel(markamp::ui::WalkthroughPanel* panel)
    {
        walkthrough_panel_ = panel;
    }

    /// Set the theme registry for theme contributions.
    void set_theme_registry(ThemeRegistry* registry)
    {
        theme_registry_ = registry;
    }

    /// Set the tree data provider registry for view contributions.
    void set_tree_registry(TreeDataProviderRegistry* registry)
    {
        tree_registry_ = registry;
    }

    // ── Extension service injection for PluginContext population ──

    /// Aggregate of all extension API service pointers. Populated by the app
    /// and stored here so activate_plugin() can fill every PluginContext field.
    struct ExtensionServices
    {
        ContextKeyService* context_key_service{nullptr};
        OutputChannelService* output_channel_service{nullptr};
        DiagnosticsService* diagnostics_service{nullptr};
        DecorationService* decoration_service{nullptr};
        WebviewService* webview_service{nullptr};
        FileSystemProviderRegistry* file_system_provider_registry{nullptr};
        LanguageProviderRegistry* language_provider_registry{nullptr};
        TreeDataProviderRegistry* tree_data_provider_registry{nullptr};
        SnippetEngine* snippet_engine{nullptr};
        WorkspaceService* workspace_service{nullptr};
        TextEditorService* text_editor_service{nullptr};
        ProgressService* progress_service{nullptr};
        ExtensionEventBus* extension_event_bus{nullptr};
        EnvironmentService* environment_service{nullptr};
        NotificationService* notification_service{nullptr};
        StatusBarItemService* status_bar_item_service{nullptr};
        InputBoxService* input_box_service{nullptr};
        QuickPickService* quick_pick_service{nullptr};
        GrammarEngine* grammar_engine{nullptr};
        TerminalService* terminal_service{nullptr};
        TaskRunnerService* task_runner_service{nullptr};
    };

    /// Inject all extension services. Must be called before activate_all().
    void set_extension_services(const ExtensionServices& services)
    {
        ext_services_ = services;
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

    // ── Tier 3: Contributed data queries ──

    /// Get all contributed colors from loaded extensions.
    [[nodiscard]] auto get_contributed_colors() const -> const std::vector<ExtensionColor>&;

    /// Get all contributed views from loaded extensions.
    [[nodiscard]] auto get_contributed_views() const -> const std::vector<ExtensionView>&;

    /// Get all contributed views containers from loaded extensions.
    [[nodiscard]] auto get_contributed_views_containers() const
        -> const std::vector<ExtensionViewsContainer>&;

    /// Get all contributed menu items from loaded extensions.
    [[nodiscard]] auto get_contributed_menus() const -> const std::vector<ExtensionMenuItem>&;

    /// Get all contributed snippets from loaded extensions.
    [[nodiscard]] auto get_contributed_snippets() const -> const std::vector<ExtensionSnippet>&;

    /// Get all contributed languages from loaded extensions.
    [[nodiscard]] auto get_contributed_languages() const -> const std::vector<ExtensionLanguage>&;

    /// Get all contributed grammars from loaded extensions.
    [[nodiscard]] auto get_contributed_grammars() const -> const std::vector<ExtensionGrammar>&;

private:
    EventBus& event_bus_;
    Config& config_;
    ShortcutManager* shortcut_manager_{nullptr};
    PaletteRegistrar palette_registrar_;

    // Tier 3: Dependency injection targets
    StatusBarItemService* status_bar_service_{nullptr};
    markamp::ui::WalkthroughPanel* walkthrough_panel_{nullptr};
    ThemeRegistry* theme_registry_{nullptr};
    TreeDataProviderRegistry* tree_registry_{nullptr};

    /// Extension services injected by the app for PluginContext population.
    ExtensionServices ext_services_;

    /// Accumulated contribution data from all loaded extensions.
    struct ContributionRegistry
    {
        std::vector<ExtensionColor> colors;
        std::vector<ExtensionView> views;
        std::vector<ExtensionViewsContainer> views_containers;
        std::vector<ExtensionMenuItem> menus;
        std::vector<ExtensionSubmenu> submenus;
        std::vector<ExtensionSnippet> snippets;
        std::vector<ExtensionLanguage> languages;
        std::vector<ExtensionGrammar> grammars;
        std::vector<ExtensionCustomEditor> custom_editors;
    };
    ContributionRegistry contributions_;

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
