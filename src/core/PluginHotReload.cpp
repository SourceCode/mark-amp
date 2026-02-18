#include "PluginHotReload.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"
#include "PluginManager.h"

namespace markamp::core
{

auto PluginHotReload::reload_plugin(const std::string& plugin_id) -> bool
{
    // Check that plugin exists
    const auto* plugin = plugin_manager_.get_plugin(plugin_id);
    if (plugin == nullptr)
    {
        MARKAMP_LOG_WARN("Cannot hot-reload unknown plugin: {}", plugin_id);
        return false;
    }

    MARKAMP_LOG_INFO("Hot-reloading plugin '{}'...", plugin_id);

    // Step 1: Deactivate if active
    if (plugin->is_active())
    {
        plugin_manager_.deactivate_plugin(plugin_id);
    }

    // Step 2: Re-activate
    const bool success = plugin_manager_.activate_plugin(plugin_id);

    if (success)
    {
        // Track reload count
        reload_counts_[plugin_id]++;
        total_reloads_++;

        MARKAMP_LOG_INFO(
            "Hot-reload succeeded for '{}' (reload #{})", plugin_id, reload_counts_[plugin_id]);

        // Publish reload event
        events::PluginActivatedEvent evt;
        evt.plugin_id = plugin_id;
        event_bus_.publish(evt);
    }
    else
    {
        MARKAMP_LOG_WARN("Hot-reload failed for '{}'", plugin_id);

        // Publish error event
        events::PluginErrorEvent err_evt;
        err_evt.plugin_id = plugin_id;
        err_evt.error_message = "Hot-reload activation failed";
        event_bus_.publish(err_evt);
    }

    return success;
}

auto PluginHotReload::reload_count(const std::string& plugin_id) const -> int
{
    auto count_it = reload_counts_.find(plugin_id);
    if (count_it != reload_counts_.end())
    {
        return count_it->second;
    }
    return 0;
}

void PluginHotReload::reset_counters()
{
    reload_counts_.clear();
    total_reloads_ = 0;
}

} // namespace markamp::core
