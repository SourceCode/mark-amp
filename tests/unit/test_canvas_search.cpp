// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/CanvasSearchModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Query and results", "[canvas_search][query]")
{
    CanvasSearchModel model;
    model.set_query("rectangle");
    model.set_results({{"obj-1", "Red Rectangle", "shape", 100.0, 200.0}});
    REQUIRE(model.query() == "rectangle");
    REQUIRE(model.result_count() == 1);
}

TEST_CASE("Query history MRU", "[canvas_search][history]")
{
    CanvasSearchModel model;
    model.set_query("first");
    model.set_query("second");
    model.set_query("first"); // moved to front
    REQUIRE(model.query_history()[0] == "first");
    REQUIRE(model.query_history().size() == 2);
}

TEST_CASE("Filter chips add remove clear", "[canvas_search][filter]")
{
    CanvasSearchModel model;
    model.add_filter({"type", "shape", true});
    model.add_filter({"tag", "draft", true});
    REQUIRE(model.active_filters().size() == 2);
    model.remove_filter("type");
    REQUIRE(model.active_filters().size() == 1);
    model.clear_filters();
    REQUIRE(model.active_filters().empty());
}

TEST_CASE("Jump highlight object", "[canvas_search][jump]")
{
    CanvasSearchModel model;
    model.set_highlighted("obj-42");
    REQUIRE(model.highlighted() == "obj-42");
}

TEST_CASE("Saved searches persist and remove", "[canvas_search][saved]")
{
    CanvasSearchModel model;
    model.save_search({"draft shapes", "shape", {{"tag", "draft", true}}});
    REQUIRE(model.saved_searches().size() == 1);
    model.remove_saved("draft shapes");
    REQUIRE(model.saved_searches().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
