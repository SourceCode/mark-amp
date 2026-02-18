/// test_repository_service.cpp — Unit tests

#include "core/RepositoryService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("RepositoryService: compiles", "[repository_service]")
{
    static_assert(sizeof(RepositoryService) > 0);
}
