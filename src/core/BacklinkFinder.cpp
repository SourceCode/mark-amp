/// @file BacklinkFinder.cpp
/// @brief V13 Phase 32 Task 10 — Backlink scanning implementation.

#include "core/BacklinkFinder.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

BacklinkFinder::BacklinkFinder(std::string workspace_root)
    : workspace_root_(std::move(workspace_root))
{
}

auto BacklinkFinder::find_backlinks(const std::string& target_name,
                                    const std::vector<std::string>& workspace_files) const
    -> std::vector<Backlink>
{
    std::vector<Backlink> results;

    for (const auto& file_path : workspace_files)
    {
        auto file_results = scan_file(file_path, target_name);
        results.insert(results.end(), file_results.begin(), file_results.end());
    }

    return results;
}

auto BacklinkFinder::find_heading_references(const std::string& target_name,
                                             const std::string& heading,
                                             const std::vector<std::string>& workspace_files) const
    -> std::vector<Backlink>
{
    std::vector<Backlink> results;

    for (const auto& file_path : workspace_files)
    {
        auto file_results = scan_file(file_path, target_name, heading);
        results.insert(results.end(), file_results.begin(), file_results.end());
    }

    return results;
}

auto BacklinkFinder::workspace_root() const -> const std::string&
{
    return workspace_root_;
}

auto BacklinkFinder::scan_file(const std::string& file_path,
                               const std::string& target_name,
                               const std::string& heading) const -> std::vector<Backlink>
{
    std::vector<Backlink> results;

    std::ifstream file(file_path);
    if (!file.is_open())
    {
        return results;
    }

    std::string line;
    int line_num = 0;

    // Build search patterns
    const std::string wiki_pattern = "[[" + target_name;
    const std::string wiki_heading_pattern =
        heading.empty() ? "" : "[[" + target_name + "#" + heading;

    while (std::getline(file, line))
    {
        ++line_num;

        // Check for wiki-links: [[target]] or [[target#heading]]
        auto wiki_pos = line.find(wiki_pattern);
        while (wiki_pos != std::string::npos)
        {
            // Verify this is actually a complete wiki-link
            auto close_pos = line.find("]]", wiki_pos);
            if (close_pos != std::string::npos)
            {
                std::string link_text = line.substr(wiki_pos, close_pos - wiki_pos + 2);

                // If searching for heading refs, filter
                if (!heading.empty())
                {
                    if (link_text.find("#" + heading) != std::string::npos)
                    {
                        Backlink backlink;
                        backlink.source_file = file_path;
                        backlink.source_line = line_num;
                        backlink.link_text = link_text;
                        backlink.link_type = "wiki";
                        results.push_back(std::move(backlink));
                    }
                }
                else
                {
                    Backlink backlink;
                    backlink.source_file = file_path;
                    backlink.source_line = line_num;
                    backlink.link_text = link_text;
                    backlink.link_type = "wiki";
                    results.push_back(std::move(backlink));
                }
            }

            wiki_pos = line.find(wiki_pattern, wiki_pos + 1);
        }

        // Check for markdown links: [text](path-containing-target)
        size_t md_pos = 0;
        while ((md_pos = line.find("](", md_pos)) != std::string::npos)
        {
            auto paren_close = line.find(')', md_pos + 2);
            if (paren_close != std::string::npos)
            {
                std::string url = line.substr(md_pos + 2, paren_close - md_pos - 2);

                // Check if URL references the target
                auto lower_url = url;
                auto lower_target = target_name;
                std::transform(lower_url.begin(),
                               lower_url.end(),
                               lower_url.begin(),
                               [](unsigned char chr) { return std::tolower(chr); });
                std::transform(lower_target.begin(),
                               lower_target.end(),
                               lower_target.begin(),
                               [](unsigned char chr) { return std::tolower(chr); });

                if (lower_url.find(lower_target) != std::string::npos)
                {
                    // Find the opening bracket
                    auto bracket_pos = line.rfind('[', md_pos);
                    std::string full_link =
                        (bracket_pos != std::string::npos)
                            ? line.substr(bracket_pos, paren_close - bracket_pos + 1)
                            : url;

                    Backlink backlink;
                    backlink.source_file = file_path;
                    backlink.source_line = line_num;
                    backlink.link_text = full_link;
                    backlink.link_type = "markdown";
                    results.push_back(std::move(backlink));
                }
            }
            md_pos = md_pos + 2;
        }
    }

    return results;
}

} // namespace markamp::core
