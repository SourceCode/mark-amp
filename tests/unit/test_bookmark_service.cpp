/// test_bookmark_service.cpp — Unit tests

#include "core/BookmarkService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BookmarkService: compiles", "[bookmark_service]")
{
    static_assert(sizeof(BookmarkService) > 0);
}
