/// test_numeric_guards.cpp — V7 Phase 11: Numeric range enforcement tests

#include "core/NumericGuards.h"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// clamp_config
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("clamp_config: returns value within range", "[numeric_guards]")
{
    REQUIRE(clamp_config(50, 0, 100) == 50);
}

TEST_CASE("clamp_config: clamps below minimum", "[numeric_guards]")
{
    REQUIRE(clamp_config(-5, 0, 100) == 0);
}

TEST_CASE("clamp_config: clamps above maximum", "[numeric_guards]")
{
    REQUIRE(clamp_config(200, 0, 100) == 100);
}

// ══════════════════════════════════════════════════════════════════════════════
// checked_add
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("checked_add: normal addition succeeds", "[numeric_guards]")
{
    auto result = checked_add(10, 20);
    REQUIRE(result.has_value());
    REQUIRE(*result == 30);
}

TEST_CASE("checked_add: detects signed overflow", "[numeric_guards]")
{
    auto result = checked_add(std::numeric_limits<int>::max(), 1);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::IntegerOverflow);
}

TEST_CASE("checked_add: detects unsigned overflow", "[numeric_guards]")
{
    auto result = checked_add(std::numeric_limits<uint32_t>::max(), 1u);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::IntegerOverflow);
}

TEST_CASE("checked_add: negative values work correctly", "[numeric_guards]")
{
    auto result = checked_add(-10, 5);
    REQUIRE(result.has_value());
    REQUIRE(*result == -5);
}

// ══════════════════════════════════════════════════════════════════════════════
// checked_multiply
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("checked_multiply: normal multiplication succeeds", "[numeric_guards]")
{
    auto result = checked_multiply(6, 7);
    REQUIRE(result.has_value());
    REQUIRE(*result == 42);
}

TEST_CASE("checked_multiply: detects overflow", "[numeric_guards]")
{
    auto result = checked_multiply(std::numeric_limits<int>::max(), 2);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::IntegerOverflow);
}

TEST_CASE("checked_multiply: zero is always safe", "[numeric_guards]")
{
    auto result = checked_multiply(0, std::numeric_limits<int>::max());
    REQUIRE(result.has_value());
    REQUIRE(*result == 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// safe_subtract
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("safe_subtract: normal subtraction succeeds", "[numeric_guards]")
{
    auto result = safe_subtract(10u, 3u);
    REQUIRE(result.has_value());
    REQUIRE(*result == 7u);
}

TEST_CASE("safe_subtract: detects unsigned underflow", "[numeric_guards]")
{
    auto result = safe_subtract(3u, 10u);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::IntegerUnderflow);
}

TEST_CASE("safe_subtract: signed subtraction works", "[numeric_guards]")
{
    auto result = safe_subtract(5, 10);
    REQUIRE(result.has_value());
    REQUIRE(*result == -5);
}

// ══════════════════════════════════════════════════════════════════════════════
// safe_cast
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("safe_cast: int to uint32_t succeeds for positive", "[numeric_guards]")
{
    auto result = safe_cast<uint32_t>(42);
    REQUIRE(result.has_value());
    REQUIRE(*result == 42u);
}

TEST_CASE("safe_cast: int to uint32_t fails for negative", "[numeric_guards]")
{
    auto result = safe_cast<uint32_t>(-1);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidRange);
}

TEST_CASE("safe_cast: int64 to int8 fails for overflow", "[numeric_guards]")
{
    auto result = safe_cast<int8_t>(static_cast<int64_t>(500));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::IntegerOverflow);
}

TEST_CASE("safe_cast: small value int64 to int8 succeeds", "[numeric_guards]")
{
    auto result = safe_cast<int8_t>(static_cast<int64_t>(42));
    REQUIRE(result.has_value());
    REQUIRE(*result == 42);
}
