// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/StencilModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Catalog and search", "[stencil][catalog]")
{
    StencilModel model;
    model.set_catalog({
        {"s1", "Process", StencilCategory::kFlowchart, "#FFF", "#333"},
        {"s2", "Decision", StencilCategory::kFlowchart, "#FFF", "#333"},
        {"s3", "Server", StencilCategory::kArchitecture, "#FFF", "#333"},
    });
    REQUIRE(model.catalog_size() == 3);
    REQUIRE(model.search("Process").size() == 1);
    REQUIRE(model.by_category(StencilCategory::kFlowchart).size() == 2);
}

TEST_CASE("Favorites toggle", "[stencil][favorites]")
{
    StencilModel model;
    model.add_favorite("s1");
    REQUIRE(model.is_favorite("s1"));
    model.remove_favorite("s1");
    REQUIRE_FALSE(model.is_favorite("s1"));
}

TEST_CASE("Recents MRU", "[stencil][recents]")
{
    StencilModel model;
    model.push_recent("s1");
    model.push_recent("s2");
    model.push_recent("s1"); // moved to front
    REQUIRE(model.recents()[0] == "s1");
    REQUIRE(model.recents().size() == 2);
}

TEST_CASE("Style overrides", "[stencil][override]")
{
    StencilModel model;
    REQUIRE_FALSE(model.has_overrides());
    model.set_override_fill("#FF0000");
    REQUIRE(model.has_overrides());
    model.clear_overrides();
    REQUIRE_FALSE(model.has_overrides());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
