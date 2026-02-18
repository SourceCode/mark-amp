/// @file QuickOpenProvider.cpp
/// @brief Implementation of QuickOpenProvider.

#include "QuickOpenProvider.h"

#include <algorithm>
#include <fstream>

namespace markamp::core
{

void QuickOpenProvider::set_workspace_root(const std::filesystem::path& root)
{
    workspace_root_ = root;
}

void QuickOpenProvider::scan_workspace()
{
    files_.clear();
    if (workspace_root_.empty() || !std::filesystem::exists(workspace_root_))
    {
        return;
    }

    for (const auto& dir_entry : std::filesystem::recursive_directory_iterator(workspace_root_))
    {
        if (!dir_entry.is_regular_file())
        {
            continue;
        }

        const auto& file_path = dir_entry.path();
        QuickOpenItem item;
        item.file_path = file_path;
        item.display_name = file_path.filename().string();
        item.relative_path = std::filesystem::relative(file_path, workspace_root_).string();
        item.file_type_icon = icon_for_extension(file_path.extension().string());
        files_.push_back(std::move(item));
    }
}

auto QuickOpenProvider::all_files() const -> const std::vector<QuickOpenItem>&
{
    return files_;
}

auto QuickOpenProvider::fuzzy_match(const std::string& query, int max_results) const
    -> std::vector<QuickOpenItem>
{
    if (query.empty())
    {
        return {};
    }

    struct ScoredItem
    {
        const QuickOpenItem* item;
        int match_score;
    };

    std::vector<ScoredItem> scored;
    for (const auto& file : files_)
    {
        const int match_score = score_match(query, file.display_name);
        if (match_score > 0)
        {
            scored.push_back({&file, match_score});
        }
    }

    std::sort(scored.begin(),
              scored.end(),
              [](const ScoredItem& left, const ScoredItem& right)
              { return left.match_score > right.match_score; });

    std::vector<QuickOpenItem> results;
    const auto limit = std::min(static_cast<int>(scored.size()), max_results);
    for (int idx = 0; idx < limit; ++idx)
    {
        results.push_back(*scored[static_cast<std::size_t>(idx)].item);
    }
    return results;
}

auto QuickOpenProvider::get_file_preview(const std::filesystem::path& file_path, int max_lines)
    -> std::vector<std::string>
{
    std::vector<std::string> lines;
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        return lines;
    }

    std::string line;
    while (std::getline(file, line) && static_cast<int>(lines.size()) < max_lines)
    {
        lines.push_back(std::move(line));
    }
    return lines;
}

void QuickOpenProvider::record_access(const std::filesystem::path& file_path)
{
    for (auto& file : files_)
    {
        if (file.file_path == file_path)
        {
            file.last_accessed = std::chrono::system_clock::now();
            break;
        }
    }
}

auto QuickOpenProvider::icon_for_extension(const std::string& extension) -> std::string
{
    if (extension == ".md" || extension == ".markdown")
    {
        return "markdown";
    }
    if (extension == ".cpp" || extension == ".cc" || extension == ".cxx")
    {
        return "cpp";
    }
    if (extension == ".h" || extension == ".hpp")
    {
        return "header";
    }
    if (extension == ".py")
    {
        return "python";
    }
    if (extension == ".js" || extension == ".ts")
    {
        return "javascript";
    }
    if (extension == ".json")
    {
        return "json";
    }
    if (extension == ".yml" || extension == ".yaml")
    {
        return "yaml";
    }
    if (extension == ".css" || extension == ".scss")
    {
        return "css";
    }
    if (extension == ".html")
    {
        return "html";
    }
    if (extension == ".png" || extension == ".jpg" || extension == ".svg")
    {
        return "image";
    }
    return "file";
}

auto QuickOpenProvider::file_count() const -> std::size_t
{
    return files_.size();
}

auto QuickOpenProvider::score_match(const std::string& query, const std::string& filename) -> int
{
    if (query.empty() || filename.empty())
    {
        return 0;
    }

    // Case-insensitive search
    std::string lower_query = query;
    std::string lower_name = filename;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    std::transform(lower_name.begin(),
                   lower_name.end(),
                   lower_name.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    // Exact match
    if (lower_name == lower_query)
    {
        return 1000;
    }

    // Prefix match
    if (lower_name.find(lower_query) == 0)
    {
        return 500;
    }

    // Substring match
    if (lower_name.find(lower_query) != std::string::npos)
    {
        return 200;
    }

    // Sequential character match
    std::size_t query_idx = 0;
    int sequential_score = 0;
    for (const char chr : lower_name)
    {
        if (query_idx < lower_query.size() && chr == lower_query[query_idx])
        {
            query_idx++;
            sequential_score += 10;
        }
    }
    if (query_idx == lower_query.size())
    {
        return sequential_score;
    }

    return 0;
}

} // namespace markamp::core
