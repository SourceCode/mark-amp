// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/PreviewControlModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Default sync mode is follow cursor", "[preview][sync]")
{
    PreviewControlModel model;
    REQUIRE(model.sync_mode() == PreviewSyncMode::kFollowCursor);
    REQUIRE(model.sync_mode_label() == "Follow Cursor");
}

TEST_CASE("Cycle sync mode: cursor -> heading -> locked -> cursor", "[preview][sync]")
{
    PreviewControlModel model;
    model.cycle_sync_mode();
    REQUIRE(model.sync_mode() == PreviewSyncMode::kFollowHeading);
    REQUIRE(model.sync_mode_label() == "Follow Heading");

    model.cycle_sync_mode();
    REQUIRE(model.sync_mode() == PreviewSyncMode::kLocked);
    REQUIRE(model.sync_mode_label() == "Locked");

    model.cycle_sync_mode();
    REQUIRE(model.sync_mode() == PreviewSyncMode::kFollowCursor);
}

TEST_CASE("Pin state", "[preview][pin]")
{
    PreviewControlModel model;
    REQUIRE_FALSE(model.is_pinned());
    model.set_pinned(true);
    REQUIRE(model.is_pinned());
}

TEST_CASE("Live refresh default and toggle", "[preview][refresh]")
{
    PreviewControlModel model;
    REQUIRE(model.live_refresh());
    model.set_live_refresh(false);
    REQUIRE_FALSE(model.live_refresh());
}

TEST_CASE("Stale flag", "[preview][stale]")
{
    PreviewControlModel model;
    REQUIRE_FALSE(model.is_stale());
    model.set_stale(true);
    REQUIRE(model.is_stale());
}

TEST_CASE("Scroll anchor", "[preview][anchor]")
{
    PreviewControlModel model;
    model.set_scroll_anchor(ScrollAnchor::kHeading);
    REQUIRE(model.scroll_anchor() == ScrollAnchor::kHeading);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
