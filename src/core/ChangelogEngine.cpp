/// @file ChangelogEngine.cpp
/// @brief V9 Phase 50 — ChangelogEngine implementation.

#include "ChangelogEngine.h"

#include <algorithm>

namespace markamp::core
{

void ChangelogEngine::add_entry(ChangelogEntry entry)
{
    entries_.push_back(std::move(entry));
}

auto ChangelogEngine::get_entries(const std::string& version) const -> std::vector<ChangelogEntry>
{
    std::vector<ChangelogEntry> result;
    for (const auto& entry : entries_)
    {
        if (entry.version == version)
        {
            result.push_back(entry);
        }
    }
    return result;
}

auto ChangelogEngine::get_all_entries() const -> const std::vector<ChangelogEntry>&
{
    return entries_;
}

auto ChangelogEngine::generate_markdown() const -> std::string
{
    if (entries_.empty())
    {
        return "";
    }

    std::string markdown = "# Changelog\n\n";

    // Group by version — collect unique versions in order
    std::vector<std::string> versions;
    for (const auto& entry : entries_)
    {
        if (std::find(versions.begin(), versions.end(), entry.version) == versions.end())
        {
            versions.push_back(entry.version);
        }
    }

    for (const auto& ver : versions)
    {
        auto ver_entries = get_entries(ver);
        if (ver_entries.empty())
            continue;

        markdown += "## [" + ver + "] - " + ver_entries[0].date + "\n\n";

        // Group by category
        std::vector<std::string> categories;
        for (const auto& entry : ver_entries)
        {
            if (std::find(categories.begin(), categories.end(), entry.category) == categories.end())
            {
                categories.push_back(entry.category);
            }
        }

        for (const auto& cat : categories)
        {
            markdown += "### " + cat + "\n\n";
            for (const auto& entry : ver_entries)
            {
                if (entry.category == cat)
                {
                    markdown += "- " + entry.message + "\n";
                }
            }
            markdown += "\n";
        }
    }

    return markdown;
}

void ChangelogEngine::load_defaults()
{
    add_entry({"2026-02-18", "9.0.0", "Added", "Data processing and computation engine"});
    add_entry({"2026-02-18", "9.0.0", "Added", "End-to-end integration testing framework"});
    add_entry({"2026-02-18", "9.0.0", "Added", "Release preparation infrastructure"});
    add_entry({"2026-02-17", "8.0.0", "Added", "Localization and internationalization support"});
    add_entry({"2026-02-17", "8.0.0", "Changed", "Multi-window workspace management"});
}

// (#59) Remove all entries for a specific version.
void ChangelogEngine::remove_entries_for_version(const std::string& version)
{
    entries_.erase(std::remove_if(entries_.begin(),
                                  entries_.end(),
                                  [&version](const ChangelogEntry& entry)
                                  { return entry.version == version; }),
                   entries_.end());
}

// (#59) Search entries by keyword across message and category.
auto ChangelogEngine::search_entries(const std::string& keyword) const
    -> std::vector<ChangelogEntry>
{
    if (keyword.empty())
    {
        return entries_;
    }
    auto lower_kw = keyword;
    std::transform(lower_kw.begin(),
                   lower_kw.end(),
                   lower_kw.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    std::vector<ChangelogEntry> results;
    for (const auto& entry : entries_)
    {
        auto lower_msg = entry.message;
        std::transform(lower_msg.begin(),
                       lower_msg.end(),
                       lower_msg.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        if (lower_msg.find(lower_kw) != std::string::npos)
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto ChangelogEngine::entry_count() const -> int
{
    return static_cast<int>(entries_.size());
}

void ChangelogEngine::clear()
{
    entries_.clear();
}

} // namespace markamp::core
