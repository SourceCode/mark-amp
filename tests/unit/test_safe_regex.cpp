/// test_safe_regex.cpp — V7 Phase 10: Safe regex tests

#include "core/SafeRegex.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("safe_regex_compile: accepts valid pattern", "[safe_regex]")
{
    auto result = safe_regex_compile("[a-z]+");
    REQUIRE(result.has_value());
}

TEST_CASE("safe_regex_compile: rejects invalid pattern", "[safe_regex]")
{
    auto result = safe_regex_compile("[invalid(");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::RegexError);
}

TEST_CASE("safe_regex_match: returns true for matching input", "[safe_regex]")
{
    auto pattern = safe_regex_compile("[0-9]+");
    REQUIRE(pattern.has_value());

    auto result = safe_regex_match("12345", *pattern);
    REQUIRE(result.has_value());
    REQUIRE(*result == true);
}

TEST_CASE("safe_regex_match: returns false for non-matching input", "[safe_regex]")
{
    auto pattern = safe_regex_compile("[0-9]+");
    REQUIRE(pattern.has_value());

    auto result = safe_regex_match("hello", *pattern);
    REQUIRE(result.has_value());
    REQUIRE(*result == false);
}

TEST_CASE("safe_regex_search: finds pattern in string", "[safe_regex]")
{
    auto pattern = safe_regex_compile("world");
    REQUIRE(pattern.has_value());

    auto result = safe_regex_search("hello world", *pattern);
    REQUIRE(result.has_value());
    REQUIRE(*result == true);
}

TEST_CASE("safe_regex_search: returns false when not found", "[safe_regex]")
{
    auto pattern = safe_regex_compile("xyz");
    REQUIRE(pattern.has_value());

    auto result = safe_regex_search("hello world", *pattern);
    REQUIRE(result.has_value());
    REQUIRE(*result == false);
}

TEST_CASE("safe_regex_replace: replaces matches", "[safe_regex]")
{
    auto pattern = safe_regex_compile("dog");
    REQUIRE(pattern.has_value());

    auto result = safe_regex_replace("the dog ran", *pattern, "cat");
    REQUIRE(result.has_value());
    REQUIRE(*result == "the cat ran");
}

TEST_CASE("safe_regex_replace: no match returns original", "[safe_regex]")
{
    auto pattern = safe_regex_compile("xyz");
    REQUIRE(pattern.has_value());

    auto result = safe_regex_replace("hello world", *pattern, "replaced");
    REQUIRE(result.has_value());
    REQUIRE(*result == "hello world");
}
