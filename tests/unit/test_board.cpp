/// test_board.cpp — Unit tests
#include "canvas/Board.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("Board: default construction", "[board]")
{
    Board board;
    REQUIRE(board.object_count() == 0);
}
