/// test_thread_boundary.cpp — Phase 22: ThreadBoundary tests
///
/// Validates safe_call, safe_call_void, SafeCallResult, and error handling.

#include "core/ThreadBoundary.h"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>

using namespace markamp::core;

// ══════════════════════════════════════════
// safe_call<R>
// ══════════════════════════════════════════

TEST_CASE("safe_call: returns value on success", "[thread_boundary]")
{
    auto result = safe_call<int>([]() { return 42; }, -1);

    REQUIRE(result.ok());
    REQUIRE(result.value == 42);
    REQUIRE(result.status == SafeCallStatus::Success);
    REQUIRE(result.error_message.empty());
    REQUIRE(result.duration_us >= 0);
}

TEST_CASE("safe_call: returns fallback on runtime_error", "[thread_boundary]")
{
    auto result = safe_call<int>([]() -> int { throw std::runtime_error("boom"); }, -1);

    REQUIRE_FALSE(result.ok());
    REQUIRE(result.value == -1);
    REQUIRE(result.status == SafeCallStatus::Exception);
    REQUIRE(result.error_message.find("runtime_error: boom") != std::string::npos);
}

TEST_CASE("safe_call: returns fallback on std::exception", "[thread_boundary]")
{
    auto result = safe_call<std::string>([]() -> std::string { throw std::logic_error("logic"); },
                                         "fallback");

    REQUIRE_FALSE(result.ok());
    REQUIRE(result.value == "fallback");
    REQUIRE(result.error_message.find("exception: logic") != std::string::npos);
}

TEST_CASE("safe_call: returns fallback on unknown exception", "[thread_boundary]")
{
    auto result =
        safe_call<int>([]() -> int { throw 42; }, // NOLINT: intentional non-std throw for testing
                       -99);

    REQUIRE_FALSE(result.ok());
    REQUIRE(result.value == -99);
    REQUIRE(result.error_message.find("unknown") != std::string::npos);
}

// ══════════════════════════════════════════
// safe_call_void
// ══════════════════════════════════════════

TEST_CASE("safe_call_void: succeeds without exception", "[thread_boundary]")
{
    int side_effect = 0;
    auto result = safe_call_void([&]() { side_effect = 42; });

    REQUIRE(result.ok());
    REQUIRE(side_effect == 42);
    REQUIRE(result.error_message.empty());
}

TEST_CASE("safe_call_void: captures exception message", "[thread_boundary]")
{
    auto result = safe_call_void([]() { throw std::runtime_error("void boom"); });

    REQUIRE_FALSE(result.ok());
    REQUIRE(result.error_message.find("void boom") != std::string::npos);
}

// ══════════════════════════════════════════
// Duration tracking
// ══════════════════════════════════════════

TEST_CASE("safe_call: tracks duration", "[thread_boundary]")
{
    auto result = safe_call<int>([]() { return 1; }, 0);
    REQUIRE(result.duration_us >= 0);
}

// ══════════════════════════════════════════
// Main thread assertion
// ══════════════════════════════════════════

TEST_CASE("set_main_thread_id: sets current thread", "[thread_boundary]")
{
    set_main_thread_id();
    REQUIRE(g_main_thread_id == std::this_thread::get_id());
}
