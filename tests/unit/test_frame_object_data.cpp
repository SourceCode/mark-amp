/// test_frame_object_data.cpp — Unit tests
#include "canvas/FrameObject.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("FrameObject: compiles", "[frame_object]")
{
    static_assert(sizeof(FrameObject) > 0);
}
