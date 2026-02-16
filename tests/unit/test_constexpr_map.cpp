/// test_constexpr_map.cpp — Phase 26: ConstexprMap tests

#include "core/ConstexprMap.h"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

using namespace markamp::core;

TEST_CASE("ConstexprMap: find existing key", "[constexpr_map]")
{
    constexpr auto map = make_constexpr_map<std::string_view, int>({
        {"alpha", 1},
        {"beta", 2},
        {"gamma", 3},
    });

    const auto* found = map.find("beta");
    REQUIRE(found != nullptr);
    REQUIRE(*found == 2);
}

TEST_CASE("ConstexprMap: find missing key returns nullptr", "[constexpr_map]")
{
    constexpr auto map = make_constexpr_map<std::string_view, int>({
        {"a", 1},
        {"b", 2},
    });

    REQUIRE(map.find("c") == nullptr);
}

TEST_CASE("ConstexprMap: get with default value", "[constexpr_map]")
{
    constexpr auto map = make_constexpr_map<std::string_view, int>({
        {"x", 42},
    });

    REQUIRE(map.get("x", -1) == 42);
    REQUIRE(map.get("y", -1) == -1);
}

TEST_CASE("ConstexprMap: contains", "[constexpr_map]")
{
    constexpr auto map = make_constexpr_map<std::string_view, int>({
        {"foo", 1},
        {"bar", 2},
    });

    REQUIRE(map.contains("foo"));
    REQUIRE_FALSE(map.contains("baz"));
}

TEST_CASE("ConstexprMap: unsorted input is sorted", "[constexpr_map]")
{
    constexpr auto map = make_constexpr_map<std::string_view, int>({
        {"charlie", 3},
        {"alpha", 1},
        {"bravo", 2},
    });

    // Verify sorted order
    const auto& data = map.data();
    REQUIRE(data[0].key == "alpha");
    REQUIRE(data[1].key == "bravo");
    REQUIRE(data[2].key == "charlie");
}

TEST_CASE("ConstexprMap: size returns entry count", "[constexpr_map]")
{
    constexpr auto map = make_constexpr_map<std::string_view, int>({
        {"a", 1},
        {"b", 2},
        {"c", 3},
    });

    REQUIRE(map.size() == 3);
}

TEST_CASE("ConstexprMap: kMimeTypes lookup", "[constexpr_map]")
{
    const auto* mime = kMimeTypes.find("json");
    REQUIRE(mime != nullptr);
    REQUIRE(*mime == "application/json");

    REQUIRE(kMimeTypes.find("unknown_ext") == nullptr);
}

TEST_CASE("ConstexprMap: kMimeTypes has expected entries", "[constexpr_map]")
{
    REQUIRE(kMimeTypes.contains("html"));
    REQUIRE(kMimeTypes.contains("css"));
    REQUIRE(kMimeTypes.contains("md"));
    REQUIRE(kMimeTypes.contains("png"));
}

TEST_CASE("ConstexprMap: kCppKeywords lookup", "[constexpr_map]")
{
    REQUIRE(kCppKeywords.contains("class"));
    REQUIRE(kCppKeywords.contains("constexpr"));
    REQUIRE(kCppKeywords.contains("return"));
    REQUIRE_FALSE(kCppKeywords.contains("println"));
}

TEST_CASE("ConstexprMap: single element map", "[constexpr_map]")
{
    constexpr auto map = make_constexpr_map<std::string_view, int>({
        {"only", 42},
    });

    REQUIRE(map.find("only") != nullptr);
    REQUIRE(*map.find("only") == 42);
    REQUIRE(map.find("nope") == nullptr);
}
