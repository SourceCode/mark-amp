/// test_file_node.cpp — Unit tests for FileNode
#include "core/FileNode.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("FileNode: default construction", "[file_node]")
{
    FileNode node;
    REQUIRE(node.name.empty());
    REQUIRE(node.children.empty());
}
TEST_CASE("FileNode: type compiles", "[file_node]")
{
    static_assert(sizeof(FileNode) > 0);
}
