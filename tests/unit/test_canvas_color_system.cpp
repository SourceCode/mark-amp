/**
 * @file test_canvas_color_system.cpp
 * @brief Phase 46: Tests for CanvasColorToken and ColorPaletteController.
 */

#include "canvas/CanvasColorToken.h"
#include "canvas/ColorPaletteController.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// CanvasColorToken
// ═══════════════════════════════════════════════════════

TEST_CASE("CanvasColorToken - role name", "[canvas][color]")
{
    CanvasColorToken token;
    token.role = ColorRole::kFill;
    CHECK(token.role_name() == "fill");

    token.role = ColorRole::kText;
    CHECK(token.role_name() == "text");
}

TEST_CASE("CanvasColorToken - RGBA channels", "[canvas][color]")
{
    CanvasColorToken token;
    token.rgba = 0xFF8040C0; // R=255, G=128, B=64, A=192

    CHECK(token.red() == 255);
    CHECK(token.green() == 128);
    CHECK(token.blue() == 64);
    CHECK(token.alpha() == 192);
}

TEST_CASE("CanvasColorToken - luminance", "[canvas][color]")
{
    CanvasColorToken white;
    white.rgba = 0xFFFFFFFF;
    CHECK(white.luminance() > 0.99);

    CanvasColorToken black;
    black.rgba = 0x000000FF;
    CHECK(black.luminance() < 0.01);
}

TEST_CASE("CanvasColorToken - contrast ratio", "[canvas][color]")
{
    CanvasColorToken black;
    black.rgba = 0x000000FF;
    CanvasColorToken white;
    white.rgba = 0xFFFFFFFF;

    double ratio = CanvasColorToken::contrast_ratio(black, white);
    CHECK(ratio > 20.0); // Should be ~21:1
}

TEST_CASE("CanvasColorToken - WCAG AA check", "[canvas][color]")
{
    CanvasColorToken dark_text;
    dark_text.rgba = 0x333333FF;
    CanvasColorToken white_bg;
    white_bg.rgba = 0xFFFFFFFF;

    CHECK(CanvasColorToken::meets_wcag_aa(dark_text, white_bg));

    CanvasColorToken light_text;
    light_text.rgba = 0xCCCCCCFF;
    CHECK_FALSE(CanvasColorToken::meets_wcag_aa(light_text, white_bg));
}

// ═══════════════════════════════════════════════════════
// ColorPaletteController
// ═══════════════════════════════════════════════════════

TEST_CASE("ColorPaletteController - default palette", "[canvas][color]")
{
    ColorPaletteController ctrl;
    ctrl.register_default_palette();
    CHECK(ctrl.palette_size() == 8);
}

TEST_CASE("ColorPaletteController - recent colors", "[canvas][color]")
{
    ColorPaletteController ctrl;
    ctrl.record_recent(0xFF0000FF);
    ctrl.record_recent(0x00FF00FF);
    CHECK(ctrl.recent_count() == 2);

    // Most recent first
    CHECK(ctrl.recent_colors()[0] == 0x00FF00FF);
}

TEST_CASE("ColorPaletteController - recent dedup", "[canvas][color]")
{
    ColorPaletteController ctrl;
    ctrl.record_recent(0xFF0000FF);
    ctrl.record_recent(0x00FF00FF);
    ctrl.record_recent(0xFF0000FF); // Duplicate
    CHECK(ctrl.recent_count() == 2);
    CHECK(ctrl.recent_colors()[0] == 0xFF0000FF); // Moved to front
}

TEST_CASE("ColorPaletteController - recent max limit", "[canvas][color]")
{
    ColorPaletteController ctrl;
    ctrl.set_max_recent(3);
    ctrl.record_recent(0x01);
    ctrl.record_recent(0x02);
    ctrl.record_recent(0x03);
    ctrl.record_recent(0x04);
    CHECK(ctrl.recent_count() == 3);
}

TEST_CASE("ColorPaletteController - style copy/paste", "[canvas][color]")
{
    ColorPaletteController ctrl;
    CHECK_FALSE(ctrl.has_copied_style());

    CopiedStyle style;
    style.fill_color = 0xFF0000FF;
    style.stroke_color = 0x000000FF;
    ctrl.copy_style(style);

    CHECK(ctrl.has_copied_style());
    CHECK(ctrl.copied_style().fill_color == 0xFF0000FF);

    ctrl.clear_copied_style();
    CHECK_FALSE(ctrl.has_copied_style());
}
