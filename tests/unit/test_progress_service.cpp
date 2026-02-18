/// test_progress_service.cpp — Unit tests
#include "core/ProgressService.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ProgressLocation: enum values", "[progress_service]")
{
    REQUIRE(static_cast<uint8_t>(ProgressLocation::kNotification) != static_cast<uint8_t>(ProgressLocation::kWindow));
}

TEST_CASE("ProgressService: compiles", "[progress_service]")
{
    static_assert(sizeof(ProgressService) > 0);
}
