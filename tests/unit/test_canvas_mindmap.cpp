// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/MindMapModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Add child and sibling nodes", "[mindmap][node]")
{
    MindMapModel model;
    model.set_nodes({{"root", "Root", "", false, 0, ""}});
    model.add_child("root", "c1", "Child 1");
    model.add_sibling("c1", "c2", "Child 2");
    REQUIRE(model.node_count() == 3);
    REQUIRE(model.children_of("root").size() == 2);
}

TEST_CASE("Remove node cascades to children", "[mindmap][node]")
{
    MindMapModel model;
    model.set_nodes({{"root", "Root", "", false, 0, ""}});
    model.add_child("root", "c1", "Child");
    model.add_child("c1", "gc1", "Grandchild");
    model.remove_node("c1");
    REQUIRE(model.node_count() == 1); // only root remains
}

TEST_CASE("Collapse/expand toggle", "[mindmap][collapse]")
{
    MindMapModel model;
    model.set_nodes({{"n1", "Node", "", false, 0, ""}});
    REQUIRE_FALSE(model.is_collapsed("n1"));
    model.toggle_collapse("n1");
    REQUIRE(model.is_collapsed("n1"));
    model.toggle_collapse("n1");
    REQUIRE_FALSE(model.is_collapsed("n1"));
}

TEST_CASE("Layout modes", "[mindmap][layout]")
{
    MindMapModel model;
    REQUIRE(model.layout() == MindMapLayout::kRadial);
    model.set_layout(MindMapLayout::kDownward);
    REQUIRE(model.layout() == MindMapLayout::kDownward);
}

TEST_CASE("Depth color cycling", "[mindmap][color]")
{
    MindMapModel model;
    model.set_depth_colors({"#FF0000", "#00FF00", "#0000FF"});
    REQUIRE(model.color_for_depth(0) == "#FF0000");
    REQUIRE(model.color_for_depth(3) == "#FF0000"); // wraps
    REQUIRE(model.color_for_depth(1) == "#00FF00");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
