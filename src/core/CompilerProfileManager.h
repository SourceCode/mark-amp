#pragma once

/// @file CompilerProfileManager.h
/// @brief Phase 37 Task 14 — Manage compiler configuration profiles.

#include "CompilerConfig.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A named compiler configuration profile.
struct CompilerProfile
{
    std::string name; ///< e.g., "Debug", "Release", "Sanitizer"
    std::string description;
    CompilerConfig config;
    bool is_builtin{false}; ///< True for built-in profiles
};

/// Manages compiler configuration profiles (Debug, Release, sanitizer, etc.).
class CompilerProfileManager
{
public:
    CompilerProfileManager();

    /// Register a profile. Replaces if name already exists.
    void add_profile(CompilerProfile profile);

    /// Remove a profile by name. Returns false if not found or is built-in.
    auto remove_profile(const std::string& name) -> bool;

    /// Find a profile by name. Returns nullptr if not found.
    [[nodiscard]] auto find_profile(const std::string& name) const -> const CompilerProfile*;

    /// Get all registered profiles.
    [[nodiscard]] auto all_profiles() const -> const std::vector<CompilerProfile>&;

    /// Get profile names in order.
    [[nodiscard]] auto profile_names() const -> std::vector<std::string>;

    /// Apply a profile to a CompilerConfig.
    [[nodiscard]] auto apply_profile(const std::string& name) const -> CompilerConfig;

    /// Profile count.
    [[nodiscard]] auto profile_count() const -> std::size_t;

    /// Save all profiles to a directory.
    void save_profiles(const std::string& dir_path) const;

    /// Load profiles from a directory.
    void load_profiles(const std::string& dir_path);

    /// Export a profile to JSON.
    [[nodiscard]] auto export_profile(const std::string& name) const -> std::string;

    /// Import a profile from JSON.
    void import_profile(const std::string& json_str);

private:
    std::vector<CompilerProfile> profiles_;

    /// Register the built-in profiles (Debug, Release, RelWithDebInfo, Sanitizer).
    void register_builtins();
};

} // namespace markamp::core
