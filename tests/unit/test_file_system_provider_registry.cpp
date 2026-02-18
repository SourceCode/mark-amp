/// test_file_system_provider_registry.cpp — Unit tests
#include "core/FileSystemProviderRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FileType: compiles", "[file_system_provider_registry]")
{
    static_assert(sizeof(FileType) > 0);
}
