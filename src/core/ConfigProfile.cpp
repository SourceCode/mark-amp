#include "ConfigProfile.h"

#include "Logger.h"

#include <nlohmann/json.hpp>

namespace markamp::core
{

ConfigProfileManager::ConfigProfileManager()
{
    register_builtins();
}

void ConfigProfileManager::register_profile(ConfigProfile profile)
{
    // Replace existing profile with same name
    for (auto& existing : profiles_)
    {
        if (existing.name == profile.name)
        {
            existing = std::move(profile);
            return;
        }
    }
    profiles_.push_back(std::move(profile));
}

void ConfigProfileManager::apply_profile(const std::string& profile_name, Config& target) const
{
    const auto* profile = find_profile(profile_name);
    if (profile == nullptr)
    {
        MARKAMP_LOG_WARN("ConfigProfileManager: profile '{}' not found", profile_name);
        return;
    }

    MARKAMP_LOG_INFO("Applying settings profile: {}", profile_name);

    for (const auto& [key, value] : profile->overrides)
    {
        target.set(key, std::string_view(value));
    }
}

void ConfigProfileManager::apply_profile(ProfileId profile_id, Config& target) const
{
    const auto* profile = find_profile(profile_id);
    if (profile == nullptr)
    {
        MARKAMP_LOG_WARN("ConfigProfileManager: profile ID not found");
        return;
    }
    apply_profile(profile->name, target);
}

auto ConfigProfileManager::profile_names() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    for (const auto& profile : profiles_)
    {
        names.push_back(profile.name);
    }
    return names;
}

auto ConfigProfileManager::find_profile(const std::string& name) const -> const ConfigProfile*
{
    for (const auto& profile : profiles_)
    {
        if (profile.name == name)
        {
            return &profile;
        }
    }
    return nullptr;
}

auto ConfigProfileManager::find_profile(ProfileId profile_id) const -> const ConfigProfile*
{
    for (const auto& profile : profiles_)
    {
        if (profile.profile_id == profile_id)
        {
            return &profile;
        }
    }
    return nullptr;
}

auto ConfigProfileManager::profile_count() const -> std::size_t
{
    return profiles_.size();
}

auto ConfigProfileManager::export_profile(const std::string& name) const -> std::string
{
    const auto* profile = find_profile(name);
    if (profile == nullptr)
    {
        return "{}";
    }

    nlohmann::json json_obj;
    json_obj["name"] = profile->name;
    json_obj["description"] = profile->description;
    json_obj["overrides"] = profile->overrides;
    return json_obj.dump(2);
}

auto ConfigProfileManager::create_from_diff(const std::string& name,
                                            const Config& current,
                                            const Config& defaults) -> ConfigProfile
{
    ConfigProfile profile;
    profile.name = name;
    profile.description = "Custom profile";
    profile.profile_id = ProfileId::kCustom;

    auto diff_keys = current.diff(defaults);
    for (const auto& key : diff_keys)
    {
        profile.overrides[key] = current.get_string(key);
    }

    register_profile(profile);
    return profile;
}

void ConfigProfileManager::register_builtins()
{
    // Developer profile: code-focused, all tools visible
    {
        ConfigProfile dev;
        dev.name = "Developer";
        dev.description = "Optimized for software development with full tooling visibility";
        dev.profile_id = ProfileId::kDeveloper;
        dev.overrides = {
            {"show_line_numbers", "true"},
            {"show_minimap", "true"},
            {"bracket_matching", "true"},
            {"bracket_pair_colorization", "true"},
            {"code_folding", "true"},
            {"indent_guides", "true"},
            {"auto_indent", "true"},
            {"highlight_current_line", "true"},
            {"show_whitespace", "false"},
            {"tab_size", "4"},
            {"edge_column", "120"},
            {"view_mode", "split"},
            {"editor.formatOnSave", "true"},
            {"editor.linkedEditing", "true"},
        };
        profiles_.push_back(std::move(dev));
    }

    // Research profile: reading-focused, distraction-free
    {
        ConfigProfile research;
        research.name = "Research";
        research.description = "Optimized for reading and research with focus mode";
        research.profile_id = ProfileId::kResearch;
        research.overrides = {
            {"show_line_numbers", "false"},
            {"show_minimap", "false"},
            {"word_wrap", "true"},
            {"word_wrap_column", "80"},
            {"font_size", "16"},
            {"view_mode", "preview"},
            {"sidebar_visible", "false"},
            {"highlight_current_line", "false"},
            {"show_whitespace", "false"},
            {"edge_column", "0"},
            {"editor.padding_top", "20"},
            {"editor.padding_bottom", "20"},
        };
        profiles_.push_back(std::move(research));
    }

    // Whiteboard profile: canvas-focused
    {
        ConfigProfile whiteboard;
        whiteboard.name = "Whiteboard";
        whiteboard.description = "Optimized for canvas and whiteboard workflows";
        whiteboard.profile_id = ProfileId::kWhiteboard;
        whiteboard.overrides = {
            {"view_mode", "editor"},
            {"sidebar_visible", "true"},
            {"show_line_numbers", "false"},
            {"show_minimap", "false"},
            {"word_wrap", "true"},
            {"font_size", "14"},
            {"show_status_bar", "true"},
        };
        profiles_.push_back(std::move(whiteboard));
    }

    // Notebook profile: writing-focused with comfortable typography
    {
        ConfigProfile notebook;
        notebook.name = "Notebook";
        notebook.description = "Optimized for note-taking with comfortable typography";
        notebook.profile_id = ProfileId::kNotebook;
        notebook.overrides = {
            {"font_size", "15"},
            {"font_family", "Georgia"},
            {"word_wrap", "true"},
            {"word_wrap_column", "72"},
            {"view_mode", "split"},
            {"show_line_numbers", "false"},
            {"show_minimap", "false"},
            {"highlight_current_line", "true"},
            {"auto_save", "true"},
            {"auto_save_interval_seconds", "30"},
            {"sidebar_visible", "true"},
            {"editor.line_height", "4"},
            {"editor.letter_spacing", "0.3"},
            {"editor.padding_top", "16"},
            {"editor.padding_bottom", "16"},
        };
        profiles_.push_back(std::move(notebook));
    }
}

} // namespace markamp::core
