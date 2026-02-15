#include "canvas/Board.h"
#include "canvas/MindMapController.h"
#include "canvas/MindMapNode.h"
#include "canvas/UndoRedoStack.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("MindMap: create mind map", "[mind_map]")
{
    Board board;
    UndoRedoStack undo;
    MindMapController controller(board, undo);

    const auto root_id = controller.create_mind_map({100.0, 100.0}, "Central Idea");
    REQUIRE(root_id != kInvalidObjectId);

    const auto* root = dynamic_cast<const MindMapNode*>(board.get_object(root_id));
    REQUIRE(root != nullptr);
    REQUIRE(root->text() == "Central Idea");
    REQUIRE(root->depth() == 0);
}

TEST_CASE("MindMap: add child", "[mind_map]")
{
    Board board;
    UndoRedoStack undo;
    MindMapController controller(board, undo);

    const auto root_id = controller.create_mind_map({0, 0}, "Root");
    const auto child_id = controller.add_child(root_id, "Child A");
    REQUIRE(child_id != kInvalidObjectId);

    const auto* root = dynamic_cast<const MindMapNode*>(board.get_object(root_id));
    REQUIRE(root->child_node_ids().size() == 1);
    REQUIRE(root->child_node_ids()[0] == child_id);

    const auto* child = dynamic_cast<const MindMapNode*>(board.get_object(child_id));
    REQUIRE(child->parent_node_id() == root_id);
    REQUIRE(child->depth() == 1);
}

TEST_CASE("MindMap: add sibling", "[mind_map]")
{
    Board board;
    UndoRedoStack undo;
    MindMapController controller(board, undo);

    const auto root_id = controller.create_mind_map({0, 0}, "Root");
    const auto child_a = controller.add_child(root_id, "Child A");
    const auto child_b = controller.add_sibling(child_a, "Child B");
    REQUIRE(child_b != kInvalidObjectId);

    const auto* root = dynamic_cast<const MindMapNode*>(board.get_object(root_id));
    REQUIRE(root->child_node_ids().size() == 2);

    const auto* sibling = dynamic_cast<const MindMapNode*>(board.get_object(child_b));
    REQUIRE(sibling->parent_node_id() == root_id);
}

TEST_CASE("MindMap: remove branch", "[mind_map]")
{
    Board board;
    UndoRedoStack undo;
    MindMapController controller(board, undo);

    const auto root_id = controller.create_mind_map({0, 0}, "Root");
    const auto child_id = controller.add_child(root_id, "Child");
    const auto grandchild_id = controller.add_child(child_id, "Grandchild");

    controller.remove_branch(child_id);

    REQUIRE(board.get_object(child_id) == nullptr);
    REQUIRE(board.get_object(grandchild_id) == nullptr);

    const auto* root = dynamic_cast<const MindMapNode*>(board.get_object(root_id));
    REQUIRE(root->child_node_ids().empty());
}

TEST_CASE("MindMap: layout positions", "[mind_map]")
{
    Board board;
    UndoRedoStack undo;
    MindMapController controller(board, undo);

    const auto root_id = controller.create_mind_map({200, 200}, "Root");
    controller.add_child(root_id, "A");
    controller.add_child(root_id, "B");
    controller.add_child(root_id, "C");

    controller.relayout(root_id);

    // Root should still be at roughly (200, 200).
    const auto* root = dynamic_cast<const MindMapNode*>(board.get_object(root_id));
    REQUIRE(root != nullptr);
    const auto& xform = root->transform();
    REQUIRE(xform.tx == 200.0);
    REQUIRE(xform.ty == 200.0);
}

TEST_CASE("MindMap: find root", "[mind_map]")
{
    Board board;
    UndoRedoStack undo;
    MindMapController controller(board, undo);

    const auto root_id = controller.create_mind_map({0, 0}, "Root");
    const auto child_id = controller.add_child(root_id, "Child");
    const auto grandchild_id = controller.add_child(child_id, "Grandchild");

    REQUIRE(controller.find_root(grandchild_id) == root_id);
    REQUIRE(controller.find_root(child_id) == root_id);
    REQUIRE(controller.find_root(root_id) == root_id);
}

TEST_CASE("MindMap: depth-based color", "[mind_map]")
{
    Board board;
    UndoRedoStack undo;
    MindMapController controller(board, undo);

    const auto root_id = controller.create_mind_map({0, 0}, "Root");
    const auto child_id = controller.add_child(root_id, "Child");

    const auto* root = dynamic_cast<const MindMapNode*>(board.get_object(root_id));
    const auto* child = dynamic_cast<const MindMapNode*>(board.get_object(child_id));
    // Child color should differ from root.
    REQUIRE(child->node_color().r != root->node_color().r);
}

TEST_CASE("MindMap: JSON round-trip", "[mind_map]")
{
    MindMapNode node;
    node.set_text("Test Node");
    node.set_depth(2);
    node.add_child_node_id(10);
    node.add_child_node_id(20);

    const auto json = node.to_json();
    REQUIRE(json.find("\"text\":\"Test Node\"") != std::string::npos);
    REQUIRE(json.find("\"depth\":2") != std::string::npos);
    REQUIRE(json.find("[10,20]") != std::string::npos);
}
