// ============================================================================
// File: src/core/PublishingProfileManager.cpp
// Phase 24: Export & Publishing — Saved publishing profile management
// ============================================================================

#include "PublishingProfileManager.h"

#include <algorithm>
#include <chrono>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

PublishingProfileManager::PublishingProfileManager() = default;

// ============================================================================
// CRUD
// ============================================================================

auto PublishingProfileManager::create_profile(PublishingProfile profile) -> std::string
{
    // Assign an id if not provided.
    if (profile.profile_id.empty())
    {
        profile.profile_id = "profile-" + std::to_string(next_id_++);
    }

    // Check for duplicate id.
    if (find_iter(profile.profile_id) != profiles_.end())
    {
        return "";
    }

    auto id = profile.profile_id;
    profiles_.push_back(std::move(profile));
    return id;
}

auto PublishingProfileManager::update_profile(const PublishingProfile& profile) -> bool
{
    auto iter = find_iter(profile.profile_id);
    if (iter == profiles_.end())
    {
        return false;
    }
    *iter = profile;
    return true;
}

auto PublishingProfileManager::delete_profile(const std::string& profile_id) -> bool
{
    auto iter = find_iter(profile_id);
    if (iter == profiles_.end())
    {
        return false;
    }
    profiles_.erase(iter);
    return true;
}

auto PublishingProfileManager::get_profile(const std::string& profile_id) const
    -> std::optional<PublishingProfile>
{
    auto iter = find_iter(profile_id);
    if (iter == profiles_.end())
    {
        return std::nullopt;
    }
    return *iter;
}

auto PublishingProfileManager::list_profiles() const -> std::vector<PublishingProfile>
{
    auto result = profiles_;
    std::sort(result.begin(),
              result.end(),
              [](const PublishingProfile& lhs, const PublishingProfile& rhs)
              { return lhs.name < rhs.name; });
    return result;
}

auto PublishingProfileManager::duplicate_profile(const std::string& source_id,
                                                 const std::string& new_name) -> std::string
{
    auto source = get_profile(source_id);
    if (!source.has_value())
    {
        return "";
    }

    PublishingProfile dup = source.value();
    dup.profile_id = "profile-" + std::to_string(next_id_++);
    dup.name = new_name;
    dup.last_used_ms = 0;
    dup.use_count = 0;

    auto id = dup.profile_id;
    profiles_.push_back(std::move(dup));
    return id;
}

// ============================================================================
// Execution tracking
// ============================================================================

auto PublishingProfileManager::mark_used(const std::string& profile_id) -> void
{
    auto iter = find_iter(profile_id);
    if (iter == profiles_.end())
    {
        return;
    }
    iter->use_count++;
    auto now = std::chrono::system_clock::now();
    iter->last_used_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

auto PublishingProfileManager::most_recent_profile() const -> std::optional<PublishingProfile>
{
    if (profiles_.empty())
    {
        return std::nullopt;
    }

    auto iter = std::max_element(profiles_.begin(),
                                 profiles_.end(),
                                 [](const PublishingProfile& lhs, const PublishingProfile& rhs)
                                 { return lhs.last_used_ms < rhs.last_used_ms; });

    if (iter->last_used_ms == 0)
    {
        return std::nullopt; // None have been used.
    }
    return *iter;
}

// ============================================================================
// Persistence
// ============================================================================

auto PublishingProfileManager::serialize() const -> std::string
{
    std::ostringstream oss;
    oss << "[\n";

    for (size_t idx = 0; idx < profiles_.size(); ++idx)
    {
        const auto& prof = profiles_[idx];
        oss << "  {\n"
            << "    \"profile_id\": \"" << prof.profile_id << "\",\n"
            << "    \"name\": \"" << prof.name << "\",\n"
            << "    \"template_id\": \"" << prof.template_id << "\",\n"
            << "    \"output_directory\": \"" << prof.output_directory << "\",\n"
            << "    \"auto_open\": " << (prof.auto_open ? "true" : "false") << ",\n"
            << "    \"last_used_ms\": " << prof.last_used_ms << ",\n"
            << "    \"use_count\": " << prof.use_count << ",\n"
            << "    \"format\": " << static_cast<int>(prof.export_options.format) << ",\n"
            << "    \"include_toc\": " << (prof.export_options.include_toc ? "true" : "false")
            << ",\n"
            << "    \"include_frontmatter\": "
            << (prof.export_options.include_frontmatter ? "true" : "false") << "\n"
            << "  }";
        if (idx + 1 < profiles_.size())
        {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "]\n";
    return oss.str();
}

auto PublishingProfileManager::deserialize(const std::string& json_str) -> int
{
    int loaded = 0;

    // Match JSON objects containing profile_id and name fields.
    static const std::regex kProfileBlock(
        R"xx(\{[^}]*"profile_id"\s*:\s*"([^"]*)"[^}]*"name"\s*:\s*"([^"]*)"[^}]*\})xx");

    static const std::regex kTemplateIdRe(R"xx("template_id"\s*:\s*"([^"]*)")xx");
    static const std::regex kOutputDirRe(R"xx("output_directory"\s*:\s*"([^"]*)")xx");
    static const std::regex kAutoOpenRe(R"xx("auto_open"\s*:\s*(true|false))xx");
    static const std::regex kFormatRe(R"xx("format"\s*:\s*(\d+))xx");
    static const std::regex kUseCountRe(R"xx("use_count"\s*:\s*(\d+))xx");

    auto begin = std::sregex_iterator(json_str.begin(), json_str.end(), kProfileBlock);
    auto end_iter = std::sregex_iterator();

    for (auto iter = begin; iter != end_iter; ++iter)
    {
        const std::string match_block = (*iter).str();
        PublishingProfile prof;
        prof.profile_id = (*iter)[1].str();
        prof.name = (*iter)[2].str();

        std::smatch sub_match;
        if (std::regex_search(match_block, sub_match, kTemplateIdRe))
        {
            prof.template_id = sub_match[1].str();
        }
        if (std::regex_search(match_block, sub_match, kOutputDirRe))
        {
            prof.output_directory = sub_match[1].str();
        }
        if (std::regex_search(match_block, sub_match, kAutoOpenRe))
        {
            prof.auto_open = (sub_match[1].str() == "true");
        }
        if (std::regex_search(match_block, sub_match, kFormatRe))
        {
            prof.export_options.format = static_cast<ExportFormat>(std::stoi(sub_match[1].str()));
        }
        if (std::regex_search(match_block, sub_match, kUseCountRe))
        {
            prof.use_count = std::stoi(sub_match[1].str());
        }

        // Update next_id_ to avoid collisions.
        static const std::regex kIdNum(R"xx(profile-(\d+))xx");
        if (std::regex_search(prof.profile_id, sub_match, kIdNum))
        {
            const int parsed_id = std::stoi(sub_match[1].str());
            if (parsed_id >= next_id_)
            {
                next_id_ = parsed_id + 1;
            }
        }

        profiles_.push_back(std::move(prof));
        ++loaded;
    }

    return loaded;
}

// ============================================================================
// Validation
// ============================================================================

auto PublishingProfileManager::validate(const PublishingProfile& profile)
    -> std::vector<std::string>
{
    std::vector<std::string> errors;

    if (profile.name.empty())
    {
        errors.emplace_back("Profile name is required");
    }
    if (profile.output_directory.empty())
    {
        errors.emplace_back("Output directory is required");
    }

    return errors;
}

// ============================================================================
// Stats & helpers
// ============================================================================

auto PublishingProfileManager::count() const -> size_t
{
    return profiles_.size();
}

auto PublishingProfileManager::find_iter(const std::string& profile_id)
    -> std::vector<PublishingProfile>::iterator
{
    return std::find_if(profiles_.begin(),
                        profiles_.end(),
                        [&](const PublishingProfile& prof)
                        { return prof.profile_id == profile_id; });
}

auto PublishingProfileManager::find_iter(const std::string& profile_id) const
    -> std::vector<PublishingProfile>::const_iterator
{
    return std::find_if(profiles_.begin(),
                        profiles_.end(),
                        [&](const PublishingProfile& prof)
                        { return prof.profile_id == profile_id; });
}

} // namespace markamp::core
