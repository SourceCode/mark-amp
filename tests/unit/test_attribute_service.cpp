/// test_attribute_service.cpp — Unit tests

#include "core/AttributeService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AttributeService: compiles", "[attribute_service]")
{
    static_assert(sizeof(AttributeService) > 0);
}
