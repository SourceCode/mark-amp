/// test_link_service.cpp — Unit tests
#include "core/LinkService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("LinkService: type compiles", "[link_service]")
{
    static_assert(sizeof(LinkService) > 0);
}
