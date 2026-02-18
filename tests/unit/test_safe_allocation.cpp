/// test_safe_allocation.cpp — V7 Phase 16: Safe allocation tests

#include "core/SafeAllocation.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

struct TestWidget
{
    int id{0};
    std::string name;

    TestWidget() = default;
    TestWidget(int widget_id, std::string widget_name)
        : id(widget_id)
        , name(std::move(widget_name))
    {
    }
};

TEST_CASE("safe_make_unique: creates object successfully", "[safe_allocation]")
{
    auto result = safe_make_unique<TestWidget>(42, "test");
    REQUIRE(result.has_value());
    REQUIRE((*result)->id == 42);
    REQUIRE((*result)->name == "test");
}

TEST_CASE("safe_make_unique: default construction works", "[safe_allocation]")
{
    auto result = safe_make_unique<int>();
    REQUIRE(result.has_value());
    REQUIRE(**result == 0);
}

TEST_CASE("safe_make_shared: creates object successfully", "[safe_allocation]")
{
    auto result = safe_make_shared<TestWidget>(99, "shared");
    REQUIRE(result.has_value());
    REQUIRE((*result)->id == 99);
    REQUIRE((*result)->name == "shared");
}

TEST_CASE("safe_make_shared: reference counting works", "[safe_allocation]")
{
    auto result = safe_make_shared<std::string>("hello");
    REQUIRE(result.has_value());
    auto copy = *result;
    REQUIRE(copy.use_count() == 2);
    REQUIRE(*copy == "hello");
}

TEST_CASE("safe_make_unique: creates vector successfully", "[safe_allocation]")
{
    auto result = safe_make_unique<std::vector<int>>(std::initializer_list<int>{1, 2, 3});
    REQUIRE(result.has_value());
    REQUIRE((*result)->size() == 3);
}
