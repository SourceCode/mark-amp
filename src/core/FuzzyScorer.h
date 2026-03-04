/// @file FuzzyScorer.h
/// @brief V13 Phase 31 Task 1 — Multi-signal fuzzy scoring engine.
///
/// Replaces naive FuzzyScore methods in CommandPalette and CommandPaletteModel.
/// Scoring considers: consecutive char bonus, word-boundary bonus, camelCase
/// transitions, acronym matching, exact prefix boost, and gap penalty.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Result of a fuzzy match operation.
struct FuzzyMatchResult
{
    int score{0};                     ///< Overall match score (higher = better, 0 = no match)
    std::vector<int> match_positions; ///< Character indices in candidate that matched
    bool is_exact_match{false};       ///< True if query == candidate (case-insensitive)
};

/// Multi-signal fuzzy scoring engine.
///
/// Scoring weights:
///   - Consecutive characters:   +5
///   - Word boundary match:      +10
///   - camelCase transition:     +8
///   - Exact prefix match:       +20
///   - Gap between matches:      -3
///   - Unmatched tail penalty:   -1
///
/// Usage:
/// ```cpp
/// auto result = FuzzyScorer::score("fdo", "Format Document");
/// // result.score > 0, result.match_positions = {0, 7, 8}
/// ```
class FuzzyScorer
{
public:
    /// Score a query against a candidate string.
    /// Returns FuzzyMatchResult with score, match positions, and exact-match flag.
    /// A score of 0 means no match.
    [[nodiscard]] static auto score(const std::string& query, const std::string& candidate)
        -> FuzzyMatchResult;

    // ── Scoring constants (public for testing) ──

    static constexpr int kConsecutiveBonus = 5;
    static constexpr int kWordBoundaryBonus = 10;
    static constexpr int kCamelCaseBonus = 8;
    static constexpr int kExactPrefixBonus = 20;
    static constexpr int kGapPenalty = -3;
    static constexpr int kUnmatchedTailPenalty = -1;
    static constexpr int kBaseMatchScore = 1;

private:
    /// Check if a character is at a word boundary in the candidate string.
    [[nodiscard]] static auto is_word_boundary(const std::string& candidate, size_t pos) -> bool;

    /// Check if a position is a camelCase transition (lowercase -> uppercase).
    [[nodiscard]] static auto is_camel_case_transition(const std::string& candidate, size_t pos)
        -> bool;
};

} // namespace markamp::core
