/// test_recent_files.cpp — Unit tests
#include "core/RecentFiles.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("RecentFiles: compiles", "[recent_files]")
{
    static_assert(sizeof(RecentFiles) > 0);
}
