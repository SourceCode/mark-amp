/// test_mind_map_node.cpp
#include "canvas/MindMapNode.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("MindMapNode: type compiles", "[mind_map_node]")
{
    static_assert(sizeof(MindMapNode) > 0);
}
