#include "core/Theme.h"
#include "core/loader/ThemeLoader.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ThemeLoader V2 parses semantic tokens and textmate rules", "[theme][v2]")
{
    std::string yaml = R"(
id: "test-v2-theme"
name: "Test V2 Theme"
colors:
  --bg-app: "#101010"
  editor.background: "#151515"
  tab.activeBackground: "#202020"
tokenColors:
  - scope: ["keyword.control", "keyword.operator"]
    settings:
      foreground: "#FF0000"
      fontStyle: "italic bold"
  - scope: "string"
    settings:
      foreground: "#00FF00"
)";

    auto result = ThemeLoader::parse_yaml_content(yaml);
    REQUIRE(result.has_value());

    const auto& theme = *result;
    REQUIRE(theme.id == "test-v2-theme");

    // Check base colors
    REQUIRE(theme.colors.bg_app.r == 0x10);

    // Check semantic tokens map
    REQUIRE(theme.semantic_tokens.contains("editor.background"));
    REQUIRE(theme.semantic_tokens.at("editor.background").Red() == 0x15);
    REQUIRE(theme.semantic_tokens.contains("tab.activeBackground"));

    // Check TextMate token rules
    REQUIRE(theme.token_colors.size() == 3); // keyword.control, keyword.operator, string

    auto rule0 = theme.token_colors[0];
    REQUIRE(rule0.scope == "keyword.control");
    REQUIRE(rule0.foreground.has_value());
    REQUIRE(rule0.foreground->r == 0xFF);
    REQUIRE(rule0.font_style.has_value());
    REQUIRE(*rule0.font_style == "italic bold");

    auto rule1 = theme.token_colors[1];
    REQUIRE(rule1.scope == "keyword.operator");

    auto rule2 = theme.token_colors[2];
    REQUIRE(rule2.scope == "string");
    REQUIRE(rule2.foreground.has_value());
    REQUIRE(rule2.foreground->g == 0xFF);
    REQUIRE(!rule2.font_style.has_value());
}
