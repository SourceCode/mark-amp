/// test_tag.cpp — Unit tests
#include "core/Tag.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("TagInfo: compiles", "[tag]")
{
    static_assert(sizeof(TagInfo) > 0);
}
