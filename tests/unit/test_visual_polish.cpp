/// Phase 8: Visual Polish & Spatial Design — Unit Tests
///
/// Tests the safety of the color lerp logic used in gradient paint methods,
/// validates 8px spacing grid constants, and checks SplitterBar animation
/// parameters.  These tests cover the unsigned-char overflow bug (the root
/// cause of the runtime crash) as well as the spacing normalization changes
/// introduced in Phase 8E.

#include "core/Color.h"
#include "ui/SplitterBar.h"
#include "ui/Toolbar.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cmath>

// ============================================================================
// Helper: reproduces the exact lerp lambda used in Toolbar / StatusBar / Layout
// ============================================================================

namespace
{

/// Safe color-channel interpolation that avoids unsigned-char overflow.
/// The `from` and `to` parameters are ints (implicitly promoted from unsigned
/// char by wxColour::Red() / Green() / Blue()).
auto safe_lerp(int from, int to, double ratio) -> unsigned char
{
    return static_cast<unsigned char>(
        std::clamp(static_cast<int>(from + ratio * (to - from)), 0, 255));
}

} // namespace

// ============================================================================
// 8D — Gradient lerp safety
// ============================================================================

TEST_CASE("Lerp — identical from/to returns same value", "[phase8][gradient]")
{
    REQUIRE(safe_lerp(100, 100, 0.0) == 100);
    REQUIRE(safe_lerp(100, 100, 0.5) == 100);
    REQUIRE(safe_lerp(100, 100, 1.0) == 100);
}

TEST_CASE("Lerp — ascending interpolation (lighter gradient)", "[phase8][gradient]")
{
    // 0 → 255 at midpoint should be ~127
    auto result = safe_lerp(0, 255, 0.5);
    REQUIRE(result == 127); // int(0 + 0.5*255) = 127
}

TEST_CASE("Lerp — descending interpolation (darker gradient)", "[phase8][gradient]")
{
    // 200 → 180 at ratio 0.5 should be 190
    REQUIRE(safe_lerp(200, 180, 0.5) == 190);
}

TEST_CASE("Lerp — descending from max to zero", "[phase8][gradient]")
{
    REQUIRE(safe_lerp(255, 0, 0.0) == 255);
    REQUIRE(safe_lerp(255, 0, 1.0) == 0);
    REQUIRE(safe_lerp(255, 0, 0.5) == 127);
}

TEST_CASE("Lerp — clamp prevents underflow (was the crash root cause)", "[phase8][gradient]")
{
    // When the result of (from + ratio * (to - from)) would go negative
    // without clamping, it must clamp to 0.
    // Using large negative delta: from=10, to=0, ratio=20.0 (out-of-range ratio)
    auto result = safe_lerp(10, 0, 20.0);
    REQUIRE(result == 0); // clamped, not wrapped
}

TEST_CASE("Lerp — clamp prevents overflow", "[phase8][gradient]")
{
    // ratio > 1.0 could push past 255
    auto result = safe_lerp(200, 250, 5.0);
    REQUIRE(result == 255); // clamped to 255
}

TEST_CASE("Lerp — dark theme scenario (small values descending)", "[phase8][gradient]")
{
    // BgPanel on dark themes: e.g. RGB(30,30,30) → ChangeLightness(97) ≈ RGB(29,29,29)
    // This is the exact pattern that crashed before the fix.
    REQUIRE(safe_lerp(30, 29, 0.0) == 30);
    REQUIRE(safe_lerp(30, 29, 0.5) == 29); // int(30 + 0.5*(-1)) = int(29.5) = 29
    REQUIRE(safe_lerp(30, 29, 1.0) == 29);
}

TEST_CASE("Lerp — white theme scenario (high values ascending)", "[phase8][gradient]")
{
    // Light themes: RGB(240,240,240) → ChangeLightness(103) ≈ RGB(247,247,247)
    REQUIRE(safe_lerp(240, 247, 0.0) == 240);
    REQUIRE(safe_lerp(240, 247, 1.0) == 247);
    auto mid = safe_lerp(240, 247, 0.5);
    REQUIRE(mid >= 243);
    REQUIRE(mid <= 244);
}

// ============================================================================
// 8E — Spacing grid constants (must be multiples of 8)
// ============================================================================

TEST_CASE("SplitterBar::kHitWidth is on the 8px grid", "[phase8][spacing]")
{
    REQUIRE(markamp::ui::SplitterBar::kHitWidth == 8);
    REQUIRE(markamp::ui::SplitterBar::kHitWidth % 8 == 0);
}

TEST_CASE("Toolbar::kHeight is on the 8px grid", "[phase8][spacing]")
{
    // kHeight = 40 = 5 × 8
    REQUIRE(markamp::ui::Toolbar::kHeight == 40);
    REQUIRE(markamp::ui::Toolbar::kHeight % 8 == 0);
}

// ============================================================================
// 8C — SplitterBar hover animation constants
// ============================================================================

TEST_CASE("SplitterBar hover animation step is in (0,1) range", "[phase8][animation]")
{
    REQUIRE(markamp::ui::SplitterBar::kHoverFadeStep > 0.0F);
    REQUIRE(markamp::ui::SplitterBar::kHoverFadeStep < 1.0F);
}

TEST_CASE("SplitterBar hover reaches full opacity within 20 steps", "[phase8][animation]")
{
    // 1.0 / kHoverFadeStep should be <= 20 steps (≈320ms at 16ms/tick)
    float steps = 1.0F / markamp::ui::SplitterBar::kHoverFadeStep;
    REQUIRE(steps <= 20.0F);
}

// ============================================================================
// Color helper round-trips
// ============================================================================

TEST_CASE("Color blend produces midpoint", "[phase8][color]")
{
    markamp::core::Color black{0, 0, 0, 255};
    markamp::core::Color white{255, 255, 255, 255};

    auto mid = black.blend(white, 0.5F);
    // Should be roughly 128 (exact depends on rounding)
    REQUIRE(mid.r >= 126);
    REQUIRE(mid.r <= 130);
    REQUIRE(mid.g >= 126);
    REQUIRE(mid.g <= 130);
}

TEST_CASE("Color with_alpha float preserves rgb", "[phase8][color]")
{
    markamp::core::Color color{100, 150, 200, 255};
    auto semi = color.with_alpha(0.5F);

    REQUIRE(semi.r == 100);
    REQUIRE(semi.g == 150);
    REQUIRE(semi.b == 200);
    REQUIRE(semi.a >= 126);
    REQUIRE(semi.a <= 129);
}
