// test_constexpr_map.cpp — 10 tests for ConstexprMap and built-in tables
#include "core/ConstexprMap.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ConstexprMap find returns valid pointer for existing key", "[constexpr_map]")
{
    constexpr auto kMap =
        make_constexpr_map<std::string_view, int>({{"alpha", 1}, {"beta", 2}, {"gamma", 3}});
    const auto* val = kMap.find("beta");
    REQUIRE(val != nullptr);
    CHECK(*val == 2);
}

TEST_CASE("ConstexprMap find returns nullptr for missing key", "[constexpr_map]")
{
    constexpr auto kMap = make_constexpr_map<std::string_view, int>({{"x", 10}, {"y", 20}});
    CHECK(kMap.find("z") == nullptr);
}

TEST_CASE("ConstexprMap get with default value", "[constexpr_map]")
{
    constexpr auto kMap = make_constexpr_map<std::string_view, int>({{"a", 1}, {"b", 2}});
    CHECK(kMap.get("a", -1) == 1);
    CHECK(kMap.get("missing", -1) == -1);
}

TEST_CASE("ConstexprMap contains check", "[constexpr_map]")
{
    constexpr auto kMap =
        make_constexpr_map<std::string_view, int>({{"one", 1}, {"two", 2}, {"three", 3}});
    CHECK(kMap.contains("one"));
    CHECK(kMap.contains("three"));
    CHECK_FALSE(kMap.contains("four"));
}

TEST_CASE("ConstexprMap size reflects entry count", "[constexpr_map]")
{
    constexpr auto kMap =
        make_constexpr_map<std::string_view, int>({{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}});
    CHECK(kMap.size() == 4);
}

TEST_CASE("ConstexprMap auto-sorts unsorted input", "[constexpr_map]")
{
    constexpr auto kMap =
        make_constexpr_map<std::string_view, int>({{"z", 26}, {"a", 1}, {"m", 13}});
    // Sorted: a, m, z — all findable
    CHECK(kMap.contains("a"));
    CHECK(kMap.contains("m"));
    CHECK(kMap.contains("z"));
    const auto& data = kMap.data();
    CHECK(data[0].key == "a");
    CHECK(data[1].key == "m");
    CHECK(data[2].key == "z");
}

TEST_CASE("kMimeTypes table contains md -> text/markdown", "[constexpr_map][mime]")
{
    const auto* mime = kMimeTypes.find("md");
    REQUIRE(mime != nullptr);
    CHECK(*mime == "text/markdown");
}

TEST_CASE("kMimeTypes table contains json -> application/json", "[constexpr_map][mime]")
{
    const auto* mime = kMimeTypes.find("json");
    REQUIRE(mime != nullptr);
    CHECK(*mime == "application/json");
}

TEST_CASE("kCppKeywords table recognizes known keywords", "[constexpr_map][keywords]")
{
    CHECK(kCppKeywords.contains("class"));
    CHECK(kCppKeywords.contains("constexpr"));
    CHECK(kCppKeywords.contains("namespace"));
    CHECK_FALSE(kCppKeywords.contains("println"));
}

TEST_CASE("kMimeTypes returns nullptr for unknown extension", "[constexpr_map][mime]")
{
    CHECK(kMimeTypes.find("xyz_unknown") == nullptr);
}
