/// test_tag_service.cpp
#include "core/TagService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("TagService: type compiles", "[tag_service]")
{
    static_assert(sizeof(TagService) > 0);
}
