/// test_diagram_shape_data.cpp
#include "canvas/DiagramShapeObject.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("DiagramShapeObject: type compiles", "[diagram_shape_data]")
{
    static_assert(sizeof(DiagramShapeObject) > 0);
}
