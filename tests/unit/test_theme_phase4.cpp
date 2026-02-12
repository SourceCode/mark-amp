#include "core/Color.h"
#include "core/Theme.h"
#include "core/ThemeEngine.h"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

using namespace markamp::core;

// ===== ThemeChromeColors defaults =====

TEST_CASE("ThemeChromeColors — default constructed", "[phase4]")
{
    ThemeChromeColors chrome;
    // All fields default-constructed (Color defaults to 0,0,0,255)
    REQUIRE(chrome.bg_app.r == 0);
    REQUIRE(chrome.bg_app.g == 0);
    REQUIRE(chrome.bg_app.b == 0);
    REQUIRE(chrome.bg_app.a == 255);
}

// ===== ThemeSyntaxColors defaults =====

TEST_CASE("ThemeSyntaxColors — syntax token defaults", "[phase4]")
{
    ThemeSyntaxColors syntax;
    // keyword has a non-zero default
    REQUIRE(syntax.keyword.r == 140);
    REQUIRE(syntax.keyword.g == 120);
    REQUIRE(syntax.keyword.b == 255);

    // string_literal default
    REQUIRE(syntax.string_literal.r == 80);
    REQUIRE(syntax.string_literal.g == 200);
    REQUIRE(syntax.string_literal.b == 120);

    // comment default
    REQUIRE(syntax.comment.r == 100);
    REQUIRE(syntax.comment.g == 100);
    REQUIRE(syntax.comment.b == 130);
}

// ===== ThemeRenderColors defaults =====

TEST_CASE("ThemeRenderColors — render token defaults", "[phase4]")
{
    ThemeRenderColors render;
    REQUIRE(render.heading.r == 230);
    REQUIRE(render.heading.g == 230);
    REQUIRE(render.heading.b == 250);

    REQUIRE(render.link.r == 100);
    REQUIRE(render.link.g == 160);
    REQUIRE(render.link.b == 255);

    REQUIRE(render.code_bg.r == 30);
    REQUIRE(render.code_bg.g == 30);
    REQUIRE(render.code_bg.b == 50);
}

// ===== TitleBarGradient =====

TEST_CASE("TitleBarGradient — disabled by default", "[phase4]")
{
    TitleBarGradient grad;
    REQUIRE_FALSE(grad.is_enabled());
}

TEST_CASE("TitleBarGradient — enabled when both start and end set", "[phase4]")
{
    TitleBarGradient grad;
    grad.start = "#1A1A2E";
    grad.end = "#6C63FF";
    REQUIRE(grad.is_enabled());
}

TEST_CASE("TitleBarGradient — disabled when only start set", "[phase4]")
{
    TitleBarGradient grad;
    grad.start = "#1A1A2E";
    REQUIRE_FALSE(grad.is_enabled());
}

// ===== sync_layers_from_colors =====

TEST_CASE("Theme::sync_layers_from_colors populates chrome layer", "[phase4]")
{
    Theme theme;
    theme.colors.bg_app = Color(10, 20, 30);
    theme.colors.bg_panel = Color(40, 50, 60);
    theme.colors.accent_primary = Color(100, 200, 255);
    theme.colors.editor_bg = Color(5, 5, 5);
    theme.colors.editor_fg = Color(200, 200, 200);

    theme.sync_layers_from_colors();

    REQUIRE(theme.chrome.bg_app.r == 10);
    REQUIRE(theme.chrome.bg_app.g == 20);
    REQUIRE(theme.chrome.bg_app.b == 30);
    REQUIRE(theme.chrome.bg_panel.r == 40);
    REQUIRE(theme.chrome.accent_primary.r == 100);

    REQUIRE(theme.syntax.editor_bg.r == 5);
    REQUIRE(theme.syntax.editor_fg.r == 200);
}

// ===== Theme neon_edge default =====

TEST_CASE("Theme — neon_edge defaults to false", "[phase4]")
{
    Theme theme;
    REQUIRE_FALSE(theme.neon_edge);
}

TEST_CASE("Theme — neon_edge can be enabled", "[phase4]")
{
    Theme theme;
    theme.neon_edge = true;
    REQUIRE(theme.neon_edge);
}

// ===== ThemeColorToken new entries exist =====

TEST_CASE("ThemeColorToken — syntax tokens are distinct values", "[phase4]")
{
    // Verify that the new syntax tokens have unique enum values
    REQUIRE(static_cast<int>(ThemeColorToken::SyntaxKeyword) !=
            static_cast<int>(ThemeColorToken::SyntaxString));
    REQUIRE(static_cast<int>(ThemeColorToken::SyntaxComment) !=
            static_cast<int>(ThemeColorToken::SyntaxNumber));
    REQUIRE(static_cast<int>(ThemeColorToken::SyntaxType) !=
            static_cast<int>(ThemeColorToken::SyntaxFunction));
    REQUIRE(static_cast<int>(ThemeColorToken::SyntaxOperator) !=
            static_cast<int>(ThemeColorToken::SyntaxPreprocessor));
}

TEST_CASE("ThemeColorToken — render tokens are distinct values", "[phase4]")
{
    REQUIRE(static_cast<int>(ThemeColorToken::RenderHeading) !=
            static_cast<int>(ThemeColorToken::RenderLink));
    REQUIRE(static_cast<int>(ThemeColorToken::RenderCodeBg) !=
            static_cast<int>(ThemeColorToken::RenderCodeFg));
    REQUIRE(static_cast<int>(ThemeColorToken::RenderBlockquoteBorder) !=
            static_cast<int>(ThemeColorToken::RenderBlockquoteBg));
    REQUIRE(static_cast<int>(ThemeColorToken::RenderTableBorder) !=
            static_cast<int>(ThemeColorToken::RenderTableHeaderBg));
}

// ===== Layer equality =====

TEST_CASE("ThemeChromeColors — equality comparison", "[phase4]")
{
    ThemeChromeColors lhs;
    ThemeChromeColors rhs;
    REQUIRE(lhs == rhs);

    rhs.accent_primary = Color(255, 0, 0);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("ThemeSyntaxColors — equality comparison", "[phase4]")
{
    ThemeSyntaxColors lhs;
    ThemeSyntaxColors rhs;
    REQUIRE(lhs == rhs);

    rhs.keyword = Color(0, 0, 0);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("ThemeRenderColors — equality comparison", "[phase4]")
{
    ThemeRenderColors lhs;
    ThemeRenderColors rhs;
    REQUIRE(lhs == rhs);

    rhs.heading = Color(1, 2, 3);
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("TitleBarGradient — equality comparison", "[phase4]")
{
    TitleBarGradient lhs;
    TitleBarGradient rhs;
    REQUIRE(lhs == rhs);

    rhs.start = "#FF0000";
    REQUIRE_FALSE(lhs == rhs);
}

// ===== sync_layers preserves render defaults =====

TEST_CASE("sync_layers_from_colors — render layer keeps defaults", "[phase4]")
{
    Theme theme;
    theme.colors.bg_app = Color(10, 10, 10);

    theme.sync_layers_from_colors();

    // Render defaults should be untouched
    REQUIRE(theme.render.heading.r == 230);
    REQUIRE(theme.render.link.g == 160);
    REQUIRE(theme.render.code_bg.b == 50);
}

// ===== sync_layers preserves syntax defaults =====

TEST_CASE("sync_layers_from_colors — syntax keyword defaults preserved", "[phase4]")
{
    Theme theme;
    theme.colors.editor_bg = Color(20, 20, 20);
    theme.sync_layers_from_colors();

    // keyword should keep its struct default, not be overwritten
    REQUIRE(theme.syntax.keyword.r == 140);
    REQUIRE(theme.syntax.keyword.g == 120);
    REQUIRE(theme.syntax.keyword.b == 255);
}

// ===== WindowEffects defaults =====

TEST_CASE("WindowEffects — all effects disabled by default", "[phase4d]")
{
    WindowEffects effects;
    REQUIRE_FALSE(effects.frosted_glass);
    REQUIRE_FALSE(effects.inner_shadow);
    REQUIRE_FALSE(effects.edge_glow);
    REQUIRE_FALSE(effects.vignette);
    REQUIRE_FALSE(effects.surface_blur);
}

TEST_CASE("WindowEffects — default parameter values", "[phase4d]")
{
    WindowEffects effects;
    REQUIRE(effects.inner_shadow_radius == 3);
    REQUIRE(effects.inner_shadow_alpha == 40);
    REQUIRE(effects.edge_glow_width == 1);
    REQUIRE(effects.edge_glow_alpha == 60);
    REQUIRE(effects.edge_glow_color.r == 100);
    REQUIRE(effects.edge_glow_color.g == 99);
    REQUIRE(effects.edge_glow_color.b == 255);
    REQUIRE(effects.vignette_strength == 30);
}

// ===== WindowEffects equality =====

TEST_CASE("WindowEffects — equality comparison", "[phase4d]")
{
    WindowEffects lhs;
    WindowEffects rhs;
    REQUIRE(lhs == rhs);

    rhs.frosted_glass = true;
    REQUIRE_FALSE(lhs == rhs);
}

TEST_CASE("WindowEffects — edge_glow color difference breaks equality", "[phase4d]")
{
    WindowEffects lhs;
    WindowEffects rhs;
    lhs.edge_glow_color = Color(255, 0, 0);
    rhs.edge_glow_color = Color(0, 255, 0);
    REQUIRE_FALSE(lhs == rhs);
}

// ===== WindowEffects JSON round-trip =====

TEST_CASE("WindowEffects — JSON round-trip via Theme", "[phase4d]")
{
    Theme original;
    original.id = "test-effects";
    original.name = "Effects Theme";
    original.colors.bg_app = Color(10, 10, 10);
    original.colors.bg_panel = Color(20, 20, 20);
    original.colors.bg_header = Color(30, 30, 30);
    original.colors.bg_input = Color(40, 40, 40);
    original.colors.text_main = Color(200, 200, 200);
    original.colors.text_muted = Color(120, 120, 120);
    original.colors.accent_primary = Color(100, 100, 255);
    original.colors.accent_secondary = Color(200, 100, 255);
    original.colors.border_light = Color(60, 60, 60);
    original.colors.border_dark = Color(30, 30, 30);

    original.effects.frosted_glass = true;
    original.effects.inner_shadow = true;
    original.effects.inner_shadow_radius = 5;
    original.effects.inner_shadow_alpha = 80;
    original.effects.edge_glow = true;
    original.effects.edge_glow_color = Color(255, 0, 128);
    original.effects.edge_glow_width = 3;
    original.effects.edge_glow_alpha = 120;
    original.effects.vignette = true;
    original.effects.vignette_strength = 50;
    original.effects.surface_blur = true;

    // Serialize + deserialize
    nlohmann::json j_theme = original;
    Theme restored = j_theme.get<Theme>();

    REQUIRE(restored.effects.frosted_glass == true);
    REQUIRE(restored.effects.inner_shadow == true);
    REQUIRE(restored.effects.inner_shadow_radius == 5);
    REQUIRE(restored.effects.inner_shadow_alpha == 80);
    REQUIRE(restored.effects.edge_glow == true);
    REQUIRE(restored.effects.edge_glow_color.r == 255);
    REQUIRE(restored.effects.edge_glow_color.g == 0);
    REQUIRE(restored.effects.edge_glow_color.b == 128);
    REQUIRE(restored.effects.edge_glow_width == 3);
    REQUIRE(restored.effects.edge_glow_alpha == 120);
    REQUIRE(restored.effects.vignette == true);
    REQUIRE(restored.effects.vignette_strength == 50);
    REQUIRE(restored.effects.surface_blur == true);
}

// ===== Backward compat: neon_edge → effects.edge_glow =====

TEST_CASE("sync_layers_from_colors — neon_edge migrates to effects.edge_glow", "[phase4d]")
{
    Theme theme;
    theme.id = "compat-test";
    theme.name = "Compat";
    theme.colors.accent_primary = Color(100, 200, 255);
    theme.neon_edge = true;

    theme.sync_layers_from_colors();

    REQUIRE(theme.effects.edge_glow == true);
    REQUIRE(theme.effects.edge_glow_color.r == 100);
    REQUIRE(theme.effects.edge_glow_color.g == 200);
    REQUIRE(theme.effects.edge_glow_color.b == 255);
}

TEST_CASE("sync_layers_from_colors — neon_edge does not override existing edge_glow", "[phase4d]")
{
    Theme theme;
    theme.neon_edge = true;
    theme.effects.edge_glow = true;
    theme.effects.edge_glow_color = Color(255, 0, 0);

    theme.sync_layers_from_colors();

    // Should keep the existing color, not overwrite from accent_primary
    REQUIRE(theme.effects.edge_glow_color.r == 255);
    REQUIRE(theme.effects.edge_glow_color.g == 0);
    REQUIRE(theme.effects.edge_glow_color.b == 0);
}

// ===== Theme.effects field is default constructed =====

TEST_CASE("Theme — effects field defaults to all disabled", "[phase4d]")
{
    Theme theme;
    REQUIRE_FALSE(theme.effects.frosted_glass);
    REQUIRE_FALSE(theme.effects.inner_shadow);
    REQUIRE_FALSE(theme.effects.edge_glow);
    REQUIRE_FALSE(theme.effects.vignette);
    REQUIRE_FALSE(theme.effects.surface_blur);
}
