/// test_block_ref.cpp — Unit tests
#include "core/BlockRef.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("RefType: compiles", "[block_ref]")
{
    static_assert(sizeof(RefType) > 0);
}
