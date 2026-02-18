/// test_undo_redo_stack.cpp — Unit tests

#include "canvas/UndoRedoStack.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("UndoRedoStack: starts empty", "[undo_redo_stack]")
{
    UndoRedoStack stack;
    REQUIRE_FALSE(stack.can_undo());
    REQUIRE_FALSE(stack.can_redo());
}

TEST_CASE("UndoRedoStack: clear", "[undo_redo_stack]")
{
    UndoRedoStack stack;
    stack.clear();
    REQUIRE_FALSE(stack.can_undo());
}
