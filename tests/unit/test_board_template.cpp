/// test_board_template.cpp
#include "canvas/BoardTemplate.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("BoardTemplateLibrary: type compiles", "[board_template]")
{
    static_assert(sizeof(BoardTemplateLibrary) > 0);
}
