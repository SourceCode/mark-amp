#include "core/Theme.h"
#include "core/loader/ThemeLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace markamp::core;

TEST_CASE("ThemeLoader loads valid markdown theme", "[ThemeLoader]")
{
    // Create a temporary theme file
    fs::path temp_dir = fs::temp_directory_path() / "markamp_test_themes";
    fs::create_directories(temp_dir);
    fs::path theme_path = temp_dir / "test_theme.md";

    std::ofstream out(theme_path);
    out << R"(---
id: test-theme
name: Test Theme
type: dark
colors:
  --bg-app: "#1a1b26"
  --text-main: "#a9b1d6"
  --editor-bg: "#24283b"
  --editor-selection: "#364a82"
---
# Markdown Content
)";
    out.close();

    auto result = ThemeLoader::load_from_file(theme_path);
    REQUIRE(result.has_value());

    const Theme& theme = *result;
    CHECK(theme.id == "test-theme");
    CHECK(theme.name == "Test Theme");
    CHECK(theme.colors.bg_app.to_rgba_string() == "rgba(26, 27, 38, 1)");
    CHECK(theme.colors.text_main.to_rgba_string() == "rgba(169, 177, 214, 1)");
    CHECK(theme.colors.editor_bg.to_rgba_string() == "rgba(36, 40, 59, 1)");

    // Cleanup
    fs::remove(theme_path);
    fs::remove(temp_dir);
}

TEST_CASE("ThemeLoader handles missing file", "[ThemeLoader]")
{
    auto result = ThemeLoader::load_from_file("non_existent_file.md");
    REQUIRE_FALSE(result.has_value());
    CHECK(result.error().find("File not found") != std::string::npos);
}

TEST_CASE("ThemeLoader validates theme", "[ThemeLoader]")
{
    // Create invalid theme (missing required fields)
    fs::path temp_dir = fs::temp_directory_path() / "markamp_test_themes";
    fs::create_directories(temp_dir);
    fs::path theme_path = temp_dir / "invalid_theme.md";

    std::ofstream out(theme_path);
    out << R"(---
colors:
  --bg-app: "#000000"
---
)";
    out.close();

    auto result = ThemeLoader::load_from_file(theme_path);
    REQUIRE_FALSE(result.has_value());
    CHECK(result.error().find("Missing required fields") != std::string::npos);

    // Cleanup
    fs::remove(theme_path);
    fs::remove(temp_dir);
}
