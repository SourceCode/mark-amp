/// test_recent_workspaces.cpp
#include "core/RecentWorkspaces.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("RecentWorkspaces: type compiles", "[recent_workspaces]")
{
    static_assert(sizeof(RecentWorkspaces) > 0);
}
TEST_CASE("RecentWorkspaces: kMaxEntries is 10", "[recent_workspaces]")
{
    REQUIRE(RecentWorkspaces::kMaxEntries == 10);
}
