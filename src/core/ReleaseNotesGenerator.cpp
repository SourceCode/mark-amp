/// @file ReleaseNotesGenerator.cpp
/// @brief V9 Phase 50 — ReleaseNotesGenerator implementation.

#include "ReleaseNotesGenerator.h"

#include <algorithm>

namespace markamp::core
{

auto ReleaseNotesGenerator::create_release(const std::string& version, const std::string& date)
    -> std::string
{
    ReleaseNotes release;
    release.release_id = "rel_" + std::to_string(next_id_++);
    release.version = version;
    release.date = date;
    releases_.push_back(std::move(release));
    return releases_.back().release_id;
}

auto ReleaseNotesGenerator::add_entry(const std::string& release_id, ReleaseEntry entry) -> bool
{
    for (auto& release : releases_)
    {
        if (release.release_id == release_id)
        {
            release.entries.push_back(std::move(entry));
            return true;
        }
    }
    return false;
}

auto ReleaseNotesGenerator::get_release(const std::string& release_id) const -> const ReleaseNotes*
{
    for (const auto& release : releases_)
    {
        if (release.release_id == release_id)
        {
            return &release;
        }
    }
    return nullptr;
}

auto ReleaseNotesGenerator::generate_markdown(const std::string& release_id) const -> std::string
{
    const auto* release = get_release(release_id);
    if (release == nullptr)
    {
        return "";
    }

    std::string markdown;
    markdown += "# " + release->version + " (" + release->date + ")\n\n";

    if (!release->summary.empty())
    {
        markdown += release->summary + "\n\n";
    }

    // Group entries by category
    for (auto cat : {ReleaseCategory::kBreaking,
                     ReleaseCategory::kFeature,
                     ReleaseCategory::kImprovement,
                     ReleaseCategory::kFix})
    {
        std::vector<const ReleaseEntry*> cat_entries;
        for (const auto& entry : release->entries)
        {
            if (entry.category == cat)
            {
                cat_entries.push_back(&entry);
            }
        }

        if (!cat_entries.empty())
        {
            markdown += "## " + category_label(cat) + "\n\n";
            for (const auto* entry : cat_entries)
            {
                markdown += "- **" + entry->title + "**";
                if (!entry->description.empty())
                {
                    markdown += ": " + entry->description;
                }
                if (!entry->author.empty())
                {
                    markdown += " (@" + entry->author + ")";
                }
                markdown += "\n";
            }
            markdown += "\n";
        }
    }

    return markdown;
}

auto ReleaseNotesGenerator::list_releases() const -> std::vector<const ReleaseNotes*>
{
    std::vector<const ReleaseNotes*> result;
    result.reserve(releases_.size());
    for (const auto& release : releases_)
    {
        result.push_back(&release);
    }
    return result;
}

auto ReleaseNotesGenerator::release_count() const -> int
{
    return static_cast<int>(releases_.size());
}

void ReleaseNotesGenerator::clear()
{
    releases_.clear();
}

auto ReleaseNotesGenerator::category_label(ReleaseCategory cat) -> std::string
{
    switch (cat)
    {
        case ReleaseCategory::kFeature:
            return "Features";
        case ReleaseCategory::kFix:
            return "Bug Fixes";
        case ReleaseCategory::kBreaking:
            return "Breaking Changes";
        case ReleaseCategory::kImprovement:
            return "Improvements";
    }
    return "Other";
}

} // namespace markamp::core
