/// test_file_node.cpp — Unit tests
#include "core/FileNode.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("FileNodeType: enum values", "[file_node]")
{
    REQUIRE(static_cast<int>(FileNodeType::File) != static_cast<int>(FileNodeType::Folder));
}
