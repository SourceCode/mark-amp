#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

class WorkspaceSearchEngine
{
public:
    struct SearchOptions
    {
        std::string query;
        bool regex_mode{false};
        bool case_sensitive{false};
        bool whole_word{false};
        std::vector<std::string> include_patterns;
        std::vector<std::string> exclude_patterns;
        int max_results{10000};

        // Scope limits (e.g. for Task 13 search in selection)
        std::string limit_file_path; // if empty, scan all
        int limit_start_line{0};     // 1-based, inclusive
        int limit_end_line{0};       // 1-based, inclusive

        int context_lines{0}; // Number of lines of context before and after match
    };

    struct SearchMatch
    {
        std::string file_path;
        int line_number{1};
        int column{1};
        int match_length{0};
        std::string line_content;
        std::vector<std::string> context_before;
        std::vector<std::string> context_after;
    };

    struct SearchResult
    {
        std::vector<SearchMatch> matches;
        int files_searched{0};
        int files_with_matches{0};
        std::chrono::milliseconds duration{0};
        bool truncated{false};
    };

    WorkspaceSearchEngine();
    ~WorkspaceSearchEngine();

    WorkspaceSearchEngine(const WorkspaceSearchEngine&) = delete;
    WorkspaceSearchEngine& operator=(const WorkspaceSearchEngine&) = delete;
    WorkspaceSearchEngine(WorkspaceSearchEngine&&) = delete;
    WorkspaceSearchEngine& operator=(WorkspaceSearchEngine&&) = delete;

    /// Request cancellation of the ongoing search
    void Cancel();

    /// Set a callback for progress reporting (files_scanned, total_files, matches_found)
    using ProgressCallback =
        std::function<void(int files_scanned, int total_files, int matches_found)>;
    void SetProgressCallback(ProgressCallback callback);

    /// Execute a search synchronously (intended to be called from a background thread)
    auto Search(const SearchOptions& options, const std::string& workspace_root) -> SearchResult;

    /// Replace all matches with the given text
    auto ReplaceMatches(const std::vector<SearchMatch>& matches,
                        const std::string& replacement_text,
                        const std::string& workspace_root) -> int;

    /// Replace a single match with the given text
    auto ReplaceSingleMatch(const SearchMatch& match,
                            const std::string& replacement_text,
                            const std::string& workspace_root) -> bool;

private:
    std::atomic<bool> is_cancelled_{false};
    ProgressCallback progress_callback_;

    // Helpers
    auto FileMatchesGlob(const std::string& path,
                         const std::vector<std::string>& include,
                         const std::vector<std::string>& exclude) const -> bool;
    void SearchInFile(const std::string& full_path,
                      const std::string& rel_path,
                      const SearchOptions& options,
                      SearchResult& result);
};

} // namespace markamp::core
