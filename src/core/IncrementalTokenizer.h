/// IncrementalTokenizer.h — Phase 28: Incremental re-tokenization
///
/// Only re-tokenizes lines that changed since the last tokenization pass.
/// Uses a dirty-line bitmap and caches per-line token results.
///
/// Pattern implemented: #11 Incremental re-tokenization (per-line dirty tracking)

#pragma once

#include "SyntaxHighlighter.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Per-line cached token result.
struct LineTokens
{
    std::vector<Token> tokens;
    bool valid{false};
};

/// Incremental tokenizer that caches per-line results and only re-tokenizes
/// lines marked dirty.
///
/// Usage:
///   IncrementalTokenizer tokenizer(highlighter, "cpp");
///   tokenizer.set_content(lines);
///   tokenizer.invalidate_range(5, 8);
///   tokenizer.retokenize();           // only re-tokenizes lines 5-8
///   auto& tokens = tokenizer.line(5); // cached result
class IncrementalTokenizer
{
public:
    IncrementalTokenizer(SyntaxHighlighter& highlighter, std::string language)
        : highlighter_(highlighter)
        , language_(std::move(language))
    {
    }

    /// Set or replace all content lines. Invalidates everything.
    void set_content(const std::vector<std::string>& lines)
    {
        lines_ = lines;
        cache_.clear();
        cache_.resize(lines.size());
        dirty_.clear();
        dirty_.resize(lines.size(), true);
    }

    /// Mark a range of lines as dirty (inclusive).
    void invalidate_range(std::size_t start_line, std::size_t end_line)
    {
        auto clamped_end = std::min(end_line, lines_.size());
        for (std::size_t idx = start_line; idx < clamped_end; ++idx)
        {
            dirty_[idx] = true;
            if (idx < cache_.size())
            {
                cache_[idx].valid = false;
            }
        }
    }

    /// Invalidate a single line.
    void invalidate_line(std::size_t line_idx)
    {
        if (line_idx < lines_.size())
        {
            dirty_[line_idx] = true;
            cache_[line_idx].valid = false;
        }
    }

    /// Re-tokenize all dirty lines. Returns the number of lines re-tokenized.
    [[nodiscard]] auto retokenize() -> std::size_t
    {
        std::size_t retokenized = 0;

        for (std::size_t idx = 0; idx < lines_.size(); ++idx)
        {
            if (dirty_[idx])
            {
                cache_[idx].tokens = highlighter_.tokenize(lines_[idx], language_);
                cache_[idx].valid = true;
                dirty_[idx] = false;
                ++retokenized;
            }
        }
        ++generation_;
        return retokenized;
    }

    /// Get cached tokens for a line. Empty if not yet tokenized.
    [[nodiscard]] auto line(std::size_t line_idx) const -> const std::vector<Token>&
    {
        static const std::vector<Token> empty;
        if (line_idx >= cache_.size() || !cache_[line_idx].valid)
        {
            return empty;
        }
        return cache_[line_idx].tokens;
    }

    /// Whether a line's cached tokens are valid.
    [[nodiscard]] auto is_valid(std::size_t line_idx) const -> bool
    {
        return line_idx < cache_.size() && cache_[line_idx].valid;
    }

    /// Total number of lines.
    [[nodiscard]] auto line_count() const noexcept -> std::size_t
    {
        return lines_.size();
    }

    /// Number of lines currently dirty.
    [[nodiscard]] auto dirty_count() const -> std::size_t
    {
        std::size_t count = 0;
        for (bool dirty : dirty_)
        {
            if (dirty)
            {
                ++count;
            }
        }
        return count;
    }

    /// Generation counter (increments on each retokenize pass).
    [[nodiscard]] auto generation() const noexcept -> uint64_t
    {
        return generation_;
    }

    /// Insert lines at a given position. Lines below are shifted.
    void insert_lines(std::size_t at, const std::vector<std::string>& new_lines)
    {
        auto clamped = std::min(at, lines_.size());
        lines_.insert(
            lines_.begin() + static_cast<ptrdiff_t>(clamped), new_lines.begin(), new_lines.end());
        cache_.insert(
            cache_.begin() + static_cast<ptrdiff_t>(clamped), new_lines.size(), LineTokens{});
        dirty_.insert(dirty_.begin() + static_cast<ptrdiff_t>(clamped), new_lines.size(), true);
    }

    /// Remove a range of lines [start, end).
    void remove_lines(std::size_t start_line, std::size_t count)
    {
        auto clamped_start = std::min(start_line, lines_.size());
        auto clamped_count = std::min(count, lines_.size() - clamped_start);

        lines_.erase(lines_.begin() + static_cast<ptrdiff_t>(clamped_start),
                     lines_.begin() + static_cast<ptrdiff_t>(clamped_start + clamped_count));
        cache_.erase(cache_.begin() + static_cast<ptrdiff_t>(clamped_start),
                     cache_.begin() + static_cast<ptrdiff_t>(clamped_start + clamped_count));
        dirty_.erase(dirty_.begin() + static_cast<ptrdiff_t>(clamped_start),
                     dirty_.begin() + static_cast<ptrdiff_t>(clamped_start + clamped_count));
    }

    /// Replace a single line. Marks it dirty.
    void replace_line(std::size_t line_idx, const std::string& new_content)
    {
        if (line_idx < lines_.size())
        {
            lines_[line_idx] = new_content;
            invalidate_line(line_idx);
        }
    }

private:
    SyntaxHighlighter& highlighter_;
    std::string language_;
    std::vector<std::string> lines_;
    std::vector<LineTokens> cache_;
    std::vector<bool> dirty_;
    uint64_t generation_{0};
};

} // namespace markamp::core
