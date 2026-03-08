/// test_async_file_loader.cpp — Unit tests for AsyncFileLoader
#include "core/AsyncFileLoader.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("FileChunk: default values", "[async_file_loader]")
{
    FileChunk chunk;
    REQUIRE(chunk.offset == 0);
    REQUIRE(chunk.data.empty());
    REQUIRE_FALSE(chunk.is_last);
}
TEST_CASE("FileChunk: field assignment", "[async_file_loader]")
{
    FileChunk chunk;
    chunk.offset = 1024;
    chunk.data = "test data";
    chunk.is_last = true;
    REQUIRE(chunk.offset == 1024);
    REQUIRE(chunk.is_last);
}
TEST_CASE("AsyncFileLoader: type compiles", "[async_file_loader]")
{
    static_assert(sizeof(AsyncFileLoader) > 0);
}
