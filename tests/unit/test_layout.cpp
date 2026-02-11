#include "core/BuiltinThemes.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "ui/LayoutManager.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>

using namespace markamp::core;
using namespace markamp::ui;

TEST_CASE("LayoutManager: default sidebar width", "[layout]")
{
    REQUIRE(LayoutManager::kDefaultSidebarWidth == 256);
    REQUIRE(LayoutManager::kMinSidebarWidth == 180);
    REQUIRE(LayoutManager::kMaxSidebarWidth == 400);
    REQUIRE(LayoutManager::kStatusBarHeight == 24);
}

TEST_CASE("LayoutManager: sidebar width clamping", "[layout]")
{
    // Test the clamped range: std::clamp(width, 180, 400)
    REQUIRE(std::clamp(100, LayoutManager::kMinSidebarWidth, LayoutManager::kMaxSidebarWidth) ==
            180);
    REQUIRE(std::clamp(256, LayoutManager::kMinSidebarWidth, LayoutManager::kMaxSidebarWidth) ==
            256);
    REQUIRE(std::clamp(500, LayoutManager::kMinSidebarWidth, LayoutManager::kMaxSidebarWidth) ==
            400);
}

TEST_CASE("LayoutManager: SidebarToggleEvent is defined", "[layout]")
{
    events::SidebarToggleEvent evt;
    evt.visible = false;
    REQUIRE_FALSE(evt.visible);
    REQUIRE(evt.type_name() == "SidebarToggleEvent");
}

TEST_CASE("LayoutManager: toggle event round-trip via EventBus", "[layout]")
{
    EventBus bus;
    bool received = false;
    bool received_visible = false;

    auto sub = bus.subscribe<events::SidebarToggleEvent>(
        [&](const events::SidebarToggleEvent& e)
        {
            received = true;
            received_visible = e.visible;
        });

    events::SidebarToggleEvent evt;
    evt.visible = true;
    bus.publish(evt);

    REQUIRE(received);
    REQUIRE(received_visible);
}

TEST_CASE("LayoutManager: layout persistence keys in Config", "[layout]")
{
    Config config;
    // Don't load from disk, just test getter defaults
    REQUIRE(config.get_bool("layout.sidebar_visible", true) == true);
    REQUIRE(config.get_int("layout.sidebar_width", 256) == 256);
}

TEST_CASE("LayoutManager: sidebar width persistence read-back", "[layout]")
{
    Config config;
    config.set("layout.sidebar_visible", false);
    config.set("layout.sidebar_width", 300);

    REQUIRE_FALSE(config.get_bool("layout.sidebar_visible", true));
    REQUIRE(config.get_int("layout.sidebar_width", 256) == 300);
}
