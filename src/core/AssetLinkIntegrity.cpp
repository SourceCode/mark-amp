/// @file AssetLinkIntegrity.cpp
/// @brief V9 Phase 37 — AssetLinkIntegrity implementation.

#include "AssetLinkIntegrity.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

auto AssetLinkIntegrity::scan_document(const std::string& document_id,
                                       const std::string& content,
                                       const std::vector<AssetInfo>& available_assets) const
    -> std::vector<BrokenAssetLink>
{
    std::vector<BrokenAssetLink> broken;
    auto links = extract_asset_links(content);

    for (const auto& [line_num, path] : links)
    {
        // Check if any available asset matches the path
        const auto* found = find_asset_by_path(path, available_assets);
        if (found != nullptr)
        {
            continue; // Link is valid
        }

        BrokenAssetLink broken_link;
        broken_link.document_id = document_id;
        broken_link.line_number = line_num;
        broken_link.link_text = "![...](" + path + ")";
        broken_link.expected_path = path;

        // Try to find a suggestion
        double best_score = 0.0;
        for (const auto& asset : available_assets)
        {
            double sim = filename_similarity(path, asset.original_name);
            if (sim > best_score)
            {
                best_score = sim;
                broken_link.suggestion = asset.original_name;
                broken_link.confidence = sim;
            }
        }

        broken.push_back(std::move(broken_link));
    }

    return broken;
}

auto AssetLinkIntegrity::scan_all(const std::vector<std::pair<std::string, std::string>>& documents,
                                  const std::vector<AssetInfo>& available_assets) const
    -> LinkIntegrityReport
{
    LinkIntegrityReport report;

    for (const auto& [doc_id, content] : documents)
    {
        auto doc_links = extract_asset_links(content);
        report.total_links += static_cast<int>(doc_links.size());

        auto broken = scan_document(doc_id, content, available_assets);
        for (auto& broken_link : broken)
        {
            report.broken++;
            if (!broken_link.suggestion.empty())
            {
                report.suggestions++;
            }
            report.broken_links.push_back(std::move(broken_link));
        }
    }

    return report;
}

auto AssetLinkIntegrity::suggest_repair(const BrokenAssetLink& broken_link,
                                        const std::vector<AssetInfo>& available_assets) const
    -> std::string
{
    double best_score = 0.0;
    std::string best_match;

    for (const auto& asset : available_assets)
    {
        double sim = filename_similarity(broken_link.expected_path, asset.original_name);
        if (sim > best_score)
        {
            best_score = sim;
            best_match = asset.original_name;
        }
    }

    return best_score > 0.3 ? best_match : "";
}

auto AssetLinkIntegrity::auto_repair(std::vector<BrokenAssetLink>& broken_links,
                                     const std::vector<AssetInfo>& available_assets,
                                     double confidence_threshold) const -> int
{
    int repaired = 0;

    for (auto& broken_link : broken_links)
    {
        if (broken_link.suggestion.empty())
        {
            // Try to find a suggestion
            broken_link.suggestion = suggest_repair(broken_link, available_assets);
            broken_link.confidence =
                broken_link.suggestion.empty()
                    ? 0.0
                    : filename_similarity(broken_link.expected_path, broken_link.suggestion);
        }

        if (broken_link.confidence >= confidence_threshold && !broken_link.suggestion.empty())
        {
            broken_link.expected_path = broken_link.suggestion;
            repaired++;
        }
    }

    return repaired;
}

auto AssetLinkIntegrity::extract_asset_links(const std::string& content)
    -> std::vector<std::pair<int, std::string>>
{
    std::vector<std::pair<int, std::string>> links;
    std::istringstream stream(content);
    std::string line;
    int line_num = 0;

    while (std::getline(stream, line))
    {
        line_num++;

        // Find ![alt](path) patterns
        std::string::size_type pos = 0;
        while ((pos = line.find("![", pos)) != std::string::npos)
        {
            auto close_bracket = line.find("](", pos);
            if (close_bracket == std::string::npos)
            {
                break;
            }
            auto open_paren = close_bracket + 1;
            auto close_paren = line.find(')', open_paren + 1);
            if (close_paren == std::string::npos)
            {
                break;
            }

            std::string path = line.substr(open_paren + 1, close_paren - open_paren - 1);
            // Only include local file paths, not URLs
            if (!path.empty() && path.find("http://") == std::string::npos &&
                path.find("https://") == std::string::npos)
            {
                links.emplace_back(line_num, path);
            }

            pos = close_paren + 1;
        }
    }

    return links;
}

auto AssetLinkIntegrity::filename_similarity(const std::string& name_a, const std::string& name_b)
    -> double
{
    if (name_a.empty() || name_b.empty())
    {
        return 0.0;
    }
    if (name_a == name_b)
    {
        return 1.0;
    }

    // Extract just the filenames (strip directory components)
    auto basename = [](const std::string& path) -> std::string
    {
        auto last_slash = path.find_last_of('/');
        if (last_slash == std::string::npos)
        {
            last_slash = path.find_last_of('\\');
        }
        return last_slash != std::string::npos ? path.substr(last_slash + 1) : path;
    };

    const std::string base_a = basename(name_a);
    const std::string base_b = basename(name_b);

    if (base_a == base_b)
    {
        return 0.9; // Same filename, different path
    }

    // Lowercase comparison
    auto to_lower = [](std::string str)
    {
        std::transform(str.begin(),
                       str.end(),
                       str.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        return str;
    };

    if (to_lower(base_a) == to_lower(base_b))
    {
        return 0.85; // Case-insensitive match
    }

    // Compute simple character overlap score
    int common = 0;
    const std::string lower_a = to_lower(base_a);
    const std::string lower_b = to_lower(base_b);
    const std::size_t max_len = std::max(lower_a.size(), lower_b.size());
    const std::size_t min_len = std::min(lower_a.size(), lower_b.size());

    for (std::size_t i = 0; i < min_len; ++i)
    {
        if (lower_a[i] == lower_b[i])
        {
            common++;
        }
    }

    return static_cast<double>(common) / static_cast<double>(max_len);
}

auto AssetLinkIntegrity::find_asset_by_path(const std::string& path,
                                            const std::vector<AssetInfo>& assets)
    -> const AssetInfo*
{
    for (const auto& asset : assets)
    {
        if (asset.original_name == path || asset.file_path.string() == path)
        {
            return &asset;
        }
        // Also check just the filename
        if (asset.file_path.filename().string() == path)
        {
            return &asset;
        }
    }
    return nullptr;
}

} // namespace markamp::core
