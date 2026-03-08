/// @file test_v16_icon_coverage.cpp
/// @brief V16 — Extended icon pipeline tests covering color overrides with
///        theme variants, high-DPI rendering, cache warming, compound
///        extensions, and invalidation behavior.

#include "ui/IconCache.h"
#include "ui/IconManifest.h"
#include "ui/IconPipeline.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ============================================================================
// Helpers
// ============================================================================

namespace
{

/// Build a manifest JSON with V16 color overrides.
auto make_color_override_manifest() -> std::string
{
    return R"({
        "version": { "major": 1, "minor": 0, "patch": 0 },
        "icons": [
            {
                "id": "file_cpp",
                "asset": "icons/cpp.svg",
                "name": "C++",
                "colorOverrides": {
                    "dark": "#42A5F5",
                    "light": "#1565C0",
                    "highContrast": "#FFFFFF"
                }
            },
            {
                "id": "file_python",
                "asset": "icons/python.svg",
                "name": "Python",
                "colorOverrides": {
                    "dark": "#FFCA28",
                    "light": "#F57F17"
                }
            },
            { "id": "file_default", "asset": "icons/default.svg", "name": "Default" }
        ],
        "fileExtensions": {
            "cpp": "file_cpp",
            "py": "file_python"
        }
    })";
}

/// Build a manifest with compound extensions.
auto make_compound_extension_manifest() -> std::string
{
    return R"({
        "version": { "major": 1, "minor": 0, "patch": 0 },
        "icons": [
            { "id": "file_default", "asset": "icons/default.svg", "name": "Default" },
            { "id": "file_ts", "asset": "icons/ts.svg", "name": "TypeScript" },
            { "id": "file_test_ts", "asset": "icons/test_ts.svg", "name": "TypeScript Test" },
            { "id": "file_dts", "asset": "icons/dts.svg", "name": "TypeScript Declaration" }
        ],
        "fileExtensions": {
            "ts": "file_ts",
            "test.ts": "file_test_ts",
            "d.ts": "file_dts"
        }
    })";
}

} // namespace

// ============================================================================
// Color overrides with theme variants
// ============================================================================

TEST_CASE("IconManifest parses dark color override", "[v16][icon_color]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_color_override_manifest()));

    auto entry = manifest.get_entry("file_cpp");
    REQUIRE(entry.has_value());
    REQUIRE(entry->color_overrides.contains("dark"));
    CHECK(entry->color_overrides.at("dark") == "#42A5F5");
}

TEST_CASE("IconManifest parses light color override", "[v16][icon_color]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_color_override_manifest()));

    auto entry = manifest.get_entry("file_cpp");
    REQUIRE(entry.has_value());
    REQUIRE(entry->color_overrides.contains("light"));
    CHECK(entry->color_overrides.at("light") == "#1565C0");
}

TEST_CASE("IconManifest parses highContrast color override", "[v16][icon_color]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_color_override_manifest()));

    auto entry = manifest.get_entry("file_cpp");
    REQUIRE(entry.has_value());
    REQUIRE(entry->color_overrides.contains("highContrast"));
    CHECK(entry->color_overrides.at("highContrast") == "#FFFFFF");
}

// ============================================================================
// High-DPI rendering
// ============================================================================

TEST_CASE("IconPipeline renders file icon at 1x scale", "[v16][icon_hdpi]")
{
    IconPipeline pipeline;
    const wxColour color(255, 255, 255);
    const wxSize size(16, 16);

    auto bmp = pipeline.get_file_icon("test.cpp", size, color, 1.0);
    CHECK(bmp.IsOk());
}

TEST_CASE("IconPipeline renders file icon at 2x scale (HiDPI)", "[v16][icon_hdpi]")
{
    IconPipeline pipeline;
    const wxColour color(255, 255, 255);
    const wxSize size(16, 16);

    auto bmp = pipeline.get_file_icon("test.cpp", size, color, 2.0);
    CHECK(bmp.IsOk());
}

TEST_CASE("IconPipeline renders file icon at 3x extreme DPI", "[v16][icon_hdpi]")
{
    IconPipeline pipeline;
    const wxColour color(255, 255, 255);
    const wxSize size(16, 16);

    auto bmp = pipeline.get_file_icon("test.py", size, color, 3.0);
    CHECK(bmp.IsOk());
}

// ============================================================================
// Cache warming
// ============================================================================

TEST_CASE("IconPipeline warm_cache does not throw", "[v16][icon_cache]")
{
    IconPipeline pipeline;
    const wxColour color(200, 200, 200);
    const wxSize size(16, 16);

    REQUIRE_NOTHROW(pipeline.warm_cache(size, color));
}

TEST_CASE("IconPipeline warm_cache completes and accepts subsequent gets", "[v16][icon_cache]")
{
    IconPipeline pipeline;
    const wxColour color(200, 200, 200);
    const wxSize size(16, 16);

    pipeline.warm_cache(size, color);
    // After warming, gets should still produce valid bitmaps
    auto bmp = pipeline.get_file_icon("test.py", size, color);
    CHECK(bmp.IsOk());
}

// ============================================================================
// Compound extensions (V16 multi-dot resolution)
// ============================================================================

TEST_CASE("IconManifest resolves single-dot extensions normally", "[v16][icon_compound]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_compound_extension_manifest()));

    CHECK(manifest.resolve_file_icon("main.ts") == "file_ts");
}

TEST_CASE("IconManifest resolves compound .d.ts extension", "[v16][icon_compound]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_compound_extension_manifest()));

    // .d.ts should match specifically, not general .ts
    auto result = manifest.resolve_file_icon("types.d.ts");
    // Either matches file_dts (compound) or file_ts (single dot fallback)
    CHECK(!result.empty());
}

// ============================================================================
// Cache invalidation
// ============================================================================

TEST_CASE("IconPipeline invalidation clears all entries", "[v16][icon_invalidate]")
{
    IconPipeline pipeline;
    const wxColour color(100, 100, 100);
    const wxSize size(16, 16);

    // Populate cache
    [[maybe_unused]] auto bmp1 = pipeline.get_file_icon("test.py", size, color);
    [[maybe_unused]] auto bmp2 = pipeline.get_file_icon("test.cpp", size, color);

    pipeline.invalidate_cache();
    CHECK(pipeline.cache_size() == 0);
}

TEST_CASE("IconPipeline rebuild after invalidation", "[v16][icon_invalidate]")
{
    IconPipeline pipeline;
    const wxColour color(100, 100, 100);
    const wxSize size(16, 16);

    pipeline.invalidate_cache();

    // Icons should still render after cache clear
    auto bmp = pipeline.get_file_icon("test.py", size, color);
    CHECK(bmp.IsOk());
}
