#pragma once

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

class Config;

/// A single key migration rule: maps an old config key to a new one,
/// with an optional value transform function.
struct MigrationRule
{
    std::string old_key;
    std::string new_key;
    /// Optional transform: converts the old value string to the new value string.
    /// If nullptr, the value is copied as-is.
    std::function<std::string(const std::string&)> transform;
};

/// Manages config key migrations (e.g. renaming or restructuring keys).
/// Run migrations on load() to transparently upgrade old config formats.
class ConfigMigration
{
public:
    /// Register a simple key rename (no value transform).
    void add_rename(std::string old_key, std::string new_key);

    /// Register a key migration with a custom value transform.
    void add_rule(MigrationRule rule);

    /// Apply all registered migrations to a Config instance.
    /// Returns the number of keys migrated.
    auto apply(Config& config) const -> std::size_t;

    /// Return the number of registered migration rules.
    [[nodiscard]] auto rule_count() const -> std::size_t;

    /// Return all registered rules (for introspection).
    [[nodiscard]] auto rules() const -> const std::vector<MigrationRule>&;

    /// (#91) Check if a migration rule exists for a given old key.
    [[nodiscard]] auto has_rule(const std::string& old_key) const -> bool;

    // ── Batch 19-22 (#122-123) ──

    /// (#122) Return the number of rules that would trigger for a given Config.
    [[nodiscard]] auto pending_count(const Config& config) const -> std::size_t;

    /// (#123) Remove all registered migration rules.
    void clear_rules();

private:
    std::vector<MigrationRule> rules_;
};

} // namespace markamp::core
