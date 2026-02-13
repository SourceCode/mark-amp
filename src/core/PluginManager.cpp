#include "PluginManager.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"
#include "ShortcutManager.h"

#include <algorithm>

namespace markamp::core
{

PluginManager::PluginManager(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto PluginManager::register_plugin(std::unique_ptr<IPlugin> plugin) -> bool
{
    if (!plugin)
    {
        return false;
    }

    const auto& id = plugin->manifest().id;

    // Check for duplicate
    auto it = std::find_if(plugins_.begin(),
                           plugins_.end(),
                           [&id](const PluginEntry& e) { return e.plugin->manifest().id == id; });
    if (it != plugins_.end())
    {
        MARKAMP_LOG_WARN("Plugin '{}' is already registered, skipping", id);
        return false;
    }

    MARKAMP_LOG_INFO(
        "Registered plugin: {} v{}", plugin->manifest().name, plugin->manifest().version);

    plugins_.push_back(PluginEntry{std::move(plugin), {}});
    return true;
}

void PluginManager::unregister_plugin(const std::string& plugin_id)
{
    auto it = std::find_if(plugins_.begin(),
                           plugins_.end(),
                           [&plugin_id](const PluginEntry& e)
                           { return e.plugin->manifest().id == plugin_id; });
    if (it != plugins_.end())
    {
        if (it->plugin->is_active())
        {
            it->plugin->deactivate();
        }
        MARKAMP_LOG_INFO("Unregistered plugin: {}", plugin_id);
        plugins_.erase(it);
    }
}

void PluginManager::activate_all()
{
    for (auto& entry : plugins_)
    {
        if (!entry.plugin->is_active())
        {
            activate_plugin(entry.plugin->manifest().id);
        }
    }
}

void PluginManager::deactivate_all()
{
    for (auto& entry : plugins_)
    {
        if (entry.plugin->is_active())
        {
            entry.plugin->deactivate();
            MARKAMP_LOG_INFO("Deactivated plugin: {}", entry.plugin->manifest().name);
        }
    }
}

auto PluginManager::activate_plugin(const std::string& plugin_id) -> bool
{
    auto it = std::find_if(plugins_.begin(),
                           plugins_.end(),
                           [&plugin_id](const PluginEntry& e)
                           { return e.plugin->manifest().id == plugin_id; });
    if (it == plugins_.end())
    {
        MARKAMP_LOG_WARN("Cannot activate unknown plugin: {}", plugin_id);
        return false;
    }

    if (it->plugin->is_active())
    {
        return true; // Already active
    }

    // Process contribution points first
    process_contributions(*it);

    // Create plugin context
    PluginContext ctx;
    ctx.event_bus = &event_bus_;
    ctx.config = &config_;
    ctx.register_command_handler =
        [&entry = *it](const std::string& command_id, std::function<void()> handler)
    { entry.command_handlers[command_id] = std::move(handler); };

    // R19 Fix 11: Guard plugin activation against uncaught exceptions
    try
    {
        it->plugin->activate(ctx);
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_WARN(
            "Plugin '{}' threw during activation: {}", it->plugin->manifest().name, ex.what());
        return false;
    }

    MARKAMP_LOG_INFO(
        "Activated plugin: {} v{}", it->plugin->manifest().name, it->plugin->manifest().version);

    // Publish activation event
    events::PluginActivatedEvent evt;
    evt.plugin_id = it->plugin->manifest().id;
    event_bus_.publish(evt);

    return true;
}

auto PluginManager::deactivate_plugin(const std::string& plugin_id) -> bool
{
    auto it = std::find_if(plugins_.begin(),
                           plugins_.end(),
                           [&plugin_id](const PluginEntry& e)
                           { return e.plugin->manifest().id == plugin_id; });
    if (it == plugins_.end() || !it->plugin->is_active())
    {
        return false;
    }

    // R19 Fix 12: Guard plugin deactivation against uncaught exceptions
    try
    {
        it->plugin->deactivate();
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_WARN(
            "Plugin '{}' threw during deactivation: {}", it->plugin->manifest().name, ex.what());
    }
    it->command_handlers.clear();

    MARKAMP_LOG_INFO("Deactivated plugin: {}", it->plugin->manifest().name);

    events::PluginDeactivatedEvent evt;
    evt.plugin_id = plugin_id;
    event_bus_.publish(evt);

    return true;
}

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
                for (const auto& kb : contrib.keybindings)
                {
                    if (kb.command_id == cmd.id)
                    {
                        shortcut_text = ShortcutManager::format_shortcut(kb.key_code, kb.modifiers);
                        break;
                    }
                }
            }

            palette_registrar_(cmd.title,
                               cmd.category,
                               shortcut_text,
                               [&handlers = entry.command_handlers, id = cmd.id]()
                               {
                                   auto it = handlers.find(id);
                                   if (it != handlers.end())
                                   {
                                       it->second();
                                   }
                               });
        }
    }

    // Process keybinding contributions → register in shortcut manager
    if (shortcut_manager_)
    {
        for (const auto& kb : contrib.keybindings)
        {
            Shortcut shortcut;
            shortcut.id = kb.command_id;
            shortcut.key_code = kb.key_code;
            shortcut.modifiers = kb.modifiers;
            shortcut.context = kb.context;
            shortcut.category = "Plugin";

            // Find matching command for description
            for (const auto& cmd : contrib.commands)
            {
                if (cmd.id == kb.command_id)
                {
                    shortcut.description = cmd.title;
                    break;
                }
            }

            shortcut.action = [&handlers = entry.command_handlers, id = kb.command_id]()
            {
                auto it = handlers.find(id);
                if (it != handlers.end())
                {
                    it->second();
                }
            };

            shortcut_manager_->register_shortcut(std::move(shortcut));
        }
    }

    // Process setting contributions → apply defaults to Config
    apply_setting_defaults(contrib.settings);
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

auto PluginManager::get_plugin(const std::string& plugin_id) const -> const IPlugin*
{
    auto it = std::find_if(plugins_.begin(),
                           plugins_.end(),
                           [&plugin_id](const PluginEntry& e)
                           { return e.plugin->manifest().id == plugin_id; });
    return (it != plugins_.end()) ? it->plugin.get() : nullptr;
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
    auto it = std::find_if(plugins_.begin(),
                           plugins_.end(),
                           [&plugin_id](const PluginEntry& e)
                           { return e.plugin->manifest().id == plugin_id; });
    return (it != plugins_.end()) && it->plugin->is_active();
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

} // namespace markamp::core
