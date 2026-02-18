/// test_text_box_object.cpp — Unit tests
#include "canvas/TextBox.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("TextBox: compiles", "[text_box]")
{
    static_assert(sizeof(TextBox) > 0);
}
