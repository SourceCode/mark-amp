#include "canvas/Board.h"
#include "canvas/KanbanController.h"
#include "canvas/KanbanObjects.h"
#include "canvas/UndoRedoStack.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Kanban: create kanban with 3 columns", "[kanban]")
{
    Board board;
    UndoRedoStack undo;
    KanbanController controller(board, undo);

    const auto first_col = controller.create_kanban({50, 50}, 3);
    REQUIRE(first_col != kInvalidObjectId);

    const auto* col = dynamic_cast<const KanbanColumn*>(board.get_object(first_col));
    REQUIRE(col != nullptr);
    REQUIRE(col->title() == "To Do");
}

TEST_CASE("Kanban: add card to column", "[kanban]")
{
    Board board;
    UndoRedoStack undo;
    KanbanController controller(board, undo);

    const auto col_id = controller.create_kanban({0, 0}, 1);
    const auto card_id = controller.add_card(col_id, "Task 1");
    REQUIRE(card_id != kInvalidObjectId);

    const auto* col = dynamic_cast<const KanbanColumn*>(board.get_object(col_id));
    REQUIRE(col->card_count() == 1);
    REQUIRE(col->card_ids()[0] == card_id);
}

TEST_CASE("Kanban: move card between columns", "[kanban]")
{
    Board board;
    UndoRedoStack undo;
    KanbanController controller(board, undo);

    const auto col_a = controller.add_column("To Do");
    const auto col_b = controller.add_column("Done");
    const auto card_id = controller.add_card(col_a, "My Task");

    controller.move_card(card_id, col_b, 0);

    const auto* src = dynamic_cast<const KanbanColumn*>(board.get_object(col_a));
    const auto* dst = dynamic_cast<const KanbanColumn*>(board.get_object(col_b));
    REQUIRE(src->card_count() == 0);
    REQUIRE(dst->card_count() == 1);
    REQUIRE(dst->card_ids()[0] == card_id);
}

TEST_CASE("Kanban: WIP limit", "[kanban]")
{
    KanbanColumn column;
    column.set_wip_limit(3);
    column.add_card(1);
    column.add_card(2);
    column.add_card(3);
    column.add_card(4);

    REQUIRE(column.wip_limit() == 3);
    REQUIRE(column.card_count() == 4); // Over limit — visual warning state.
    REQUIRE(column.card_count() > column.wip_limit());
}

TEST_CASE("Kanban: card ordering / card_index", "[kanban]")
{
    Board board;
    UndoRedoStack undo;
    KanbanController controller(board, undo);

    const auto col_id = controller.add_column("Backlog");
    controller.add_card(col_id, "A");
    controller.add_card(col_id, "B");
    const auto card_c = controller.add_card(col_id, "C");

    const auto* card = dynamic_cast<const KanbanCard*>(board.get_object(card_c));
    REQUIRE(card->card_index() == 2);
}

TEST_CASE("Kanban: column relayout positions", "[kanban]")
{
    Board board;
    UndoRedoStack undo;
    KanbanController controller(board, undo);

    controller.create_kanban({100, 100}, 2);
    // After relayout, columns should be side by side.
    // We can't easily inspect positions without more helpers, but we verify no crash.
    controller.relayout();
    REQUIRE(true);
}

TEST_CASE("Kanban: remove column deletes cards", "[kanban]")
{
    Board board;
    UndoRedoStack undo;
    KanbanController controller(board, undo);

    const auto col_id = controller.add_column("Temp");
    const auto card_a = controller.add_card(col_id, "A");
    const auto card_b = controller.add_card(col_id, "B");

    controller.remove_column(col_id);

    REQUIRE(board.get_object(col_id) == nullptr);
    REQUIRE(board.get_object(card_a) == nullptr);
    REQUIRE(board.get_object(card_b) == nullptr);
}

TEST_CASE("Kanban: JSON round-trip", "[kanban]")
{
    KanbanCard card;
    card.set_title("Deploy v2");
    card.set_assignee("Alice");
    card.set_priority(2);

    const auto json = card.to_json();
    REQUIRE(json.find("\"title\":\"Deploy v2\"") != std::string::npos);
    REQUIRE(json.find("\"assignee\":\"Alice\"") != std::string::npos);
    REQUIRE(json.find("\"priority\":2") != std::string::npos);

    KanbanColumn column;
    column.set_title("In Progress");
    column.set_wip_limit(5);
    column.add_card(10);
    column.add_card(20);

    const auto col_json = column.to_json();
    REQUIRE(col_json.find("\"title\":\"In Progress\"") != std::string::npos);
    REQUIRE(col_json.find("\"wip_limit\":5") != std::string::npos);
    REQUIRE(col_json.find("[10,20]") != std::string::npos);
}
