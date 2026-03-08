/// test_canvas_commands.cpp
#include "canvas/CanvasCommands.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("AddObjectCommand: type compiles", "[canvas_commands]")
{
    static_assert(sizeof(AddObjectCommand) > 0);
}
