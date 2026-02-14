#include "ExtensionEnablement.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"

#include <sstream>

namespace markamp::core
{

ExtensionEnablementService::ExtensionEnablementService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    load_from_config();
}

auto ExtensionEnablementService::is_enabled(const std::string& extension_id) const -> bool
{
    return disabled_ids_.find(extension_id) == disabled_ids_.end();
}

void ExtensionEnablementService::enable(const std::string& extension_id)
{
    if (disabled_ids_.erase(extension_id) > 0)
    {
        save_to_config();

        events::ExtensionEnablementChangedEvent evt;
        evt.extension_id = extension_id;
        evt.enabled = true;
        event_bus_.publish(evt);
    }
}

void ExtensionEnablementService::disable(const std::string& extension_id)
{
    if (disabled_ids_.insert(extension_id).second)
    {
        save_to_config();

        events::ExtensionEnablementChangedEvent evt;
        evt.extension_id = extension_id;
        evt.enabled = false;
        event_bus_.publish(evt);
    }
}

auto ExtensionEnablementService::toggle(const std::string& extension_id) -> bool
{
    if (is_enabled(extension_id))
    {
        disable(extension_id);
        return false;
    }

    enable(extension_id);
    return true;
}

auto ExtensionEnablementService::get_disabled_ids() const -> std::vector<std::string>
{
    return {disabled_ids_.begin(), disabled_ids_.end()};
}

auto ExtensionEnablementService::disabled_count() const -> size_t
{
    return disabled_ids_.size();
}

void ExtensionEnablementService::load_from_config()
{
    // Disabled IDs are stored as a comma-separated string
    const std::string disabled_str = config_.get_string(kConfigKey, "");
    disabled_ids_.clear();

    if (disabled_str.empty())
    {
        return;
    }

    std::istringstream stream(disabled_str);
    std::string token;
    while (std::getline(stream, token, ','))
    {
        // Trim whitespace
        const auto start = token.find_first_not_of(' ');
        const auto end_pos = token.find_last_not_of(' ');
        if (start != std::string::npos)
        {
            disabled_ids_.insert(token.substr(start, end_pos - start + 1));
        }
    }
}

void ExtensionEnablementService::save_to_config() const
{
    std::string result;
    for (const auto& ext_id : disabled_ids_)
    {
        if (!result.empty())
        {
            result += ",";
        }
        result += ext_id;
    }
    config_.set(kConfigKey, std::string_view{result});
}

} // namespace markamp::core
