#include "ConfigMigration.h"

#include "Config.h"
#include "Logger.h"

namespace markamp::core
{

void ConfigMigration::add_rename(std::string old_key, std::string new_key)
{
    rules_.push_back({std::move(old_key), std::move(new_key), nullptr});
}

void ConfigMigration::add_rule(MigrationRule rule)
{
    rules_.push_back(std::move(rule));
}

auto ConfigMigration::apply(Config& config) const -> std::size_t
{
    std::size_t migrated = 0;

    for (const auto& rule : rules_)
    {
        if (!config.has_key(rule.old_key))
        {
            continue;
        }

        auto old_value = config.get_string(rule.old_key);

        std::string new_value = old_value;
        if (rule.transform)
        {
            new_value = rule.transform(old_value);
        }

        // Only migrate if the new key doesn't already exist
        if (!config.has_key(rule.new_key))
        {
            config.set(rule.new_key, std::string_view(new_value));
            MARKAMP_LOG_INFO("Config migration: '{}' → '{}' (value: '{}')",
                             rule.old_key,
                             rule.new_key,
                             new_value);
        }

        config.remove(rule.old_key);
        ++migrated;
    }

    if (migrated > 0)
    {
        MARKAMP_LOG_INFO("Applied {} config key migration(s)", migrated);
    }

    return migrated;
}

auto ConfigMigration::rule_count() const -> std::size_t
{
    return rules_.size();
}

auto ConfigMigration::rules() const -> const std::vector<MigrationRule>&
{
    return rules_;
}

} // namespace markamp::core
