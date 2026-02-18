/// test_canvas_commands.cpp — Unit tests

#include "canvas/CanvasCommands.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("CompoundCommand: compiles", "[canvas_commands]")
{
    static_assert(sizeof(CompoundCommand) > 0);
}
