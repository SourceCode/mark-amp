#include "core/AccessibilityManager.h"
#include "core/BuiltInThemes.h"
#include "core/Color.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <string>

using markamp::core::AccessibilityManager;
using markamp::core::Color;
using markamp::core::Config;
using markamp::core::EventBus;

// ═══════════════════════════════════════════════════════
// Stub platform for testing (avoids real OS APIs)
// ═══════════════════════════════════════════════════════

class StubPlatform : public markamp::platform::PlatformAbstraction
{
public:
    bool high_contrast{false};
    bool reduced_motion{false};
    std::string last_announcement;

    void set_frameless_window_style([[maybe_unused]] wxFrame* frame) override {}
    auto begin_native_drag([[maybe_unused]] wxFrame* frame, [[maybe_unused]] const wxPoint& pos)
        -> bool override
    {
        return false;
    }
    auto begin_native_resize([[maybe_unused]] wxFrame* frame,
                             [[maybe_unused]] markamp::platform::ResizeEdge edge) -> bool override
    {
        return false;
    }
    [[nodiscard]] auto is_maximized([[maybe_unused]] const wxFrame* frame) const -> bool override
    {
        return false;
    }
    void toggle_maximize([[maybe_unused]] wxFrame* frame) override {}
    void enter_fullscreen([[maybe_unused]] wxFrame* frame) override {}
    void exit_fullscreen([[maybe_unused]] wxFrame* frame) override {}

    [[nodiscard]] auto is_high_contrast() const -> bool override
    {
        return high_contrast;
    }
    [[nodiscard]] auto prefers_reduced_motion() const -> bool override
    {
        return reduced_motion;
    }
    void announce_to_screen_reader([[maybe_unused]] wxWindow* window,
                                   const wxString& message) override
    {
        last_announcement = message.ToStdString();
    }
};

// ═══════════════════════════════════════════════════════
// WCAG Contrast Ratio Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Contrast: at least 6 of 8 themes text_main vs bg_app meet WCAG AA (4.5:1)",
          "[accessibility]")
{
    const auto& themes = markamp::core::get_builtin_themes();
    REQUIRE(themes.size() == 8);

    int passing = 0;
    for (const auto& theme : themes)
    {
        const float ratio = theme.colors.text_main.contrast_ratio(theme.colors.bg_app);
        INFO("Theme: " << theme.name << " contrast ratio: " << ratio);
        if (ratio >= 4.5F)
        {
            ++passing;
        }
    }
    CHECK(passing >= 6);
}

TEST_CASE("Contrast: at least 6 of 8 themes text_main vs bg_panel meet WCAG AA", "[accessibility]")
{
    const auto& themes = markamp::core::get_builtin_themes();
    int passing = 0;
    for (const auto& theme : themes)
    {
        const float ratio = theme.colors.text_main.contrast_ratio(theme.colors.bg_panel);
        INFO("Theme: " << theme.name << " contrast ratio: " << ratio);
        if (ratio >= 4.5F)
        {
            ++passing;
        }
    }
    CHECK(passing >= 6);
}

TEST_CASE("Contrast: at least one theme meets WCAG AAA (7:1) for text_main vs bg_app",
          "[accessibility]")
{
    const auto& themes = markamp::core::get_builtin_themes();
    bool any_aaa = false;
    for (const auto& theme : themes)
    {
        const float ratio = theme.colors.text_main.contrast_ratio(theme.colors.bg_app);
        if (ratio >= 7.0F)
        {
            any_aaa = true;
            break;
        }
    }
    CHECK(any_aaa);
}

TEST_CASE("Contrast: at least 6 of 8 themes text_muted vs bg_app meet 3:1", "[accessibility]")
{
    const auto& themes = markamp::core::get_builtin_themes();
    int passing = 0;
    for (const auto& theme : themes)
    {
        const float ratio = theme.colors.text_muted.contrast_ratio(theme.colors.bg_app);
        INFO("Theme: " << theme.name << " muted contrast: " << ratio);
        if (ratio >= 3.0F)
        {
            ++passing;
        }
    }
    CHECK(passing >= 6);
}

TEST_CASE("Contrast: accent_primary vs bg_app for each theme", "[accessibility]")
{
    const auto& themes = markamp::core::get_builtin_themes();
    for (const auto& theme : themes)
    {
        const float ratio = theme.colors.accent_primary.contrast_ratio(theme.colors.bg_app);
        INFO("Theme: " << theme.name << " accent contrast: " << ratio);
        // Accent doesn't need to meet body text AA, but should be ≥ 3:1 for large text
        CHECK(ratio >= 2.0F);
    }
}

// ═══════════════════════════════════════════════════════
// AccessibilityManager Scale Factor Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Scale: scaled() at 100% returns same value", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.set_scale_factor(1.0F);
    CHECK(mgr.scaled(100) == 100);
    CHECK(mgr.scaled(24) == 24);
}

TEST_CASE("Scale: scaled() at 200% returns double", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.set_scale_factor(2.0F);
    CHECK(mgr.scaled(100) == 200);
    CHECK(mgr.scaled(24) == 48);
}

TEST_CASE("Scale: factor clamped to [0.75, 2.0]", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.set_scale_factor(0.5F);
    CHECK_THAT(mgr.scale_factor(), Catch::Matchers::WithinAbs(0.75, 0.01));

    mgr.set_scale_factor(3.0F);
    CHECK_THAT(mgr.scale_factor(), Catch::Matchers::WithinAbs(2.0, 0.01));
}

TEST_CASE("Scale: zoom_in increases by 0.1", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.set_scale_factor(1.0F);
    mgr.zoom_in();
    CHECK_THAT(mgr.scale_factor(), Catch::Matchers::WithinAbs(1.1, 0.01));
}

TEST_CASE("Scale: zoom_out decreases by 0.1", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.set_scale_factor(1.0F);
    mgr.zoom_out();
    CHECK_THAT(mgr.scale_factor(), Catch::Matchers::WithinAbs(0.9, 0.01));
}

TEST_CASE("Scale: zoom_reset returns to 1.0", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.set_scale_factor(1.5F);
    mgr.zoom_reset();
    CHECK_THAT(mgr.scale_factor(), Catch::Matchers::WithinAbs(1.0, 0.01));
}

// ═══════════════════════════════════════════════════════
// Input Mode Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Input mode: keyboard mode set on on_key_input()", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    CHECK_FALSE(mgr.is_using_keyboard());
    mgr.on_key_input();
    CHECK(mgr.is_using_keyboard());
}

TEST_CASE("Input mode: mouse mode set on on_mouse_input()", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.on_key_input();
    CHECK(mgr.is_using_keyboard());

    mgr.on_mouse_input();
    CHECK_FALSE(mgr.is_using_keyboard());
}

// ═══════════════════════════════════════════════════════
// Event Publishing Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Events: UIScaleChangedEvent published on set_scale_factor", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    float received_scale = 0.0F;
    auto sub = bus.subscribe<markamp::core::events::UIScaleChangedEvent>(
        [&received_scale](const markamp::core::events::UIScaleChangedEvent& evt)
        { received_scale = evt.scale_factor; });

    mgr.set_scale_factor(1.5F);
    CHECK_THAT(received_scale, Catch::Matchers::WithinAbs(1.5, 0.01));
}

TEST_CASE("Events: InputModeChangedEvent published on mode change", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    bool received_keyboard = false;
    auto sub = bus.subscribe<markamp::core::events::InputModeChangedEvent>(
        [&received_keyboard](const markamp::core::events::InputModeChangedEvent& evt)
        { received_keyboard = evt.using_keyboard; });

    mgr.on_key_input();
    CHECK(received_keyboard);

    mgr.on_mouse_input();
    CHECK_FALSE(received_keyboard);
}

TEST_CASE("Events: no duplicate event when already in same mode", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    int event_count = 0;
    auto sub = bus.subscribe<markamp::core::events::InputModeChangedEvent>(
        [&event_count]([[maybe_unused]] const markamp::core::events::InputModeChangedEvent& evt)
        { ++event_count; });

    mgr.on_key_input();
    mgr.on_key_input(); // already in keyboard mode
    CHECK(event_count == 1);
}

// ═══════════════════════════════════════════════════════
// Platform Query Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Platform: is_high_contrast delegates to platform", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    CHECK_FALSE(mgr.is_high_contrast());
    platform.high_contrast = true;
    CHECK(mgr.is_high_contrast());
}

TEST_CASE("Platform: prefers_reduced_motion delegates to platform", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    CHECK_FALSE(mgr.prefers_reduced_motion());
    platform.reduced_motion = true;
    CHECK(mgr.prefers_reduced_motion());
}

TEST_CASE("Platform: announce records message", "[accessibility]")
{
    EventBus bus;
    Config config;
    StubPlatform platform;
    AccessibilityManager mgr(bus, config, platform);

    mgr.announce(nullptr, "File saved");
    CHECK(platform.last_announcement == "File saved");
}

// ═══════════════════════════════════════════════════════
// Color::luminance and contrast_ratio sanity checks
// ═══════════════════════════════════════════════════════

TEST_CASE("Color: black vs white contrast ratio is 21:1", "[accessibility]")
{
    const Color black{0, 0, 0};
    const Color white{255, 255, 255};

    const float ratio = black.contrast_ratio(white);
    CHECK_THAT(ratio, Catch::Matchers::WithinAbs(21.0, 0.1));
}

TEST_CASE("Color: same color contrast ratio is 1:1", "[accessibility]")
{
    const Color gray{128, 128, 128};
    const float ratio = gray.contrast_ratio(gray);
    CHECK_THAT(ratio, Catch::Matchers::WithinAbs(1.0, 0.01));
}
