#include "canvas/Board.h"
#include "canvas/CanvasCommands.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/UndoRedoStack.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>

using namespace markamp::canvas;

/// Minimal CanvasObject for undo/redo tests.
class UndoTestObj : public CanvasObject
{
public:
    UndoTestObj()
        : CanvasObject(CanvasObjectType::Shape)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return AABB{0.0, 0.0, 100.0, 50.0};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        auto copy = std::make_unique<UndoTestObj>();
        copy->set_transform(transform());
        copy->set_z_index(z_index());
        return copy;
    }
};

// ============================================================================
// UndoRedoStack basic tests
// ============================================================================

TEST_CASE("UndoRedoStack initial state", "[canvas][undo]")
{
    UndoRedoStack stack;
    REQUIRE_FALSE(stack.can_undo());
    REQUIRE_FALSE(stack.can_redo());
    REQUIRE(stack.undo_count() == 0);
    REQUIRE(stack.redo_count() == 0);
    REQUIRE(stack.undo_description().empty());
    REQUIRE(stack.redo_description().empty());
}

TEST_CASE("UndoRedoStack execute and undo", "[canvas][undo]")
{
    Board board;
    UndoRedoStack stack;

    auto obj = std::make_unique<UndoTestObj>();
    const auto obj_id = obj->id();

    stack.execute(std::make_unique<AddObjectCommand>(board, std::move(obj)));

    REQUIRE(board.object_count() == 1);
    REQUIRE(stack.can_undo());
    REQUIRE(stack.undo_description() == "Add Object");

    REQUIRE(stack.undo());
    REQUIRE(board.object_count() == 0);
    REQUIRE(stack.can_redo());
    REQUIRE(stack.redo_description() == "Add Object");

    REQUIRE(stack.redo());
    REQUIRE(board.object_count() == 1);
}

TEST_CASE("UndoRedoStack redo cleared on new execute", "[canvas][undo]")
{
    Board board;
    UndoRedoStack stack;

    stack.execute(std::make_unique<AddObjectCommand>(board, std::make_unique<UndoTestObj>()));
    stack.undo();
    REQUIRE(stack.can_redo());

    stack.execute(std::make_unique<AddObjectCommand>(board, std::make_unique<UndoTestObj>()));
    REQUIRE_FALSE(stack.can_redo());
}

TEST_CASE("UndoRedoStack max history", "[canvas][undo]")
{
    Board board;
    UndoRedoStack stack(3);

    for (int idx = 0; idx < 5; ++idx)
    {
        stack.execute(std::make_unique<AddObjectCommand>(board, std::make_unique<UndoTestObj>()));
    }

    REQUIRE(stack.undo_count() == 3);
}

TEST_CASE("UndoRedoStack clear", "[canvas][undo]")
{
    Board board;
    UndoRedoStack stack;

    stack.execute(std::make_unique<AddObjectCommand>(board, std::make_unique<UndoTestObj>()));
    stack.execute(std::make_unique<AddObjectCommand>(board, std::make_unique<UndoTestObj>()));

    stack.clear();
    REQUIRE_FALSE(stack.can_undo());
    REQUIRE_FALSE(stack.can_redo());
}

// ============================================================================
// MoveObjectsCommand tests
// ============================================================================

TEST_CASE("MoveObjectsCommand execute and undo", "[canvas][commands]")
{
    Board board;
    auto obj = std::make_unique<UndoTestObj>();
    const auto obj_id = obj->id();
    board.add_object(std::move(obj));
    board.clear_dirty();

    UndoRedoStack stack;
    stack.execute(
        std::make_unique<MoveObjectsCommand>(board, std::vector<ObjectId>{obj_id}, 50.0, 30.0));

    const auto* moved = board.get_object(obj_id);
    REQUIRE(moved != nullptr);
    REQUIRE(moved->transform().tx == Catch::Approx(50.0));
    REQUIRE(moved->transform().ty == Catch::Approx(30.0));

    REQUIRE(stack.undo());
    REQUIRE(board.get_object(obj_id)->transform().tx == Catch::Approx(0.0));
    REQUIRE(board.get_object(obj_id)->transform().ty == Catch::Approx(0.0));
}

// ============================================================================
// CompoundCommand tests
// ============================================================================

TEST_CASE("CompoundCommand groups multiple commands", "[canvas][commands]")
{
    Board board;

    auto compound = std::make_unique<CompoundCommand>("Add Two Objects");
    compound->add(std::make_unique<AddObjectCommand>(board, std::make_unique<UndoTestObj>()));
    compound->add(std::make_unique<AddObjectCommand>(board, std::make_unique<UndoTestObj>()));

    REQUIRE(compound->command_count() == 2);
    REQUIRE(compound->description() == "Add Two Objects");

    UndoRedoStack stack;
    stack.execute(std::move(compound));

    REQUIRE(board.object_count() == 2);

    stack.undo();
    REQUIRE(board.object_count() == 0);
}

// ============================================================================
// RemoveObjectCommand tests
// ============================================================================

TEST_CASE("RemoveObjectCommand execute and undo", "[canvas][commands]")
{
    Board board;
    auto obj = std::make_unique<UndoTestObj>();
    const auto obj_id = obj->id();
    board.add_object(std::move(obj));

    UndoRedoStack stack;
    stack.execute(std::make_unique<RemoveObjectCommand>(board, obj_id));

    REQUIRE(board.object_count() == 0);

    stack.undo();
    REQUIRE(board.object_count() == 1);
}

// ============================================================================
// ReorderZCommand tests
// ============================================================================

TEST_CASE("ReorderZCommand bring to front and undo", "[canvas][commands]")
{
    Board board;
    auto obj = std::make_unique<UndoTestObj>();
    const auto obj_id = obj->id();
    board.add_object(std::move(obj));

    const int original_z = board.get_object(obj_id)->z_index();

    UndoRedoStack stack;
    stack.execute(
        std::make_unique<ReorderZCommand>(board, obj_id, ReorderZCommand::Direction::BringToFront));

    // z_index should have changed.
    REQUIRE(board.get_object(obj_id)->z_index() != original_z);

    stack.undo();
    REQUIRE(board.get_object(obj_id)->z_index() == original_z);
}
