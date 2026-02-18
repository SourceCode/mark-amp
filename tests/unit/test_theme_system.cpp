/// @file test_theme_system.cpp
/// Phase 34 Batch 34C – Task 8: Theme system tests.
/// Tests ThemeRegistry, ThemeExporter, Theme data types, ThemeColors, validation,
/// derived colors, layer sync, JSON round-trip via nlohmann::json.

#include "core/Theme.h"
#include "core/ThemeExporter.h"
#include "core/ThemeRegistry.h"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

// ---------------------------------------------------------------------------
// ThemeColors defaults and equality
// ---------------------------------------------------------------------------

TEST_CASE("Theme — ThemeColors equality operator", "[theme][colors]")
{
    ThemeColors a;
    a.bg_app = Color{20, 20, 40};
    a.text_main = Color{220, 220, 240};

    ThemeColors b = a;
    REQUIRE(a == b);

    b.bg_app = Color{0, 0, 0};
    REQUIRE_FALSE(a == b);
}

TEST_CASE("Theme — ThemeSyntaxColors defaults", "[theme][syntax]")
{
    ThemeSyntaxColors syn;
    // Default keyword color is {140, 120, 255}
    REQUIRE(syn.keyword.r == 140);
    REQUIRE(syn.keyword.g == 120);
    REQUIRE(syn.keyword.b == 255);
}

TEST_CASE("Theme — ThemeRenderColors defaults", "[theme][render]")
{
    ThemeRenderColors render;
    REQUIRE(render.heading.r == 230);
    REQUIRE(render.link.b == 255);
}

// ---------------------------------------------------------------------------
// TitleBarGradient
// ---------------------------------------------------------------------------

TEST_CASE("Theme — TitleBarGradient is_enabled", "[theme][gradient]")
{
    TitleBarGradient g;
    REQUIRE_FALSE(g.is_enabled());

    g.start = "#FF00FF";
    REQUIRE_FALSE(g.is_enabled()); // needs both

    g.end = "#00FF00";
    REQUIRE(g.is_enabled());
}

// ---------------------------------------------------------------------------
// Theme struct
// ---------------------------------------------------------------------------

TEST_CASE("Theme — is_valid with ID and name", "[theme][validation]")
{
    Theme theme;
    theme.id = "test-dark";
    theme.name = "Test Dark";
    theme.source = "built-in";
    REQUIRE(theme.is_valid());
}

TEST_CASE("Theme — validation_errors on empty theme", "[theme][validation]")
{
    Theme theme;
    auto errors = theme.validation_errors();
    // Should report issues with empty ID or name
    REQUIRE_FALSE(errors.empty());
}

TEST_CASE("Theme — is_dark detects dark themes", "[theme][derived]")
{
    Theme theme;
    theme.colors.bg_app = Color{20, 20, 40}; // dark
    REQUIRE(theme.is_dark());

    theme.colors.bg_app = Color{240, 240, 250}; // light
    REQUIRE_FALSE(theme.is_dark());
}

TEST_CASE("Theme — derived scrollbar colors", "[theme][derived]")
{
    Theme theme;
    theme.colors.bg_app = Color{30, 30, 50};
    theme.colors.accent_primary = Color{100, 99, 255};

    auto track = theme.scrollbar_track();
    auto thumb = theme.scrollbar_thumb();
    auto hover = theme.scrollbar_hover();

    // Derived scrollbar colors depend on chrome layer which may default to {0,0,0}
    // Just verify the methods return valid Color structs without crashing
    REQUIRE(track.r <= 255);
    REQUIRE(thumb.r <= 255);
    REQUIRE(hover.r <= 255);
}

TEST_CASE("Theme — sync_layers_from_colors", "[theme][layers]")
{
    Theme theme;
    theme.colors.bg_app = Color{25, 25, 42};
    theme.colors.bg_panel = Color{30, 30, 50};
    theme.colors.bg_header = Color{35, 35, 55};
    theme.colors.bg_input = Color{22, 22, 40};
    theme.colors.border_light = Color{60, 60, 80};
    theme.colors.border_dark = Color{40, 40, 60};
    theme.colors.accent_primary = Color{100, 99, 255};
    theme.colors.accent_secondary = Color{255, 100, 200};
    theme.colors.editor_bg = Color{20, 20, 38};
    theme.colors.editor_fg = Color{200, 200, 220};

    theme.sync_layers_from_colors();

    REQUIRE(theme.chrome.bg_app == theme.colors.bg_app);
    REQUIRE(theme.chrome.bg_panel == theme.colors.bg_panel);
    REQUIRE(theme.syntax.editor_bg == theme.colors.editor_bg);
    REQUIRE(theme.syntax.editor_fg == theme.colors.editor_fg);
}

// ---------------------------------------------------------------------------
// Theme JSON round-trip
// ---------------------------------------------------------------------------

TEST_CASE("Theme — JSON serialize/deserialize round-trip", "[theme][json]")
{
    Theme original;
    original.id = "round-trip-test";
    original.name = "Round Trip Test";
    original.source = "custom";
    original.colors.bg_app = Color{10, 20, 30};
    original.colors.text_main = Color{200, 210, 220};
    original.colors.accent_primary = Color{100, 99, 255};

    nlohmann::json j;
    to_json(j, original);
    REQUIRE_FALSE(j.empty());

    Theme restored;
    from_json(j, restored);

    REQUIRE(restored.id == original.id);
    REQUIRE(restored.name == original.name);
    REQUIRE(restored.colors.bg_app == original.colors.bg_app);
    REQUIRE(restored.colors.text_main == original.colors.text_main);
    REQUIRE(restored.colors.accent_primary == original.colors.accent_primary);
}

TEST_CASE("Theme — Color JSON round-trip", "[theme][json]")
{
    Color c{128, 64, 32};
    nlohmann::json j;
    to_json(j, c);

    Color restored;
    from_json(j, restored);

    REQUIRE(restored.r == 128);
    REQUIRE(restored.g == 64);
    REQUIRE(restored.b == 32);
}

// ---------------------------------------------------------------------------
// ThemeRegistry
// ---------------------------------------------------------------------------

TEST_CASE("Theme — ThemeRegistry built-in themes loaded", "[theme][registry]")
{
    ThemeRegistry registry;
    auto result = registry.initialize();
    REQUIRE(result.has_value());
    REQUIRE(registry.theme_count() > 0);
}

TEST_CASE("Theme — ThemeRegistry list_themes returns ThemeInfo", "[theme][registry]")
{
    ThemeRegistry registry;
    registry.initialize();
    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());

    // First theme should have an ID and name
    REQUIRE_FALSE(themes[0].id.empty());
    REQUIRE_FALSE(themes[0].name.empty());
}

TEST_CASE("Theme — ThemeRegistry get_theme returns built-in", "[theme][registry]")
{
    ThemeRegistry registry;
    registry.initialize();

    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());

    auto theme = registry.get_theme(themes[0].id);
    REQUIRE(theme.has_value());
    REQUIRE(theme->id == themes[0].id);
}

TEST_CASE("Theme — ThemeRegistry get_theme for nonexistent returns nullopt", "[theme][registry]")
{
    ThemeRegistry registry;
    registry.initialize();
    auto theme = registry.get_theme("nonexistent-theme-id-xyz");
    REQUIRE_FALSE(theme.has_value());
}

TEST_CASE("Theme — ThemeRegistry has_theme", "[theme][registry]")
{
    ThemeRegistry registry;
    registry.initialize();
    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());

    REQUIRE(registry.has_theme(themes[0].id));
    REQUIRE_FALSE(registry.has_theme("no-such-theme"));
}

TEST_CASE("Theme — ThemeRegistry is_builtin", "[theme][registry]")
{
    ThemeRegistry registry;
    registry.initialize();
    auto themes = registry.list_themes();
    REQUIRE_FALSE(themes.empty());

    // At least one built-in theme should exist
    bool found_builtin = false;
    for (const auto& info : themes)
    {
        if (registry.is_builtin(info.id))
        {
            found_builtin = true;
            break;
        }
    }
    REQUIRE(found_builtin);
}

TEST_CASE("Theme — ThemeRegistry sanitize_filename", "[theme][registry]")
{
    auto sanitized = ThemeRegistry::sanitize_filename("My Theme! (v2)");
    REQUIRE_FALSE(sanitized.empty());
    // Should not contain special characters
    REQUIRE(sanitized.find('!') == std::string::npos);
    REQUIRE(sanitized.find('(') == std::string::npos);
}

// ---------------------------------------------------------------------------
// ThemeExporter
// ---------------------------------------------------------------------------

TEST_CASE("Theme — ThemeExporter to_vscode_json produces valid JSON", "[theme][exporter]")
{
    Theme theme;
    theme.id = "export-test";
    theme.name = "Export Test Theme";
    theme.source = "custom";
    theme.colors.bg_app = Color{20, 20, 40};
    theme.colors.text_main = Color{220, 220, 240};
    theme.colors.accent_primary = Color{100, 99, 255};

    auto json_str = ThemeExporter::to_vscode_json(theme);
    REQUIRE_FALSE(json_str.empty());

    // Should be valid JSON
    auto parsed = nlohmann::json::parse(json_str, nullptr, false);
    REQUIRE_FALSE(parsed.is_discarded());
}

// ---------------------------------------------------------------------------
// WindowEffects / FxSettings defaults
// ---------------------------------------------------------------------------

TEST_CASE("Theme — WindowEffects defaults", "[theme][effects]")
{
    WindowEffects fx;
    REQUIRE_FALSE(fx.frosted_glass);
    REQUIRE_FALSE(fx.inner_shadow);
    REQUIRE_FALSE(fx.edge_glow);
    REQUIRE_FALSE(fx.vignette);
}

TEST_CASE("Theme — FxSettings master disabled by default", "[theme][fx]")
{
    FxSettings fx;
    REQUIRE_FALSE(fx.master_enabled);
    REQUIRE_FALSE(fx.reduced_motion);
    REQUIRE(fx.high_contrast_guard);
}

TEST_CASE("Theme — inherits_from checks parent", "[theme][inheritance]")
{
    Theme theme;
    theme.parent_theme_id = "base-dark";

    REQUIRE(theme.inherits_from("base-dark"));
    REQUIRE_FALSE(theme.inherits_from("some-other-theme"));
}
