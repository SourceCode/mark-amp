/// test_canvas_object.cpp — Unit tests
#include "canvas/CanvasObject.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("BlendMode: enum values", "[canvas_object]")
{
    REQUIRE(static_cast<uint8_t>(BlendMode::kNormal) != static_cast<uint8_t>(BlendMode::kMultiply));
}

TEST_CASE("ObjectBorder: default values", "[canvas_object]")
{
    ObjectBorder border;
    REQUIRE(border.width == 1.0f);
}
