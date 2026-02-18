/// test_icon_object.cpp — Unit tests
#include "canvas/IconObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("IconObject: compiles", "[icon_object]")
{
    static_assert(sizeof(IconObject) > 0);
}
