/// test_rendering_safety.cpp — V7 Phase 27: Rendering pipeline safety tests

#include "rendering/RenderingSafety.h"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>

using namespace markamp::rendering;

// ══════════════════════════════════════════════════════════════════════════════
// Safe rendering
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("RenderingSafety: successful render", "[rendering_safety]")
{
    RenderingSafety safety;
    auto outcome = safety.safe_render([]() { return std::string("<p>Hello</p>"); },
                                      []() { return std::string("Hello"); });

    REQUIRE(outcome.output == "<p>Hello</p>");
    REQUIRE_FALSE(outcome.used_fallback);
    REQUIRE(safety.success_count() == 1);
    REQUIRE(safety.fallback_count() == 0);
}

TEST_CASE("RenderingSafety: exception triggers fallback", "[rendering_safety]")
{
    RenderingSafety safety;
    auto outcome =
        safety.safe_render([]() -> std::string { throw std::runtime_error("render crash"); },
                           []() { return std::string("fallback text"); });

    REQUIRE(outcome.output == "fallback text");
    REQUIRE(outcome.used_fallback);
    REQUIRE(safety.fallback_count() == 1);
    REQUIRE(safety.success_count() == 0);
}

TEST_CASE("RenderingSafety: both render and fallback throw", "[rendering_safety]")
{
    RenderingSafety safety;
    auto outcome =
        safety.safe_render([]() -> std::string { throw std::runtime_error("render crash"); },
                           []() -> std::string { throw std::runtime_error("fallback crash"); });

    REQUIRE(outcome.output == "[Rendering error]");
    REQUIRE(outcome.used_fallback);
}

TEST_CASE("RenderingSafety: oversized output triggers fallback", "[rendering_safety]")
{
    RenderingSafety safety;
    std::string huge_output(kMaxRenderOutputBytes + 1, 'x');

    auto outcome = safety.safe_render([&huge_output]() { return huge_output; },
                                      []() { return std::string("safe fallback"); });

    REQUIRE(outcome.output == "safe fallback");
    REQUIRE(outcome.used_fallback);
    REQUIRE(safety.fallback_count() == 1);
}

TEST_CASE("RenderingSafety: elapsed time is tracked", "[rendering_safety]")
{
    RenderingSafety safety;
    auto outcome = safety.safe_render([]() { return std::string("ok"); },
                                      []() { return std::string("fallback"); });

    REQUIRE(outcome.elapsed.count() >= 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// Output validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("RenderingSafety: validate_output_size accepts normal", "[rendering_safety]")
{
    auto result = RenderingSafety::validate_output_size("normal output");
    REQUIRE(result.has_value());
}

TEST_CASE("RenderingSafety: validate_output_size rejects oversized", "[rendering_safety]")
{
    std::string huge(kMaxRenderOutputBytes + 1, 'x');
    auto result = RenderingSafety::validate_output_size(huge);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == markamp::core::ErrorCode::FrameBudgetExceeded);
}

// ══════════════════════════════════════════════════════════════════════════════
// Counters
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("RenderingSafety: reset_counters", "[rendering_safety]")
{
    RenderingSafety safety;
    safety.safe_render([]() { return std::string("ok"); }, []() { return std::string("fb"); });
    safety.safe_render([]() -> std::string { throw std::runtime_error("err"); },
                       []() { return std::string("fb"); });

    REQUIRE(safety.success_count() == 1);
    REQUIRE(safety.fallback_count() == 1);

    safety.reset_counters();
    REQUIRE(safety.success_count() == 0);
    REQUIRE(safety.fallback_count() == 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// Constants
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("RenderingSafety: frame budget constants", "[rendering_safety]")
{
    REQUIRE(kDefaultFrameBudgetMs.count() == 16);
    REQUIRE(kMaxRenderTimeoutMs.count() == 5000);
    REQUIRE(kMaxRenderOutputBytes == 10 * 1024 * 1024);
}
