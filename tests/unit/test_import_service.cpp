/// test_import_service.cpp — Unit tests
#include "core/ImportService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ImportService: type compiles", "[import_service]")
{
    static_assert(sizeof(ImportService) > 0);
}
