/// @file GitBlameEngine.cpp
/// @brief Phase 25: Version Control Integration — Git blame implementation.

#include "core/GitBlameEngine.h"

#include "core/GitService.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_set>

namespace markamp::core
{

GitBlameEngine::GitBlameEngine(GitService& git_service)
    : git_service_(git_service)
{
}

auto GitBlameEngine::blame_file(const std::string& file_path) -> BlameResult
{
    BlameResult result;
    result.file_path = file_path;

    // Get commit history for this file.
    auto commits = git_service_.file_log(file_path);

    // Get the current file content via diff (working tree has the latest).
    auto diff = git_service_.diff_file(file_path);

    // For our stubbed implementation, we generate synthetic blame data.
    // In a real implementation this would walk the commit DAG and trace
    // each line back through the diffs.

    // If no commits exist yet, attribute all lines to "uncommitted".
    if (commits.empty())
    {
        // Use the diff hunks to estimate line count.
        int total_lines = 0;
        for (const auto& hunk : diff.hunks)
        {
            total_lines += hunk.new_count;
        }

        // Minimum of 1 line if we have diff data.
        if (total_lines == 0 && diff.additions > 0)
        {
            total_lines = diff.additions;
        }

        for (int i = 1; i <= total_lines; ++i)
        {
            BlameLine line;
            line.line_number = i;
            line.commit_hash = "0000000000000000000000000000000000000000";
            line.short_hash = "0000000";
            line.author = "Uncommitted";
            line.date = std::chrono::system_clock::now();
            line.content = "(line " + std::to_string(i) + ")";
            result.lines.push_back(std::move(line));
        }
    }
    else
    {
        // Distribute lines across commits in reverse-chronological order.
        // The most recent commit "owns" the most lines (simplified model).
        int estimated_lines = std::max(diff.additions + 1, 10);
        int lines_per_commit = std::max(1, estimated_lines / static_cast<int>(commits.size()));

        int line_num = 1;
        for (const auto& commit : commits)
        {
            for (int j = 0; j < lines_per_commit && line_num <= estimated_lines; ++j, ++line_num)
            {
                BlameLine blame_line;
                blame_line.line_number = line_num;
                blame_line.commit_hash = commit.hash;
                blame_line.short_hash = commit.short_hash;
                blame_line.author = commit.author;
                blame_line.date = commit.timestamp;
                blame_line.content = "(line " + std::to_string(line_num) + ")";
                result.lines.push_back(std::move(blame_line));
            }
        }
    }

    result.total_lines = static_cast<int>(result.lines.size());

    // Count unique commits.
    std::unordered_set<std::string> unique_commits;
    for (const auto& line : result.lines)
    {
        unique_commits.insert(line.commit_hash);
    }
    result.total_commits = static_cast<int>(unique_commits.size());

    // Compute top authors.
    result.top_authors = compute_contributions(result.lines);

    return result;
}

auto GitBlameEngine::blame_range(const std::string& file_path, int start_line, int end_line)
    -> BlameResult
{
    auto full = blame_file(file_path);

    BlameResult result;
    result.file_path = file_path;

    for (const auto& line : full.lines)
    {
        if (line.line_number >= start_line && line.line_number <= end_line)
        {
            result.lines.push_back(line);
        }
    }

    result.total_lines = static_cast<int>(result.lines.size());

    std::unordered_set<std::string> unique_commits;
    for (const auto& line : result.lines)
    {
        unique_commits.insert(line.commit_hash);
    }
    result.total_commits = static_cast<int>(unique_commits.size());
    result.top_authors = compute_contributions(result.lines);

    return result;
}

auto GitBlameEngine::top_contributors(const std::string& file_path, int top_n)
    -> std::vector<AuthorContribution>
{
    auto blame = blame_file(file_path);
    auto contribs = blame.top_authors;

    if (static_cast<int>(contribs.size()) > top_n)
    {
        contribs.resize(static_cast<size_t>(top_n));
    }

    return contribs;
}

auto GitBlameEngine::line_age(const std::string& file_path) -> std::vector<std::pair<int, double>>
{
    auto blame = blame_file(file_path);
    auto now = std::chrono::system_clock::now();

    std::vector<std::pair<int, double>> ages;
    ages.reserve(blame.lines.size());

    for (const auto& line : blame.lines)
    {
        auto duration = now - line.date;
        double days = std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24.0;
        ages.emplace_back(line.line_number, std::max(0.0, days));
    }

    return ages;
}

// ── Private helpers ──

auto GitBlameEngine::compute_contributions(const std::vector<BlameLine>& lines)
    -> std::vector<AuthorContribution>
{
    std::unordered_map<std::string, int> author_counts;
    for (const auto& line : lines)
    {
        author_counts[line.author]++;
    }

    int total = static_cast<int>(lines.size());
    std::vector<AuthorContribution> result;
    result.reserve(author_counts.size());

    for (const auto& [author, count] : author_counts)
    {
        AuthorContribution contrib;
        contrib.author = author;
        contrib.line_count = count;
        contrib.percentage =
            total > 0 ? (static_cast<double>(count) / static_cast<double>(total)) * 100.0 : 0.0;
        result.push_back(std::move(contrib));
    }

    // Sort by line count descending.
    std::ranges::sort(result,
                      [](const AuthorContribution& lhs, const AuthorContribution& rhs)
                      { return lhs.line_count > rhs.line_count; });

    return result;
}

} // namespace markamp::core
