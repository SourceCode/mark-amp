#pragma once

/// @file FileSearchScorer.h
/// @brief Phase 35 – File-path fuzzy search with multi-signal scoring.

#include "FuzzyScorer.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Scoring result for a file path match.
struct FileSearchResult
{
    std::string file_path;
    std::string display_name;  ///< Filename only.
    std::string relative_path; ///< Path relative to workspace root.
    int score{0};
    std::vector<int> match_positions; ///< Matched character positions in display.
};

/// Multi-signal scorer for file paths.
/// Scores against both filename (3x weight) and path segments (1x weight),
/// with bonuses for directory proximity and open tabs.
class FileSearchScorer
{
public:
    /// Score multiplier for filename matches.
    static constexpr int kFilenameWeight = 3;

    /// Score multiplier for path matches.
    static constexpr int kPathWeight = 1;

    /// Bonus for files in the same directory as the active file.
    static constexpr int kProximityBonus = 15;

    /// Bonus for files currently open in tabs.
    static constexpr int kOpenTabBonus = 20;

    /// Score a file path against a query.
    [[nodiscard]] auto score_file(const std::string& file_path,
                                  const std::string& query,
                                  const std::string& active_directory = "",
                                  bool is_open_tab = false) const -> FileSearchResult;

    /// Score and rank multiple files against a query.
    [[nodiscard]] auto score_files(const std::vector<std::string>& file_paths,
                                   const std::string& query,
                                   const std::string& active_directory = "",
                                   const std::vector<std::string>& open_tabs = {},
                                   std::size_t limit = 50) const -> std::vector<FileSearchResult>;

private:
    FuzzyScorer fuzzy_scorer_;

    /// Extract filename from a path.
    [[nodiscard]] static auto extract_filename(const std::string& path) -> std::string;

    /// Extract directory from a path.
    [[nodiscard]] static auto extract_directory(const std::string& path) -> std::string;

    /// Split a query on '/' for multi-segment matching.
    [[nodiscard]] static auto split_query(const std::string& query) -> std::vector<std::string>;
};

} // namespace markamp::core
