#include "FileSearchScorer.h"

#include <algorithm>
#include <filesystem>
#include <unordered_set>

namespace markamp::core
{

auto FileSearchScorer::score_file(const std::string& file_path,
                                  const std::string& query,
                                  const std::string& active_directory,
                                  bool is_open_tab) const -> FileSearchResult
{
    FileSearchResult result;
    result.file_path = file_path;
    result.display_name = extract_filename(file_path);
    result.relative_path = file_path;

    if (query.empty())
    {
        result.score = 0;
        return result;
    }

    auto segments = split_query(query);

    if (segments.size() <= 1)
    {
        // Single query: score against filename and full path
        auto filename_match = fuzzy_scorer_.score(query, result.display_name);
        auto path_match = fuzzy_scorer_.score(query, file_path);

        result.score = (filename_match.score * kFilenameWeight) + (path_match.score * kPathWeight);
        result.match_positions = filename_match.match_positions;
    }
    else
    {
        // Multi-segment query (e.g., "ui/ed"): match each segment against path parts
        int segment_score = 0;
        auto path_parts = split_query(file_path);

        // Match the last segment against filename
        auto filename_match = fuzzy_scorer_.score(segments.back(), result.display_name);
        segment_score += filename_match.score * kFilenameWeight;
        result.match_positions = filename_match.match_positions;

        // Match earlier segments against path components
        for (std::size_t seg_idx = 0; seg_idx < segments.size() - 1; ++seg_idx)
        {
            int best_part_score = 0;
            for (const auto& part : path_parts)
            {
                auto part_match = fuzzy_scorer_.score(segments[seg_idx], part);
                best_part_score = std::max(best_part_score, part_match.score);
            }
            segment_score += best_part_score * kPathWeight;
        }

        result.score = segment_score;
    }

    // Apply bonuses
    if (!active_directory.empty())
    {
        auto file_dir = extract_directory(file_path);
        if (file_dir == active_directory)
        {
            result.score += kProximityBonus;
        }
    }

    if (is_open_tab)
    {
        result.score += kOpenTabBonus;
    }

    return result;
}

auto FileSearchScorer::score_files(const std::vector<std::string>& file_paths,
                                   const std::string& query,
                                   const std::string& active_directory,
                                   const std::vector<std::string>& open_tabs,
                                   std::size_t limit) const -> std::vector<FileSearchResult>
{
    std::unordered_set<std::string> open_set(open_tabs.begin(), open_tabs.end());

    std::vector<FileSearchResult> results;
    results.reserve(file_paths.size());

    for (const auto& path : file_paths)
    {
        bool is_open = open_set.count(path) > 0;
        auto result = score_file(path, query, active_directory, is_open);
        if (result.score > 0)
        {
            results.push_back(std::move(result));
        }
    }

    // Sort by score descending
    std::sort(results.begin(),
              results.end(),
              [](const FileSearchResult& lhs, const FileSearchResult& rhs)
              { return lhs.score > rhs.score; });

    if (results.size() > limit)
    {
        results.resize(limit);
    }

    return results;
}

auto FileSearchScorer::extract_filename(const std::string& path) -> std::string
{
    std::filesystem::path fs_path(path);
    return fs_path.filename().string();
}

auto FileSearchScorer::extract_directory(const std::string& path) -> std::string
{
    std::filesystem::path fs_path(path);
    return fs_path.parent_path().string();
}

auto FileSearchScorer::split_query(const std::string& query) -> std::vector<std::string>
{
    std::vector<std::string> parts;
    std::string current;
    for (char chr : query)
    {
        if (chr == '/' || chr == '\\')
        {
            if (!current.empty())
            {
                parts.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += chr;
        }
    }
    if (!current.empty())
    {
        parts.push_back(current);
    }
    return parts;
}

} // namespace markamp::core
