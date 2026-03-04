// ============================================================================
// File: src/ui/ChangelogPanel.cpp
// Phase 48: Welcome and Onboarding — Changelog panel model
// ============================================================================
#include "ChangelogPanel.h"

#include <sstream>

namespace markamp::ui
{

void ChangelogPanelModel::parse(const std::string& changelog_text)
{
    versions_.clear();

    std::istringstream stream(changelog_text);
    std::string line;
    ChangelogVersion* current_version = nullptr;

    while (std::getline(stream, line))
    {
        // Detect version headers: "## [1.2.3] - 2024-01-01" or "## v1.2.3"
        if (line.starts_with("## "))
        {
            versions_.emplace_back();
            current_version = &versions_.back();

            std::string header = line.substr(3);
            // Extract version and date.
            auto dash_pos = header.find(" - ");
            if (dash_pos != std::string::npos)
            {
                current_version->version = header.substr(0, dash_pos);
                current_version->date = header.substr(dash_pos + 3);
            }
            else
            {
                current_version->version = header;
            }

            // Remove brackets if present: "[1.2.3]" -> "1.2.3"
            auto& ver = current_version->version;
            if (ver.starts_with("[") && ver.ends_with("]"))
            {
                ver = ver.substr(1, ver.size() - 2);
            }
        }
        else if (current_version && line.starts_with("- "))
        {
            std::string entry_text = line.substr(2);
            current_version->entries.push_back(
                {.description = entry_text, .category = detect_category(entry_text)});
        }
    }
}

void ChangelogPanelModel::toggle_version(size_t index)
{
    if (index < versions_.size())
    {
        versions_[index].is_collapsed = !versions_[index].is_collapsed;
    }
}

auto ChangelogPanelModel::detect_category(const std::string& text) -> ChangelogCategory
{
    // Simple keyword detection.
    if (text.find("BREAKING") != std::string::npos || text.find("breaking") != std::string::npos)
    {
        return ChangelogCategory::Breaking;
    }
    if (text.find("[fix]") != std::string::npos || text.find("Fix") != std::string::npos ||
        text.find("fix:") != std::string::npos)
    {
        return ChangelogCategory::Fix;
    }
    if (text.find("deprecat") != std::string::npos)
    {
        return ChangelogCategory::Deprecated;
    }
    if (text.find("improve") != std::string::npos || text.find("refactor") != std::string::npos ||
        text.find("perf") != std::string::npos)
    {
        return ChangelogCategory::Improvement;
    }
    return ChangelogCategory::Feature;
}

} // namespace markamp::ui
