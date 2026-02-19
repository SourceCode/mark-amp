// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/ConnectorModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Anchor attachment", "[connector][anchor]")
{
    ConnectorModel model;
    model.set_source({"obj-1", 0.5, 1.0});
    model.set_target({"obj-2", 0.5, 0.0});
    REQUIRE(model.is_attached());
    REQUIRE(model.source().object_id == "obj-1");
    REQUIRE(model.target().object_id == "obj-2");
}

TEST_CASE("Not attached when missing object", "[connector][anchor]")
{
    ConnectorModel model;
    model.set_source({"obj-1", 0.5, 0.5});
    REQUIRE_FALSE(model.is_attached());
}

TEST_CASE("Routing modes", "[connector][routing]")
{
    ConnectorModel model;
    REQUIRE(model.routing() == RoutingMode::kStraight);
    model.set_routing(RoutingMode::kOrthogonal);
    REQUIRE(model.routing() == RoutingMode::kOrthogonal);
    model.set_routing(RoutingMode::kCurved);
    REQUIRE(model.routing() == RoutingMode::kCurved);
}

TEST_CASE("Bend point management", "[connector][bend]")
{
    ConnectorModel model;
    model.add_bend({50.0, 50.0});
    model.add_bend({100.0, 100.0});
    REQUIRE(model.bend_count() == 2);
    model.remove_bend(0);
    REQUIRE(model.bend_count() == 1);
    REQUIRE(model.bends()[0].x == 100.0);
    model.clear_bends();
    REQUIRE(model.bend_count() == 0);
}

TEST_CASE("Connector label", "[connector][label]")
{
    ConnectorModel model;
    REQUIRE_FALSE(model.has_label());
    model.set_label({"Connection", 0.5});
    REQUIRE(model.has_label());
    REQUIRE(model.label().text == "Connection");
    REQUIRE(model.label().position == 0.5);
}

TEST_CASE("Remove bend out of range is safe", "[connector][bend]")
{
    ConnectorModel model;
    model.add_bend({10.0, 10.0});
    model.remove_bend(5); // out of range
    REQUIRE(model.bend_count() == 1);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
