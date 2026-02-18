/// test_environment_service.cpp — Unit tests

#include "core/EnvironmentService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("EnvironmentService: default construction", "[environment_service]")
{
    EnvironmentService svc;
    (void)svc;
}
