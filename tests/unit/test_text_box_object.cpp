/// test_text_box_object.cpp
#include "canvas/TextBox.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("TextBox: type compiles", "[text_box_object]")
{
    static_assert(sizeof(TextBox) > 0);
}
