/// test_recent_files.cpp
#include "core/RecentFiles.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("RecentFiles: type compiles", "[recent_files]")
{
    static_assert(sizeof(RecentFiles) > 0);
}
TEST_CASE("RecentFiles: kMaxEntries is 20", "[recent_files]")
{
    REQUIRE(RecentFiles::kMaxEntries == 20);
}
