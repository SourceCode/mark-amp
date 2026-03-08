/// @file test_icon_manifest.cpp
/// @brief V16 Phase 01-04 — Tests for IconManifest and IconPipeline.
///
/// Contract tests for manifest parsing, file/folder/command resolution,
/// validation, and pipeline integration.

#include "ui/IconManifest.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ════════════════════════════════════════════════════════
// Manifest Parsing
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest parses valid manifest JSON", "[icon_manifest]")
{
    const auto json = R"({
        "version": { "major": 1, "minor": 0, "patch": 0 },
        "icons": [
            { "id": "file_default", "asset": "icons/file_default.svg", "name": "Default" },
            { "id": "file_cpp", "asset": "icons/file_cpp.svg", "name": "C++" },
            { "id": "file_header", "asset": "icons/file_header.svg", "name": "Header" },
            { "id": "command_default", "asset": "icons/cmd_default.svg", "name": "Cmd" }
        ],
        "fileExtensions": {
            "cpp": "file_cpp",
            "cc": "file_cpp",
            "h": "file_header",
            "hpp": "file_header"
        },
        "fileNames": {
            "dockerfile": "file_default"
        },
        "folders": [
            { "id": "folder_git", "openAsset": "icons/f_open_git.svg", "closedAsset": "icons/f_closed_git.svg", "name": "Git" }
        ],
        "folderNames": {
            ".git": "folder_git"
        },
        "commandIcons": {
            "file.save": "command_default"
        }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    SECTION("Version is parsed correctly")
    {
        CHECK(manifest.version().major == 1);
        CHECK(manifest.version().minor == 0);
        CHECK(manifest.version().patch == 0);
    }

    SECTION("Icon entries are loaded")
    {
        CHECK(manifest.icon_count() == 4);
        auto entry = manifest.get_entry("file_cpp");
        REQUIRE(entry.has_value());
        CHECK(entry->canonical_id == "file_cpp");
        CHECK(entry->asset_path == "icons/file_cpp.svg");
        CHECK(entry->display_name == "C++");
    }

    SECTION("Extension mappings are loaded")
    {
        CHECK(manifest.extension_count() == 4);
    }

    SECTION("Folder mappings are loaded")
    {
        CHECK(manifest.folder_mapping_count() == 1);
    }

    SECTION("Command mappings are loaded")
    {
        CHECK(manifest.command_mapping_count() == 1);
    }

    SECTION("No validation errors")
    {
        CHECK(manifest.validation_errors().empty());
    }
}

TEST_CASE("IconManifest rejects invalid JSON", "[icon_manifest]")
{
    IconManifest manifest;
    REQUIRE_FALSE(manifest.load_from_string("not valid json {{{"));
    CHECK_FALSE(manifest.validation_errors().empty());
}

TEST_CASE("IconManifest detects duplicate icon IDs", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_cpp", "asset": "icons/a.svg", "name": "A" },
            { "id": "file_cpp", "asset": "icons/b.svg", "name": "B" }
        ]
    })";

    IconManifest manifest;
    manifest.load_from_string(json);
    // Should detect duplicate
    bool has_dup = false;
    for (const auto& err : manifest.validation_errors())
    {
        if (err.find("Duplicate") != std::string::npos)
        {
            has_dup = true;
        }
    }
    CHECK(has_dup);
}

TEST_CASE("IconManifest detects missing asset paths", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_no_asset", "asset": "", "name": "No Asset" }
        ]
    })";

    IconManifest manifest;
    manifest.load_from_string(json);
    bool has_empty_path = false;
    for (const auto& err : manifest.validation_errors())
    {
        if (err.find("empty asset") != std::string::npos)
        {
            has_empty_path = true;
        }
    }
    CHECK(has_empty_path);
}

TEST_CASE("IconManifest detects dangling extension references", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_default", "asset": "icons/d.svg", "name": "Default" }
        ],
        "fileExtensions": {
            "rs": "file_rust_nonexistent"
        }
    })";

    IconManifest manifest;
    manifest.load_from_string(json);
    bool has_dangling = false;
    for (const auto& err : manifest.validation_errors())
    {
        if (err.find("unknown icon ID") != std::string::npos)
        {
            has_dangling = true;
        }
    }
    CHECK(has_dangling);
}

// ════════════════════════════════════════════════════════
// File Icon Resolution
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest resolves file icons by extension", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_default", "asset": "icons/default.svg", "name": "Default" },
            { "id": "file_cpp", "asset": "icons/cpp.svg", "name": "C++" },
            { "id": "file_python", "asset": "icons/py.svg", "name": "Python" }
        ],
        "fileExtensions": {
            "cpp": "file_cpp",
            "cc": "file_cpp",
            "py": "file_python"
        }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    CHECK(manifest.resolve_file_icon("main.cpp") == "file_cpp");
    CHECK(manifest.resolve_file_icon("util.cc") == "file_cpp");
    CHECK(manifest.resolve_file_icon("script.py") == "file_python");
    CHECK(manifest.resolve_file_icon("README.TXT") == "file_default"); // No mapping for txt
}

TEST_CASE("IconManifest resolves special filenames before extension", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_default", "asset": "icons/default.svg", "name": "Default" },
            { "id": "file_docker", "asset": "icons/docker.svg", "name": "Docker" },
            { "id": "file_git", "asset": "icons/git.svg", "name": "Git" }
        ],
        "fileExtensions": {},
        "fileNames": {
            "dockerfile": "file_docker",
            ".gitignore": "file_git"
        }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    CHECK(manifest.resolve_file_icon("Dockerfile") == "file_docker");
    CHECK(manifest.resolve_file_icon("DOCKERFILE") == "file_docker");
    CHECK(manifest.resolve_file_icon(".gitignore") == "file_git");
}

TEST_CASE("IconManifest resolves paths correctly", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_default", "asset": "icons/default.svg", "name": "Default" },
            { "id": "file_cpp", "asset": "icons/cpp.svg", "name": "C++" }
        ],
        "fileExtensions": { "cpp": "file_cpp" }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    // Should extract filename from path and resolve correctly
    CHECK(manifest.resolve_file_icon("/Users/dev/project/src/main.cpp") == "file_cpp");
    CHECK(manifest.resolve_file_icon("C:\\Users\\dev\\project\\src\\main.cpp") == "file_cpp");
}

TEST_CASE("IconManifest falls back to default for unknown extensions", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_default", "asset": "icons/default.svg", "name": "Default" }
        ],
        "fileExtensions": {}
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    CHECK(manifest.resolve_file_icon("mystery.xyz") == "file_default");
    CHECK(manifest.resolve_file_icon("no_extension") == "file_default");
}

// ════════════════════════════════════════════════════════
// Folder Icon Resolution
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest resolves folder icons by name", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [],
        "folders": [
            { "id": "folder_git", "openAsset": "icons/git_open.svg", "closedAsset": "icons/git_closed.svg", "name": "Git" },
            { "id": "folder_src", "openAsset": "icons/src_open.svg", "closedAsset": "icons/src_closed.svg", "name": "Source" }
        ],
        "folderNames": {
            ".git": "folder_git",
            "src": "folder_src"
        }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    auto git = manifest.resolve_folder_icon(".git");
    REQUIRE(git.has_value());
    CHECK(git->canonical_id == "folder_git");
    CHECK(git->open_asset == "icons/git_open.svg");
    CHECK(git->closed_asset == "icons/git_closed.svg");

    auto src = manifest.resolve_folder_icon("src");
    REQUIRE(src.has_value());
    CHECK(src->canonical_id == "folder_src");
}

TEST_CASE("IconManifest returns nullopt for unknown folders", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [],
        "folders": [],
        "folderNames": {}
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    CHECK_FALSE(manifest.resolve_folder_icon("random_folder").has_value());
}

// ════════════════════════════════════════════════════════
// Command Icon Resolution
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest resolves command icons", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "command_default", "asset": "icons/cmd.svg", "name": "Cmd" },
            { "id": "command_file", "asset": "icons/file_cmd.svg", "name": "File" }
        ],
        "commandIcons": {
            "file.save": "command_file",
            "file.open": "command_file"
        }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    CHECK(manifest.resolve_command_icon("file.save") == "command_file");
    CHECK(manifest.resolve_command_icon("file.open") == "command_file");
    CHECK(manifest.resolve_command_icon("unknown.cmd") == "command_default");
}

// ════════════════════════════════════════════════════════
// Introspection
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest all_icon_ids returns sorted list", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_cpp", "asset": "a.svg", "name": "C++" },
            { "id": "file_python", "asset": "b.svg", "name": "Python" },
            { "id": "file_default", "asset": "c.svg", "name": "Default" }
        ]
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    auto ids = manifest.all_icon_ids();
    REQUIRE(ids.size() == 3);
    CHECK(ids[0] == "file_cpp");
    CHECK(ids[1] == "file_default");
    CHECK(ids[2] == "file_python");
}

// ════════════════════════════════════════════════════════
// Color overrides
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest parses color overrides", "[icon_manifest]")
{
    const auto json = R"({
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
            }
        ]
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    auto entry = manifest.get_entry("file_cpp");
    REQUIRE(entry.has_value());
    CHECK(entry->color_overrides.size() == 3);
    CHECK(entry->color_overrides.at("dark") == "#42A5F5");
    CHECK(entry->color_overrides.at("light") == "#1565C0");
    CHECK(entry->color_overrides.at("highContrast") == "#FFFFFF");
}

// ════════════════════════════════════════════════════════
// Case insensitivity
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest handles case-insensitive extension matching", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_default", "asset": "d.svg", "name": "Default" },
            { "id": "file_cpp", "asset": "cpp.svg", "name": "C++" }
        ],
        "fileExtensions": { "cpp": "file_cpp" }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    CHECK(manifest.resolve_file_icon("test.CPP") == "file_cpp");
    CHECK(manifest.resolve_file_icon("test.Cpp") == "file_cpp");
    CHECK(manifest.resolve_file_icon("test.cpp") == "file_cpp");
}

// ════════════════════════════════════════════════════════
// Empty and edge cases
// ════════════════════════════════════════════════════════

TEST_CASE("IconManifest handles empty manifest", "[icon_manifest]")
{
    IconManifest manifest;
    REQUIRE(manifest.load_from_string("{}"));

    CHECK(manifest.icon_count() == 0);
    CHECK(manifest.extension_count() == 0);
    CHECK(manifest.resolve_file_icon("test.cpp") == "file_default");
}

TEST_CASE("IconManifest handles file without extension", "[icon_manifest]")
{
    const auto json = R"({
        "icons": [
            { "id": "file_default", "asset": "d.svg", "name": "Default" },
            { "id": "file_makefile", "asset": "mk.svg", "name": "Makefile" }
        ],
        "fileNames": { "makefile": "file_makefile" }
    })";

    IconManifest manifest;
    REQUIRE(manifest.load_from_string(json));

    CHECK(manifest.resolve_file_icon("Makefile") == "file_makefile");
    CHECK(manifest.resolve_file_icon("MAKEFILE") == "file_makefile");
}

TEST_CASE("IconManifest version to_string", "[icon_manifest]")
{
    ManifestVersion ver{2, 3, 1};
    CHECK(ver.to_string() == "2.3.1");
}
