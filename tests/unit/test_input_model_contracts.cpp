// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/InputModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ── Task 1: InputGesture enum completeness ──────────────────────────

TEST_CASE("InputGesture enum has all canonical gestures", "[input][gestures]")
{
    // Verify key enum values exist and cast correctly
    auto check_gesture = [](InputGesture gesture)
    {
        auto val = static_cast<uint8_t>(gesture);
        REQUIRE(val >= 0);
    };

    check_gesture(InputGesture::kClick);
    check_gesture(InputGesture::kDoubleClick);
    check_gesture(InputGesture::kRightClick);
    check_gesture(InputGesture::kCtrlClick);
    check_gesture(InputGesture::kShiftClick);
    check_gesture(InputGesture::kScrollVertical);
    check_gesture(InputGesture::kScrollHorizontal);
    check_gesture(InputGesture::kKeyEnter);
    check_gesture(InputGesture::kKeySpace);
    check_gesture(InputGesture::kKeyEscape);
    check_gesture(InputGesture::kContextMenu);
}

// ── Task 2: Scroll normalization defaults ───────────────────────────

TEST_CASE("ScrollConfig defaults are reasonable", "[input][scroll]")
{
    const ScrollConfig config;
    REQUIRE(config.lines_per_notch == 3);
    REQUIRE_FALSE(config.smooth_scrolling);
    REQUIRE(config.respect_inertia);
    REQUIRE(config.horizontal_speed == 1.0F);
}

TEST_CASE("InputModel scroll config is mutable", "[input][scroll]")
{
    auto& model = InputModel::get();
    ScrollConfig custom;
    custom.lines_per_notch = 5;
    custom.smooth_scrolling = true;
    model.set_scroll_config(custom);

    REQUIRE(model.scroll_config().lines_per_notch == 5);
    REQUIRE(model.scroll_config().smooth_scrolling);

    // Reset
    model.set_scroll_config(ScrollConfig{});
}

// ── Task 3: Double-click safety ─────────────────────────────────────

TEST_CASE("DoubleClickPolicy defaults guard destructive targets", "[input][doubleclick]")
{
    const DoubleClickPolicy policy;
    REQUIRE(policy.timing_ms == 500);
    REQUIRE(policy.guard_destructive);
}

// ── Task 4: Context menu policy ─────────────────────────────────────

TEST_CASE("ContextMenuPolicy enables both keyboard and mouse by default", "[input][context]")
{
    const ContextMenuPolicy policy;
    REQUIRE(policy.keyboard_menu_enabled);
    REQUIRE(policy.right_click_enabled);
    REQUIRE(policy.show_at_focus);
}

TEST_CASE("InputModel context policy is mutable", "[input][context]")
{
    auto& model = InputModel::get();
    ContextMenuPolicy custom;
    custom.keyboard_menu_enabled = false;
    model.set_context_menu_policy(custom);

    REQUIRE_FALSE(model.context_menu_policy().keyboard_menu_enabled);

    // Reset
    model.set_context_menu_policy(ContextMenuPolicy{});
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
