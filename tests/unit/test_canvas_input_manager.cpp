/// test_canvas_input_manager.cpp
#include "canvas/CanvasInputManager.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("CanvasInputManager: type compiles", "[canvas_input_manager]")
{
    static_assert(sizeof(CanvasInputManager) > 0);
}
