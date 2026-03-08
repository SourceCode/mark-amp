/// test_progress_service.cpp
#include "core/ProgressService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ProgressService: type compiles", "[progress_service]")
{
    static_assert(sizeof(ProgressService) > 0);
}
