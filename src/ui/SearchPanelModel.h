#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Search query options.
struct SearchQuery
{
    std::string text;
    bool is_regex{false};
    bool case_sensitive{false};
    bool whole_word{false};
    std::string include_paths; ///< Glob pattern to include
    std::string exclude_paths; ///< Glob pattern to exclude
};

/// A single search match result.
struct SearchMatch
{
    std::string file_path;
    int line_number{0};
    std::string line_text; ///< Full line content
    int match_start{0};    ///< Column offset
    int match_length{0};
    bool is_included{true}; ///< For replace: include/exclude toggle
};

/// A file group of search matches.
struct SearchFileGroup
{
    std::string file_path;
    std::vector<SearchMatch> matches;
    bool is_collapsed{false};
};

/// Testable model for the Search Panel (Phase 21).
///
/// Encapsulates:
/// - Query option management (regex, case, whole-word, scope)
/// - Result grouping by file with collapse
/// - Replace preview with per-result include/exclude
/// - Search session history
class SearchPanelModel
{
public:
    /// Set the current query.
    void set_query(SearchQuery query);
    [[nodiscard]] auto query() const -> const SearchQuery&;

    /// Set raw matches (will be grouped by file).
    void set_matches(std::vector<SearchMatch> matches);

    // ── Grouped results ─────────────────────────────────────────────

    /// Get matches grouped by file.
    [[nodiscard]] auto grouped_results() const -> std::vector<SearchFileGroup>;

    /// Toggle collapse for a file group.
    void toggle_collapse(const std::string& file_path);

    /// Total match count.
    [[nodiscard]] auto match_count() const -> int;

    /// File count with matches.
    [[nodiscard]] auto file_count() const -> int;

    // ── Replace preview ─────────────────────────────────────────────

    /// Set replace text.
    void set_replace_text(const std::string& text);
    [[nodiscard]] auto replace_text() const -> const std::string&;

    /// Toggle include/exclude for a specific match (by file + line).
    void toggle_match_inclusion(const std::string& file_path, int line_number);

    /// Get count of included matches for replace.
    [[nodiscard]] auto included_count() const -> int;

    // ── History ─────────────────────────────────────────────────────

    /// Push current query to history.
    void push_history();

    /// Get search history (most recent first).
    [[nodiscard]] auto history() const -> const std::vector<SearchQuery>&;

    /// Clear history.
    void clear_history();

private:
    SearchQuery query_;
    std::vector<SearchMatch> matches_;
    std::string replace_text_;
    std::vector<SearchQuery> history_;
    std::vector<std::string> collapsed_files_;
};

} // namespace markamp::ui
