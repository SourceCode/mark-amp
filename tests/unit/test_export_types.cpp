/// test_export_types.cpp — Unit tests
#include "core/ExportTypes.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ExportFormat: compiles", "[export_types]")
{
    static_assert(sizeof(ExportFormat) > 0);
}

TEST_CASE("ExportOptions: compiles", "[export_types]")
{
    static_assert(sizeof(ExportOptions) > 0);
}
