#include "canvas/CanvasTypes.h"
#include "canvas/ShapeData.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("ShapeObject default construction", "[canvas][shapes]")
{
    ShapeObject shape;
    REQUIRE(shape.type() == CanvasObjectType::Shape);
    REQUIRE(shape.shape_type() == ShapeType::kRectangle);
    REQUIRE(shape.width() == Catch::Approx(120.0));
    REQUIRE(shape.height() == Catch::Approx(80.0));
    REQUIRE(shape.text().empty());
}

TEST_CASE("ShapeObject typed construction", "[canvas][shapes]")
{
    ShapeObject ellipse(ShapeType::kEllipse);
    REQUIRE(ellipse.shape_type() == ShapeType::kEllipse);
}

TEST_CASE("ShapeObject set shape type", "[canvas][shapes]")
{
    ShapeObject shape;
    shape.set_shape_type(ShapeType::kDiamond);
    REQUIRE(shape.shape_type() == ShapeType::kDiamond);
}

TEST_CASE("ShapeObject style", "[canvas][shapes]")
{
    ShapeObject shape;
    ShapeStyle style;
    style.fill_color = CanvasColor{255, 0, 0, 255};
    style.stroke_width = 4.0;
    style.dash_pattern = DashPattern::kDashed;
    shape.set_style(style);

    REQUIRE(shape.style().fill_color.r == 255);
    REQUIRE(shape.style().stroke_width == Catch::Approx(4.0));
    REQUIRE(shape.style().dash_pattern == DashPattern::kDashed);
}

TEST_CASE("ShapeObject resize with min constraint", "[canvas][shapes]")
{
    ShapeObject shape;
    shape.resize(400.0, 300.0);
    REQUIRE(shape.width() == Catch::Approx(400.0));
    REQUIRE(shape.height() == Catch::Approx(300.0));

    shape.resize(2.0, 3.0);
    REQUIRE(shape.width() == Catch::Approx(10.0));
    REQUIRE(shape.height() == Catch::Approx(10.0));
}

TEST_CASE("ShapeObject embedded text", "[canvas][shapes]")
{
    ShapeObject shape;
    shape.set_text("Label");
    REQUIRE(shape.text() == "Label");

    TextStyle ts;
    ts.font_size = 20.0;
    ts.bold = true;
    shape.set_text_style(ts);
    REQUIRE(shape.text_style().font_size == Catch::Approx(20.0));
    REQUIRE(shape.text_style().bold);
}

TEST_CASE("ShapeObject local_bounds", "[canvas][shapes]")
{
    ShapeObject shape;
    shape.resize(200.0, 100.0);
    const auto bounds = shape.local_bounds();
    REQUIRE(bounds.width() == Catch::Approx(200.0));
    REQUIRE(bounds.height() == Catch::Approx(100.0));
}

TEST_CASE("ShapeObject clone", "[canvas][shapes]")
{
    ShapeObject shape(ShapeType::kHexagon);
    shape.set_text("Hex");
    shape.resize(300.0, 200.0);

    ShapeStyle style;
    style.stroke_width = 5.0;
    shape.set_style(style);

    auto cloned = shape.clone();
    const auto* cloned_shape = dynamic_cast<ShapeObject*>(cloned.get());
    REQUIRE(cloned_shape != nullptr);
    REQUIRE(cloned_shape->shape_type() == ShapeType::kHexagon);
    REQUIRE(cloned_shape->text() == "Hex");
    REQUIRE(cloned_shape->width() == Catch::Approx(300.0));
    REQUIRE(cloned_shape->style().stroke_width == Catch::Approx(5.0));
}

TEST_CASE("ShapeObject to_json", "[canvas][shapes]")
{
    ShapeObject shape(ShapeType::kTriangle);
    const auto json = shape.to_json();
    REQUIRE(json.find("Shape") != std::string::npos);
    REQUIRE(json.find("shape_type") != std::string::npos);
}

TEST_CASE("ShapeType enum values", "[canvas][shapes]")
{
    REQUIRE(static_cast<uint8_t>(ShapeType::kRectangle) == 0);
    REQUIRE(static_cast<uint8_t>(ShapeType::kTrapezoid) == 11);
}
