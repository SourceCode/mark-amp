/// test_bookmark_card_object.cpp
#include "canvas/BookmarkCardObject.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("BookmarkCardObject: type compiles", "[bookmark_card_object]")
{
    static_assert(sizeof(BookmarkCardObject) > 0);
}
