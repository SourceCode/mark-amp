#include "core/VisualLanguageTokens.h"
#include "core/ThemeTokens.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// V22 Phase 01: Visual Language Token Tests
// ============================================================================

TEST_CASE("CornerRadiusToken resolves to correct pixel values", "[v22][visual_language]")
{
    REQUIRE(resolve_corner_radius(CornerRadiusToken::kNone) == 0);
    REQUIRE(resolve_corner_radius(CornerRadiusToken::kSm) == 2);
    REQUIRE(resolve_corner_radius(CornerRadiusToken::kMd) == 4);
    REQUIRE(resolve_corner_radius(CornerRadiusToken::kLg) == 8);
    REQUIRE(resolve_corner_radius(CornerRadiusToken::kXl) == 12);
    REQUIRE(resolve_corner_radius(CornerRadiusToken::kPill) == 9999);
}

TEST_CASE("BorderWeightToken resolves to correct pixel values", "[v22][visual_language]")
{
    REQUIRE(resolve_border_weight(BorderWeightToken::kNone) == 0);
    REQUIRE(resolve_border_weight(BorderWeightToken::kThin) == 1);
    REQUIRE(resolve_border_weight(BorderWeightToken::kMedium) == 2);
    REQUIRE(resolve_border_weight(BorderWeightToken::kThick) == 3);
}

TEST_CASE("ElevationToken resolves to valid shadow params", "[v22][visual_language]")
{
    auto none = resolve_elevation(ElevationToken::kNone);
    REQUIRE(none.blur_radius == 0);
    REQUIRE(none.opacity == 0);

    auto low = resolve_elevation(ElevationToken::kLow);
    REQUIRE(low.blur_radius == 3);
    REQUIRE(low.opacity > 0);

    auto medium = resolve_elevation(ElevationToken::kMedium);
    REQUIRE(medium.blur_radius > low.blur_radius);

    auto high = resolve_elevation(ElevationToken::kHigh);
    REQUIRE(high.blur_radius > medium.blur_radius);

    auto overlay = resolve_elevation(ElevationToken::kOverlay);
    REQUIRE(overlay.blur_radius > high.blur_radius);
    REQUIRE(overlay.opacity > high.opacity);
}

TEST_CASE("TypeScaleToken resolves to valid metrics", "[v22][visual_language]")
{
    auto caption = resolve_type_scale(TypeScaleToken::kCaption);
    REQUIRE(caption.point_size == 10);
    REQUIRE(caption.line_height_px == 14);
    REQUIRE_FALSE(caption.is_bold);

    auto body = resolve_type_scale(TypeScaleToken::kBody);
    REQUIRE(body.point_size == 12);
    REQUIRE(body.line_height_px > body.point_size);

    auto body_large = resolve_type_scale(TypeScaleToken::kBodyLarge);
    REQUIRE(body_large.point_size == 13);

    auto subtitle = resolve_type_scale(TypeScaleToken::kSubtitle);
    REQUIRE(subtitle.point_size == 14);
    REQUIRE(subtitle.is_bold);

    auto title = resolve_type_scale(TypeScaleToken::kTitle);
    REQUIRE(title.point_size == 16);

    auto display = resolve_type_scale(TypeScaleToken::kDisplay);
    REQUIRE(display.point_size == 20);
    REQUIRE(display.is_bold);
}

TEST_CASE("IconMetricToken resolves to correct sizes", "[v22][visual_language]")
{
    REQUIRE(resolve_icon_metric(IconMetricToken::kMicro) == 12);
    REQUIRE(resolve_icon_metric(IconMetricToken::kSmall) == 14);
    REQUIRE(resolve_icon_metric(IconMetricToken::kMedium) == 16);
    REQUIRE(resolve_icon_metric(IconMetricToken::kLarge) == 20);
    REQUIRE(resolve_icon_metric(IconMetricToken::kXLarge) == 24);
    REQUIRE(resolve_icon_metric(IconMetricToken::kHero) == 32);
}

TEST_CASE("IconStrokeWeight resolves correctly", "[v22][visual_language]")
{
    REQUIRE(resolve_icon_stroke(IconStrokeWeight::kLight) == 1.0F);
    REQUIRE(resolve_icon_stroke(IconStrokeWeight::kRegular) == 1.5F);
    REQUIRE(resolve_icon_stroke(IconStrokeWeight::kBold) == 2.0F);
}

TEST_CASE("ContainerStyle composition", "[v22][visual_language]")
{
    ContainerStyle plain;
    REQUIRE_FALSE(plain.has_decoration());

    ContainerStyle bordered;
    bordered.border = BorderWeightToken::kThin;
    REQUIRE(bordered.has_decoration());

    ContainerStyle elevated;
    elevated.elevation = ElevationToken::kMedium;
    REQUIRE(elevated.has_decoration());
}

TEST_CASE("V22 ThemeColorToken count includes new tokens", "[v22][tokens]")
{
    // Ensure the new V22 tokens are included in the count
    auto last_token_idx = static_cast<std::size_t>(ThemeColorToken::TooltipBorder);
    REQUIRE(kColorTokenCount == last_token_idx + 1);

    // Verify key V22 tokens have expected enum positions
    REQUIRE(static_cast<int>(ThemeColorToken::ContainerBgSurface) >
            static_cast<int>(ThemeColorToken::FocusRingColor));
    REQUIRE(static_cast<int>(ThemeColorToken::TooltipBorder) >
            static_cast<int>(ThemeColorToken::ContainerBgSurface));
}

TEST_CASE("TypeScaleToken monotonically increases in size", "[v22][visual_language]")
{
    auto prev_size = resolve_type_scale(TypeScaleToken::kCaption).point_size;
    for (auto token : {TypeScaleToken::kBody,
                       TypeScaleToken::kBodyLarge,
                       TypeScaleToken::kSubtitle,
                       TypeScaleToken::kTitle,
                       TypeScaleToken::kDisplay})
    {
        auto size = resolve_type_scale(token).point_size;
        REQUIRE(size >= prev_size);
        prev_size = size;
    }
}
