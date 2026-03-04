/**
 * @file test_canvas_shapes.cpp
 * @brief Phase 43: Tests for ShapePrimitiveSet and GeometryHandleController.
 */

#include "canvas/GeometryHandleController.h"
#include "canvas/ShapePrimitiveSet.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// ShapePrimitive
// ═══════════════════════════════════════════════════════

TEST_CASE("ShapePrimitive - type name", "[canvas][shapes]")
{
    ShapePrimitive shape;
    shape.type = ShapeType::kRect;
    CHECK(shape.type_name() == "rect");

    shape.type = ShapeType::kStar;
    CHECK(shape.type_name() == "star");
}

TEST_CASE("ShapePrimitive - center and area", "[canvas][shapes]")
{
    ShapePrimitive shape;
    shape.pos_x = 10.0;
    shape.pos_y = 20.0;
    shape.width = 100.0;
    shape.height = 50.0;

    CHECK(shape.center_x() == 60.0);
    CHECK(shape.center_y() == 45.0);
    CHECK(shape.area() == 5000.0);
}

// ═══════════════════════════════════════════════════════
// ShapePrimitiveSet
// ═══════════════════════════════════════════════════════

TEST_CASE("ShapePrimitiveSet - add and find", "[canvas][shapes]")
{
    ShapePrimitiveSet set;
    ShapePrimitive shape;
    shape.shape_id = "rect1";
    shape.type = ShapeType::kRect;
    set.add_shape(shape);

    CHECK(set.shape_count() == 1);
    const auto* found = set.find_shape("rect1");
    REQUIRE(found != nullptr);
    CHECK(found->type == ShapeType::kRect);
}

TEST_CASE("ShapePrimitiveSet - remove", "[canvas][shapes]")
{
    ShapePrimitiveSet set;
    ShapePrimitive shape;
    shape.shape_id = "s1";
    set.add_shape(shape);
    set.remove_shape("s1");
    CHECK(set.shape_count() == 0);
}

TEST_CASE("ShapePrimitiveSet - style inheritance", "[canvas][shapes]")
{
    ShapePrimitiveSet set;
    ShapeDefaults style;
    style.fill_color = 0xFF0000FF;
    style.stroke_width = 4.0;
    set.set_last_style(style);

    auto created = set.create_with_style("new1", ShapeType::kEllipse, 10, 20, 100, 80);
    CHECK(created.defaults.fill_color == 0xFF0000FF);
    CHECK(created.defaults.stroke_width == 4.0);
    CHECK(created.type == ShapeType::kEllipse);
}

TEST_CASE("ShapePrimitiveSet - shapes of type", "[canvas][shapes]")
{
    ShapePrimitiveSet set;
    ShapePrimitive rect;
    rect.shape_id = "r1";
    rect.type = ShapeType::kRect;
    set.add_shape(rect);

    ShapePrimitive ellipse;
    ellipse.shape_id = "e1";
    ellipse.type = ShapeType::kEllipse;
    set.add_shape(ellipse);

    auto rects = set.shapes_of_type(ShapeType::kRect);
    CHECK(rects.size() == 1);
}

// ═══════════════════════════════════════════════════════
// GeometryHandle
// ═══════════════════════════════════════════════════════

TEST_CASE("GeometryHandle - type and position names", "[canvas][handles]")
{
    GeometryHandle handle;
    handle.type = HandleType::kCorner;
    handle.position = HandlePosition::kTopLeft;
    CHECK(handle.type_name() == "corner");
    CHECK(handle.position_name() == "top_left");
}

// ═══════════════════════════════════════════════════════
// GeometryHandleController
// ═══════════════════════════════════════════════════════

TEST_CASE("GeometryHandleController - generate handles", "[canvas][handles]")
{
    GeometryHandleController ctrl;
    ctrl.generate_handles(0.0, 0.0, 100.0, 50.0);

    CHECK(ctrl.handle_count() == 10); // 4 corners + 4 edges + rotation + radius
}

TEST_CASE("GeometryHandleController - hit handle", "[canvas][handles]")
{
    GeometryHandleController ctrl;
    ctrl.generate_handles(0.0, 0.0, 100.0, 50.0);

    // Hit top-left corner at (0, 0)
    const auto* hit = ctrl.hit_handle(1.0, 1.0, 5.0);
    REQUIRE(hit != nullptr);
    CHECK(hit->position == HandlePosition::kTopLeft);

    // Miss
    const auto* miss = ctrl.hit_handle(50.0, 25.0, 3.0);
    CHECK(miss == nullptr);
}

TEST_CASE("GeometryHandleController - constrained resize", "[canvas][handles]")
{
    GeometryHandleController ctrl;
    GeometryHandleController::Bounds original{0.0, 0.0, 100.0, 100.0};

    // No constraint
    auto result = ctrl.constrained_resize(HandlePosition::kBottomRight, 20.0, 10.0, original);
    CHECK(result.width == 120.0);
    CHECK(result.height == 110.0);
}

TEST_CASE("GeometryHandleController - proportional constraint", "[canvas][handles]")
{
    GeometryHandleController ctrl;
    ctrl.set_constraint(ConstraintMode::kProportional);
    GeometryHandleController::Bounds original{0.0, 0.0, 100.0, 50.0};

    auto result = ctrl.constrained_resize(HandlePosition::kBottomRight, 20.0, 0.0, original);
    // Width = 120, aspect = 2:1, so height should be 60
    CHECK(result.width == 120.0);
    CHECK(result.height == 60.0);
}

TEST_CASE("GeometryHandleController - snap grid", "[canvas][handles]")
{
    GeometryHandleController ctrl;
    ctrl.set_snap_grid(10.0);

    CHECK(ctrl.snap_value(13.0) == 10.0);
    CHECK(ctrl.snap_value(17.0) == 20.0);
    CHECK(ctrl.snap_value(25.0) == 30.0);
}

TEST_CASE("GeometryHandleController - no snap when grid=0", "[canvas][handles]")
{
    GeometryHandleController ctrl;
    CHECK(ctrl.snap_value(13.7) == 13.7);
}
