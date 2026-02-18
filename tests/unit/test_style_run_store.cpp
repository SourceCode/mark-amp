/// test_style_run_store.cpp — Unit tests for StyleRunStore

#include "core/StyleRunStore.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace markamp::core;

TEST_CASE("StyleRun: end() calculation", "[style_run_store]")
{
    StyleRun run{10, 5, 1};
    REQUIRE(run.end() == 15);
}

TEST_CASE("StyleRunStore: starts empty", "[style_run_store]")
{
    StyleRunStore store;
    REQUIRE(store.size() == 0);
}

TEST_CASE("StyleRunStore: update_line and get_line", "[style_run_store]")
{
    StyleRunStore store;
    std::vector<StyleRun> runs = {{0, 5, 1}, {5, 3, 2}};
    store.update_line(0, runs);
    auto& result = store.get_line(0);
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].start == 0);
    REQUIRE(result[1].start == 5);
}

TEST_CASE("StyleRunStore: merges adjacent same-style runs", "[style_run_store]")
{
    StyleRunStore store;
    std::vector<StyleRun> runs = {{0, 3, 1}, {3, 4, 1}, {7, 2, 2}};
    store.update_line(0, runs);
    auto& result = store.get_line(0);
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].length == 7);
}

TEST_CASE("StyleRunStore: get_line out of range returns empty", "[style_run_store]")
{
    StyleRunStore store;
    auto& result = store.get_line(99);
    REQUIRE(result.empty());
}

TEST_CASE("StyleRunStore: has_runs", "[style_run_store]")
{
    StyleRunStore store;
    REQUIRE_FALSE(store.has_runs(0));
    std::vector<StyleRun> runs = {{0, 5, 1}};
    store.update_line(0, runs);
    REQUIRE(store.has_runs(0));
}

TEST_CASE("StyleRunStore: invalidate_range", "[style_run_store]")
{
    StyleRunStore store;
    store.update_line(0, {{0, 5, 1}});
    store.update_line(1, {{0, 3, 2}});
    store.update_line(2, {{0, 4, 3}});
    store.invalidate_range(0, 2);
    REQUIRE_FALSE(store.has_runs(0));
    REQUIRE_FALSE(store.has_runs(1));
    REQUIRE(store.has_runs(2));
}

TEST_CASE("StyleRunStore: invalidate_all", "[style_run_store]")
{
    StyleRunStore store;
    store.update_line(0, {{0, 5, 1}});
    store.invalidate_all();
    REQUIRE(store.size() == 0);
}

TEST_CASE("StyleRunStore: total_run_count", "[style_run_store]")
{
    StyleRunStore store;
    store.update_line(0, {{0, 5, 1}, {5, 3, 2}});
    store.update_line(1, {{0, 10, 3}});
    REQUIRE(store.total_run_count() == 3);
}
