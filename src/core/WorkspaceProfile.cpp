/// @file WorkspaceProfile.cpp
/// @brief V9 Phase 38 — WorkspaceProfileManager implementation.

#include "WorkspaceProfile.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ── CRUD ──────────────────────────────────────────────────────────────────────

void WorkspaceProfileManager::save_profile(WorkspaceProfile profile)
{
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

auto WorkspaceProfileManager::get_profile(const std::string& name) const -> const WorkspaceProfile*
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

auto WorkspaceProfileManager::delete_profile(const std::string& name) -> bool
{
    auto iter = std::find_if(profiles_.begin(),
                             profiles_.end(),
                             [&name](const WorkspaceProfile& prof) { return prof.name == name; });
    if (iter == profiles_.end() || iter->is_builtin)
    {
        return false;
    }
    profiles_.erase(iter);
    return true;
}

auto WorkspaceProfileManager::rename_profile(const std::string& old_name,
                                             const std::string& new_name) -> bool
{
    // Check new name doesn't exist
    for (const auto& prof : profiles_)
    {
        if (prof.name == new_name)
        {
            return false;
        }
    }
    for (auto& prof : profiles_)
    {
        if (prof.name == old_name && !prof.is_builtin)
        {
            prof.name = new_name;
            return true;
        }
    }
    return false;
}

auto WorkspaceProfileManager::list_profiles() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    for (const auto& prof : profiles_)
    {
        names.push_back(prof.name);
    }
    return names;
}

auto WorkspaceProfileManager::profile_count() const -> std::size_t
{
    return profiles_.size();
}

auto WorkspaceProfileManager::has_profile(const std::string& name) const -> bool
{
    return get_profile(name) != nullptr;
}

// ── Diff ──────────────────────────────────────────────────────────────────────

auto WorkspaceProfileManager::diff_profiles(const std::string& name_a,
                                            const std::string& name_b) const -> ProfileDiff
{
    ProfileDiff diff;
    diff.left_name = name_a;
    diff.right_name = name_b;

    const auto* prof_a = get_profile(name_a);
    const auto* prof_b = get_profile(name_b);
    if (prof_a == nullptr || prof_b == nullptr)
    {
        return diff;
    }

    if (prof_a->settings_json != prof_b->settings_json)
    {
        ProfileDiffEntry entry;
        entry.section = "settings";
        entry.key = "content";
        entry.left_val = prof_a->settings_json.empty() ? "(empty)" : "(configured)";
        entry.right_val = prof_b->settings_json.empty() ? "(empty)" : "(configured)";
        diff.differences.push_back(std::move(entry));
    }
    if (prof_a->layout_json != prof_b->layout_json)
    {
        ProfileDiffEntry entry;
        entry.section = "layout";
        entry.key = "content";
        entry.left_val = prof_a->layout_json.empty() ? "(empty)" : "(configured)";
        entry.right_val = prof_b->layout_json.empty() ? "(empty)" : "(configured)";
        diff.differences.push_back(std::move(entry));
    }
    if (prof_a->vault_css != prof_b->vault_css)
    {
        ProfileDiffEntry entry;
        entry.section = "css";
        entry.key = "content";
        entry.left_val = prof_a->vault_css.empty() ? "(empty)" : "(configured)";
        entry.right_val = prof_b->vault_css.empty() ? "(empty)" : "(configured)";
        diff.differences.push_back(std::move(entry));
    }
    if (prof_a->font_config != prof_b->font_config)
    {
        ProfileDiffEntry entry;
        entry.section = "fonts";
        entry.key = "content";
        entry.left_val = prof_a->font_config.empty() ? "(empty)" : "(configured)";
        entry.right_val = prof_b->font_config.empty() ? "(empty)" : "(configured)";
        diff.differences.push_back(std::move(entry));
    }
    if (prof_a->theme_overrides != prof_b->theme_overrides)
    {
        ProfileDiffEntry entry;
        entry.section = "theme";
        entry.key = "content";
        entry.left_val = prof_a->theme_overrides.empty() ? "(empty)" : "(configured)";
        entry.right_val = prof_b->theme_overrides.empty() ? "(empty)" : "(configured)";
        diff.differences.push_back(std::move(entry));
    }

    return diff;
}

// ── Export / Import ───────────────────────────────────────────────────────────

auto WorkspaceProfileManager::export_profile(const std::string& name) const -> std::string
{
    const auto* prof = get_profile(name);
    if (prof == nullptr)
    {
        return "{}";
    }

    std::ostringstream oss;
    oss << R"({"name":")" << prof->name << R"(","description":")" << prof->description
        << R"(","settings_json":")" << prof->settings_json << R"(","layout_json":")"
        << prof->layout_json << R"(","vault_css":")" << prof->vault_css << R"(","font_config":")"
        << prof->font_config << R"(","theme_overrides":")" << prof->theme_overrides
        << R"(","is_builtin":)" << (prof->is_builtin ? "true" : "false") << "}";
    return oss.str();
}

auto WorkspaceProfileManager::import_profile(const std::string& json_data) -> std::string
{
    WorkspaceProfile profile;

    // Extract name
    auto name_pos = json_data.find(R"("name":")");
    if (name_pos == std::string::npos)
    {
        return "";
    }
    name_pos += 8;
    auto name_end = json_data.find('"', name_pos);
    if (name_end == std::string::npos)
    {
        return "";
    }
    profile.name = json_data.substr(name_pos, name_end - name_pos);

    // Extract description
    auto desc_pos = json_data.find(R"("description":")");
    if (desc_pos != std::string::npos)
    {
        desc_pos += 15;
        auto desc_end = json_data.find('"', desc_pos);
        if (desc_end != std::string::npos)
        {
            profile.description = json_data.substr(desc_pos, desc_end - desc_pos);
        }
    }

    // Extract settings_json
    auto sett_pos = json_data.find(R"("settings_json":")");
    if (sett_pos != std::string::npos)
    {
        sett_pos += 17;
        auto sett_end = json_data.find('"', sett_pos);
        if (sett_end != std::string::npos)
        {
            profile.settings_json = json_data.substr(sett_pos, sett_end - sett_pos);
        }
    }

    // Extract layout_json
    auto lay_pos = json_data.find(R"("layout_json":")");
    if (lay_pos != std::string::npos)
    {
        lay_pos += 15;
        auto lay_end = json_data.find('"', lay_pos);
        if (lay_end != std::string::npos)
        {
            profile.layout_json = json_data.substr(lay_pos, lay_end - lay_pos);
        }
    }

    // Extract vault_css
    auto css_pos = json_data.find(R"("vault_css":")");
    if (css_pos != std::string::npos)
    {
        css_pos += 13;
        auto css_end = json_data.find('"', css_pos);
        if (css_end != std::string::npos)
        {
            profile.vault_css = json_data.substr(css_pos, css_end - css_pos);
        }
    }

    if (!profile.name.empty())
    {
        const std::string imported_name = profile.name;
        save_profile(std::move(profile));
        return imported_name;
    }
    return "";
}

auto WorkspaceProfileManager::export_all() const -> std::string
{
    std::ostringstream oss;
    oss << R"({"profiles":[)";
    for (std::size_t idx = 0; idx < profiles_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }
        oss << export_profile(profiles_[idx].name);
    }
    oss << "]}";
    return oss.str();
}

// ── Builtins ──────────────────────────────────────────────────────────────────

void WorkspaceProfileManager::load_builtins()
{
    {
        WorkspaceProfile prof;
        prof.name = "Default";
        prof.description = "Default workspace configuration";
        prof.is_builtin = true;
        profiles_.push_back(std::move(prof));
    }
    {
        WorkspaceProfile prof;
        prof.name = "Writing Focus";
        prof.description = "Minimal UI for focused writing";
        prof.settings_json = R"({"editor.minimap": false, "editor.lineNumbers": false})";
        prof.layout_json = R"({"sidebar": false, "statusbar": true})";
        prof.vault_css = "body { max-width: 700px; margin: 0 auto; font-size: 18px; }";
        prof.is_builtin = true;
        profiles_.push_back(std::move(prof));
    }
    {
        WorkspaceProfile prof;
        prof.name = "Research Mode";
        prof.description = "Side-by-side panels for research";
        prof.settings_json = R"({"editor.wordWrap": true, "editor.minimap": true})";
        prof.layout_json = R"({"sidebar": true, "outline": true, "split": true})";
        prof.is_builtin = true;
        profiles_.push_back(std::move(prof));
    }
}

} // namespace markamp::core
