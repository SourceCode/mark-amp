/// test_context_key_service.cpp — Unit tests

#include "core/ContextKeyService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ContextKeyService: compiles", "[context_key_service]")
{
    static_assert(sizeof(ContextKeyService) > 0);
}
