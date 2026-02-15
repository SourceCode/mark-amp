#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/ConnectorData.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>

using namespace markamp::canvas;

/// Minimal CanvasObject for connector endpoint resolution tests.
class ConnTestObj : public CanvasObject
{
public:
    ConnTestObj()
        : CanvasObject(CanvasObjectType::Shape)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return AABB{0.0, 0.0, 100.0, 50.0};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        auto copy = std::make_unique<ConnTestObj>();
        copy->set_transform(transform());
        return copy;
    }
};

TEST_CASE("ConnectorObject default construction", "[canvas][connector]")
{
    ConnectorObject conn;
    REQUIRE(conn.type() == CanvasObjectType::Connector);
    REQUIRE(conn.line_style() == ConnectorLineStyle::kSolid);
    REQUIRE(conn.line_width() == Catch::Approx(2.0));
    REQUIRE(conn.start_arrow() == ArrowheadStyle::kNone);
    REQUIRE(conn.end_arrow() == ArrowheadStyle::kNone);
    REQUIRE(conn.label().empty());
    REQUIRE(conn.waypoints().empty());
}

TEST_CASE("ConnectorObject free endpoints", "[canvas][connector]")
{
    ConnectorObject conn;
    conn.set_free_start(Point2D{10.0, 20.0});
    conn.set_free_end(Point2D{200.0, 150.0});

    REQUIRE_FALSE(conn.start_endpoint().is_attached());
    REQUIRE_FALSE(conn.end_endpoint().is_attached());
    REQUIRE(conn.start_endpoint().offset.x == Catch::Approx(10.0));
    REQUIRE(conn.end_endpoint().offset.x == Catch::Approx(200.0));
}

TEST_CASE("ConnectorObject attached endpoints resolve", "[canvas][connector]")
{
    Board board;
    auto obj = std::make_unique<ConnTestObj>();
    const auto obj_id = obj->id();
    board.add_object(std::move(obj));

    ConnectorObject conn;
    conn.set_start_object(obj_id, AnchorPosition::kTop);
    conn.set_free_end(Point2D{300.0, 300.0});

    REQUIRE(conn.start_endpoint().is_attached());

    const auto resolved = conn.resolve_start(board);
    // Object at (0,0), size 100x50 → top anchor = (50, 0).
    REQUIRE(resolved.x == Catch::Approx(50.0));
    REQUIRE(resolved.y == Catch::Approx(0.0));
}

TEST_CASE("ConnectorObject anchor positions", "[canvas][connector]")
{
    Board board;
    auto obj = std::make_unique<ConnTestObj>();
    const auto obj_id = obj->id();

    Transform2D xform;
    xform.tx = 100.0;
    xform.ty = 100.0;
    obj->set_transform(xform);

    board.add_object(std::move(obj));

    ConnectorObject conn;

    // Test all anchor positions.
    conn.set_start_object(obj_id, AnchorPosition::kBottom);
    auto resolved = conn.resolve_start(board);
    // Object at (100,100), size 100x50 → bottom = (150, 150).
    REQUIRE(resolved.x == Catch::Approx(150.0));
    REQUIRE(resolved.y == Catch::Approx(150.0));

    conn.set_start_object(obj_id, AnchorPosition::kLeft);
    resolved = conn.resolve_start(board);
    REQUIRE(resolved.x == Catch::Approx(100.0));
    REQUIRE(resolved.y == Catch::Approx(125.0));

    conn.set_start_object(obj_id, AnchorPosition::kRight);
    resolved = conn.resolve_start(board);
    REQUIRE(resolved.x == Catch::Approx(200.0));
    REQUIRE(resolved.y == Catch::Approx(125.0));
}

TEST_CASE("ConnectorObject waypoints", "[canvas][connector]")
{
    ConnectorObject conn;
    conn.add_waypoint(Point2D{50.0, 50.0});
    conn.add_waypoint(Point2D{100.0, 100.0});
    REQUIRE(conn.waypoints().size() == 2);

    conn.clear_waypoints();
    REQUIRE(conn.waypoints().empty());
}

TEST_CASE("ConnectorObject line style and arrows", "[canvas][connector]")
{
    ConnectorObject conn;
    conn.set_line_style(ConnectorLineStyle::kDashed);
    conn.set_line_width(4.0);
    conn.set_line_color(CanvasColor{255, 0, 0, 255});
    conn.set_start_arrow(ArrowheadStyle::kArrow);
    conn.set_end_arrow(ArrowheadStyle::kFilledArrow);

    REQUIRE(conn.line_style() == ConnectorLineStyle::kDashed);
    REQUIRE(conn.line_width() == Catch::Approx(4.0));
    REQUIRE(conn.line_color().r == 255);
    REQUIRE(conn.start_arrow() == ArrowheadStyle::kArrow);
    REQUIRE(conn.end_arrow() == ArrowheadStyle::kFilledArrow);
}

TEST_CASE("ConnectorObject label", "[canvas][connector]")
{
    ConnectorObject conn;
    conn.set_label("Connection A→B");
    REQUIRE(conn.label() == "Connection A→B");
}

TEST_CASE("ConnectorObject clone", "[canvas][connector]")
{
    ConnectorObject conn;
    conn.set_free_start(Point2D{10.0, 20.0});
    conn.set_free_end(Point2D{300.0, 400.0});
    conn.set_label("Test");
    conn.set_end_arrow(ArrowheadStyle::kFilledArrow);
    conn.add_waypoint(Point2D{150.0, 200.0});

    auto cloned = conn.clone();
    const auto* cloned_conn = dynamic_cast<ConnectorObject*>(cloned.get());
    REQUIRE(cloned_conn != nullptr);
    REQUIRE(cloned_conn->label() == "Test");
    REQUIRE(cloned_conn->end_arrow() == ArrowheadStyle::kFilledArrow);
    REQUIRE(cloned_conn->waypoints().size() == 1);
}
