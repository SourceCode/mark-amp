/// test_board.cpp
#include "canvas/Board.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("Board: type compiles", "[board]")
{
    static_assert(sizeof(Board) > 0);
}
