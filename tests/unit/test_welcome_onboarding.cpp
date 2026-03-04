/// @file test_welcome_onboarding.cpp
/// @brief Phase 48 — Unit tests for Welcome and Onboarding.

#include "ui/ChangelogPanel.h"
#include "ui/InteractiveTutorial.h"
#include "ui/RecentItemsList.h"
#include "ui/SpotlightOverlay.h"
#include "ui/TipOfTheDay.h"
#include "ui/WelcomeTab.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ============================================================================
// RecentItemsList
// ============================================================================

TEST_CASE("RecentItemsList: renders correct count", "[welcome_onboarding]")
{
    RecentItemsListModel list;
    list.set_items({
        {.path = "/a.md", .name = "A"},
        {.path = "/b.md", .name = "B"},
        {.path = "/c.md", .name = "C"},
    });

    REQUIRE(list.count() == 3);
}

TEST_CASE("RecentItemsList: filters by search query", "[welcome_onboarding]")
{
    RecentItemsListModel list;
    list.set_items({
        {.path = "/notes/readme.md", .name = "README"},
        {.path = "/docs/setup.md", .name = "Setup Guide"},
        {.path = "/docs/readme2.md", .name = "README2"},
    });

    list.filter("readme");
    REQUIRE(list.count() == 2);
}

// ============================================================================
// TipOfTheDay
// ============================================================================

TEST_CASE("TipOfTheDay: non-repeating selection", "[welcome_onboarding]")
{
    TipOfTheDayModel tips;
    tips.add_tip({.id = "tip1", .title = "Tip 1", .content = "Content 1"});
    tips.add_tip({.id = "tip2", .title = "Tip 2", .content = "Content 2"});

    auto* first = tips.tip_for_today();
    REQUIRE(first != nullptr);
    REQUIRE(first->id == "tip1");

    tips.mark_shown("tip1");
    auto* second = tips.tip_for_today();
    REQUIRE(second != nullptr);
    REQUIRE(second->id == "tip2"); // Should skip tip1
}

TEST_CASE("TipOfTheDay: cooldown cycles when all shown", "[welcome_onboarding]")
{
    TipOfTheDayModel tips;
    tips.add_tip({.id = "only", .title = "Only Tip"});

    tips.mark_shown("only");
    auto* tip = tips.tip_for_today();
    REQUIRE(tip != nullptr); // Should cycle back to first
}

// ============================================================================
// InteractiveTutorial
// ============================================================================

TEST_CASE("InteractiveTutorial: step advancement", "[welcome_onboarding]")
{
    InteractiveTutorialEngine engine;
    engine.add_tutorial({.id = "intro",
                         .title = "Introduction",
                         .steps = {{.id = "s1", .title = "Step 1"},
                                   {.id = "s2", .title = "Step 2"},
                                   {.id = "s3", .title = "Step 3"}}});

    engine.start("intro");
    REQUIRE(engine.is_active());

    auto* step = engine.current_step();
    REQUIRE(step != nullptr);
    REQUIRE(step->id == "s1");

    engine.advance();
    step = engine.current_step();
    REQUIRE(step != nullptr);
    REQUIRE(step->id == "s2");
}

TEST_CASE("InteractiveTutorial: completion tracking", "[welcome_onboarding]")
{
    InteractiveTutorialEngine engine;
    engine.add_tutorial({.id = "t1", .steps = {{.id = "s1"}, {.id = "s2"}}});

    engine.start("t1");
    engine.advance(); // s1 completed
    engine.advance(); // s2 completed, tutorial done

    REQUIRE_FALSE(engine.is_active());
    REQUIRE(engine.completed_tutorial_count() == 1);
}

// ============================================================================
// ChangelogPanel
// ============================================================================

TEST_CASE("ChangelogPanel: parses version sections", "[welcome_onboarding]")
{
    ChangelogPanelModel changelog;
    changelog.parse("## [2.0.0] - 2024-01-15\n"
                    "- Added new diff editor\n"
                    "- Fix: resolved crash on startup\n"
                    "\n"
                    "## [1.9.0] - 2024-01-01\n"
                    "- Improved performance\n");

    REQUIRE(changelog.version_count() == 2);
    REQUIRE(changelog.versions()[0].version == "2.0.0");
    REQUIRE(changelog.versions()[0].entries.size() == 2);
    REQUIRE(changelog.versions()[1].version == "1.9.0");
}

// ============================================================================
// WelcomeTab
// ============================================================================

TEST_CASE("WelcomeTab: show on startup toggle", "[welcome_onboarding]")
{
    WelcomeTabModel model;
    REQUIRE(model.show_on_startup()); // Default is true.

    model.set_show_on_startup(false);
    REQUIRE_FALSE(model.show_on_startup());
}

TEST_CASE("WalkthroughCard: completion percentage", "[welcome_onboarding]")
{
    WalkthroughCard card{.total_steps = 10, .completed_steps = 7};
    REQUIRE(card.completion_percentage() == 70);
}
