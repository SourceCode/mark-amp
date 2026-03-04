// ============================================================================
// File: src/core/WordDiffEngine.h
// Phase 46: Diff Editor and Merge Tool — Word-level diff engine
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Type of word-level change.
enum class WordChangeType : uint8_t
{
    Equal,
    Inserted,
    Deleted,
    Modified
};

/// A span within a line identifying a word-level change.
struct WordSpan
{
    int start_col{0};                           ///< Start column (0-indexed)
    int end_col{0};                             ///< End column (exclusive)
    WordChangeType type{WordChangeType::Equal}; ///< Change type
    std::string text;                           ///< The word text
};

/// Word-level diff result for a single line pair.
struct WordDiffLine
{
    std::vector<WordSpan> old_spans; ///< Spans in the old line
    std::vector<WordSpan> new_spans; ///< Spans in the new line
    bool has_changes{false};         ///< Whether any words differ
};

/// Word-level diff engine.
/// Tokenizes lines into words and runs LCS to identify changed words
/// within changed lines for fine-grained highlighting.
class WordDiffEngine
{
public:
    WordDiffEngine() = default;

    /// Compute word-level diff between two lines.
    [[nodiscard]] auto diff_lines(std::string_view old_line, std::string_view new_line) const
        -> WordDiffLine;

    /// Tokenize a line into words (splits on whitespace/punctuation boundaries).
    [[nodiscard]] static auto tokenize(std::string_view line) -> std::vector<std::string>;

    /// Compute character-level similarity between two words (0.0–1.0).
    [[nodiscard]] static auto word_similarity(std::string_view left, std::string_view right)
        -> double;

private:
    /// Longest Common Subsequence for word sequences.
    [[nodiscard]] auto lcs_words(const std::vector<std::string>& old_words,
                                 const std::vector<std::string>& new_words) const
        -> std::vector<std::pair<int, int>>; // (old_idx, new_idx) matches
};

} // namespace markamp::core
