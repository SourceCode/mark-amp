/// @file GitBlameEngine.h
/// @brief Phase 25: Version Control Integration — Git blame analysis.
/// Generates per-line blame annotations from GitService commit history,
/// computes contributor statistics and line-age metrics.

#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

class GitService;

// ============================================================================
// Data structures
// ============================================================================

/// A single line's blame annotation.
struct BlameLine
{
    int line_number{0};
    std::string commit_hash;
    std::string short_hash;
    std::string author;
    std::chrono::system_clock::time_point date;
    std::string content; ///< The actual line content.
};

/// Author contribution statistics.
struct AuthorContribution
{
    std::string author;
    int line_count{0};
    double percentage{0.0}; ///< Percentage of total lines.
};

/// Complete blame result for a file.
struct BlameResult
{
    std::string file_path;
    std::vector<BlameLine> lines;
    int total_lines{0};
    int total_commits{0};
    std::vector<AuthorContribution> top_authors;
};

// ============================================================================
// GitBlameEngine
// ============================================================================

/// Produces blame annotations by correlating GitService commit logs
/// with diff hunks to attribute each line to a commit.
class GitBlameEngine
{
public:
    explicit GitBlameEngine(GitService& git_service);

    /// Generate blame annotations for an entire file.
    [[nodiscard]] auto blame_file(const std::string& file_path) -> BlameResult;

    /// Generate blame annotations for a specific line range (1-based, inclusive).
    [[nodiscard]] auto blame_range(const std::string& file_path, int start_line, int end_line)
        -> BlameResult;

    /// Get top N contributors for a file.
    [[nodiscard]] auto top_contributors(const std::string& file_path, int top_n = 5)
        -> std::vector<AuthorContribution>;

    /// Compute the age of each line in days from now.
    [[nodiscard]] auto line_age(const std::string& file_path)
        -> std::vector<std::pair<int, double>>;

private:
    GitService& git_service_;

    /// Build contributor stats from blame lines.
    [[nodiscard]] static auto compute_contributions(const std::vector<BlameLine>& lines)
        -> std::vector<AuthorContribution>;
};

} // namespace markamp::core
