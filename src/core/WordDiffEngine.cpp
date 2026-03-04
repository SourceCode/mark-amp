// ============================================================================
// File: src/core/WordDiffEngine.cpp
// Phase 46: Diff Editor and Merge Tool — Word-level diff engine
// ============================================================================
#include "WordDiffEngine.h"

#include <algorithm>
#include <cctype>

namespace markamp::core
{

auto WordDiffEngine::tokenize(std::string_view line) -> std::vector<std::string>
{
    std::vector<std::string> words;
    std::string current;

    for (char ch : line)
    {
        if (std::isspace(static_cast<unsigned char>(ch)) ||
            std::ispunct(static_cast<unsigned char>(ch)))
        {
            if (!current.empty())
            {
                words.push_back(std::move(current));
                current.clear();
            }
            // Keep punctuation/whitespace as individual tokens.
            words.emplace_back(1, ch);
        }
        else
        {
            current += ch;
        }
    }

    if (!current.empty())
    {
        words.push_back(std::move(current));
    }

    return words;
}

auto WordDiffEngine::word_similarity(std::string_view left, std::string_view right) -> double
{
    if (left == right)
    {
        return 1.0;
    }
    if (left.empty() || right.empty())
    {
        return 0.0;
    }

    // Simple character-level LCS ratio.
    const auto m = left.size();
    const auto n = right.size();

    // Use 1D DP for space efficiency.
    std::vector<size_t> prev(n + 1, 0);
    std::vector<size_t> curr(n + 1, 0);

    for (size_t i = 1; i <= m; ++i)
    {
        for (size_t j = 1; j <= n; ++j)
        {
            if (left[i - 1] == right[j - 1])
            {
                curr[j] = prev[j - 1] + 1;
            }
            else
            {
                curr[j] = std::max(prev[j], curr[j - 1]);
            }
        }
        std::swap(prev, curr);
        std::fill(curr.begin(), curr.end(), 0);
    }

    const auto lcs_len = prev[n];
    return (2.0 * static_cast<double>(lcs_len)) / static_cast<double>(m + n);
}

auto WordDiffEngine::lcs_words(const std::vector<std::string>& old_words,
                               const std::vector<std::string>& new_words) const
    -> std::vector<std::pair<int, int>>
{
    const auto m = static_cast<int>(old_words.size());
    const auto n = static_cast<int>(new_words.size());

    // Standard LCS DP.
    std::vector<std::vector<int>> dp(static_cast<size_t>(m + 1),
                                     std::vector<int>(static_cast<size_t>(n + 1), 0));

    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (old_words[static_cast<size_t>(i - 1)] == new_words[static_cast<size_t>(j - 1)])
            {
                dp[static_cast<size_t>(i)][static_cast<size_t>(j)] =
                    dp[static_cast<size_t>(i - 1)][static_cast<size_t>(j - 1)] + 1;
            }
            else
            {
                dp[static_cast<size_t>(i)][static_cast<size_t>(j)] =
                    std::max(dp[static_cast<size_t>(i - 1)][static_cast<size_t>(j)],
                             dp[static_cast<size_t>(i)][static_cast<size_t>(j - 1)]);
            }
        }
    }

    // Backtrack to find matches.
    std::vector<std::pair<int, int>> matches;
    int i = m;
    int j = n;
    while (i > 0 && j > 0)
    {
        if (old_words[static_cast<size_t>(i - 1)] == new_words[static_cast<size_t>(j - 1)])
        {
            matches.emplace_back(i - 1, j - 1);
            --i;
            --j;
        }
        else if (dp[static_cast<size_t>(i - 1)][static_cast<size_t>(j)] >=
                 dp[static_cast<size_t>(i)][static_cast<size_t>(j - 1)])
        {
            --i;
        }
        else
        {
            --j;
        }
    }

    std::ranges::reverse(matches);
    return matches;
}

auto WordDiffEngine::diff_lines(std::string_view old_line, std::string_view new_line) const
    -> WordDiffLine
{
    WordDiffLine result;

    if (old_line == new_line)
    {
        // Identical lines — single equal span covering entire line.
        result.has_changes = false;
        if (!old_line.empty())
        {
            result.old_spans.push_back({.start_col = 0,
                                        .end_col = static_cast<int>(old_line.size()),
                                        .type = WordChangeType::Equal,
                                        .text = std::string(old_line)});
            result.new_spans.push_back({.start_col = 0,
                                        .end_col = static_cast<int>(new_line.size()),
                                        .type = WordChangeType::Equal,
                                        .text = std::string(new_line)});
        }
        return result;
    }

    auto old_words = tokenize(old_line);
    auto new_words = tokenize(new_line);
    auto matches = lcs_words(old_words, new_words);

    result.has_changes = true;

    // Build old spans.
    size_t match_idx = 0;
    int old_col = 0;
    for (int wi = 0; wi < static_cast<int>(old_words.size()); ++wi)
    {
        const auto& word = old_words[static_cast<size_t>(wi)];
        bool matched = false;
        if (match_idx < matches.size() && matches[match_idx].first == wi)
        {
            matched = true;
            ++match_idx;
        }
        result.old_spans.push_back(
            {.start_col = old_col,
             .end_col = old_col + static_cast<int>(word.size()),
             .type = matched ? WordChangeType::Equal : WordChangeType::Deleted,
             .text = word});
        old_col += static_cast<int>(word.size());
    }

    // Build new spans.
    match_idx = 0;
    int new_col = 0;
    for (int wi = 0; wi < static_cast<int>(new_words.size()); ++wi)
    {
        const auto& word = new_words[static_cast<size_t>(wi)];
        bool matched = false;
        if (match_idx < matches.size() && matches[match_idx].second == wi)
        {
            matched = true;
            ++match_idx;
        }
        result.new_spans.push_back(
            {.start_col = new_col,
             .end_col = new_col + static_cast<int>(word.size()),
             .type = matched ? WordChangeType::Equal : WordChangeType::Inserted,
             .text = word});
        new_col += static_cast<int>(word.size());
    }

    return result;
}

} // namespace markamp::core
