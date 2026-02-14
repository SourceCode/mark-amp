#include "core/FeatureRegistry.h"

#include "core/Logger.h"

namespace markamp::core
{

FeatureRegistry::FeatureRegistry(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

void FeatureRegistry::register_feature(const FeatureInfo& info)
{
    const std::lock_guard lock(mutex_);

    if (features_.contains(info.id))
    {
        MARKAMP_LOG_WARN("FeatureRegistry: duplicate feature registration: {}", info.id);
        return;
    }

    FeatureEntry entry;
    entry.info = info;

    // Read persisted state from Config, falling back to default
    const auto key = config_key(info.id);
    entry.enabled = config_.get_bool(key, info.default_enabled);

    features_.emplace(info.id, std::move(entry));
}

auto FeatureRegistry::is_enabled(const std::string& feature_id) const -> bool
{
    const std::lock_guard lock(mutex_);
    auto iter = features_.find(feature_id);
    if (iter == features_.end())
    {
        return false;
    }
    return iter->second.enabled;
}

void FeatureRegistry::enable(const std::string& feature_id)
{
    set_enabled(feature_id, true);
}

void FeatureRegistry::disable(const std::string& feature_id)
{
    set_enabled(feature_id, false);
}

void FeatureRegistry::toggle(const std::string& feature_id)
{
    bool new_state = false;
    {
        const std::lock_guard lock(mutex_);
        auto iter = features_.find(feature_id);
        if (iter == features_.end())
        {
            return;
        }

        new_state = !iter->second.enabled;
        iter->second.enabled = new_state;

        // Persist to Config
        config_.set(config_key(feature_id), new_state);
    }

    // Fire event outside lock
    events::FeatureToggledEvent evt;
    evt.feature_id = feature_id;
    evt.enabled = new_state;
    event_bus_.publish(evt);
}

void FeatureRegistry::set_enabled(const std::string& feature_id, bool enabled)
{
    {
        const std::lock_guard lock(mutex_);
        auto iter = features_.find(feature_id);
        if (iter == features_.end())
        {
            return;
        }

        if (iter->second.enabled == enabled)
        {
            return; // No change
        }

        iter->second.enabled = enabled;
        config_.set(config_key(feature_id), enabled);
    }

    // Fire event outside lock
    events::FeatureToggledEvent evt;
    evt.feature_id = feature_id;
    evt.enabled = enabled;
    event_bus_.publish(evt);
}

auto FeatureRegistry::get_all_features() const -> std::vector<FeatureInfo>
{
    const std::lock_guard lock(mutex_);
    std::vector<FeatureInfo> result;
    result.reserve(features_.size());
    for (const auto& [id, entry] : features_)
    {
        result.push_back(entry.info);
    }
    return result;
}

auto FeatureRegistry::get_feature(const std::string& feature_id) const -> const FeatureInfo*
{
    const std::lock_guard lock(mutex_);
    auto iter = features_.find(feature_id);
    if (iter == features_.end())
    {
        return nullptr;
    }
    return &iter->second.info;
}

auto FeatureRegistry::feature_count() const -> std::size_t
{
    const std::lock_guard lock(mutex_);
    return features_.size();
}

auto FeatureRegistry::config_key(const std::string& feature_id) -> std::string
{
    return "feature." + feature_id + ".enabled";
}

} // namespace markamp::core
