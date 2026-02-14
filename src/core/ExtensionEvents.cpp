#include "ExtensionEvents.h"

#include <algorithm>

namespace markamp::core
{

auto ExtensionEventBus::on_will_save(WillSaveCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back({listener_id, true, std::move(callback), nullptr});
    return listener_id;
}

auto ExtensionEventBus::on_did_change_configuration(ConfigChangeCallback callback) -> std::size_t
{
    auto listener_id = next_listener_id_++;
    listeners_.push_back({listener_id, false, nullptr, std::move(callback)});
    return listener_id;
}

void ExtensionEventBus::remove_listener(std::size_t listener_id)
{
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [listener_id](const ListenerEntry& entry)
                                    { return entry.listener_id == listener_id; }),
                     listeners_.end());
}

void ExtensionEventBus::fire_will_save(const TextDocumentWillSaveEvent& event)
{
    for (const auto& entry : listeners_)
    {
        if (entry.is_will_save && entry.will_save_callback)
        {
            entry.will_save_callback(event);
        }
    }
}

void ExtensionEventBus::fire_configuration_change(const ConfigurationChangeEvent& event)
{
    for (const auto& entry : listeners_)
    {
        if (!entry.is_will_save && entry.config_callback)
        {
            entry.config_callback(event);
        }
    }
}

} // namespace markamp::core
