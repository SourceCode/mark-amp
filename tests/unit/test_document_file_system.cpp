/// test_document_file_system.cpp — Unit tests for DocumentFileSystem
#include "core/DocumentFileSystem.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("DocumentFileSystem: type compiles", "[document_file_system]")
{
    static_assert(sizeof(DocumentFileSystem) > 0);
}

TEST_CASE("DocumentFileSystem: header types available", "[document_file_system]")
{
    // Verify the key types are available
    REQUIRE(sizeof(DocumentFileSystem) > 0);
}
