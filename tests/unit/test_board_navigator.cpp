/// test_board_navigator.cpp
#include "canvas/BoardNavigator.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("BoardNavigator: type compiles", "[board_navigator]")
{
    static_assert(sizeof(BoardNavigator) > 0);
}
