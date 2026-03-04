/// @file FuzzyScorer.cpp
/// @brief V13 Phase 31 Task 1 — Multi-signal fuzzy scoring implementation.

#include "core/FuzzyScorer.h"

#include <algorithm>
#include <cctype>

namespace markamp::core
{

auto FuzzyScorer::score(const std::string& query, const std::string& candidate) -> FuzzyMatchResult
{
    FuzzyMatchResult result;

    if (query.empty() || candidate.empty())
    {
        return result;
    }

    // ── Exact match check (case-insensitive) ──
    if (query.size() == candidate.size())
    {
        bool exact = true;
        for (size_t i = 0; i < query.size(); ++i)
        {
            if (std::tolower(static_cast<unsigned char>(query[i])) !=
                std::tolower(static_cast<unsigned char>(candidate[i])))
            {
                exact = false;
                break;
            }
        }
        if (exact)
        {
            result.is_exact_match = true;
            result.score = static_cast<int>(query.size()) * kBaseMatchScore + kExactPrefixBonus * 2;
            result.match_positions.reserve(query.size());
            for (size_t i = 0; i < query.size(); ++i)
            {
                result.match_positions.push_back(static_cast<int>(i));
            }
            return result;
        }
    }

    // ── Fuzzy matching ──
    size_t query_idx = 0;
    int last_match_pos = -1;
    int total_score = 0;
    std::vector<int> positions;
    positions.reserve(query.size());

    for (size_t cand_idx = 0; cand_idx < candidate.size() && query_idx < query.size(); ++cand_idx)
    {
        const char query_char =
            static_cast<char>(std::tolower(static_cast<unsigned char>(query[query_idx])));
        const char cand_char =
            static_cast<char>(std::tolower(static_cast<unsigned char>(candidate[cand_idx])));

        if (query_char == cand_char)
        {
            total_score += kBaseMatchScore;

            // Consecutive char bonus
            if (last_match_pos >= 0 && static_cast<int>(cand_idx) == last_match_pos + 1)
            {
                total_score += kConsecutiveBonus;
            }
            else if (last_match_pos >= 0)
            {
                // Gap penalty for non-consecutive matches
                const int gap = static_cast<int>(cand_idx) - last_match_pos - 1;
                total_score += gap * kGapPenalty;
            }

            // Word boundary bonus
            if (is_word_boundary(candidate, cand_idx))
            {
                total_score += kWordBoundaryBonus;
            }

            // camelCase transition bonus
            if (is_camel_case_transition(candidate, cand_idx))
            {
                total_score += kCamelCaseBonus;
            }

            // Exact prefix bonus (first chars match at start)
            if (query_idx == 0 && cand_idx == 0)
            {
                total_score += kExactPrefixBonus;
            }

            positions.push_back(static_cast<int>(cand_idx));
            last_match_pos = static_cast<int>(cand_idx);
            ++query_idx;
        }
    }

    // All query characters must match
    if (query_idx < query.size())
    {
        return result; // No match — score remains 0
    }

    // Unmatched tail penalty
    if (!positions.empty())
    {
        const int tail_length = static_cast<int>(candidate.size()) - positions.back() - 1;
        total_score += tail_length * kUnmatchedTailPenalty;
    }

    result.score = std::max(1, total_score); // Ensure positive for valid matches
    result.match_positions = std::move(positions);
    return result;
}

auto FuzzyScorer::is_word_boundary(const std::string& candidate, size_t pos) -> bool
{
    if (pos == 0)
    {
        return true; // First char is always a boundary
    }

    const char prev = candidate[pos - 1];
    const char curr = candidate[pos];

    // After a separator character
    if (prev == ' ' || prev == '_' || prev == '-' || prev == '.' || prev == '/' || prev == '\\')
    {
        return true;
    }

    // After opening bracket/paren
    if (prev == '(' || prev == '[' || prev == '{')
    {
        return true;
    }

    // Transition from lowercase to uppercase (camelCase)
    if (std::islower(static_cast<unsigned char>(prev)) &&
        std::isupper(static_cast<unsigned char>(curr)))
    {
        return true;
    }

    return false;
}

auto FuzzyScorer::is_camel_case_transition(const std::string& candidate, size_t pos) -> bool
{
    if (pos == 0)
    {
        return false;
    }

    return std::islower(static_cast<unsigned char>(candidate[pos - 1])) &&
           std::isupper(static_cast<unsigned char>(candidate[pos]));
}

} // namespace markamp::core
