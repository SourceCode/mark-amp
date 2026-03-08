/// test_block_service.cpp
#include "core/BlockService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("BlockService: type compiles", "[block_service]")
{
    static_assert(sizeof(BlockService) > 0);
}
