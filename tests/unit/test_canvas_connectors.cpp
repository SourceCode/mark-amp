/**
 * @file test_canvas_connectors.cpp
 * @brief Phase 44: Tests for ConnectorAnchorModel and ConnectorRouteEngine.
 */

#include "canvas/ConnectorAnchorModel.h"
#include "canvas/ConnectorRouteEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// ConnectorAnchor
// ═══════════════════════════════════════════════════════

TEST_CASE("ConnectorAnchor - position name", "[canvas][connectors]")
{
    ConnectorAnchor anchor;
    anchor.position = AnchorPosition::kTop;
    CHECK(anchor.position_name() == "top");

    anchor.position = AnchorPosition::kAuto;
    CHECK(anchor.position_name() == "auto");
}

// ═══════════════════════════════════════════════════════
// ConnectorAnchorModel
// ═══════════════════════════════════════════════════════

TEST_CASE("ConnectorAnchorModel - resolve anchor", "[canvas][connectors]")
{
    ObjectBounds bounds{"obj1", 100.0, 200.0, 80.0, 60.0};

    ConnectorAnchor top_anchor;
    top_anchor.object_id = "obj1";
    top_anchor.position = AnchorPosition::kTop;
    auto top_pt = ConnectorAnchorModel::resolve_anchor(top_anchor, bounds);
    CHECK(top_pt.pos_x == 140.0); // 100 + 80/2
    CHECK(top_pt.pos_y == 200.0);

    ConnectorAnchor center_anchor;
    center_anchor.object_id = "obj1";
    center_anchor.position = AnchorPosition::kCenter;
    auto center_pt = ConnectorAnchorModel::resolve_anchor(center_anchor, bounds);
    CHECK(center_pt.pos_x == 140.0);
    CHECK(center_pt.pos_y == 230.0); // 200 + 60/2
}

TEST_CASE("ConnectorAnchorModel - add and find connection", "[canvas][connectors]")
{
    ConnectorAnchorModel model;
    ConnectorAnchorModel::Connection conn;
    conn.connection_id = "c1";
    conn.source.object_id = "a";
    conn.target.object_id = "b";
    model.add_connection(conn);

    CHECK(model.connection_count() == 1);
    const auto* found = model.find_connection("c1");
    REQUIRE(found != nullptr);
    CHECK(found->source.object_id == "a");
}

TEST_CASE("ConnectorAnchorModel - remove connection", "[canvas][connectors]")
{
    ConnectorAnchorModel model;
    ConnectorAnchorModel::Connection conn;
    conn.connection_id = "c1";
    model.add_connection(conn);
    model.remove_connection("c1");
    CHECK(model.connection_count() == 0);
}

TEST_CASE("ConnectorAnchorModel - connections for object", "[canvas][connectors]")
{
    ConnectorAnchorModel model;
    ConnectorAnchorModel::Connection conn;
    conn.connection_id = "c1";
    conn.source.object_id = "a";
    conn.target.object_id = "b";
    model.add_connection(conn);

    auto conns_a = model.connections_for_object("a");
    CHECK(conns_a.size() == 1);

    auto conns_c = model.connections_for_object("c");
    CHECK(conns_c.empty());
}

TEST_CASE("ConnectorAnchorModel - anchor validity", "[canvas][connectors]")
{
    ConnectorAnchorModel model;
    ConnectorAnchor anchor;
    anchor.object_id = "obj1";

    std::vector<ObjectBounds> objects = {{"obj1", 0, 0, 100, 100}};
    CHECK(model.is_anchor_valid(anchor, objects));

    std::vector<ObjectBounds> empty_objects;
    CHECK_FALSE(model.is_anchor_valid(anchor, empty_objects));
}

// ═══════════════════════════════════════════════════════
// ConnectorRouteEngine
// ═══════════════════════════════════════════════════════

TEST_CASE("ConnectorRouteEngine - straight route", "[canvas][connectors]")
{
    ConnectorRouteEngine engine;
    engine.set_mode(RouteMode::kStraight);
    engine.compute_route(0.0, 0.0, 100.0, 100.0);

    CHECK(engine.point_count() == 2); // src + dst
    CHECK(ConnectorRouteEngine::mode_name(RouteMode::kStraight) == "straight");
}

TEST_CASE("ConnectorRouteEngine - orthogonal route", "[canvas][connectors]")
{
    ConnectorRouteEngine engine;
    engine.set_mode(RouteMode::kOrthogonal);
    engine.compute_route(0.0, 0.0, 100.0, 100.0);

    CHECK(engine.point_count() == 4); // src + 2 midpoints + dst
}

TEST_CASE("ConnectorRouteEngine - curved route", "[canvas][connectors]")
{
    ConnectorRouteEngine engine;
    engine.set_mode(RouteMode::kCurved);
    engine.compute_route(0.0, 0.0, 100.0, 100.0);

    CHECK(engine.point_count() == 4); // src + 2 control points + dst
}

TEST_CASE("ConnectorRouteEngine - bend editing", "[canvas][connectors]")
{
    ConnectorRouteEngine engine;
    engine.compute_route(0.0, 0.0, 100.0, 0.0);
    int initial = engine.point_count();

    engine.add_bend(1, 50.0, 30.0);
    CHECK(engine.point_count() == initial + 1);
    CHECK(engine.bend_count() == 1);

    engine.move_bend(1, 60.0, 40.0);
    CHECK(engine.route_points()[1].pos_x == 60.0);

    engine.remove_bend(1);
    CHECK(engine.bend_count() == 0);
}

TEST_CASE("ConnectorRouteEngine - label", "[canvas][connectors]")
{
    ConnectorRouteEngine engine;
    engine.compute_route(0.0, 0.0, 100.0, 0.0);

    CHECK_FALSE(engine.has_label());

    ConnectorLabel lbl;
    lbl.text = "edge";
    lbl.offset = 0.5;
    engine.set_label(lbl);

    CHECK(engine.has_label());
    CHECK(engine.label().text == "edge");

    auto pos = engine.compute_label_position();
    CHECK(pos.pos_x == 50.0); // midpoint of 0..100
}
