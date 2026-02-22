#include "ui/ComponentSizeResolver.h"
#include "ui/DensityProfile.h"
#include "ui/DesignTokenRegistry.h"
#include "ui/ElevationSystem.h"
#include "ui/LayoutMetrics.h"
#include "ui/SpacingGrid.h"
#include "ui/TypographyScale.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <wx/font.h>

using namespace markamp::ui;

TEST_CASE("SpacingGrid resolves core tokens", "[design_system]")
{
    SpacingGrid spacing;

    SECTION("Default density mappings")
    {
        spacing.set_density(DensityProfile::kDefault);
        REQUIRE(spacing.scaled(SpacingToken::kNone) == 0);
        REQUIRE(spacing.scaled(SpacingToken::kXxs) == 2);
        REQUIRE(spacing.scaled(SpacingToken::kXs) == 4);
        REQUIRE(spacing.scaled(SpacingToken::kSm) == 8);
        REQUIRE(spacing.scaled(SpacingToken::kMd) == 12);
        REQUIRE(spacing.scaled(SpacingToken::kLg) == 16);
        REQUIRE(spacing.scaled(SpacingToken::kXl) == 24);
        REQUIRE(spacing.scaled(SpacingToken::kXxl) == 32);
        REQUIRE(spacing.scaled(SpacingToken::kXxxl) == 48);
    }

    SECTION("Compact density applies 0.75x multiplier")
    {
        spacing.set_density(DensityProfile::kCompact);
        REQUIRE(spacing.scaled(SpacingToken::kSm) == 6);  // 8 * 0.75 = 6
        REQUIRE(spacing.scaled(SpacingToken::kMd) == 10); // 12 * 0.75 = 9 -> rounds to even 10
    }

    SECTION("Comfortable density applies 1.25x multiplier")
    {
        spacing.set_density(DensityProfile::kComfortable);
        REQUIRE(spacing.scaled(SpacingToken::kSm) == 10); // 8 * 1.25 = 10
        REQUIRE(spacing.scaled(SpacingToken::kMd) == 16); // 12 * 1.25 = 15 -> rounds to even 16
    }
}

TEST_CASE("TypographyScale respects type slots and density", "[design_system]")
{
    TypographyScale type_scale;
    type_scale.set_base_sans("System Default");
    type_scale.set_base_mono("Monospace");

    SECTION("Base families are applied")
    {
        auto body_spec = type_scale.resolve(TypeSlot::kBody);
        REQUIRE(body_spec.family == "System Default");

        auto mono_spec = type_scale.resolve(TypeSlot::kMono);
        REQUIRE(mono_spec.family == "Monospace");
    }

    SECTION("Default sizes and weights")
    {
        type_scale.set_density(DensityProfile::kDefault);

        REQUIRE(type_scale.resolve(TypeSlot::kCaption).size_pt == 11);
        REQUIRE(type_scale.resolve(TypeSlot::kBody).size_pt == 13);
        REQUIRE(type_scale.resolve(TypeSlot::kBodyStrong).weight == wxFONTWEIGHT_BOLD);
        REQUIRE(type_scale.resolve(TypeSlot::kTitle).size_pt == 18);
        REQUIRE(type_scale.resolve(TypeSlot::kHeadline).size_pt == 24);
    }

    SECTION("Compact density reduces font sizes")
    {
        type_scale.set_density(DensityProfile::kCompact);
        REQUIRE(type_scale.resolve(TypeSlot::kBody).size_pt == 12);
        REQUIRE(type_scale.resolve(TypeSlot::kTitle).size_pt == 17);
    }

    SECTION("Comfortable density increases font sizes")
    {
        type_scale.set_density(DensityProfile::kComfortable);
        REQUIRE(type_scale.resolve(TypeSlot::kBody).size_pt == 14);
        REQUIRE(type_scale.resolve(TypeSlot::kTitle).size_pt == 19);
    }
}

TEST_CASE("ElevationSystem has levels", "[design_system]")
{
    // ElevationSystem primarily provides a Render() method taking a wxGraphicsContext*.
    // We only test that it can be constructed and darkness can be set without crashing.
    ElevationSystem elevation;
    REQUIRE_NOTHROW(elevation.SetDarkMode(true));
    REQUIRE_NOTHROW(elevation.SetDarkMode(false));
}

TEST_CASE("ComponentSizeResolver scales UI controls", "[design_system]")
{
    auto& sizes = ComponentSizeResolver::get();

    SECTION("Default heights")
    {
        sizes.set_density(DensityProfile::kDefault);
        REQUIRE(sizes.resolve(ComponentKind::kButton).height == 32);
        REQUIRE(sizes.resolve(ComponentKind::kToolbarButton).height == 28);
        REQUIRE(sizes.resolve(ComponentKind::kTab).height == 36);
        REQUIRE(sizes.resolve(ComponentKind::kInput).height == 28);
    }

    SECTION("Compact density heights")
    {
        sizes.set_density(DensityProfile::kCompact);
        REQUIRE(sizes.resolve(ComponentKind::kButton).height == 28);
        REQUIRE(sizes.resolve(ComponentKind::kToolbarButton).height == 24);
        REQUIRE(sizes.resolve(ComponentKind::kTab).height == 32);
    }

    SECTION("Comfortable density heights")
    {
        sizes.set_density(DensityProfile::kComfortable);
        REQUIRE(sizes.resolve(ComponentKind::kButton).height == 36);
        REQUIRE(sizes.resolve(ComponentKind::kToolbarButton).height == 32);
        REQUIRE(sizes.resolve(ComponentKind::kTab).height == 40);
    }
}
