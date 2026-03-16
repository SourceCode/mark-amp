/// @file ThemeCompatibility.h
/// @brief V9 Phase 3 — Theme compatibility scoring and repair suggestions.

#pragma once

#include "Theme.h"
#include "ThemeEngine.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Result of a theme compatibility assessment.
struct CompatibilityResult
{
    /// Overall score from 0.0 (no mapping) to 1.0 (fully mapped).
    double score{0.0};

    /// Token names that are fully mapped from the theme.
    std::vector<std::string> mapped_tokens;

    /// Token names that have no definition and fell back to defaults.
    std::vector<std::string> unmapped_tokens;

    /// Suggested repairs for unmapped tokens (token_name -> suggested hex).
    std::vector<std::pair<std::string, std::string>> repair_suggestions;

    /// Human-readable summary line.
    std::string summary;
};

/// Assess how well a Theme covers all expected semantic tokens.
class ThemeCompatibilityScorer
{
public:
    /// Score a theme's token coverage against the full token catalog.
    /// Returns a CompatibilityResult with score 0.0–1.0, mapped/unmapped lists.
    [[nodiscard]] static auto score_theme(const Theme& theme) -> CompatibilityResult;

    /// Generate repair suggestions for unmapped tokens using heuristics
    /// (derive from nearest existing color, or from accent/bg as fallback).
    [[nodiscard]] static auto suggest_repairs(const Theme& theme)
        -> std::vector<std::pair<std::string, std::string>>;

    /// (#104) Return the total number of tokens in the compatibility catalog.
    [[nodiscard]] static auto token_count() -> std::size_t;

    /// (#143) Return the coverage percentage (0-100) for a theme.
    [[nodiscard]] static auto coverage_percentage(const Theme& theme) -> double;
};

} // namespace markamp::core
