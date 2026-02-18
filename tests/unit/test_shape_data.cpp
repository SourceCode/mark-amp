/// test_shape_data.cpp — Unit tests
#include "canvas/ShapeData.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("ShapeType: enum values", "[shape_data]")
{
    REQUIRE(static_cast<uint8_t>(ShapeType::kRectangle) != static_cast<uint8_t>(ShapeType::kEllipse));
}

TEST_CASE("ShapeObject: compiles", "[shape_data]")
{
    static_assert(sizeof(ShapeObject) > 0);
}
