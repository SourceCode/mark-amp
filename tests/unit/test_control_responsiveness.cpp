/**
 * @file test_control_responsiveness.cpp
 * @brief Phase 36: Tests for DpiScalingController and CompactLayoutModel.
 */

#include "ui/CompactLayoutModel.h"
#include "ui/DpiScalingController.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// ScaledMetric
// ═══════════════════════════════════════════════════════

TEST_CASE("ScaledMetric - compute", "[responsive][metric]")
{
    ScaledMetric metric;
    metric.base_value = 16;
    metric.scale_factor = 2.0;
    metric.compute();
    CHECK(metric.scaled_value == 32);
}

// ═══════════════════════════════════════════════════════
// DpiScalingController
// ═══════════════════════════════════════════════════════

TEST_CASE("DpiScalingController - defaults", "[responsive][dpi]")
{
    DpiScalingController ctrl;
    CHECK(ctrl.scale_factor() == 1.0);
    CHECK(ctrl.platform() == Platform::kMacOS);
    CHECK(ctrl.dpi_change_count() == 0);
}

TEST_CASE("DpiScalingController - scale factor clamping", "[responsive][dpi]")
{
    DpiScalingController ctrl;

    ctrl.set_scale_factor(0.5);
    CHECK(ctrl.scale_factor() == 1.0); // Clamped to min

    ctrl.set_scale_factor(5.0);
    CHECK(ctrl.scale_factor() == 3.0); // Clamped to max

    ctrl.set_scale_factor(1.5);
    CHECK(ctrl.scale_factor() == 1.5);
}

TEST_CASE("DpiScalingController - scale pixels", "[responsive][dpi]")
{
    DpiScalingController ctrl;
    ctrl.set_scale_factor(2.0);

    CHECK(ctrl.scale_px(16) == 32);
    CHECK(ctrl.scale_px(8) == 16);
    CHECK(ctrl.scale_padding(4) == 8);
    CHECK(ctrl.scale_icon(24) == 48);
}

TEST_CASE("DpiScalingController - scale font", "[responsive][dpi]")
{
    DpiScalingController ctrl;
    ctrl.set_scale_factor(1.5);

    CHECK(ctrl.scale_font(12.0) == 18.0);
}

TEST_CASE("DpiScalingController - metric helper", "[responsive][dpi]")
{
    DpiScalingController ctrl;
    ctrl.set_scale_factor(2.0);

    auto result = ctrl.metric(10);
    CHECK(result.base_value == 10);
    CHECK(result.scale_factor == 2.0);
    CHECK(result.scaled_value == 20);
}

TEST_CASE("DpiScalingController - platform defaults", "[responsive][dpi]")
{
    DpiScalingController ctrl;

    ctrl.set_platform(Platform::kMacOS);
    CHECK(ctrl.platform_default_scale() == 2.0);

    ctrl.set_platform(Platform::kWindows);
    CHECK(ctrl.platform_default_scale() == 1.5);

    ctrl.set_platform(Platform::kLinux);
    CHECK(ctrl.platform_default_scale() == 1.0);
}

TEST_CASE("DpiScalingController - live DPI change", "[responsive][dpi]")
{
    DpiScalingController ctrl;
    ctrl.set_scale_factor(1.0);

    bool changed = ctrl.on_dpi_changed(2.0);
    CHECK(changed);
    CHECK(ctrl.scale_factor() == 2.0);
    CHECK(ctrl.dpi_change_count() == 1);

    // Same value — no change
    bool same = ctrl.on_dpi_changed(2.0);
    CHECK_FALSE(same);
    CHECK(ctrl.dpi_change_count() == 1);
}

// ═══════════════════════════════════════════════════════
// CompactLayoutModel — width modes
// ═══════════════════════════════════════════════════════

TEST_CASE("CompactLayoutModel - mode for width", "[responsive][layout]")
{
    CHECK(CompactLayoutModel::mode_for_width(320) == LayoutWidthMode::kCompact);
    CHECK(CompactLayoutModel::mode_for_width(800) == LayoutWidthMode::kNormal);
    CHECK(CompactLayoutModel::mode_for_width(1440) == LayoutWidthMode::kWide);
}

TEST_CASE("CompactLayoutModel - mode names", "[responsive][layout]")
{
    CHECK(CompactLayoutModel::mode_name(LayoutWidthMode::kCompact) == "compact");
    CHECK(CompactLayoutModel::mode_name(LayoutWidthMode::kNormal) == "normal");
    CHECK(CompactLayoutModel::mode_name(LayoutWidthMode::kWide) == "wide");
}

TEST_CASE("CompactLayoutModel - set width and mode", "[responsive][layout]")
{
    CompactLayoutModel model;
    model.set_width(500);
    CHECK(model.current_mode() == LayoutWidthMode::kCompact);

    model.set_width(800);
    CHECK(model.current_mode() == LayoutWidthMode::kNormal);

    model.set_width(1200);
    CHECK(model.current_mode() == LayoutWidthMode::kWide);
}

TEST_CASE("CompactLayoutModel - custom breakpoints", "[responsive][layout]")
{
    CompactLayoutModel model;
    model.set_compact_breakpoint(480);
    model.set_wide_breakpoint(1280);

    model.set_width(500);
    CHECK(model.current_mode() == LayoutWidthMode::kNormal);

    model.set_width(400);
    CHECK(model.current_mode() == LayoutWidthMode::kCompact);

    model.set_width(1300);
    CHECK(model.current_mode() == LayoutWidthMode::kWide);
}

// ═══════════════════════════════════════════════════════
// CompactLayoutModel — rules
// ═══════════════════════════════════════════════════════

TEST_CASE("CompactLayoutModel - add rules", "[responsive][layout]")
{
    CompactLayoutModel model;
    model.add_rule({"toolbar_search", OverflowBehavior::kCollapse, 200, 1});
    model.add_rule({"toolbar_nav", OverflowBehavior::kTruncate, 100, 2});
    CHECK(model.rule_count() == 2);
}

TEST_CASE("CompactLayoutModel - priority sort", "[responsive][layout]")
{
    CompactLayoutModel model;
    model.add_rule({"high_pri", OverflowBehavior::kCollapse, 0, 10});
    model.add_rule({"low_pri", OverflowBehavior::kCollapse, 0, 1});
    model.add_rule({"mid_pri", OverflowBehavior::kCollapse, 0, 5});

    auto sorted = model.rules_by_priority();
    CHECK(sorted[0].cluster_id == "low_pri");
    CHECK(sorted[1].cluster_id == "mid_pri");
    CHECK(sorted[2].cluster_id == "high_pri");
}

TEST_CASE("CompactLayoutModel - visible clusters compact", "[responsive][layout]")
{
    CompactLayoutModel model;
    model.set_width(400); // Compact

    model.add_rule({"essential", OverflowBehavior::kTruncate, 0, 1});
    model.add_rule({"optional", OverflowBehavior::kCollapse, 0, 2});

    auto visible = model.visible_clusters();
    CHECK(visible.size() == 1);
    CHECK(visible[0] == "essential");
}

TEST_CASE("CompactLayoutModel - visible clusters normal", "[responsive][layout]")
{
    CompactLayoutModel model;
    model.set_width(800); // Normal

    model.add_rule({"essential", OverflowBehavior::kTruncate, 0, 1});
    model.add_rule({"optional", OverflowBehavior::kCollapse, 0, 2});

    auto visible = model.visible_clusters();
    CHECK(visible.size() == 2);
}

TEST_CASE("CompactLayoutModel - behavior for cluster", "[responsive][layout]")
{
    CompactLayoutModel model;
    model.set_width(400);
    model.add_rule({"search", OverflowBehavior::kOverflow, 200, 1});

    CHECK(model.behavior_for("search") == OverflowBehavior::kOverflow);
}

TEST_CASE("LayoutRule - behavior name", "[responsive][layout]")
{
    LayoutRule rule;
    rule.compact_behavior = OverflowBehavior::kOverflow;
    CHECK(rule.behavior_name() == "overflow");

    rule.compact_behavior = OverflowBehavior::kWrap;
    CHECK(rule.behavior_name() == "wrap");
}
