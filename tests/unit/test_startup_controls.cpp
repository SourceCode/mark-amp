// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/StartupModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_startup() -> StartupModel
{
    StartupModel model;
    model.set_actions({
        {"new_file", "New File", "Cmd+N", "📄", 0},
        {"open_folder", "Open Folder", "Cmd+O", "📁", 1},
        {"reopen", "Reopen Last Workspace", "", "🔄", 2},
    });
    model.set_presets({
        {"writing", "Writing", "Focused writing layout", "writing"},
        {"canvas", "Canvas", "Mind map and visual layout", "canvas"},
        {"review", "Code Review", "Side-by-side diff layout", "review"},
    });
    model.set_recents({
        {"/path/project-a", "Project A", false, 10},
        {"/path/project-b", "Project B", true, 5},
        {"/path/project-c", "Project C", false, 2},
    });
    return model;
}

// ── Phase 20 Task 1: Quick actions ──────────────────────────────────

TEST_CASE("Actions sort by priority", "[startup][actions]")
{
    auto model = make_test_startup();
    const auto actions = model.sorted_actions();
    REQUIRE(actions[0].action_id == "new_file");
    REQUIRE(actions[2].action_id == "reopen");
}

// ── Phase 20 Task 2: Presets ────────────────────────────────────────

TEST_CASE("Preset lookup by ID", "[startup][preset]")
{
    auto model = make_test_startup();
    const auto* preset = model.preset_by_id("canvas");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->layout_mode == "canvas");
}

TEST_CASE("Preset lookup unknown returns nullptr", "[startup][preset]")
{
    auto model = make_test_startup();
    REQUIRE(model.preset_by_id("unknown") == nullptr);
}

// ── Phase 20 Task 3: Recents ────────────────────────────────────────

TEST_CASE("Recents sorted: pinned first, then by open count", "[startup][recents]")
{
    auto model = make_test_startup();
    const auto recents = model.sorted_recents();
    REQUIRE(recents[0].display_name == "Project B"); // pinned
    REQUIRE(recents[1].display_name == "Project A"); // 10 opens
}

TEST_CASE("Toggle pin", "[startup][recents]")
{
    auto model = make_test_startup();
    model.toggle_pin("/path/project-a");
    const auto recents = model.sorted_recents();
    // Now both A and B are pinned; A has more opens
    REQUIRE(recents[0].display_name == "Project A");
}

TEST_CASE("Remove recent", "[startup][recents]")
{
    auto model = make_test_startup();
    model.remove_recent("/path/project-c");
    REQUIRE(model.recent_count() == 2);
}

TEST_CASE("Search recents", "[startup][recents]")
{
    auto model = make_test_startup();
    const auto results = model.search_recents("project a");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].display_name == "Project A");
}

// ── Phase 20 Task 4: First-run ──────────────────────────────────────

TEST_CASE("First-run flag", "[startup][firstrun]")
{
    StartupModel model;
    REQUIRE_FALSE(model.is_first_run());
    model.set_first_run(true);
    REQUIRE(model.is_first_run());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
