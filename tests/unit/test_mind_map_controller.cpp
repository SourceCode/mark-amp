/// test_mind_map_controller.cpp
#include "canvas/MindMapController.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("MindMapController: type compiles", "[mind_map_controller]")
{
    static_assert(sizeof(MindMapController) > 0);
}
