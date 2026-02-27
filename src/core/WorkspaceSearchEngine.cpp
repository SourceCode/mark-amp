#include "WorkspaceSearchEngine.h"

#include "core/Logger.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>

namespace markamp::core
{

WorkspaceSearchEngine::WorkspaceSearchEngine() = default;
WorkspaceSearchEngine::~WorkspaceSearchEngine() = default;

void WorkspaceSearchEngine::Cancel()
{
    is_cancelled_ = true;
}

void WorkspaceSearchEngine::SetProgressCallback(ProgressCallback callback)
{
    progress_callback_ = std::move(callback);
}

auto WorkspaceSearchEngine::FileMatchesGlob(const std::string& path,
                                            const std::vector<std::string>& include,
                                            const std::vector<std::string>& exclude) const -> bool
{
    // Skip hidden files/folders by default
    if (path.find("/.") != std::string::npos || path.find("\\.") != std::string::npos ||
        path.starts_with("."))
    {
        return false;
    }

    auto match_pseudo_glob = [](const std::string& p, const std::string& pattern)
    {
        if (pattern.empty())
            return false;

        // Handle "*.ext"
        if (pattern.starts_with("*."))
        {
            std::string ext = pattern.substr(1);
            if (p.length() >= ext.length())
            {
                return (0 == p.compare(p.length() - ext.length(), ext.length(), ext));
            }
        }

        // Handle "dir/**"
        size_t starstar = pattern.find("**");
        if (starstar != std::string::npos && starstar > 0)
        {
            std::string prefix = pattern.substr(0, starstar);
            if (p.starts_with(prefix))
                return true;
        }

        // Just substring match otherwise
        return p.find(pattern) != std::string::npos;
    };

    bool excluded = false;
    for (const auto& pat : exclude)
    {
        if (match_pseudo_glob(path, pat))
        {
            excluded = true;
            break;
        }
    }
    if (excluded)
    {
        return false;
    }

    if (include.empty())
    {
        return true;
    }

    bool included = false;
    for (const auto& pat : include)
    {
        if (match_pseudo_glob(path, pat))
        {
            included = true;
            break;
        }
    }
    return included;
}

void WorkspaceSearchEngine::SearchInFile(const std::string& full_path,
                                         const std::string& rel_path,
                                         const SearchOptions& options,
                                         SearchResult& result)
{
    std::ifstream file(full_path);
    if (!file.is_open())
    {
        return;
    }

    std::string line;
    int line_num = 1;
    bool case_sensitive = options.case_sensitive;
    std::string query = options.query;

    if (!case_sensitive && !options.regex_mode)
    {
        std::transform(query.begin(), query.end(), query.begin(), ::tolower);
    }

    std::regex query_regex;
    if (options.regex_mode)
    {
        try
        {
            auto flags = std::regex_constants::ECMAScript;
            if (!case_sensitive)
            {
                flags |= std::regex_constants::icase;
            }
            query_regex = std::regex(query, flags);
        }
        catch (...)
        {
            return; // Invalid regex
        }
    }

    std::string prev_line;
    bool found_in_file = false;

    // Buffer for context_after tracking
    std::vector<SearchMatch*> matches_needing_after_context;

    while (std::getline(file, line) && !is_cancelled_ && !result.truncated)
    {
        // Strip trailing \r if present
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        bool match_found_this_line = false;

        // Fulfill context_after for previous matches
        for (auto* m : matches_needing_after_context)
        {
            m->context_after = line;
        }
        matches_needing_after_context.clear();

        // Scope check
        bool in_scope = true;
        if (options.limit_start_line > 0 && line_num < options.limit_start_line)
            in_scope = false;
        if (options.limit_end_line > 0 && line_num > options.limit_end_line)
            in_scope = false;

        if (in_scope)
        {
            if (options.regex_mode)
            {
                auto begin = std::sregex_iterator(line.begin(), line.end(), query_regex);
                auto end = std::sregex_iterator();
                for (auto it = begin; it != end; ++it)
                {
                    std::smatch m = *it;
                    SearchMatch match;
                    match.file_path = rel_path;
                    match.line_number = line_num;
                    match.column = static_cast<int>(m.position() + 1);
                    match.match_length = static_cast<int>(m.length());
                    match.line_content = line;
                    match.context_before = prev_line;

                    result.matches.push_back(match);
                    matches_needing_after_context.push_back(&result.matches.back());
                    match_found_this_line = true;

                    if (result.matches.size() >= static_cast<size_t>(options.max_results))
                    {
                        result.truncated = true;
                        break;
                    }
                }
            }
            else
            {
                std::string search_line = line;
                if (!case_sensitive)
                {
                    std::transform(
                        search_line.begin(), search_line.end(), search_line.begin(), ::tolower);
                }

                size_t pos = 0;
                while ((pos = search_line.find(query, pos)) != std::string::npos)
                {
                    bool whole_word_match = true;
                    if (options.whole_word)
                    {
                        bool left_ok = (pos == 0) || !std::isalnum(search_line[pos - 1]);
                        bool right_ok = (pos + query.length() == search_line.length()) ||
                                        !std::isalnum(search_line[pos + query.length()]);
                        whole_word_match = left_ok && right_ok;
                    }

                    if (whole_word_match)
                    {
                        SearchMatch match;
                        match.file_path = rel_path;
                        match.line_number = line_num;
                        match.column = static_cast<int>(pos + 1);
                        match.match_length = static_cast<int>(query.length());
                        match.line_content = line;
                        match.context_before = prev_line;

                        result.matches.push_back(match);
                        matches_needing_after_context.push_back(&result.matches.back());
                        match_found_this_line = true;

                        if (result.matches.size() >= static_cast<size_t>(options.max_results))
                        {
                            result.truncated = true;
                            break;
                        }
                    }
                    pos += query.length();
                }
            }
        }

        if (match_found_this_line)
        {
            found_in_file = true;
        }

        prev_line = line;
        line_num++;
    }

    if (found_in_file)
    {
        result.files_with_matches++;
    }
}

auto WorkspaceSearchEngine::Search(const SearchOptions& options, const std::string& workspace_root)
    -> SearchResult
{
    is_cancelled_ = false;
    SearchResult result;
    auto start_time = std::chrono::steady_clock::now();

    if (options.query.empty())
    {
        return result;
    }

    try
    {
        std::vector<std::string> files_to_scan;

        if (!options.limit_file_path.empty())
        {
            files_to_scan.push_back(options.limit_file_path);
        }
        else
        {
            // Scan workspace
            if (std::filesystem::exists(workspace_root) &&
                std::filesystem::is_directory(workspace_root))
            {
                auto it = std::filesystem::recursive_directory_iterator(
                    workspace_root, std::filesystem::directory_options::skip_permission_denied);
                for (const auto& entry : it)
                {
                    if (is_cancelled_)
                        break;

                    if (entry.is_regular_file())
                    {
                        std::string full_path = entry.path().string();
                        std::string rel_path =
                            std::filesystem::relative(entry.path(), workspace_root).string();

                        // Fix separators for windows/unix consistency
                        std::replace(rel_path.begin(), rel_path.end(), '\\', '/');

                        if (FileMatchesGlob(
                                rel_path, options.include_patterns, options.exclude_patterns))
                        {
                            files_to_scan.push_back(rel_path);
                        }
                    }
                }
            }
        }

        size_t total_files = files_to_scan.size();

        for (size_t i = 0; i < total_files; ++i)
        {
            if (is_cancelled_)
                break;
            if (result.truncated)
                break;

            const auto& rel_path = files_to_scan[i];
            std::filesystem::path full_path = std::filesystem::path(workspace_root) / rel_path;

            SearchInFile(full_path.string(), rel_path, options, result);
            result.files_searched++;

            if (progress_callback_)
            {
                // Update every 10 files or on the last file
                if (i % 10 == 0 || i == total_files - 1)
                {
                    progress_callback_(result.files_searched,
                                       static_cast<int>(total_files),
                                       static_cast<int>(result.matches.size()));
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        MARKAMP_LOG_ERROR("WorkspaceSearchEngine: Search error: {}", e.what());
    }

    auto end_time = std::chrono::steady_clock::now();
    result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    return result;
}

auto WorkspaceSearchEngine::ReplaceMatches(const std::vector<SearchMatch>& matches,
                                           const std::string& replacement_text,
                                           const std::string& workspace_root) -> int
{
    is_cancelled_ = false;
    if (matches.empty())
    {
        return 0;
    }

    // Group by file
    std::unordered_map<std::string, std::vector<SearchMatch>> matches_by_file;
    for (const auto& match : matches)
    {
        matches_by_file[match.file_path].push_back(match);
    }

    int total_replaced = 0;

    for (auto& [rel_path, file_matches] : matches_by_file)
    {
        if (is_cancelled_)
        {
            break;
        }

        // Sort matches in reverse order so replacements don't invalidate subsequent offsets
        std::sort(file_matches.begin(),
                  file_matches.end(),
                  [](const SearchMatch& a, const SearchMatch& b)
                  {
                      if (a.line_number != b.line_number)
                          return a.line_number > b.line_number;
                      return a.column > b.column;
                  });

        std::filesystem::path full_path = std::filesystem::path(workspace_root) / rel_path;

        // Read all lines
        std::vector<std::string> lines;
        {
            std::ifstream file(full_path);
            if (!file.is_open())
            {
                MARKAMP_LOG_ERROR("WorkspaceSearchEngine: Failed to read file {}",
                                  full_path.string());
                continue;
            }
            std::string line;
            while (std::getline(file, line))
            {
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }
                lines.push_back(line);
            }
        }

        // Apply replacements
        bool file_modified = false;
        for (const auto& match : file_matches)
        {
            int line_idx = match.line_number - 1;
            if (line_idx >= 0 && line_idx < static_cast<int>(lines.size()))
            {
                std::string& line = lines[static_cast<size_t>(line_idx)];
                int col_idx = match.column - 1;
                if (col_idx >= 0 && col_idx + match.match_length <= static_cast<int>(line.length()))
                {
                    line.replace(static_cast<size_t>(col_idx),
                                 static_cast<size_t>(match.match_length),
                                 replacement_text);
                    file_modified = true;
                    total_replaced++;
                }
            }
        }

        // Write back
        if (file_modified)
        {
            std::ofstream file(full_path, std::ios::trunc);
            if (!file.is_open())
            {
                MARKAMP_LOG_ERROR("WorkspaceSearchEngine: Failed to write file {}",
                                  full_path.string());
                continue;
            }
            for (size_t i = 0; i < lines.size(); ++i)
            {
                file << lines[i];
                if (i < lines.size() - 1)
                {
                    file << "\n";
                }
            }
        }
    }

    return total_replaced;
}

auto WorkspaceSearchEngine::ReplaceSingleMatch(const SearchMatch& match,
                                               const std::string& replacement_text,
                                               const std::string& workspace_root) -> bool
{
    return ReplaceMatches({match}, replacement_text, workspace_root) > 0;
}

} // namespace markamp::core
