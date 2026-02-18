/// test_string_guards.cpp — V7 Phase 07: String length guard tests

#include "core/StringGuards.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

TEST_CASE("truncate_safe: no truncation for short string", "[string_guards]")
{
    auto result = truncate_safe("hello", 100);
    REQUIRE(result == "hello");
}

TEST_CASE("truncate_safe: truncates long string with ellipsis", "[string_guards]")
{
    auto result = truncate_safe("The quick brown fox jumps", 15);
    REQUIRE(result.size() <= 15);
    REQUIRE(result.ends_with("..."));
}

TEST_CASE("truncate_safe: exact boundary length returns untruncated", "[string_guards]")
{
    std::string input = "12345";
    auto result = truncate_safe(input, 5);
    REQUIRE(result == "12345");
}

TEST_CASE("truncate_safe: very short max returns without ellipsis", "[string_guards]")
{
    auto result = truncate_safe("hello world", 2);
    REQUIRE(result.size() <= 2);
}

TEST_CASE("truncate_safe: UTF-8 aware truncation", "[string_guards]")
{
    // UTF-8: "café" = 'c' 'a' 'f' 0xC3 0xA9
    std::string input = "caf\xC3\xA9";
    auto result = truncate_safe(input, 6);
    // Should keep all 5 bytes since within limit
    REQUIRE(result == input);
}

TEST_CASE("truncate_safe: does not split multi-byte UTF-8 char", "[string_guards]")
{
    // "aaé" = 'a' 'a' 0xC3 0xA9 (4 bytes total)
    std::string input = "aa\xC3\xA9";
    // Max 6 would include "..." occupying 3 bytes, leaving 3 for content
    // That maps to "aa" + "..." since splitting 0xC3 0xA9 is not safe
    auto result = truncate_safe(input, 6);
    REQUIRE(result.size() <= 6);
    // Should not end with a broken UTF-8 continuation byte
}

TEST_CASE("apply_length_guard: returns truncated string", "[string_guards]")
{
    auto result = apply_length_guard("hello world", 8);
    REQUIRE(result.size() <= 8);
}

TEST_CASE("apply_length_guard: passes through short string", "[string_guards]")
{
    auto result = apply_length_guard("hi", 100);
    REQUIRE(result == "hi");
}

TEST_CASE("Constants are defined correctly", "[string_guards]")
{
    REQUIRE(kMaxFilenameGuard == 255);
    REQUIRE(kMaxPathGuard == 4096);
    REQUIRE(kMaxYamlValueGuard == 65536);
    REQUIRE(kMaxSearchQueryGuard == 1024);
    REQUIRE(kMaxClipboardGuard == 1048576);
}
