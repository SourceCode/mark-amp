/// test_safe_mode.cpp — V7 Phase 36: Multi-tier safe mode tests

#include "core/SafeMode.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// Tier names
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("SafeMode: tier names", "[safe_mode]")
{
    REQUIRE(tier_name(SafeModeTier::kNormal) == "Normal");
    REQUIRE(tier_name(SafeModeTier::kNoExtensions) == "No Extensions");
    REQUIRE(tier_name(SafeModeTier::kNoPreview) == "No Preview");
    REQUIRE(tier_name(SafeModeTier::kMinimalUi) == "Minimal UI");
    REQUIRE(tier_name(SafeModeTier::kFull) == "Full Safe Mode");
}

// ══════════════════════════════════════════════════════════════════════════════
// Flag parsing
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("SafeMode: parse --safe flag", "[safe_mode]")
{
    auto result = parse_safe_mode_flag("--safe");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == SafeModeTier::kNoExtensions);
}

TEST_CASE("SafeMode: parse --safe-no-preview flag", "[safe_mode]")
{
    auto result = parse_safe_mode_flag("--safe-no-preview");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == SafeModeTier::kNoPreview);
}

TEST_CASE("SafeMode: parse --safe-minimal-ui flag", "[safe_mode]")
{
    auto result = parse_safe_mode_flag("--safe-minimal-ui");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == SafeModeTier::kMinimalUi);
}

TEST_CASE("SafeMode: parse unknown flag fails", "[safe_mode]")
{
    auto result = parse_safe_mode_flag("--unknown");
    REQUIRE_FALSE(result.has_value());
}

// ══════════════════════════════════════════════════════════════════════════════
// Controller
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("SafeModeController: default is Normal", "[safe_mode]")
{
    SafeModeController controller;
    REQUIRE(controller.current_tier() == SafeModeTier::kNormal);
    REQUIRE(controller.extensions_allowed());
    REQUIRE(controller.preview_allowed());
    REQUIRE(controller.full_ui_allowed());
}

TEST_CASE("SafeModeController: extensions disabled in NoExtensions", "[safe_mode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kNoExtensions);
    REQUIRE_FALSE(controller.extensions_allowed());
    REQUIRE(controller.preview_allowed());
}

TEST_CASE("SafeModeController: preview disabled in NoPreview", "[safe_mode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kNoPreview);
    REQUIRE_FALSE(controller.extensions_allowed());
    REQUIRE_FALSE(controller.preview_allowed());
    REQUIRE(controller.full_ui_allowed());
}

TEST_CASE("SafeModeController: full UI disabled in MinimalUI", "[safe_mode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kMinimalUi);
    REQUIRE_FALSE(controller.full_ui_allowed());
}

TEST_CASE("SafeModeController: process_args picks most restrictive", "[safe_mode]")
{
    SafeModeController controller;
    controller.process_args({"--safe", "--safe-no-preview"});
    REQUIRE(controller.current_tier() == SafeModeTier::kNoPreview);
}

TEST_CASE("SafeModeController: reset returns to Normal", "[safe_mode]")
{
    SafeModeController controller;
    controller.set_tier(SafeModeTier::kFull);
    controller.reset();
    REQUIRE(controller.current_tier() == SafeModeTier::kNormal);
}

// ══════════════════════════════════════════════════════════════════════════════
// Crash Counter
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("CrashCounter: no crashes returns Normal", "[safe_mode]")
{
    CrashCounter counter;
    REQUIRE(counter.recent_crash_count() == 0);
}

TEST_CASE("CrashCounter: triggers after threshold", "[safe_mode]")
{
    CrashCounter counter;
    counter.set_threshold(2, std::chrono::seconds(300));

    auto tier1 = counter.record_crash();
    REQUIRE(tier1 == SafeModeTier::kNormal);

    auto tier2 = counter.record_crash();
    REQUIRE(tier2 == SafeModeTier::kNoExtensions);
}

TEST_CASE("CrashCounter: double threshold triggers Full", "[safe_mode]")
{
    CrashCounter counter;
    counter.set_threshold(2, std::chrono::seconds(300));

    counter.record_crash();
    counter.record_crash();
    counter.record_crash();
    auto tier = counter.record_crash();
    REQUIRE(tier == SafeModeTier::kFull);
}

TEST_CASE("SafeModeController: on_crash escalates tier", "[safe_mode]")
{
    SafeModeController controller;
    controller.crash_counter().set_threshold(1, std::chrono::seconds(300));

    auto tier = controller.on_crash();
    REQUIRE(tier == SafeModeTier::kNoExtensions);
    REQUIRE(controller.current_tier() == SafeModeTier::kNoExtensions);
}
