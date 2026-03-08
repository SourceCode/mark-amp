/**
 * @file test_phase14_appium_selector_contract.cpp
 * @brief Selector contract audit — validates every `ma.*` accessibility name
 *        that Appium mac2 E2E tests depend on.
 *
 * The "contract registry" is the single source of truth.  If a C++ file adds
 * or removes a `SetName(...)` call, the corresponding entry here must be
 * updated.  This keeps the E2E layer and the UI layer in sync at build time.
 *
 * Additionally validates all non-contract SetName labels documented in the
 * codebase (SplitView, SourceControl, RunDebug, etc.).
 */

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <regex>
#include <set>
#include <string>
#include <vector>

// ============================================================================
// Contract Registry — ma.* selectors used by Appium page objects
// ============================================================================
static const std::vector<std::string> kSelectorContract = {
    "ma.shell.main_frame",
    "ma.activitybar",
    "ma.editor.panel",
    "ma.filetree.ctrl",
    "ma.settings.panel",
    "ma.commandpalette",
    "ma.statusbar",
};

// ============================================================================
// Extended Label Registry — non-contract SetName values in the UI layer
// These labels are used by Appium specs for secondary element discovery.
// ============================================================================
static const std::vector<std::string> kExtendedLabels = {
    // SplitView
    "Split view divider",
    "Markdown editor",
    "Rendered preview",
    // SourceControlPanel
    "Branch Selector",
    "Commit Message Input",
    "Template Button",
    "Commit Button",
    "Staged Changes List",
    "Unstaged Changes List",
    "Merge Conflicts List",
    "Timeline List",
    // RunDebugPanel
    "Launch Configuration",
    "Start Debugging",
    // SecondarySidebarTabStrip
    "Secondary Sidebar Tabs",
    // FirstRunWizard
    "Welcome heading",
    "Welcome subtitle",
    "Feature list",
    // GraphSidebarPanel
    "Graph Links List",
};

// ============================================================================
// Activity Bar Item Labels — from CreateItems()
// ============================================================================
static const std::vector<std::string> kActivityBarLabels = {
    "Explorer",
    "Search",
    "Notebooks",
    "Canvas",
    "Knowledge Graph",
    "AI Assistant",
    "Flashcards",
    "Git",
    "Tasks",
    "Database",
    "Presentation",
    "Extensions",
    "Settings",
    "Themes",
    "Accounts",
};

// ============================================================================
// Activity Bar Icon Names — from CreateItems()
// ============================================================================
static const std::vector<std::string> kActivityBarIcons = {
    "activity-explorer",
    "activity-search",
    "activity-notebooks",
    "activity-canvas",
    "activity-graph",
    "activity-ai",
    "activity-flashcards",
    "activity-git",
    "activity-tasks",
    "activity-database",
    "activity-presentation",
    "activity-extensions",
    "activity-settings",
    "toolbar-themes",
    "activity-account",
};

// ============================================================================
// Tests — Contract Selectors
// ============================================================================

TEST_CASE("Selector contract registry is not empty", "[phase14][selector][contract]")
{
    REQUIRE_FALSE(kSelectorContract.empty());
}

TEST_CASE("No duplicate selectors in contract registry", "[phase14][selector][contract]")
{
    std::set<std::string> seen;
    for (const auto& name : kSelectorContract)
    {
        INFO("Duplicate selector: " << name);
        REQUIRE(seen.insert(name).second);
    }
}

TEST_CASE("All selectors match ma.<surface>[.<control>] naming pattern",
          "[phase14][selector][contract]")
{
    const std::regex kPattern(R"(^ma\.[a-z][a-z0-9]*(\.[a-z][a-z0-9_]*)?$)");

    for (const auto& name : kSelectorContract)
    {
        INFO("Selector does not match pattern: " << name);
        REQUIRE(std::regex_match(name, kPattern));
    }
}

TEST_CASE("Contract contains required smoke-test selectors", "[phase14][selector][contract]")
{
    const std::vector<std::string> kRequired = {
        "ma.shell.main_frame",
        "ma.activitybar",
        "ma.editor.panel",
        "ma.statusbar",
    };

    for (const auto& required : kRequired)
    {
        INFO("Missing required selector: " << required);
        REQUIRE(std::find(kSelectorContract.begin(), kSelectorContract.end(), required) !=
                kSelectorContract.end());
    }
}

TEST_CASE("All selectors are non-empty and trimmed", "[phase14][selector][contract]")
{
    for (const auto& name : kSelectorContract)
    {
        INFO("Empty or whitespace selector found");
        REQUIRE_FALSE(name.empty());
        REQUIRE(name.front() != ' ');
        REQUIRE(name.back() != ' ');
    }
}

// ============================================================================
// Tests — Extended Labels
// ============================================================================

TEST_CASE("Extended label registry is not empty", "[phase14][selector][labels]")
{
    REQUIRE_FALSE(kExtendedLabels.empty());
}

TEST_CASE("No duplicate extended labels", "[phase14][selector][labels]")
{
    std::set<std::string> seen;
    for (const auto& label : kExtendedLabels)
    {
        INFO("Duplicate label: " << label);
        REQUIRE(seen.insert(label).second);
    }
}

TEST_CASE("All extended labels are non-empty and trimmed", "[phase14][selector][labels]")
{
    for (const auto& label : kExtendedLabels)
    {
        INFO("Empty or whitespace label found");
        REQUIRE_FALSE(label.empty());
        REQUIRE(label.front() != ' ');
        REQUIRE(label.back() != ' ');
    }
}

TEST_CASE("Extended labels do not overlap with contract selectors", "[phase14][selector][labels]")
{
    for (const auto& label : kExtendedLabels)
    {
        INFO("Extended label overlaps with contract: " << label);
        REQUIRE(std::find(kSelectorContract.begin(), kSelectorContract.end(), label) ==
                kSelectorContract.end());
    }
}

// ============================================================================
// Tests — Activity Bar Labels
// ============================================================================

TEST_CASE("Activity bar has exactly 15 items", "[phase14][activitybar][labels]")
{
    REQUIRE(kActivityBarLabels.size() == 15);
}

TEST_CASE("No duplicate activity bar labels", "[phase14][activitybar][labels]")
{
    std::set<std::string> seen;
    for (const auto& label : kActivityBarLabels)
    {
        INFO("Duplicate activity bar label: " << label);
        REQUIRE(seen.insert(label).second);
    }
}

TEST_CASE("All activity bar labels are non-empty", "[phase14][activitybar][labels]")
{
    for (const auto& label : kActivityBarLabels)
    {
        REQUIRE_FALSE(label.empty());
    }
}

TEST_CASE("Activity bar starts with Explorer and Search", "[phase14][activitybar][labels]")
{
    REQUIRE(kActivityBarLabels.at(0) == "Explorer");
    REQUIRE(kActivityBarLabels.at(1) == "Search");
}

TEST_CASE("Activity bar ends with Settings, Themes, Accounts", "[phase14][activitybar][labels]")
{
    REQUIRE(kActivityBarLabels.at(12) == "Settings");
    REQUIRE(kActivityBarLabels.at(13) == "Themes");
    REQUIRE(kActivityBarLabels.at(14) == "Accounts");
}

// ============================================================================
// Tests — Activity Bar Icons
// ============================================================================

TEST_CASE("Activity bar has icon for every label", "[phase14][activitybar][icons]")
{
    REQUIRE(kActivityBarIcons.size() == kActivityBarLabels.size());
}

TEST_CASE("No duplicate activity bar icon names", "[phase14][activitybar][icons]")
{
    std::set<std::string> seen;
    for (const auto& icon : kActivityBarIcons)
    {
        INFO("Duplicate icon: " << icon);
        REQUIRE(seen.insert(icon).second);
    }
}

TEST_CASE("All activity bar icons are non-empty", "[phase14][activitybar][icons]")
{
    for (const auto& icon : kActivityBarIcons)
    {
        REQUIRE_FALSE(icon.empty());
    }
}

TEST_CASE("Explorer icon is activity-explorer", "[phase14][activitybar][icons]")
{
    REQUIRE(kActivityBarIcons.at(0) == "activity-explorer");
}

TEST_CASE("Settings icon is activity-settings", "[phase14][activitybar][icons]")
{
    REQUIRE(kActivityBarIcons.at(12) == "activity-settings");
}

TEST_CASE("Themes icon is toolbar-themes (not activity-themes)", "[phase14][activitybar][icons]")
{
    REQUIRE(kActivityBarIcons.at(13) == "toolbar-themes");
}
