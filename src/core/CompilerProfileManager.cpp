/// @file CompilerProfileManager.cpp
/// @brief Phase 37 Task 14 — Compiler profile management implementation.

#include "core/CompilerProfileManager.h"

#include <filesystem>
#include <fstream>

namespace markamp::core
{

CompilerProfileManager::CompilerProfileManager()
{
    register_builtins();
}

void CompilerProfileManager::register_builtins()
{
    // Debug profile
    {
        CompilerProfile debug;
        debug.name = "Debug";
        debug.description = "Debug build with full symbols, no optimization, extra warnings";
        debug.is_builtin = true;
        debug.config = CompilerConfig::default_debug();
        profiles_.push_back(std::move(debug));
    }

    // Release profile
    {
        CompilerProfile release;
        release.name = "Release";
        release.description = "Optimized release build with NDEBUG and ThinLTO";
        release.is_builtin = true;
        release.config = CompilerConfig::default_release();
        profiles_.push_back(std::move(release));
    }

    // RelWithDebInfo profile
    {
        CompilerProfile rwdi;
        rwdi.name = "RelWithDebInfo";
        rwdi.description = "Release build with debug info for profiling";
        rwdi.is_builtin = true;
        rwdi.config.optimization_level = "O2";
        rwdi.config.warning_level = "all";
        rwdi.config.cpp_standard = "c++17";
        rwdi.config.defines.push_back("NDEBUG");
        rwdi.config.custom_flags.push_back("-g");
        profiles_.push_back(std::move(rwdi));
    }

    // Sanitizer profile
    {
        CompilerProfile sanitizer;
        sanitizer.name = "Sanitizer";
        sanitizer.description = "Debug build with ASan and UBSan for runtime checks";
        sanitizer.is_builtin = true;
        sanitizer.config.optimization_level = "O0";
        sanitizer.config.warning_level = "extra";
        sanitizer.config.cpp_standard = "c++17";
        sanitizer.config.enable_asan = true;
        sanitizer.config.enable_ubsan = true;
        sanitizer.config.defines.push_back("DEBUG=1");
        profiles_.push_back(std::move(sanitizer));
    }
}

void CompilerProfileManager::add_profile(CompilerProfile profile)
{
    for (auto& p : profiles_)
    {
        if (p.name == profile.name)
        {
            p = std::move(profile);
            return;
        }
    }
    profiles_.push_back(std::move(profile));
}

auto CompilerProfileManager::remove_profile(const std::string& name) -> bool
{
    for (auto it = profiles_.begin(); it != profiles_.end(); ++it)
    {
        if (it->name == name)
        {
            if (it->is_builtin)
                return false;
            profiles_.erase(it);
            return true;
        }
    }
    return false;
}

auto CompilerProfileManager::find_profile(const std::string& name) const -> const CompilerProfile*
{
    for (const auto& p : profiles_)
    {
        if (p.name == name)
            return &p;
    }
    return nullptr;
}

auto CompilerProfileManager::all_profiles() const -> const std::vector<CompilerProfile>&
{
    return profiles_;
}

auto CompilerProfileManager::profile_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    for (const auto& p : profiles_)
    {
        names.push_back(p.name);
    }
    return names;
}

auto CompilerProfileManager::apply_profile(const std::string& name) const -> CompilerConfig
{
    auto* profile = find_profile(name);
    if (profile)
        return profile->config;
    return {};
}

auto CompilerProfileManager::profile_count() const -> std::size_t
{
    return profiles_.size();
}

void CompilerProfileManager::save_profiles(const std::string& dir_path) const
{
    std::filesystem::create_directories(dir_path);

    for (const auto& profile : profiles_)
    {
        if (profile.is_builtin)
            continue; // Don't persist built-in profiles

        auto file_path = std::filesystem::path(dir_path) / (profile.name + ".json");
        std::ofstream out(file_path);
        if (out.is_open())
        {
            out << profile.config.to_json();
        }
    }
}

void CompilerProfileManager::load_profiles(const std::string& dir_path)
{
    if (!std::filesystem::exists(dir_path))
        return;

    for (const auto& entry : std::filesystem::directory_iterator(dir_path))
    {
        if (entry.path().extension() != ".json")
            continue;

        std::ifstream in(entry.path());
        if (!in.is_open())
            continue;

        std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        CompilerProfile profile;
        profile.name = entry.path().stem().string();
        profile.config = CompilerConfig::from_json(content);
        profile.is_builtin = false;

        // Don't overwrite built-in profiles
        bool is_builtin_name = false;
        for (const auto& p : profiles_)
        {
            if (p.name == profile.name && p.is_builtin)
            {
                is_builtin_name = true;
                break;
            }
        }
        if (!is_builtin_name)
        {
            add_profile(std::move(profile));
        }
    }
}

auto CompilerProfileManager::export_profile(const std::string& name) const -> std::string
{
    auto* profile = find_profile(name);
    if (profile)
        return profile->config.to_json();
    return {};
}

void CompilerProfileManager::import_profile(const std::string& json_str)
{
    auto config = CompilerConfig::from_json(json_str);
    CompilerProfile profile;
    profile.name = "Imported";
    profile.description = "Imported profile";
    profile.config = std::move(config);
    profile.is_builtin = false;
    add_profile(std::move(profile));
}

} // namespace markamp::core
