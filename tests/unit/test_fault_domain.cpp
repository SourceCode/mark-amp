/// test_fault_domain.cpp — Unit tests
#include "core/FaultDomain.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DomainHealth: enum values", "[fault_domain]")
{
    REQUIRE(static_cast<int>(DomainHealth::kHealthy) != static_cast<int>(DomainHealth::kDegraded));
}

TEST_CASE("DomainHealth: compiles", "[fault_domain]")
{
    static_assert(sizeof(DomainHealth) > 0);
}
