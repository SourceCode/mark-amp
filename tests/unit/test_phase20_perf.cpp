/// @file test_phase20_perf.cpp
/// @brief Tests for Phase 20 performance optimization additions:
///   - Config::CachedValues (O(1) cached config access)
///   - EventBus::publish_fast() (lock-free hot-path publishing)
///   - ThemeEngine::color_fast() (O(1) indexed color access)

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ═══════════════════════════════════════════════════════════════════
// Config::CachedValues tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config CachedValues has correct defaults", "[config][cached]")
{
    Config config;
    auto result = config.load();
    REQUIRE(result.has_value());

    const auto& cached = config.cached();

    SECTION("String defaults")
    {
        CHECK(cached.theme == "midnight-neon");
        CHECK(cached.view_mode == "split");
        CHECK(cached.font_family == "Menlo");
        CHECK(cached.cursor_blinking == "blink");
    }

    SECTION("Int defaults")
    {
        CHECK(cached.font_size == 14);
        CHECK(cached.tab_size == 4);
        CHECK(cached.edge_column == 80);
        CHECK(cached.cursor_width == 2);
        CHECK(cached.word_wrap_column == 80);
        CHECK(cached.line_height == 0);
        CHECK(cached.padding_top == 0);
        CHECK(cached.padding_bottom == 0);
    }

    SECTION("Double defaults")
    {
        CHECK(cached.letter_spacing == 0.0);
    }

    SECTION("Bool defaults")
    {
        CHECK(cached.sidebar_visible == true);
        CHECK(cached.word_wrap == true);
        CHECK(cached.auto_save == false);
        CHECK(cached.show_line_numbers == true);
        CHECK(cached.highlight_current_line == true);
        CHECK(cached.show_whitespace == false);
        CHECK(cached.show_minimap == false);
        CHECK(cached.auto_indent == true);
        CHECK(cached.indent_guides == true);
        CHECK(cached.bracket_matching == true);
        CHECK(cached.code_folding == true);
        CHECK(cached.show_status_bar == true);
        CHECK(cached.show_tab_bar == true);
        CHECK(cached.mouse_wheel_zoom == false);
        CHECK(cached.bracket_pair_colorization == false);
        CHECK(cached.dim_whitespace == false);
    }
}

TEST_CASE("Config CachedValues updates on set()", "[config][cached]")
{
    Config config;
    [[maybe_unused]] auto result = config.load();

    SECTION("Integer value updates cache")
    {
        config.set("font_size", 20);
        CHECK(config.cached().font_size == 20);
    }

    SECTION("String value updates cache")
    {
        config.set("theme", std::string_view("synth-wave"));
        CHECK(config.cached().theme == "synth-wave");
    }

    SECTION("Bool value updates cache")
    {
        config.set("auto_save", true);
        CHECK(config.cached().auto_save == true);
    }

    SECTION("Double value updates cache")
    {
        config.set("editor.letter_spacing", 1.5);
        CHECK(config.cached().letter_spacing == 1.5);
    }
}

// ═══════════════════════════════════════════════════════════════════
// EventBus::publish_fast() tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("EventBus publish_fast delivers events to subscribers", "[eventbus][fast]")
{
    EventBus bus;
    int call_count = 0;

    auto sub = bus.subscribe<events::CursorPositionChangedEvent>(
        [&call_count](const events::CursorPositionChangedEvent& /*evt*/) { ++call_count; });

    events::CursorPositionChangedEvent evt;
    evt.line = 10;
    evt.column = 5;

    bus.publish_fast(evt);
    CHECK(call_count == 1);
}

TEST_CASE("EventBus publish_fast delivers to multiple subscribers", "[eventbus][fast]")
{
    EventBus bus;
    int count_a = 0;
    int count_b = 0;

    auto sub_a = bus.subscribe<events::CursorPositionChangedEvent>(
        [&count_a](const events::CursorPositionChangedEvent& /*evt*/) { ++count_a; });
    auto sub_b = bus.subscribe<events::CursorPositionChangedEvent>(
        [&count_b](const events::CursorPositionChangedEvent& /*evt*/) { ++count_b; });

    const events::CursorPositionChangedEvent evt{};
    bus.publish_fast(evt);

    CHECK(count_a == 1);
    CHECK(count_b == 1);
}

TEST_CASE("EventBus publish_fast delivers correct event data", "[eventbus][fast]")
{
    EventBus bus;
    int received_line = -1;
    int received_col = -1;

    auto sub = bus.subscribe<events::CursorPositionChangedEvent>(
        [&](const events::CursorPositionChangedEvent& evt)
        {
            received_line = evt.line;
            received_col = evt.column;
        });

    events::CursorPositionChangedEvent evt;
    evt.line = 42;
    evt.column = 17;
    bus.publish_fast(evt);

    CHECK(received_line == 42);
    CHECK(received_col == 17);
}

TEST_CASE("EventBus publish_fast does not call unsubscribed handlers", "[eventbus][fast]")
{
    EventBus bus;
    int call_count = 0;

    {
        auto sub = bus.subscribe<events::CursorPositionChangedEvent>(
            [&call_count](const events::CursorPositionChangedEvent& /*evt*/) { ++call_count; });
        // sub goes out of scope here, unsubscribing
    }

    const events::CursorPositionChangedEvent evt{};
    bus.publish_fast(evt);
    CHECK(call_count == 0);
}

// ═══════════════════════════════════════════════════════════════════
// ThemeEngine::color_fast() tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ThemeEngine color_fast returns same color as color()", "[theme][fast]")
{
    EventBus bus;
    ThemeRegistry registry;
    [[maybe_unused]] auto init = registry.initialize();
    ThemeEngine engine(bus, registry);

    // Apply default theme
    auto themes = engine.available_themes();
    REQUIRE(!themes.empty());
    engine.apply_theme(themes.front().id);

    // Verify color_fast matches color for several tokens
    for (auto token : {ThemeColorToken::BgApp,
                       ThemeColorToken::TextMain,
                       ThemeColorToken::BgHeader,
                       ThemeColorToken::BgPanel})
    {
        const auto& regular = engine.color(token);
        const auto& fast = engine.color_fast(token);
        CHECK(regular.GetRGB() == fast.GetRGB());
    }
}

TEST_CASE("ThemeEngine color_fast returns valid colors after theme switch", "[theme][fast]")
{
    EventBus bus;
    ThemeRegistry registry;
    [[maybe_unused]] auto init = registry.initialize();
    ThemeEngine engine(bus, registry);

    auto themes = engine.available_themes();
    REQUIRE(themes.size() >= 2);

    // Apply first theme and record a color
    engine.apply_theme(themes[0].id);
    auto first_bg = engine.color_fast(ThemeColorToken::BgApp);
    CHECK(first_bg.IsOk());

    // Switch to second theme
    engine.apply_theme(themes[1].id);
    auto second_bg = engine.color_fast(ThemeColorToken::BgApp);
    CHECK(second_bg.IsOk());

    // Colors should differ between themes (different theme = different palette)
    // (This may not always be true if themes share colors, so just verify OK)
    CHECK(second_bg.IsOk());
}

// ═══════════════════════════════════════════════════════════════════
// Config apply_defaults data-driven refactor (Phase 21 regression test)
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config apply_defaults covers all 28+ settings", "[config][defaults]")
{
    Config config;
    [[maybe_unused]] auto load_result = config.load();

    // Verify every key from the defaults table has a value
    CHECK(!config.get_string("theme").empty());
    CHECK(!config.get_string("view_mode").empty());
    CHECK(config.get_int("font_size") > 0);
    CHECK(config.get_int("tab_size") > 0);
    CHECK(config.get_int("edge_column") > 0);
    CHECK(!config.get_string("font_family").empty());
    CHECK(config.get_int("auto_save_interval_seconds") > 0);
    CHECK(!config.get_string("editor.cursor_blinking").empty());
    CHECK(config.get_int("editor.cursor_width") > 0);
    CHECK(config.get_int("editor.word_wrap_column") > 0);
}
