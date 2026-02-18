/// test_kanban_objects.cpp — Unit tests
#include "canvas/KanbanObjects.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("KanbanSortOrder: enum values", "[kanban_objects]")
{
    REQUIRE(static_cast<int>(KanbanSortOrder::kManual) != static_cast<int>(KanbanSortOrder::kPriority));
}

TEST_CASE("KanbanSortOrder: compiles", "[kanban_objects]")
{
    static_assert(sizeof(KanbanSortOrder) > 0);
}

TEST_CASE("KanbanCard: compiles", "[kanban_objects]")
{
    static_assert(sizeof(KanbanCard) > 0);
}

TEST_CASE("KanbanColumn: compiles", "[kanban_objects]")
{
    static_assert(sizeof(KanbanColumn) > 0);
}
