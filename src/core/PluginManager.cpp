#include "PluginManager.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"
#include "ShortcutManager.h"

#include <algorithm>
#include <stdexcept>

namespace markamp::core
{

PluginManager::PluginManager(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

// ── Entry lookup helpers ──

auto PluginManager::find_entry(const std::string& plugin_id) -> std::vector<PluginEntry>::iterator
{
    return std::find_if(plugins_.begin(),
                        plugins_.end(),
                        [&plugin_id](const PluginEntry& entry)
                        { return entry.plugin->manifest().id == plugin_id; });
}

auto PluginManager::find_entry(const std::string& plugin_id) const
    -> std::vector<PluginEntry>::const_iterator
{
    return std::find_if(plugins_.begin(),
                        plugins_.end(),
                        [&plugin_id](const PluginEntry& entry)
                        { return entry.plugin->manifest().id == plugin_id; });
}

// ── Registration ──

auto PluginManager::register_plugin(std::unique_ptr<IPlugin> plugin) -> bool
{
    if (!plugin)
    {
        return false;
    }

    const auto& id = plugin->manifest().id;
    if (find_entry(id) != plugins_.end())
    {
        MARKAMP_LOG_WARN("Plugin '{}' is already registered, skipping", id);
        return false;
    }

    MARKAMP_LOG_INFO(
        "Registered plugin: {} v{}", plugin->manifest().name, plugin->manifest().version);

    plugins_.push_back(PluginEntry{std::move(plugin), {}, std::nullopt});
    return true;
}

auto PluginManager::register_plugin(std::unique_ptr<IPlugin> plugin, ExtensionManifest ext_manifest)
    -> bool
{
    if (!plugin)
    {
        return false;
    }

    const auto& id = plugin->manifest().id;
    if (find_entry(id) != plugins_.end())
    {
        MARKAMP_LOG_WARN("Plugin '{}' is already registered, skipping", id);
        return false;
    }

    MARKAMP_LOG_INFO("Registered plugin with manifest: {} v{} (publisher: {})",
                     ext_manifest.name,
                     ext_manifest.version,
                     ext_manifest.publisher);

    plugins_.push_back(PluginEntry{std::move(plugin), {}, std::move(ext_manifest)});
    return true;
}

void PluginManager::unregister_plugin(const std::string& plugin_id)
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it != plugins_.end())
    {
        if (entry_it->plugin->is_active())
        {
            entry_it->plugin->deactivate();
        }
        MARKAMP_LOG_INFO("Unregistered plugin: {}", plugin_id);
        plugins_.erase(entry_it);
    }

    // Clean up pending activations
    pending_ids_.erase(plugin_id);
    for (auto& [event_str, ids] : pending_activations_)
    {
        std::erase(ids, plugin_id);
    }
}

// ── Lifecycle ──

void PluginManager::activate_all()
{
    for (auto& entry : plugins_)
    {
        if (entry.plugin->is_active())
        {
            continue;
        }

        const auto& id = entry.plugin->manifest().id;

        // Check for activation events in the extension manifest
        if (entry.ext_manifest.has_value() && !entry.ext_manifest->activation_events.empty())
        {
            bool has_star = false;
            for (const auto& evt : entry.ext_manifest->activation_events)
            {
                if (evt.kind == ActivationEventKind::kStar)
                {
                    has_star = true;
                    break;
                }
            }

            if (has_star)
            {
                // Activate immediately
                activate_plugin(id);
            }
            else
            {
                // Register for lazy activation
                register_activation_events(id, entry.ext_manifest->activation_events);
            }
        }
        else
        {
            // No activation events → activate immediately (legacy behavior)
            activate_plugin(id);
        }
    }
}

void PluginManager::deactivate_all()
{
    for (auto& entry : plugins_)
    {
        if (entry.plugin->is_active())
        {
            // R19 Fix 12: Guard plugin deactivation against uncaught exceptions
            try
            {
                entry.plugin->deactivate();
            }
            catch (const std::exception& ex)
            {
                MARKAMP_LOG_WARN("Plugin '{}' threw during deactivation: {}",
                                 entry.plugin->manifest().name,
                                 ex.what());
            }
            MARKAMP_LOG_INFO("Deactivated plugin: {}", entry.plugin->manifest().name);
        }
    }

    pending_activations_.clear();
    pending_ids_.clear();
}

auto PluginManager::activate_plugin(const std::string& plugin_id) -> bool
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it == plugins_.end())
    {
        MARKAMP_LOG_WARN("Cannot activate unknown plugin: {}", plugin_id);
        return false;
    }

    if (entry_it->plugin->is_active())
    {
        return true; // Already active
    }

    // Resolve and activate dependencies first
    if (entry_it->ext_manifest.has_value())
    {
        for (const auto& dep_id : entry_it->ext_manifest->extension_dependencies)
        {
            auto dep_it = find_entry(dep_id);
            if (dep_it != plugins_.end() && !dep_it->plugin->is_active())
            {
                MARKAMP_LOG_INFO("Activating dependency '{}' for '{}'", dep_id, plugin_id);
                activate_plugin(dep_id);
            }
        }
    }

    // Process contribution points first
    process_contributions(*entry_it);

    // Create plugin context
    PluginContext ctx;
    ctx.event_bus = &event_bus_;
    ctx.config = &config_;
    ctx.register_command_handler =
        [&entry = *entry_it](const std::string& command_id, std::function<void()> handler)
    { entry.command_handlers[command_id] = std::move(handler); };

    // Set extension path if manifest is available
    if (entry_it->ext_manifest.has_value())
    {
        // Extension path is typically set by the scanner; for now use empty path
        // as it will be populated when loading from ExtensionScanner
    }

    // R19 Fix 11: Guard plugin activation against uncaught exceptions
    try
    {
        entry_it->plugin->activate(ctx);
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_WARN("Plugin '{}' threw during activation: {}",
                         entry_it->plugin->manifest().name,
                         ex.what());
        return false;
    }

    MARKAMP_LOG_INFO("Activated plugin: {} v{}",
                     entry_it->plugin->manifest().name,
                     entry_it->plugin->manifest().version);

    // Remove from pending
    pending_ids_.erase(plugin_id);

    // Publish activation event
    events::PluginActivatedEvent evt;
    evt.plugin_id = entry_it->plugin->manifest().id;
    event_bus_.publish(evt);

    return true;
}

auto PluginManager::deactivate_plugin(const std::string& plugin_id) -> bool
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it == plugins_.end() || !entry_it->plugin->is_active())
    {
        return false;
    }

    // R19 Fix 12: Guard plugin deactivation against uncaught exceptions
    try
    {
        entry_it->plugin->deactivate();
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_WARN("Plugin '{}' threw during deactivation: {}",
                         entry_it->plugin->manifest().name,
                         ex.what());
    }
    entry_it->command_handlers.clear();

    MARKAMP_LOG_INFO("Deactivated plugin: {}", entry_it->plugin->manifest().name);

    events::PluginDeactivatedEvent evt;
    evt.plugin_id = plugin_id;
    event_bus_.publish(evt);

    return true;
}

void PluginManager::trigger_activation_event(const std::string& event_string)
{
    auto map_it = pending_activations_.find(event_string);
    if (map_it == pending_activations_.end())
    {
        return;
    }

    // Copy the list because activation may modify pending_activations_
    const auto plugin_ids = map_it->second;

    for (const auto& pid : plugin_ids)
    {
        if (pending_ids_.count(pid) > 0)
        {
            MARKAMP_LOG_INFO("Lazy-activating plugin '{}' on event '{}'", pid, event_string);
            activate_plugin(pid);
        }
    }

    // Clean up the event entry
    pending_activations_.erase(event_string);
}

// ── Dependency Resolution ──

auto PluginManager::resolve_dependencies(const std::string& plugin_id) const
    -> std::vector<std::string>
{
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> in_stack;
    std::vector<std::string> order;

    resolve_deps_dfs(plugin_id, visited, in_stack, order);

    // Remove the plugin itself from the result (caller only wants deps)
    std::erase(order, plugin_id);

    return order;
}

void PluginManager::resolve_deps_dfs(const std::string& plugin_id,
                                     std::unordered_set<std::string>& visited,
                                     std::unordered_set<std::string>& in_stack,
                                     std::vector<std::string>& order) const
{
    if (in_stack.count(plugin_id) > 0)
    {
        throw std::runtime_error("Circular dependency detected involving: " + plugin_id);
    }

    if (visited.count(plugin_id) > 0)
    {
        return; // Already processed
    }

    in_stack.insert(plugin_id);
    visited.insert(plugin_id);

    // Find the plugin's dependencies
    auto entry_it = find_entry(plugin_id);
    if (entry_it != plugins_.end() && entry_it->ext_manifest.has_value())
    {
        for (const auto& dep_id : entry_it->ext_manifest->extension_dependencies)
        {
            resolve_deps_dfs(dep_id, visited, in_stack, order);
        }
    }

    in_stack.erase(plugin_id);
    order.push_back(plugin_id);
}

auto PluginManager::expand_extension_pack(const std::string& plugin_id) const
    -> std::vector<std::string>
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it == plugins_.end() || !entry_it->ext_manifest.has_value())
    {
        return {};
    }

    return entry_it->ext_manifest->extension_pack;
}

// ── Activation Event Registration ──

void PluginManager::register_activation_events(const std::string& plugin_id,
                                               const std::vector<ActivationEvent>& events)
{
    pending_ids_.insert(plugin_id);

    for (const auto& evt : events)
    {
        pending_activations_[evt.raw].push_back(plugin_id);
    }

    MARKAMP_LOG_INFO(
        "Plugin '{}' registered for {} lazy activation event(s)", plugin_id, events.size());
}

// ── Contribution Processing ──

void PluginManager::process_contributions(PluginEntry& entry)
{
    const auto& manifest = entry.plugin->manifest();
    const auto& contrib = manifest.contributes;

    // Process command contributions → register in palette
    if (palette_registrar_)
    {
        for (const auto& cmd : contrib.commands)
        {
            // Find matching keybinding for shortcut text
            std::string shortcut_text;
            if (shortcut_manager_)
            {
                for (const auto& keybind : contrib.keybindings)
                {
                    if (keybind.command_id == cmd.id)
                    {
                        shortcut_text =
                            ShortcutManager::format_shortcut(keybind.key_code, keybind.modifiers);
                        break;
                    }
                }
            }

            palette_registrar_(cmd.title,
                               cmd.category,
                               shortcut_text,
                               [&handlers = entry.command_handlers, id = cmd.id]()
                               {
                                   auto handler_it = handlers.find(id);
                                   if (handler_it != handlers.end())
                                   {
                                       handler_it->second();
                                   }
                               });
        }
    }

    // Process keybinding contributions → register in shortcut manager
    if (shortcut_manager_)
    {
        for (const auto& keybind : contrib.keybindings)
        {
            Shortcut shortcut;
            shortcut.id = keybind.command_id;
            shortcut.key_code = keybind.key_code;
            shortcut.modifiers = keybind.modifiers;
            shortcut.context = keybind.context;
            shortcut.category = "Plugin";

            // Find matching command for description
            for (const auto& cmd : contrib.commands)
            {
                if (cmd.id == keybind.command_id)
                {
                    shortcut.description = cmd.title;
                    break;
                }
            }

            shortcut.action = [&handlers = entry.command_handlers, id = keybind.command_id]()
            {
                auto handler_it = handlers.find(id);
                if (handler_it != handlers.end())
                {
                    handler_it->second();
                }
            };

            shortcut_manager_->register_shortcut(std::move(shortcut));
        }
    }

    // Process setting contributions → apply defaults to Config
    apply_setting_defaults(contrib.settings);

    // ── Process ExtensionManifest contribution points (#49) ──
    // New contribution types are only available on ExtensionManifest (external plugins),
    // not on the built-in PluginManifest::ContributionPoints.
    if (!entry.ext_manifest.has_value())
    {
        return;
    }

    const auto& ext_contrib = entry.ext_manifest->contributes;

    // Submenus – register in menu system
    for (const auto& submenu : ext_contrib.submenus)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes submenu: {} ({})", submenu.label, submenu.submenu_id);
    }

    // Walkthroughs – register for getting-started panel
    for (const auto& walkthrough : ext_contrib.walkthroughs)
    {
        MARKAMP_LOG_DEBUG("Extension contributes walkthrough: {} ({} steps)",
                          walkthrough.title,
                          walkthrough.steps.size());
    }

    // Custom editors – register file-type associations
    for (const auto& editor : ext_contrib.custom_editors)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes custom editor: {} ({})", editor.display_name, editor.view_type);
    }

    // Task definitions – register task types
    for (const auto& task_def : ext_contrib.task_definitions)
    {
        MARKAMP_LOG_DEBUG("Extension contributes task definition: {}", task_def.type);
    }

    // Problem matchers – register build-error patterns
    for (const auto& matcher : ext_contrib.problem_matchers)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes problem matcher: {} (owner: {})", matcher.name, matcher.owner);
    }

    // Terminal profiles – register terminal contributions
    for (const auto& profile : ext_contrib.terminal_profiles)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes terminal profile: {} ({})", profile.title, profile.profile_id);
    }

    // Status bar items – register status bar contributions
    for (const auto& item : ext_contrib.status_bar_items)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes status bar item: {} ({})", item.name, item.item_id);
    }

    // JSON validations – register schema associations
    for (const auto& validation : ext_contrib.json_validations)
    {
        MARKAMP_LOG_DEBUG("Extension contributes JSON validation: {} → {}",
                          validation.file_match,
                          validation.url);
    }

    // Icon themes
    for (const auto& theme : ext_contrib.icon_themes)
    {
        MARKAMP_LOG_DEBUG("Extension contributes icon theme: {} ({})", theme.label, theme.theme_id);
    }

    // Product icon themes
    for (const auto& theme : ext_contrib.product_icon_themes)
    {
        MARKAMP_LOG_DEBUG(
            "Extension contributes product icon theme: {} ({})", theme.label, theme.theme_id);
    }

    // Resource label formatters
    for (const auto& formatter : ext_contrib.resource_label_formatters)
    {
        MARKAMP_LOG_DEBUG("Extension contributes resource label formatter for scheme: {}",
                          formatter.scheme);
    }
}

void PluginManager::apply_setting_defaults(const std::vector<SettingContribution>& settings)
{
    for (const auto& setting : settings)
    {
        // Only set if the key doesn't already exist in config
        auto existing = config_.get_string(setting.id);
        if (existing.empty())
        {
            config_.set(setting.id, setting.default_value);
        }
    }
}

// ── Queries ──

auto PluginManager::get_plugin(const std::string& plugin_id) const -> const IPlugin*
{
    auto entry_it = find_entry(plugin_id);
    return (entry_it != plugins_.end()) ? entry_it->plugin.get() : nullptr;
}

auto PluginManager::get_all_plugins() const -> std::vector<const IPlugin*>
{
    std::vector<const IPlugin*> result;
    result.reserve(plugins_.size());
    for (const auto& entry : plugins_)
    {
        result.push_back(entry.plugin.get());
    }
    return result;
}

auto PluginManager::plugin_count() const -> std::size_t
{
    return plugins_.size();
}

auto PluginManager::is_plugin_active(const std::string& plugin_id) const -> bool
{
    auto entry_it = find_entry(plugin_id);
    return (entry_it != plugins_.end()) && entry_it->plugin->is_active();
}

auto PluginManager::is_pending_activation(const std::string& plugin_id) const -> bool
{
    return pending_ids_.count(plugin_id) > 0;
}

auto PluginManager::get_all_setting_contributions() const -> std::vector<SettingContribution>
{
    std::vector<SettingContribution> all_settings;
    for (const auto& entry : plugins_)
    {
        const auto& settings = entry.plugin->manifest().contributes.settings;
        all_settings.insert(all_settings.end(), settings.begin(), settings.end());
    }
    return all_settings;
}

auto PluginManager::get_extension_manifest(const std::string& plugin_id) const
    -> const ExtensionManifest*
{
    auto entry_it = find_entry(plugin_id);
    if (entry_it != plugins_.end() && entry_it->ext_manifest.has_value())
    {
        return &entry_it->ext_manifest.value();
    }
    return nullptr;
}

} // namespace markamp::core
