// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ExtensionBrowserModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_extensions() -> ExtensionBrowserModel
{
    ExtensionBrowserModel model;
    model.set_extensions({
        {"ext.mermaid", "Mermaid Preview", "MarkAmp", "1.0.0", ExtensionState::kEnabled, false},
        {"ext.vim", "Vim Mode", "Community", "2.1.0", ExtensionState::kDisabled, false},
        {"ext.lint", "Linter", "MarkAmp", "1.5.0", ExtensionState::kUpdateAvailable, false},
        {"ext.theme", "Dark Pro Theme", "Themes", "3.0.0", ExtensionState::kInstalled, false},
        {"ext.new", "New Plugin", "Author", "0.1.0", ExtensionState::kNotInstalled, false},
    });
    return model;
}

TEST_CASE("Filter enabled only", "[extensions][filter]")
{
    auto model = make_test_extensions();
    model.set_filter(ExtensionFilter::kEnabled);
    REQUIRE(model.filtered().size() == 1);
}

TEST_CASE("Filter updates available", "[extensions][filter]")
{
    auto model = make_test_extensions();
    model.set_filter(ExtensionFilter::kUpdatesAvailable);
    REQUIRE(model.filtered().size() == 1);
    REQUIRE(model.filtered()[0].name == "Linter");
}

TEST_CASE("Actions for enabled state", "[extensions][actions]")
{
    const auto actions = ExtensionBrowserModel::actions_for_state(ExtensionState::kEnabled);
    REQUIRE(actions.size() == 2);
    REQUIRE(actions[0] == "Disable");
}

TEST_CASE("Actions for not-installed state", "[extensions][actions]")
{
    const auto actions = ExtensionBrowserModel::actions_for_state(ExtensionState::kNotInstalled);
    REQUIRE(actions.size() == 1);
    REQUIRE(actions[0] == "Install");
}

TEST_CASE("In-progress state has no actions", "[extensions][actions]")
{
    REQUIRE(ExtensionBrowserModel::actions_for_state(ExtensionState::kInstalling).empty());
}

TEST_CASE("Bulk selection toggle", "[extensions][bulk]")
{
    auto model = make_test_extensions();
    model.toggle_selection("ext.mermaid");
    model.toggle_selection("ext.vim");
    REQUIRE(model.selected_count() == 2);
    model.toggle_selection("ext.mermaid");
    REQUIRE(model.selected_count() == 1);
}

TEST_CASE("Select all visible", "[extensions][bulk]")
{
    auto model = make_test_extensions();
    model.set_filter(ExtensionFilter::kAll);
    model.select_all_visible();
    REQUIRE(model.selected_count() == 5);
    model.clear_selection();
    REQUIRE(model.selected_count() == 0);
}

TEST_CASE("Count by state", "[extensions][count]")
{
    auto model = make_test_extensions();
    REQUIRE(model.count_by_state(ExtensionState::kEnabled) == 1);
    REQUIRE(model.count_by_state(ExtensionState::kNotInstalled) == 1);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
