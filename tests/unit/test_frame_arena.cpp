/// test_frame_arena.cpp — Unit tests
#include "core/FrameArena.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("FrameArena: compiles", "[frame_arena]")
{
    static_assert(sizeof(FrameArena) > 0);
}
