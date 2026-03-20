/// @file SettingsStateOwner.cpp
/// @brief P06-T04: Canonical settings state owner for all entry points.

#include "SettingsStateOwner.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

SettingsStateOwner::SettingsStateOwner(EventBus& bus, Config& cfg)
    : event_bus_(bus)
    , config_(cfg)
{
}

void SettingsStateOwner::stage(const std::string& key, const std::string& value)
{
    // Check if this key already has a staged change
    for (auto& change : staged_)
    {
        if (change.key == key)
        {
            change.new_value = value;
            MARKAMP_LOG_DEBUG("Setting staged (updated): {} = {}", key, value);
            return;
        }
    }

    // New staged change
    StagedChange change;
    change.key = key;
    change.old_value = config_.get_string(key);
    change.new_value = value;
    staged_.push_back(std::move(change));
    MARKAMP_LOG_DEBUG("Setting staged: {} = {}", key, value);
}

void SettingsStateOwner::apply()
{
    if (staged_.empty())
    {
        return;
    }

    std::vector<std::string> changed_keys;
    for (const auto& change : staged_)
    {
        config_.set(change.key, change.new_value);
        event_bus_.publish(events::SettingChangedEvent{change.key, change.new_value});
        changed_keys.push_back(change.key);
    }

    // Publish batch change
    events::SettingsBatchChangedEvent batch_evt;
    batch_evt.changed_keys = std::move(changed_keys);
    event_bus_.publish(batch_evt);

    MARKAMP_LOG_INFO("Settings applied: {} changes", staged_.size());
    staged_.clear();
}

void SettingsStateOwner::cancel()
{
    if (staged_.empty())
    {
        return;
    }
    MARKAMP_LOG_INFO("Settings cancelled: {} staged changes discarded", staged_.size());
    staged_.clear();
}

void SettingsStateOwner::undo_last()
{
    if (staged_.empty())
    {
        return;
    }
    const auto& undone = staged_.back();
    MARKAMP_LOG_DEBUG("Setting unstaged: {}", undone.key);
    staged_.pop_back();
}

void SettingsStateOwner::import_settings(
    const std::unordered_map<std::string, std::string>& settings)
{
    for (const auto& [key, value] : settings)
    {
        stage(key, value);
    }
    MARKAMP_LOG_INFO("Settings imported: {} entries staged", settings.size());
}

auto SettingsStateOwner::export_settings() const
    -> std::unordered_map<std::string, std::string>
{
    // Export currently applied config values
    // This is a placeholder — real implementation would iterate Config keys
    std::unordered_map<std::string, std::string> result;
    MARKAMP_LOG_INFO("Settings export requested");
    return result;
}

} // namespace markamp::core
