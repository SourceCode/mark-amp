#include "core/BuiltinThemes.h"
#include "core/Color.h"
#include "core/Theme.h"
#include "core/ThemeRegistry.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <set>

using namespace markamp::core;
using Catch::Matchers::WithinAbs;

// ===== Color parsing tests =====

TEST_CASE("Color: from_hex #RRGGBB", "[theme][color]")
{
    auto c = Color::from_hex("#ff0055");
    REQUIRE(c.has_value());
    REQUIRE(c->r == 0xff);
    REQUIRE(c->g == 0x00);
    REQUIRE(c->b == 0x55);
    REQUIRE(c->a == 255);
}

TEST_CASE("Color: from_hex #RGB shorthand", "[theme][color]")
{
    auto c = Color::from_hex("#f05");
    REQUIRE(c.has_value());
    REQUIRE(c->r == 0xff);
    REQUIRE(c->g == 0x00);
    REQUIRE(c->b == 0x55);
}

TEST_CASE("Color: from_hex #RRGGBBAA", "[theme][color]")
{
    auto c = Color::from_hex("#ff005580");
    REQUIRE(c.has_value());
    REQUIRE(c->r == 0xff);
    REQUIRE(c->g == 0x00);
    REQUIRE(c->b == 0x55);
    REQUIRE(c->a == 0x80);
}

TEST_CASE("Color: from_hex without hash", "[theme][color]")
{
    auto c = Color::from_hex("00ffea");
    REQUIRE(c.has_value());
    REQUIRE(c->r == 0x00);
    REQUIRE(c->g == 0xff);
    REQUIRE(c->b == 0xea);
}

TEST_CASE("Color: from_hex invalid", "[theme][color]")
{
    REQUIRE_FALSE(Color::from_hex("").has_value());
    REQUIRE_FALSE(Color::from_hex("#").has_value());
    REQUIRE_FALSE(Color::from_hex("#GG0000").has_value());
    REQUIRE_FALSE(Color::from_hex("#12345").has_value());
}

TEST_CASE("Color: from_rgba_string", "[theme][color]")
{
    auto c = Color::from_rgba_string("rgba(255, 0, 85, 0.2)");
    REQUIRE(c.has_value());
    REQUIRE(c->r == 255);
    REQUIRE(c->g == 0);
    REQUIRE(c->b == 85);
    REQUIRE(c->a == 51); // 0.2 * 255 = 51
}

TEST_CASE("Color: from_rgba_string with full alpha", "[theme][color]")
{
    auto c = Color::from_rgba_string("rgba(0, 0, 0, 1.0)");
    REQUIRE(c.has_value());
    REQUIRE(c->a == 255);
}

TEST_CASE("Color: from_string auto-detects format", "[theme][color]")
{
    auto hex = Color::from_string("#ff0055");
    REQUIRE(hex.has_value());

    auto rgba = Color::from_string("rgba(255, 0, 85, 0.2)");
    REQUIRE(rgba.has_value());
}

// ===== Color conversion tests =====

TEST_CASE("Color: to_hex round-trip", "[theme][color]")
{
    Color c(0xff, 0x00, 0x55);
    auto hex = c.to_hex();
    REQUIRE(hex == "#ff0055");

    auto parsed = Color::from_hex(hex);
    REQUIRE(parsed.has_value());
    REQUIRE(*parsed == c);
}

TEST_CASE("Color: to_theme_string uses hex for opaque", "[theme][color]")
{
    Color opaque(0xff, 0x00, 0x55, 255);
    REQUIRE(opaque.to_theme_string().starts_with("#"));

    Color transparent(0xff, 0x00, 0x55, 128);
    REQUIRE(transparent.to_theme_string().starts_with("rgba("));
}

// ===== Color operations tests =====

TEST_CASE("Color: with_alpha float", "[theme][color]")
{
    Color c(255, 0, 0);
    auto half = c.with_alpha(0.5f);
    // 0.5 * 255 = 127.5, truncated to 127
    REQUIRE(half.a >= 127);
    REQUIRE(half.a <= 128);
}

TEST_CASE("Color: blend", "[theme][color]")
{
    Color black(0, 0, 0);
    Color white(255, 255, 255);
    auto mid = black.blend(white, 0.5f);
    // Should be around (127, 127, 127)
    REQUIRE(mid.r >= 126);
    REQUIRE(mid.r <= 128);
}

TEST_CASE("Color: luminance", "[theme][color]")
{
    Color black(0, 0, 0);
    REQUIRE_THAT(black.luminance(), WithinAbs(0.0, 0.001));

    Color white(255, 255, 255);
    REQUIRE_THAT(white.luminance(), WithinAbs(1.0, 0.01));
}

TEST_CASE("Color: contrast_ratio", "[theme][color]")
{
    Color black(0, 0, 0);
    Color white(255, 255, 255);
    auto ratio = black.contrast_ratio(white);
    // WCAG: black vs white should be ~21:1
    REQUIRE_THAT(ratio, WithinAbs(21.0, 0.5));
}

// ===== Theme tests =====

TEST_CASE("Theme: validation passes for valid theme", "[theme]")
{
    Theme t{
        .id = "test",
        .name = "Test Theme",
        .colors = {.bg_app = Color(0, 0, 0),
                   .bg_panel = Color(10, 10, 10),
                   .bg_header = Color(20, 20, 20),
                   .bg_input = Color(0, 0, 0),
                   .text_main = Color(200, 200, 200),
                   .text_muted = Color(100, 100, 100),
                   .accent_primary = Color(255, 0, 0),
                   .accent_secondary = Color(0, 255, 0),
                   .border_light = Color(50, 50, 50),
                   .border_dark = Color(0, 0, 0)},
    };
    REQUIRE(t.is_valid());
    REQUIRE(t.validation_errors().empty());
}

TEST_CASE("Theme: validation fails with empty id", "[theme]")
{
    Theme t{.id = "", .name = "Test"};
    REQUIRE_FALSE(t.is_valid());
}

TEST_CASE("Theme: is_dark for dark theme", "[theme]")
{
    Theme t{.id = "test", .name = "Test", .colors = {.bg_app = Color(5, 5, 16)}};
    REQUIRE(t.is_dark());
}

TEST_CASE("Theme: is_dark false for light theme", "[theme]")
{
    Theme t{.id = "test", .name = "Test", .colors = {.bg_app = Color(255, 255, 255)}};
    REQUIRE_FALSE(t.is_dark());
}

TEST_CASE("Theme: derived selection_bg has low alpha", "[theme]")
{
    Theme t{.id = "test", .name = "Test", .colors = {.accent_primary = Color(255, 0, 85)}};
    auto sel = t.selection_bg();
    REQUIRE(sel.r == 255);
    REQUIRE(sel.a < 60); // 20% alpha ≈ 51
}

// ===== JSON serialization tests =====

TEST_CASE("Theme: JSON round-trip", "[theme][json]")
{
    const auto& theme = get_default_theme();
    nlohmann::json j = theme;

    auto parsed = j.get<Theme>();
    REQUIRE(parsed.id == theme.id);
    REQUIRE(parsed.name == theme.name);
    REQUIRE(parsed.colors == theme.colors);
}

TEST_CASE("Theme: JSON format uses CSS variable names", "[theme][json]")
{
    const auto& theme = get_default_theme();
    nlohmann::json j = theme;

    REQUIRE(j.contains("colors"));
    REQUIRE(j["colors"].contains("--bg-app"));
    REQUIRE(j["colors"].contains("--accent-primary"));
}

TEST_CASE("Theme: JSON invalid throws", "[theme][json]")
{
    nlohmann::json j = {
        {"id", "test"}, {"name", "Test"}, {"colors", {{"--bg-app", "not-a-color"}}}};

    REQUIRE_THROWS(j.get<Theme>());
}

// ===== Built-in themes tests =====

TEST_CASE("BuiltinThemes: 8 themes available", "[theme][builtin]")
{
    REQUIRE(get_builtin_themes().size() == 8);
}

TEST_CASE("BuiltinThemes: default is Midnight Neon", "[theme][builtin]")
{
    const auto& def = get_default_theme();
    REQUIRE(def.id == "midnight-neon");
    REQUIRE(def.name == "Midnight Neon");
}

TEST_CASE("BuiltinThemes: all themes have unique IDs", "[theme][builtin]")
{
    const auto& themes = get_builtin_themes();
    std::set<std::string> ids;
    for (const auto& t : themes)
    {
        REQUIRE(ids.insert(t.id).second); // insert returns false on duplicate
    }
}

TEST_CASE("BuiltinThemes: all themes are valid", "[theme][builtin]")
{
    for (const auto& t : get_builtin_themes())
    {
        INFO("Theme: " << t.id);
        REQUIRE(t.is_valid());
    }
}

TEST_CASE("BuiltinThemes: Classic Mono is a light theme", "[theme][builtin]")
{
    const auto& themes = get_builtin_themes();
    auto it = std::find_if(
        themes.begin(), themes.end(), [](const Theme& t) { return t.id == "classic-mono"; });
    REQUIRE(it != themes.end());
    REQUIRE_FALSE(it->is_dark());
}

// ===== ThemeRegistry tests =====

TEST_CASE("ThemeRegistry: initializes with 8 built-in themes", "[theme][registry]")
{
    ThemeRegistry reg;
    auto result = reg.initialize();
    REQUIRE(result.has_value());
    REQUIRE(reg.theme_count() >= 8);
}

TEST_CASE("ThemeRegistry: get_theme by ID", "[theme][registry]")
{
    ThemeRegistry reg;
    reg.initialize();

    auto theme = reg.get_theme("midnight-neon");
    REQUIRE(theme.has_value());
    REQUIRE(theme->name == "Midnight Neon");

    auto missing = reg.get_theme("nonexistent");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("ThemeRegistry: list_themes returns info", "[theme][registry]")
{
    ThemeRegistry reg;
    reg.initialize();

    auto infos = reg.list_themes();
    REQUIRE(infos.size() >= 8);
    REQUIRE(infos[0].is_builtin);
}

TEST_CASE("ThemeRegistry: export and re-import theme", "[theme][registry]")
{
    ThemeRegistry reg;
    reg.initialize();

    auto tmpDir = std::filesystem::temp_directory_path() / "markamp_test";
    std::filesystem::create_directories(tmpDir);
    auto tmpFile = tmpDir / "export_test.json";

    auto exportResult = reg.export_theme("midnight-neon", tmpFile);
    REQUIRE(exportResult.has_value());
    REQUIRE(std::filesystem::exists(tmpFile));

    // Read the exported file
    std::ifstream f(tmpFile);
    nlohmann::json j;
    f >> j;
    auto reloaded = j.get<Theme>();
    REQUIRE(reloaded.id == "midnight-neon");
    REQUIRE(reloaded.name == "Midnight Neon");

    // Cleanup
    std::filesystem::remove_all(tmpDir);
}
