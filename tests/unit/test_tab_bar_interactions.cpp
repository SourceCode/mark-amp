// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/TabStripModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_strip() -> TabStripModel
{
    TabStripModel model;
    model.add_tab({"main.cpp", "main.cpp", TabIndicator::kClean, false, true, std::nullopt});
    model.add_tab({"utils.h", "utils.h", TabIndicator::kModified, false, false, std::nullopt});
    model.add_tab({"test.cpp",
                   "test.cpp",
                   TabIndicator::kClean,
                   false,
                   false,
                   TabGroupInfo{"tests", "Tests", 1}});
    model.add_tab({"bench.cpp",
                   "bench.cpp",
                   TabIndicator::kClean,
                   false,
                   false,
                   TabGroupInfo{"tests", "Tests", 1}});
    return model;
}

// ── Phase 08 Task 1: Overflow ───────────────────────────────────────

TEST_CASE("Overflow detects tabs beyond viewport", "[tabstrip][overflow]")
{
    auto model = make_test_strip();
    // 4 tabs × 100px = 400px, viewport = 250px → 2 overflowed
    const auto overflow = model.overflowed_indices(250, 100);
    REQUIRE(overflow.size() == 2);
}

TEST_CASE("Tab names list is searchable", "[tabstrip][overflow]")
{
    auto model = make_test_strip();
    const auto names = model.tab_names();
    REQUIRE(names.size() == 4);
    REQUIRE(names[0] == "main.cpp");
}

// ── Phase 08 Task 2: Tab groups ─────────────────────────────────────

TEST_CASE("Close group removes grouped tabs", "[tabstrip][groups]")
{
    auto model = make_test_strip();
    model.close_group("tests");
    REQUIRE(model.tab_count() == 2);
}

TEST_CASE("Pin group pins all tabs in group", "[tabstrip][groups]")
{
    auto model = make_test_strip();
    model.pin_group("tests");
    const auto& tabs = model.tabs();
    REQUIRE(tabs[2].is_pinned);
    REQUIRE(tabs[3].is_pinned);
}

// ── Phase 08 Task 3: Reorder ────────────────────────────────────────

TEST_CASE("Reorder moves tab to new position", "[tabstrip][reorder]")
{
    auto model = make_test_strip();
    REQUIRE(model.reorder(0, 2));
    REQUIRE(model.tabs()[0].file_path == "utils.h");
}

TEST_CASE("Cannot reorder across pinned boundary", "[tabstrip][reorder]")
{
    auto model = make_test_strip();
    model.pin("main.cpp");
    // main.cpp is pinned (index 0), utils.h is not (index 1) → blocked
    REQUIRE_FALSE(model.reorder(0, 1));
}

// ── Phase 08 Task 4: Indicators ─────────────────────────────────────

TEST_CASE("Indicator tooltips are distinct", "[tabstrip][indicators]")
{
    REQUIRE(TabStripModel::indicator_tooltip(TabIndicator::kClean) != "");
    REQUIRE(TabStripModel::indicator_tooltip(TabIndicator::kModified) !=
            TabStripModel::indicator_tooltip(TabIndicator::kClean));
    REQUIRE(TabStripModel::indicator_tooltip(TabIndicator::kConflict).find("Conflict") !=
            std::string::npos);
}

TEST_CASE("Indicator can be set per tab", "[tabstrip][indicators]")
{
    auto model = make_test_strip();
    model.set_indicator("main.cpp", TabIndicator::kConflict);
    REQUIRE(model.tabs()[0].indicator == TabIndicator::kConflict);
}

// ── Pinning ─────────────────────────────────────────────────────────

TEST_CASE("Pin and unpin toggles correctly", "[tabstrip][pin]")
{
    auto model = make_test_strip();
    model.pin("utils.h");
    REQUIRE(model.tabs()[1].is_pinned);
    model.unpin("utils.h");
    REQUIRE_FALSE(model.tabs()[1].is_pinned);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
