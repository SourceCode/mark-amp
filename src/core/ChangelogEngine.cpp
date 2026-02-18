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

auto ChangelogEngine::entry_count() const -> int
{
    return static_cast<int>(entries_.size());
}

void ChangelogEngine::clear()
{
    entries_.clear();
}

} // namespace markamp::core
