// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/SettingsNavModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_nav() -> SettingsNavModel
{
    SettingsNavModel model;
    model.set_entries({
        {"editor.fontSize",
         "Font Size",
         "Controls the font size in pixels",
         "Editor",
         "Font",
         true,
         false,
         false,
         {"size", "text"}},
        {"editor.fontFamily",
         "Font Family",
         "Controls the font family",
         "Editor",
         "Font",
         false,
         false,
         false,
         {"typeface"}},
        {"editor.tabSize",
         "Tab Size",
         "The number of spaces per tab",
         "Editor",
         "Formatting",
         false,
         true,
         false,
         {"indent"}},
        {"appearance.theme",
         "Theme",
         "Select the color theme",
         "Appearance",
         "Colors",
         false,
         false,
         false,
         {"dark mode", "light mode"}},
        {"advanced.legacy",
         "Legacy Mode",
         "Use legacy rendering engine",
         "Advanced",
         "Engine",
         false,
         false,
         true,
         {}},
    });
    return model;
}

// ── Phase 11 Task 1: Hierarchical category tree ─────────────────────

TEST_CASE("Category tree groups by group+subgroup", "[settings-nav][tree]")
{
    auto model = make_test_nav();
    const auto tree = model.category_tree();
    // Editor/Font, Editor/Formatting, Appearance/Colors, Advanced/Engine = 4 nodes
    REQUIRE(tree.size() == 4);
}

TEST_CASE("Category tree counts settings per node", "[settings-nav][tree]")
{
    auto model = make_test_nav();
    const auto tree = model.category_tree();
    // Editor/Font has 2 settings (fontSize, fontFamily)
    bool found_font = false;
    for (const auto& node : tree)
    {
        if (node.group == "Editor" && node.subgroup == "Font")
        {
            REQUIRE(node.setting_count == 2);
            found_font = true;
        }
    }
    REQUIRE(found_font);
}

// ── Phase 11 Task 2: Search ─────────────────────────────────────────

TEST_CASE("Search matches on label", "[settings-nav][search]")
{
    auto model = make_test_nav();
    const auto results = model.search("Font Size");
    REQUIRE(!results.empty());
    REQUIRE(results[0].setting_id == "editor.fontSize");
}

TEST_CASE("Search matches keywords", "[settings-nav][search]")
{
    auto model = make_test_nav();
    const auto results = model.search("dark mode");
    REQUIRE(!results.empty());
    REQUIRE(results[0].setting_id == "appearance.theme");
}

TEST_CASE("Search returns empty for no match", "[settings-nav][search]")
{
    auto model = make_test_nav();
    const auto results = model.search("xyznonexistent");
    REQUIRE(results.empty());
}

// ── Phase 11 Task 3: Deep-link breadcrumb ───────────────────────────

TEST_CASE("Breadcrumb for valid setting", "[settings-nav][deeplink]")
{
    auto model = make_test_nav();
    const auto bc = model.breadcrumb_for("editor.tabSize");
    REQUIRE(bc.has_value());
    REQUIRE(bc->group == "Editor");
    REQUIRE(bc->subgroup == "Formatting");
}

TEST_CASE("Breadcrumb for unknown setting returns nullopt", "[settings-nav][deeplink]")
{
    auto model = make_test_nav();
    REQUIRE(!model.breadcrumb_for("nonexistent.setting").has_value());
}

// ── Phase 11 Task 4: Badges ─────────────────────────────────────────

TEST_CASE("Restart-required badge", "[settings-nav][badge]")
{
    auto model = make_test_nav();
    REQUIRE(model.badge_for("editor.fontSize") == SettingBadge::kRestartRequired);
}

TEST_CASE("Experimental badge", "[settings-nav][badge]")
{
    auto model = make_test_nav();
    REQUIRE(model.badge_for("editor.tabSize") == SettingBadge::kExperimental);
}

TEST_CASE("Deprecated badge takes priority", "[settings-nav][badge]")
{
    auto model = make_test_nav();
    REQUIRE(model.badge_for("advanced.legacy") == SettingBadge::kDeprecated);
}

TEST_CASE("Badge tooltips are non-empty", "[settings-nav][badge]")
{
    REQUIRE(!SettingsNavModel::badge_tooltip(SettingBadge::kRestartRequired).empty());
    REQUIRE(!SettingsNavModel::badge_tooltip(SettingBadge::kExperimental).empty());
    REQUIRE(SettingsNavModel::badge_tooltip(SettingBadge::kNone).empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
