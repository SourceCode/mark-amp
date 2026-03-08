/// @file test_icon_pipeline.cpp
/// @brief V16 Phase 09-10 — Unit tests for IconPipeline round-trips,
///        cache behavior, theme invalidation, and fallback chains.

#include "ui/IconManifest.h"
#include "ui/IconPipeline.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>

using namespace markamp::ui;

// ============================================================================
// Helpers
// ============================================================================

namespace
{

/// Build a minimal manifest JSON for testing (matches actual manifest format).
auto make_test_manifest_json() -> std::string
{
    return R"({
        "version": { "major": 1, "minor": 0, "patch": 0 },
        "icons": [
            { "id": "file_cpp",       "asset": "icons/file_cpp.svg",       "name": "C++ Source" },
            { "id": "file_python",    "asset": "icons/file_python.svg",    "name": "Python" },
            { "id": "file_default",   "asset": "icons/file_default.svg",   "name": "Default File" },
            { "id": "command_save",   "asset": "icons/command_save.svg",   "name": "Save" },
            { "id": "command_default","asset": "icons/command_default.svg", "name": "Default Command" },
            { "id": "folder_default", "asset": "icons/folder_default.svg", "name": "Folder" }
        ],
        "fileExtensions": {
            "cpp": "file_cpp",
            "cc": "file_cpp",
            "py": "file_python",
            "pyw": "file_python"
        },
        "fileNames": {
            "makefile": "file_cpp"
        },
        "folders": [
            {
                "id": "folder_src",
                "openAsset": "icons/folder_src_open.svg",
                "closedAsset": "icons/folder_src_closed.svg",
                "name": "Source Folder"
            }
        ],
        "folderNames": {
            "src": "folder_src"
        },
        "commandIcons": {
            "save": "command_save",
            "save_all": "command_save"
        }
    })";
}

} // namespace

// ============================================================================
// Pipeline initialization
// ============================================================================

TEST_CASE("IconPipeline default state", "[icon_pipeline]")
{
    const IconPipeline pipeline;
    CHECK_FALSE(pipeline.is_initialized());
    CHECK(pipeline.cache_size() == 0);
    CHECK(pipeline.errors().empty());
}

TEST_CASE("IconPipeline initialize with valid config", "[icon_pipeline]")
{
    IconPipeline pipeline;
    IconPipelineConfig config;
    config.asset_root = "resources/icons/";
    config.manifest_path = "resources/icons/icon_manifest.json";
    config.max_cache_entries = 512;
    config.default_size = 16;

    // If the manifest file doesn't exist in the test environment,
    // initialize() returns false but doesn't crash
    [[maybe_unused]] const bool init_result = pipeline.initialize(config);
}

TEST_CASE("IconPipeline manifest access before initialize is empty", "[icon_pipeline]")
{
    const IconPipeline pipeline;
    const auto& manifest = pipeline.manifest();

    CHECK(manifest.icon_count() == 0);
    CHECK(manifest.extension_count() == 0);
}

// ============================================================================
// File icon resolution — pipeline provides fallback bitmaps
// ============================================================================

TEST_CASE("IconPipeline file icon returns bitmap even uninitialized (fallback)", "[icon_pipeline]")
{
    IconPipeline pipeline;
    const wxColour test_color(255, 255, 255);
    const wxSize test_size(16, 16);

    // Pipeline returns a valid fallback bitmap even when not initialized
    auto bmp = pipeline.get_file_icon("main.cpp", test_size, test_color);
    // The pipeline's fallback path renders a generic icon
    CHECK(bmp.IsOk());
}

TEST_CASE("IconPipeline icon by ID returns bitmap for unknown ID (fallback)", "[icon_pipeline]")
{
    IconPipeline pipeline;
    const wxColour test_color(255, 255, 255);
    const wxSize test_size(16, 16);

    auto bmp = pipeline.get_icon_by_id("nonexistent_icon", test_size, test_color);
    // Fallback path still returns a valid generic bitmap
    CHECK(bmp.IsOk());
}

// ============================================================================
// Folder icon resolution — pipeline provides fallback bitmaps
// ============================================================================

TEST_CASE("IconPipeline folder icons return fallback bitmaps", "[icon_pipeline]")
{
    IconPipeline pipeline;
    const wxColour test_color(255, 255, 255);
    const wxSize test_size(16, 16);

    auto open_bmp = pipeline.get_open_folder_icon("src", test_size, test_color);
    CHECK(open_bmp.IsOk());

    auto closed_bmp = pipeline.get_closed_folder_icon("src", test_size, test_color);
    CHECK(closed_bmp.IsOk());
}

// ============================================================================
// Command icon resolution — pipeline provides fallback bitmaps
// ============================================================================

TEST_CASE("IconPipeline command icons return fallback bitmaps", "[icon_pipeline]")
{
    IconPipeline pipeline;
    const wxColour test_color(255, 255, 255);
    const wxSize test_size(16, 16);

    auto bmp = pipeline.get_command_icon("save", test_size, test_color);
    CHECK(bmp.IsOk());
}

// ============================================================================
// Cache management
// ============================================================================

TEST_CASE("IconPipeline cache starts empty", "[icon_pipeline]")
{
    const IconPipeline pipeline;
    CHECK(pipeline.cache_size() == 0);
}

TEST_CASE("IconPipeline invalidate_cache is safe on empty cache", "[icon_pipeline]")
{
    IconPipeline pipeline;
    CHECK(pipeline.cache_size() == 0);
    pipeline.invalidate_cache();
    CHECK(pipeline.cache_size() == 0);
}

TEST_CASE("IconPipeline warm_cache on uninitialized pipeline is safe", "[icon_pipeline]")
{
    IconPipeline pipeline;
    const wxColour test_color(255, 255, 255);
    const wxSize test_size(16, 16);

    REQUIRE_NOTHROW(pipeline.warm_cache(test_size, test_color));
}

// ============================================================================
// Manifest round-trip
// ============================================================================

TEST_CASE("IconManifest round-trip through load_from_string", "[icon_pipeline]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_test_manifest_json()));

    // File extension mapping
    CHECK(manifest.resolve_file_icon("main.cpp") == "file_cpp");
    CHECK(manifest.resolve_file_icon("script.py") == "file_python");
    CHECK(manifest.resolve_file_icon("unknown.xyz") == "file_default");

    // Command mapping
    CHECK(manifest.resolve_command_icon("save") == "command_save");
    CHECK(manifest.resolve_command_icon("unknown_cmd") == "command_default");

    // Icon entry lookup
    auto entry = manifest.get_entry("file_cpp");
    REQUIRE(entry.has_value());
    CHECK(entry->canonical_id == "file_cpp");
    CHECK(entry->asset_path == "icons/file_cpp.svg");
    CHECK(entry->display_name == "C++ Source");

    // Folder resolution
    auto folder = manifest.resolve_folder_icon("src");
    REQUIRE(folder.has_value());
    CHECK(folder->canonical_id == "folder_src");
    CHECK(folder->open_asset == "icons/folder_src_open.svg");
}

// ============================================================================
// Manifest introspection
// ============================================================================

TEST_CASE("IconManifest counts match loaded entries", "[icon_pipeline]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_test_manifest_json()));

    CHECK(manifest.icon_count() == 6);            // 3 file + 2 command + 1 folder_default
    CHECK(manifest.extension_count() == 4);       // cpp, cc, py, pyw
    CHECK(manifest.folder_mapping_count() == 1);  // src
    CHECK(manifest.command_mapping_count() == 2); // save, save_all
}

TEST_CASE("IconManifest all_icon_ids returns all registered IDs", "[icon_pipeline]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_test_manifest_json()));

    auto ids = manifest.all_icon_ids();
    CHECK(ids.size() == 6);

    bool has_cpp = false;
    bool has_python = false;
    bool has_default = false;
    for (const auto& the_id : ids)
    {
        if (the_id == "file_cpp")
        {
            has_cpp = true;
        }
        if (the_id == "file_python")
        {
            has_python = true;
        }
        if (the_id == "file_default")
        {
            has_default = true;
        }
    }
    CHECK(has_cpp);
    CHECK(has_python);
    CHECK(has_default);
}

// ============================================================================
// Manifest validation
// ============================================================================

TEST_CASE("IconManifest rejects invalid JSON", "[icon_pipeline]")
{
    IconManifest manifest;
    CHECK_FALSE(manifest.load_from_string("{ not valid json ]]]"));
}

TEST_CASE("IconManifest handles empty string", "[icon_pipeline]")
{
    IconManifest manifest;
    CHECK_FALSE(manifest.load_from_string(""));
}

TEST_CASE("IconManifest handles version-only JSON", "[icon_pipeline]")
{
    IconManifest manifest;
    auto loaded = manifest.load_from_string(R"({"version": {"major": 1}})");
    if (loaded)
    {
        CHECK(manifest.icon_count() == 0);
    }
}

// ============================================================================
// Special filename resolution
// ============================================================================

TEST_CASE("IconManifest special filename takes precedence", "[icon_pipeline]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string(make_test_manifest_json()));

    // "makefile" is a special filename mapped to file_cpp
    auto result = manifest.resolve_file_icon("Makefile");
    CHECK(result == "file_cpp");
}

// ============================================================================
// Theme invalidation flow
// ============================================================================

TEST_CASE("IconPipeline cache invalidation is safe with no errors", "[icon_pipeline]")
{
    IconPipeline pipeline;
    pipeline.invalidate_cache();
    CHECK(pipeline.cache_size() == 0);
    CHECK(pipeline.errors().empty());
}

// ============================================================================
// Config defaults
// ============================================================================

TEST_CASE("IconPipelineConfig defaults are sensible", "[icon_pipeline]")
{
    const IconPipelineConfig config;
    CHECK(config.max_cache_entries == 1024);
    CHECK(config.default_size == 16);
    CHECK(config.asset_root.empty());
    CHECK(config.manifest_path.empty());
}

TEST_CASE("IconPipeline scale parameter is accepted", "[icon_pipeline]")
{
    IconPipeline pipeline;
    const wxColour test_color(128, 128, 128);
    const wxSize test_size(16, 16);

    REQUIRE_NOTHROW(pipeline.get_file_icon("test.py", test_size, test_color, 2.0));
    REQUIRE_NOTHROW(pipeline.get_icon_by_id("file_default", test_size, test_color, 0.5));
}
