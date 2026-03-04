/**
 * @file test_canvas_style_rendering.cpp
 * @brief Phase 47: Tests for StrokeBorderStyle and FillEffectsController.
 */

#include "canvas/FillEffectsController.h"
#include "canvas/StrokeBorderStyle.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// StrokeBorderStyle
// ═══════════════════════════════════════════════════════

TEST_CASE("StrokeBorderStyle - dash name", "[canvas][style]")
{
    StrokeBorderStyle style;
    style.dash = DashPattern::kDashed;
    CHECK(style.dash_name() == "dashed");

    style.dash = DashPattern::kDashDot;
    CHECK(style.dash_name() == "dash-dot");
}

TEST_CASE("StrokeBorderStyle - cap and join names", "[canvas][style]")
{
    StrokeBorderStyle style;
    style.cap = LineCap::kRound;
    CHECK(style.cap_name() == "round");

    style.join = LineJoin::kBevel;
    CHECK(style.join_name() == "bevel");
}

TEST_CASE("StrokeBorderStyle - border sides", "[canvas][style]")
{
    StrokeBorderStyle style;
    style.sides = BorderSide::kAll;
    CHECK(style.has_side(BorderSide::kTop));
    CHECK(style.has_side(BorderSide::kBottom));

    style.sides = BorderSide::kTop | BorderSide::kLeft;
    CHECK(style.has_side(BorderSide::kTop));
    CHECK_FALSE(style.has_side(BorderSide::kBottom));
}

TEST_CASE("StrokeBorderStyle - dash array", "[canvas][style]")
{
    StrokeBorderStyle style;
    style.width = 2.0;

    style.dash = DashPattern::kSolid;
    CHECK(style.dash_array().empty());

    style.dash = DashPattern::kDashed;
    auto arr = style.dash_array();
    CHECK(arr.size() == 2);
    CHECK(arr[0] == 8.0); // 2.0 * 4.0
}

// ═══════════════════════════════════════════════════════
// FillSpec
// ═══════════════════════════════════════════════════════

TEST_CASE("FillSpec - type name", "[canvas][style]")
{
    FillSpec fill;
    fill.type = FillType::kLinearGradient;
    CHECK(fill.type_name() == "linear_gradient");
}

// ═══════════════════════════════════════════════════════
// FillEffectsController
// ═══════════════════════════════════════════════════════

TEST_CASE("FillEffectsController - default state", "[canvas][style]")
{
    FillEffectsController ctrl;
    CHECK(ctrl.opacity() == 1.0);
    CHECK(ctrl.is_fill_visible());
    CHECK_FALSE(ctrl.has_effects());
}

TEST_CASE("FillEffectsController - fill types", "[canvas][style]")
{
    FillEffectsController ctrl;
    FillSpec fill;
    fill.type = FillType::kNone;
    ctrl.set_fill(fill);
    CHECK_FALSE(ctrl.is_fill_visible());

    fill.type = FillType::kSolid;
    ctrl.set_fill(fill);
    CHECK(ctrl.is_fill_visible());
}

TEST_CASE("FillEffectsController - shadow", "[canvas][style]")
{
    FillEffectsController ctrl;
    ShadowSpec shadow;
    shadow.enabled = true;
    shadow.blur = 8.0;
    ctrl.set_shadow(shadow);

    CHECK(ctrl.shadow_enabled());
    CHECK(ctrl.shadow().blur == 8.0);
    CHECK(ctrl.has_effects());

    ctrl.set_shadow_enabled(false);
    CHECK_FALSE(ctrl.shadow_enabled());
}

TEST_CASE("FillEffectsController - opacity clamped", "[canvas][style]")
{
    FillEffectsController ctrl;
    ctrl.set_opacity(2.0);
    CHECK(ctrl.opacity() == 1.0);

    ctrl.set_opacity(-1.0);
    CHECK(ctrl.opacity() == 0.0);
}
