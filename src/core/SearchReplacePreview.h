#pragma once

/// @file SearchReplacePreview.h
/// @brief V9 Phase 10 – Search-and-replace preview with diff and selection search.
///
/// Header-only implementation of:
///   - Replace preview generation with per-entry toggle
///   - Unified diff output
///   - Replacement summary/statistics
///   - Regex replacement validation
///   - Selection-based search helper

#include "Search.h"

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// ReplacePreviewEntry
// ────────────────────────────────────────────────────────────

/// A single replace preview showing before/after text.
struct ReplacePreviewEntry
{
    /// Original text (the matched region + context).
    std::string original_text;

    /// Text after replacement.
    std::string replaced_text;

    /// File path where this match occurs.
    std::string file_path;

    /// Line number of the match.
    int line_number{0};

    /// Whether this entry is selected for replacement.
    bool is_selected{true};
};

// ────────────────────────────────────────────────────────────
// ReplaceSummary
// ────────────────────────────────────────────────────────────

/// Summary statistics for a replace preview.
struct ReplaceSummary
{
    /// Total number of matches found.
    int total_matches{0};

    /// Number of matches selected for replacement.
    int selected_count{0};

    /// Number of unique files affected.
    int files_affected{0};

    /// Estimated time to apply replacements (ms).
    double estimated_time_ms{0.0};
};

// ────────────────────────────────────────────────────────────
// SelectionMatch
// ────────────────────────────────────────────────────────────

/// A match found by selection-based search.
struct SelectionMatch
{
    /// Line number (0-indexed).
    int line{0};

    /// Column offset (0-indexed).
    int column{0};

    /// Length of the match.
    int length{0};
};

// ────────────────────────────────────────────────────────────
// SearchReplacePreview
// ────────────────────────────────────────────────────────────

/// Manages search-and-replace preview with selective application.
class SearchReplacePreview
{
public:
    SearchReplacePreview() = default;

    /// Generate preview entries from search results.
    /// Each SearchHit's content is scanned for `find_text` and the replacement is shown.
    void generate_preview(const SearchResult& results,
                          const std::string& find_text,
                          const std::string& replace_text)
    {
        entries_.clear();
        find_text_ = find_text;
        replace_text_ = replace_text;

        if (find_text.empty())
        {
            return;
        }

        for (const auto& hit : results.hits)
        {
            // Find all occurrences of find_text in the hit content
            std::size_t pos = 0;
            while ((pos = hit.content.find(find_text, pos)) != std::string::npos)
            {
                ReplacePreviewEntry entry;

                // Extract context around the match
                const auto context_start = (pos > 40) ? pos - 40 : static_cast<std::size_t>(0);
                const auto context_end = std::min(pos + find_text.size() + 40, hit.content.size());

                entry.original_text =
                    hit.content.substr(context_start, context_end - context_start);

                // Build replaced text
                std::string replaced = hit.content.substr(context_start, pos - context_start);
                replaced += replace_text;
                replaced += hit.content.substr(pos + find_text.size(),
                                               context_end - (pos + find_text.size()));
                entry.replaced_text = replaced;

                entry.file_path = hit.root_id;
                entry.line_number = compute_line_number(hit.content, pos);
                entry.is_selected = true;

                entries_.push_back(std::move(entry));
                pos += find_text.size();
            }
        }
    }

    /// Get all preview entries.
    [[nodiscard]] auto get_entries() const -> const std::vector<ReplacePreviewEntry>&
    {
        return entries_;
    }

    /// Get a single preview entry.
    [[nodiscard]] auto get_preview(std::size_t index) const -> const ReplacePreviewEntry*
    {
        if (index >= entries_.size())
        {
            return nullptr;
        }
        return &entries_[index];
    }

    /// Get the number of preview entries.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return entries_.size();
    }

    /// Toggle selection state of an entry.
    void toggle_entry(std::size_t index)
    {
        if (index < entries_.size())
        {
            entries_[index].is_selected = !entries_[index].is_selected;
        }
    }

    /// Select all entries for replacement.
    void select_all()
    {
        for (auto& entry : entries_)
        {
            entry.is_selected = true;
        }
    }

    /// Deselect all entries.
    void deselect_all()
    {
        for (auto& entry : entries_)
        {
            entry.is_selected = false;
        }
    }

    /// Get the count of selected entries.
    [[nodiscard]] auto get_selected_count() const -> int
    {
        return static_cast<int>(std::count_if(entries_.begin(),
                                              entries_.end(),
                                              [](const ReplacePreviewEntry& entry)
                                              { return entry.is_selected; }));
    }

    // ── Diff Generation ──────────────────────────────────────

    /// Generate a unified diff string for a single entry.
    [[nodiscard]] auto generate_diff(std::size_t index) const -> std::string
    {
        if (index >= entries_.size())
        {
            return {};
        }

        const auto& entry = entries_[index];
        std::ostringstream oss;

        oss << "--- " << entry.file_path << ":" << entry.line_number << "\n";
        oss << "+++ " << entry.file_path << ":" << entry.line_number << "\n";
        oss << "@@ -" << entry.line_number << " +" << entry.line_number << " @@\n";
        oss << "-" << entry.original_text << "\n";
        oss << "+" << entry.replaced_text << "\n";

        return oss.str();
    }

    /// Generate a summary of the replacement operation.
    [[nodiscard]] auto generate_summary() const -> ReplaceSummary
    {
        ReplaceSummary summary;
        summary.total_matches = static_cast<int>(entries_.size());
        summary.selected_count = get_selected_count();

        // Count unique files
        std::set<std::string> unique_files;
        for (const auto& entry : entries_)
        {
            if (entry.is_selected)
            {
                unique_files.insert(entry.file_path);
            }
        }
        summary.files_affected = static_cast<int>(unique_files.size());

        // Estimate time: ~0.5ms per replacement
        summary.estimated_time_ms = summary.selected_count * 0.5;

        return summary;
    }

    /// Validate a regex pattern and replacement string.
    /// Returns {valid, error_message}.
    [[nodiscard]] static auto validate_regex_replace(const std::string& pattern,
                                                     const std::string& replacement)
        -> std::pair<bool, std::string>
    {
        if (pattern.empty())
        {
            return {false, "Pattern is empty"};
        }

        try
        {
            std::regex re(pattern);
            // Test replacement with an empty string to check for errors
            std::string test_input = "test_input_for_validation";
            std::regex_replace(test_input, re, replacement);
            return {true, ""};
        }
        catch (const std::regex_error& err)
        {
            return {false, std::string("Regex error: ") + err.what()};
        }
    }

private:
    std::vector<ReplacePreviewEntry> entries_;
    std::string find_text_;
    std::string replace_text_;

    /// Compute the line number of a position within content.
    [[nodiscard]] static auto compute_line_number(const std::string& content, std::size_t pos)
        -> int
    {
        int line = 1;
        for (std::size_t idx = 0; idx < pos && idx < content.size(); ++idx)
        {
            if (content[idx] == '\n')
            {
                ++line;
            }
        }
        return line;
    }
};

// ────────────────────────────────────────────────────────────
// SelectionSearchHelper
// ────────────────────────────────────────────────────────────

/// Finds all occurrences of selected text within a document.
class SelectionSearchHelper
{
public:
    SelectionSearchHelper() = default;

    /// Find all occurrences of selected_text within document_content.
    [[nodiscard]] static auto search_selection(const std::string& selected_text,
                                               const std::string& document_content)
        -> std::vector<SelectionMatch>
    {
        std::vector<SelectionMatch> matches;

        if (selected_text.empty() || document_content.empty())
        {
            return matches;
        }

        int current_line = 0;
        int line_start = 0;
        std::size_t pos = 0;

        // Track line boundaries
        while (pos < document_content.size())
        {
            auto match_pos = document_content.find(selected_text, pos);
            if (match_pos == std::string::npos)
            {
                break;
            }

            // Count lines up to match_pos
            while (pos < match_pos)
            {
                if (document_content[pos] == '\n')
                {
                    current_line++;
                    line_start = static_cast<int>(pos) + 1;
                }
                pos++;
            }

            SelectionMatch match;
            match.line = current_line;
            match.column = static_cast<int>(match_pos) - line_start;
            match.length = static_cast<int>(selected_text.size());
            matches.push_back(match);

            pos = match_pos + 1; // Advance past this match start
        }

        return matches;
    }

    /// Generate highlight ranges for all occurrences of text in document.
    [[nodiscard]] static auto highlight_all_occurrences(const std::string& text,
                                                        const std::string& document)
        -> std::vector<HighlightRange>
    {
        std::vector<HighlightRange> ranges;

        if (text.empty() || document.empty())
        {
            return ranges;
        }

        std::size_t pos = 0;
        while ((pos = document.find(text, pos)) != std::string::npos)
        {
            HighlightRange range;
            range.start = pos;
            range.end = pos + text.size();
            ranges.push_back(range);
            pos += text.size();
        }

        return ranges;
    }

    /// Escape special regex characters in a string for literal search.
    [[nodiscard]] static auto escape_for_search(const std::string& text) -> std::string
    {
        static const std::string kSpecialChars = R"(\.^$|*+?()[]{}-)";

        std::string escaped;
        escaped.reserve(text.size() * 2);

        for (char chr : text)
        {
            if (kSpecialChars.find(chr) != std::string::npos)
            {
                escaped += '\\';
            }
            escaped += chr;
        }

        return escaped;
    }
};

} // namespace markamp::core
