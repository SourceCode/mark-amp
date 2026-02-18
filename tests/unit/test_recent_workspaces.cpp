/// test_recent_workspaces.cpp — Unit tests
#include "core/RecentWorkspaces.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("RecentWorkspaces: compiles", "[recent_workspaces]")
{
    static_assert(sizeof(RecentWorkspaces) > 0);
}
