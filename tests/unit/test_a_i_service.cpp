/// test_a_i_service.cpp — Unit tests for AIService
#include "core/AIService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("AIService: type compiles", "[ai_service]")
{
    static_assert(sizeof(AIService) > 0);
}
