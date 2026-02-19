// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/UndoRedoModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Push undo and redo", "[undo][basic]")
{
    UndoRedoModel model;
    model.push({"Move object", "move", 100});
    model.push({"Resize object", "resize", 200});
    REQUIRE(model.undo_count() == 2);
    REQUIRE(model.can_undo());
    model.undo();
    REQUIRE(model.undo_count() == 1);
    REQUIRE(model.can_redo());
    model.redo();
    REQUIRE(model.undo_count() == 2);
}

TEST_CASE("History labels", "[undo][metadata]")
{
    UndoRedoModel model;
    model.push({"Move", "move", 10});
    model.push({"Delete", "delete", 20});
    REQUIRE(model.next_undo_label() == "Delete");
    model.undo();
    REQUIRE(model.next_redo_label() == "Delete");
    REQUIRE(model.next_undo_label() == "Move");
}

TEST_CASE("Compound transaction atomicity", "[undo][compound]")
{
    UndoRedoModel model;
    model.begin_compound("Align objects");
    REQUIRE(model.is_in_compound());
    model.push({"move-a", "move", 50});
    model.push({"move-b", "move", 50});
    model.push({"move-c", "move", 50});
    model.end_compound();
    REQUIRE_FALSE(model.is_in_compound());
    // All 3 sub-actions collapsed into one compound entry
    REQUIRE(model.undo_count() == 1);
    REQUIRE(model.next_undo_label() == "Align objects");
}

TEST_CASE("Step limit enforcement", "[undo][budget]")
{
    UndoRedoModel model;
    model.set_max_steps(3);
    model.push({"A", "t", 10});
    model.push({"B", "t", 10});
    model.push({"C", "t", 10});
    model.push({"D", "t", 10});
    REQUIRE(model.undo_count() == 3); // oldest dropped
}

TEST_CASE("New push clears redo stack", "[undo][redo]")
{
    UndoRedoModel model;
    model.push({"A", "t", 10});
    model.push({"B", "t", 10});
    model.undo();
    REQUIRE(model.can_redo());
    model.push({"C", "t", 10});
    REQUIRE_FALSE(model.can_redo()); // redo stack cleared
}

TEST_CASE("Clear all stacks", "[undo][clear]")
{
    UndoRedoModel model;
    model.push({"A", "t", 10});
    model.undo();
    model.clear();
    REQUIRE_FALSE(model.can_undo());
    REQUIRE_FALSE(model.can_redo());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
