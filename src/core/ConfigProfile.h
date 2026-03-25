#pragma once

#include "Config.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Predefined profile identifiers.
enum class ProfileId
{
    kDeveloper,
    kResearch,
    kWhiteboard,
    kCustom
};

/// A named collection of setting overrides that can be applied as a group.
struct ConfigProfile
{
    std::string name;
    std::string description;
    ProfileId profile_id{ProfileId::kCustom};
    std::unordered_map<std::string, std::string> overrides;
};

/// Manages named settings profiles (Developer, Research, Whiteboard, etc.).
/// Profiles are applied as a batch of setting overrides to a Config instance.
class ConfigProfileManager
{
public:
    ConfigProfileManager();

    /// Register a named profile.
    void register_profile(ConfigProfile profile);

    /// Apply a profile to a Config, batch-setting all overrides.
    void apply_profile(const std::string& profile_name, Config& target) const;

    /// Apply a profile by ID.
    void apply_profile(ProfileId profile_id, Config& target) const;

    /// Return all registered profile names.
    [[nodiscard]] auto profile_names() const -> std::vector<std::string>;

    /// Return a specific profile by name. Returns nullptr if not found.
    [[nodiscard]] auto find_profile(const std::string& name) const -> const ConfigProfile*;

    /// Return a specific profile by ID. Returns nullptr if not found.
    [[nodiscard]] auto find_profile(ProfileId profile_id) const -> const ConfigProfile*;

    /// Number of registered profiles.
    [[nodiscard]] auto profile_count() const -> std::size_t;

    /// Export a profile to a JSON-like string.
    [[nodiscard]] auto export_profile(const std::string& name) const -> std::string;

    /// Import a profile from a JSON string.
    auto import_profile(const std::string& json_str) -> bool;

    /// Create a custom profile from the current diff between a Config and its defaults.
    [[nodiscard]] auto create_from_diff(const std::string& name,
                                        const Config& current,
                                        const Config& defaults) -> ConfigProfile;

private:
    std::vector<ConfigProfile> profiles_;

    /// Register the 4 built-in profiles.
    void register_builtins();
};

} // namespace markamp::core
