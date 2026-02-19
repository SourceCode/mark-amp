// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/BenchmarkModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Scenario fixture registry and category filter", "[benchmark][fixtures]")
{
    BenchmarkModel model;
    model.set_fixtures({
        {"f1", "Simple drawing", 50, "drawing"},
        {"f2", "Complex diagram", 500, "diagram"},
        {"f3", "Dense layout", 10000, "dense"},
    });
    REQUIRE(model.fixtures().size() == 3);
    REQUIRE(model.fixtures_in_category("drawing").size() == 1);
}

TEST_CASE("Benchmark results with threshold pass", "[benchmark][results]")
{
    BenchmarkModel model;
    model.add_result({"render_50", 8.0, 16.0, true});   // passes
    model.add_result({"render_10k", 12.0, 16.0, true}); // passes
    REQUIRE(model.all_passed());
    REQUIRE(model.failed_count() == 0);
}

TEST_CASE("Benchmark result fails when over threshold", "[benchmark][results]")
{
    BenchmarkModel model;
    model.add_result({"render_50", 8.0, 16.0, true});
    model.add_result({"render_stress", 25.0, 16.0, true}); // will fail
    REQUIRE_FALSE(model.all_passed());
    REQUIRE(model.failed_count() == 1);
}

TEST_CASE("CI gate status", "[benchmark][gate]")
{
    BenchmarkModel model;
    REQUIRE_FALSE(model.gate_passed());
    model.set_gate_status(true);
    REQUIRE(model.gate_passed());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
