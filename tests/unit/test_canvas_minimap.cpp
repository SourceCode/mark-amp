// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/MinimapModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Viewport rectangle", "[minimap][viewport]")
{
    MinimapModel model;
    model.set_viewport(100.0, 200.0, 400.0, 300.0);
    REQUIRE(model.viewport_x() == 100.0);
    REQUIRE(model.viewport_w() == 400.0);
}

TEST_CASE("Viewport clamps negative dimensions", "[minimap][viewport]")
{
    MinimapModel model;
    model.set_viewport(0.0, 0.0, -10.0, -10.0);
    REQUIRE(model.viewport_w() == 1.0);
    REQUIRE(model.viewport_h() == 1.0);
}

TEST_CASE("Render mode toggle", "[minimap][mode]")
{
    MinimapModel model;
    REQUIRE(model.mode() == MinimapMode::kSimplified);
    model.set_mode(MinimapMode::kDetailed);
    REQUIRE(model.mode() == MinimapMode::kDetailed);
}

TEST_CASE("Frame markers for quick jump", "[minimap][markers]")
{
    MinimapModel model;
    model.set_markers({{"f1", "Overview", 500.0, 300.0}});
    REQUIRE(model.markers().size() == 1);
    REQUIRE(model.markers()[0].label == "Overview");
}

TEST_CASE("Navigation history push and back", "[minimap][nav]")
{
    MinimapModel model;
    model.set_viewport(0.0, 0.0, 800.0, 600.0);
    model.push_location();
    model.set_viewport(500.0, 400.0, 800.0, 600.0);
    REQUIRE(model.history_depth() == 1);
    model.navigate_back();
    REQUIRE(model.viewport_x() == 0.0);
    REQUIRE(model.history_depth() == 0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
