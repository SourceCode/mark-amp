/// test_ub_edge_cases.cpp — Phase 25: UB elimination edge case tests
///
/// Tests boundary conditions that are common sources of undefined behavior.

#include <catch2/catch_test_macros.hpp>

#include <climits>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

// ══════════════════════════════════════════
// Integer Overflow Boundaries
// ══════════════════════════════════════════

TEST_CASE("UB: int32_t max + 0 is safe", "[ub_edge_cases]")
{
    int32_t val = std::numeric_limits<int32_t>::max();
    REQUIRE(val > 0);
    // Adding 0 should not trigger overflow
    int32_t result = val + 0;
    REQUIRE(result == std::numeric_limits<int32_t>::max());
}

TEST_CASE("UB: uint32_t wraps on overflow", "[ub_edge_cases]")
{
    uint32_t val = std::numeric_limits<uint32_t>::max();
    uint32_t result = val + 1U; // well-defined wrap-around for unsigned
    REQUIRE(result == 0U);
}

TEST_CASE("UB: size_t subtraction wraps", "[ub_edge_cases]")
{
    std::size_t a = 0;
    std::size_t b = 1;
    // Unsigned subtraction wraps (defined behavior, but often a bug)
    std::size_t result = a - b;
    REQUIRE(result == std::numeric_limits<std::size_t>::max());
}

// ══════════════════════════════════════════
// Empty Container Access Guards
// ══════════════════════════════════════════

TEST_CASE("UB: empty vector begin == end", "[ub_edge_cases]")
{
    std::vector<int> vec;
    REQUIRE(vec.begin() == vec.end());
    REQUIRE(vec.empty());
}

TEST_CASE("UB: empty string_view is valid", "[ub_edge_cases]")
{
    std::string_view sv;
    REQUIRE(sv.empty());
    REQUIRE(sv.size() == 0);
    REQUIRE(sv.data() == nullptr);
}

TEST_CASE("UB: empty string substr is safe", "[ub_edge_cases]")
{
    std::string empty_str;
    auto sub = empty_str.substr(0, 0);
    REQUIRE(sub.empty());
}

// ══════════════════════════════════════════
// Signed/Unsigned Conversion
// ══════════════════════════════════════════

TEST_CASE("UB: casting negative int to size_t", "[ub_edge_cases]")
{
    int negative = -1;
    auto as_size = static_cast<std::size_t>(negative);
    REQUIRE(as_size == std::numeric_limits<std::size_t>::max());
}

TEST_CASE("UB: safe narrow from size_t to int32_t", "[ub_edge_cases]")
{
    std::size_t small_val = 42;
    auto narrowed = static_cast<int32_t>(small_val);
    REQUIRE(narrowed == 42);
}

// ══════════════════════════════════════════
// String View Lifetime
// ══════════════════════════════════════════

TEST_CASE("UB: string_view from persistent string is valid", "[ub_edge_cases]")
{
    std::string owner = "hello";
    std::string_view view = owner;

    REQUIRE(view == "hello");

    // Modifying owner invalidates view — but we just test that
    // view is valid while owner exists
    REQUIRE(view.data() == owner.data());
}

TEST_CASE("UB: string_view from string literal is always valid", "[ub_edge_cases]")
{
    std::string_view view = "string literal";
    REQUIRE(view == "string literal");
    REQUIRE(view.size() == 14);
}

// ══════════════════════════════════════════
// Division and Modulo
// ══════════════════════════════════════════

TEST_CASE("UB: division by non-zero", "[ub_edge_cases]")
{
    int a = 100;
    int b = 3;
    REQUIRE(a / b == 33);
    REQUIRE(a % b == 1);
}

TEST_CASE("UB: safe division guard pattern", "[ub_edge_cases]")
{
    int denominator = 0;
    int result = (denominator != 0) ? (100 / denominator) : 0;
    REQUIRE(result == 0);
}
