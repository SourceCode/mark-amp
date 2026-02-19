// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/KanbanModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Lane and card management", "[kanban][basic]")
{
    KanbanModel model;
    model.add_lane({"todo", "To Do", 0, 250.0});
    model.add_lane({"done", "Done", 1, 250.0});
    model.add_card({"c1", "Task 1", "todo", 0, CardPriority::kHigh, "", ""});
    model.add_card({"c2", "Task 2", "todo", 1, CardPriority::kLow, "", ""});
    REQUIRE(model.card_count_in_lane("todo") == 2);
    REQUIRE(model.card_count_in_lane("done") == 0);
}

TEST_CASE("Move card across lanes", "[kanban][move]")
{
    KanbanModel model;
    model.add_lane({"todo", "To Do", 0, 250.0});
    model.add_lane({"done", "Done", 1, 250.0});
    model.add_card({"c1", "Task 1", "todo", 0, CardPriority::kNone, "", ""});
    model.move_card("c1", "done", 0);
    REQUIRE(model.card_count_in_lane("todo") == 0);
    REQUIRE(model.card_count_in_lane("done") == 1);
}

TEST_CASE("Remove lane cascades cards", "[kanban][remove]")
{
    KanbanModel model;
    model.add_lane({"todo", "To Do", 0, 250.0});
    model.add_card({"c1", "Task 1", "todo", 0, CardPriority::kNone, "", ""});
    model.remove_lane("todo");
    REQUIRE(model.lanes().empty());
    REQUIRE(model.cards().empty());
}

TEST_CASE("Cards in lane sorted by position", "[kanban][sort]")
{
    KanbanModel model;
    model.add_lane({"todo", "To Do", 0, 250.0});
    model.add_card({"c2", "Second", "todo", 2, CardPriority::kNone, "", ""});
    model.add_card({"c1", "First", "todo", 1, CardPriority::kNone, "", ""});
    const auto cards = model.cards_in_lane("todo");
    REQUIRE(cards[0].card_id == "c1");
    REQUIRE(cards[1].card_id == "c2");
}

TEST_CASE("Remove card", "[kanban][remove]")
{
    KanbanModel model;
    model.add_lane({"todo", "To Do", 0, 250.0});
    model.add_card({"c1", "Task", "todo", 0, CardPriority::kNone, "", ""});
    model.remove_card("c1");
    REQUIRE(model.cards().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
