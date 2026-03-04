#include "../../src/ui/RichDescriptionRenderer.h"
#include "../../src/ui/SettingsCompareView.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ============================================================================
// RichDescriptionRenderer: Parsing Tests
// ============================================================================

TEST_CASE("RichDescriptionRenderer plain text", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse("This is plain text.");
    REQUIRE(segments.size() == 1);
    REQUIRE(segments[0].type == DescriptionSegmentType::kText);
    REQUIRE(segments[0].text == "This is plain text.");
}

TEST_CASE("RichDescriptionRenderer empty string", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse("");
    REQUIRE(segments.empty());
}

TEST_CASE("RichDescriptionRenderer markdown link", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse("See [docs](https://example.com) for details.");
    REQUIRE(segments.size() == 3);
    REQUIRE(segments[0].type == DescriptionSegmentType::kText);
    REQUIRE(segments[0].text == "See ");
    REQUIRE(segments[1].type == DescriptionSegmentType::kLink);
    REQUIRE(segments[1].text == "docs");
    REQUIRE(segments[1].target == "https://example.com");
    REQUIRE(segments[2].type == DescriptionSegmentType::kText);
    REQUIRE(segments[2].text == " for details.");
}

TEST_CASE("RichDescriptionRenderer inline code", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse("Use `fontSize` property.");
    REQUIRE(segments.size() == 3);
    REQUIRE(segments[0].type == DescriptionSegmentType::kText);
    REQUIRE(segments[0].text == "Use ");
    REQUIRE(segments[1].type == DescriptionSegmentType::kCode);
    REQUIRE(segments[1].text == "fontSize");
    REQUIRE(segments[2].type == DescriptionSegmentType::kText);
    REQUIRE(segments[2].text == " property.");
}

TEST_CASE("RichDescriptionRenderer setting reference", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse("Related to {editor.fontSize}.");
    REQUIRE(segments.size() == 3);
    REQUIRE(segments[0].type == DescriptionSegmentType::kText);
    REQUIRE(segments[0].text == "Related to ");
    REQUIRE(segments[1].type == DescriptionSegmentType::kSettingRef);
    REQUIRE(segments[1].text == "editor.fontSize");
    REQUIRE(segments[1].target == "editor.fontSize");
    REQUIRE(segments[2].type == DescriptionSegmentType::kText);
    REQUIRE(segments[2].text == ".");
}

TEST_CASE("RichDescriptionRenderer mixed content", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse(
        "Set `editor.fontSize` via {editor.fontSize} or see [help](http://h).");
    REQUIRE(segments.size() == 7);
    REQUIRE(segments[0].type == DescriptionSegmentType::kText);
    REQUIRE(segments[1].type == DescriptionSegmentType::kCode);
    REQUIRE(segments[2].type == DescriptionSegmentType::kText);
    REQUIRE(segments[3].type == DescriptionSegmentType::kSettingRef);
    REQUIRE(segments[4].type == DescriptionSegmentType::kText);
    REQUIRE(segments[5].type == DescriptionSegmentType::kLink);
    REQUIRE(segments[6].type == DescriptionSegmentType::kText);
}

TEST_CASE("RichDescriptionRenderer braces without dot are text", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse("Use {variable} here.");
    // {variable} has no dot, so treated as text
    REQUIRE(segments.size() == 1);
    REQUIRE(segments[0].type == DescriptionSegmentType::kText);
    REQUIRE(segments[0].text == "Use {variable} here.");
}

TEST_CASE("RichDescriptionRenderer multiple links", "[settings][renderer]")
{
    auto segments = RichDescriptionRenderer::parse("[a](http://a) and [b](http://b)");
    REQUIRE(segments.size() == 3);
    REQUIRE(segments[0].type == DescriptionSegmentType::kLink);
    REQUIRE(segments[0].text == "a");
    REQUIRE(segments[1].type == DescriptionSegmentType::kText);
    REQUIRE(segments[1].text == " and ");
    REQUIRE(segments[2].type == DescriptionSegmentType::kLink);
    REQUIRE(segments[2].text == "b");
}

TEST_CASE("RichDescriptionRenderer plain_text strips markup", "[settings][renderer]")
{
    auto text = RichDescriptionRenderer::plain_text("See [docs](http://x) and use `code`.");
    REQUIRE(text == "See docs and use code.");
}

// ============================================================================
// SettingsCompareModel Tests
// ============================================================================

TEST_CASE("SettingsCompareModel empty state", "[settings][compare]")
{
    SettingsCompareModel model;
    REQUIRE(model.total_count() == 0);
    REQUIRE(model.diff_count() == 0);
    REQUIRE(model.summary_text() == "No differences found");
}

TEST_CASE("SettingsCompareModel same settings", "[settings][compare]")
{
    SettingsCompareModel model;
    std::vector<std::pair<std::string, std::string>> left = {{"editor.fontSize", "14"},
                                                             {"editor.tabSize", "4"}};
    std::vector<std::pair<std::string, std::string>> right = {{"editor.fontSize", "14"},
                                                              {"editor.tabSize", "4"}};
    model.compute(left, right);
    REQUIRE(model.total_count() == 2);
    REQUIRE(model.diff_count() == 0);
    REQUIRE(model.summary_text() == "No differences found");
}

TEST_CASE("SettingsCompareModel different values", "[settings][compare]")
{
    SettingsCompareModel model;
    std::vector<std::pair<std::string, std::string>> left = {{"editor.fontSize", "14"},
                                                             {"editor.tabSize", "4"}};
    std::vector<std::pair<std::string, std::string>> right = {{"editor.fontSize", "16"},
                                                              {"editor.tabSize", "4"}};
    model.compute(left, right);
    REQUIRE(model.diff_count() == 1);

    auto diffs = model.different_entries();
    REQUIRE(diffs.size() == 1);
    REQUIRE(diffs[0].setting_id == "editor.fontSize");
    REQUIRE(diffs[0].left_value == "14");
    REQUIRE(diffs[0].right_value == "16");
    REQUIRE(diffs[0].status == SettingCompareStatus::kDifferent);
}

TEST_CASE("SettingsCompareModel only in left scope", "[settings][compare]")
{
    SettingsCompareModel model;
    std::vector<std::pair<std::string, std::string>> left = {{"editor.fontSize", "14"},
                                                             {"editor.lineHeight", "1.5"}};
    std::vector<std::pair<std::string, std::string>> right = {{"editor.fontSize", "14"}};
    model.compute(left, right);
    REQUIRE(model.diff_count() == 1);

    auto diffs = model.different_entries();
    REQUIRE(diffs[0].setting_id == "editor.lineHeight");
    REQUIRE(diffs[0].status == SettingCompareStatus::kOnlyInLeft);
}

TEST_CASE("SettingsCompareModel only in right scope", "[settings][compare]")
{
    SettingsCompareModel model;
    std::vector<std::pair<std::string, std::string>> left = {{"editor.fontSize", "14"}};
    std::vector<std::pair<std::string, std::string>> right = {{"editor.fontSize", "14"},
                                                              {"editor.wordWrap", "true"}};
    model.compute(left, right);
    REQUIRE(model.diff_count() == 1);

    auto diffs = model.different_entries();
    REQUIRE(diffs[0].setting_id == "editor.wordWrap");
    REQUIRE(diffs[0].status == SettingCompareStatus::kOnlyInRight);
}

TEST_CASE("SettingsCompareModel multiple differences", "[settings][compare]")
{
    SettingsCompareModel model;
    std::vector<std::pair<std::string, std::string>> left = {{"a", "1"}, {"b", "2"}, {"c", "3"}};
    std::vector<std::pair<std::string, std::string>> right = {{"a", "1"}, {"b", "99"}, {"d", "4"}};
    model.compute(left, right);

    REQUIRE(model.total_count() == 4); // a, b, c, d
    REQUIRE(model.diff_count() == 3);  // b different, c only-left, d only-right
    REQUIRE(model.summary_text() == "3 differences found");
}

TEST_CASE("SettingsCompareModel clear", "[settings][compare]")
{
    SettingsCompareModel model;
    model.compute({{"a", "1"}}, {{"a", "2"}});
    model.clear();
    REQUIRE(model.total_count() == 0);
    REQUIRE(model.diff_count() == 0);
}

TEST_CASE("SettingsCompareModel singular difference text", "[settings][compare]")
{
    SettingsCompareModel model;
    model.compute({{"a", "1"}}, {{"a", "2"}});
    REQUIRE(model.summary_text() == "1 difference found");
}
