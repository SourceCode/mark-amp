/// test_extension_storage.cpp — Unit tests

#include "core/ExtensionStorage.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ExtensionStorageService: compiles", "[extension_storage]")
{
    static_assert(sizeof(ExtensionStorageService) > 0);
}
