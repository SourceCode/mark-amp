/// test_bookmark.cpp — Unit tests
#include "core/Bookmark.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("BookmarkEntry: compiles", "[bookmark]")
{
    static_assert(sizeof(BookmarkEntry) > 0);
}

TEST_CASE("BookmarkGroup: compiles", "[bookmark]")
{
    static_assert(sizeof(BookmarkGroup) > 0);
}
