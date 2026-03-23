/// @file test_v26_p07_syntax.cpp
/// @brief V26 Phase 07: Syntax highlighting presentation and code readability tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26EditorConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P07: Syntax readability tier count", "[v26][p07]")
{
    REQUIRE(SyntaxPresentationMetrics::tier_count() == 4);
}

TEST_CASE("V26 P07: Syntax opacity descends through tiers", "[v26][p07]")
{
    REQUIRE(SyntaxPresentationMetrics::kPrimaryOpacity > SyntaxPresentationMetrics::kSecondaryOpacity);
    REQUIRE(SyntaxPresentationMetrics::kSecondaryOpacity > SyntaxPresentationMetrics::kTertiaryOpacity);
    REQUIRE(SyntaxPresentationMetrics::kTertiaryOpacity > SyntaxPresentationMetrics::kSubduedOpacity);
    REQUIRE(SyntaxPresentationMetrics::kSubduedOpacity > 0.0F);
}

TEST_CASE("V26 P07: Opacity for tier lookup", "[v26][p07]")
{
    REQUIRE(SyntaxPresentationMetrics::opacity_for_tier(SyntaxReadabilityTier::kPrimary) == 1.0F);
    REQUIRE(SyntaxPresentationMetrics::opacity_for_tier(SyntaxReadabilityTier::kSecondary) == 0.85F);
    REQUIRE(SyntaxPresentationMetrics::opacity_for_tier(SyntaxReadabilityTier::kTertiary) == 0.55F);
    REQUIRE(SyntaxPresentationMetrics::opacity_for_tier(SyntaxReadabilityTier::kSubdued) == 0.30F);
}

TEST_CASE("V26 P07: Code block presentation metrics", "[v26][p07]")
{
    REQUIRE(SyntaxPresentationMetrics::kCodeBlockPaddingH > 0);
    REQUIRE(SyntaxPresentationMetrics::kCodeBlockPaddingV > 0);
    REQUIRE(SyntaxPresentationMetrics::kCodeBlockRadius > 0);
}

TEST_CASE("V26 P07: Inline code is tighter than code block", "[v26][p07]")
{
    REQUIRE(SyntaxPresentationMetrics::kInlineCodePaddingH < SyntaxPresentationMetrics::kCodeBlockPaddingH);
    REQUIRE(SyntaxPresentationMetrics::kInlineCodeRadius < SyntaxPresentationMetrics::kCodeBlockRadius);
}

TEST_CASE("V26 P07: Syntax readability tier enum", "[v26][p07]")
{
    REQUIRE(static_cast<int>(SyntaxReadabilityTier::kPrimary) == 0);
    REQUIRE(static_cast<int>(SyntaxReadabilityTier::kSubdued) == 3);
}
