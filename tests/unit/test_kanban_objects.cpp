/// test_kanban_objects.cpp
#include "canvas/KanbanObjects.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("KanbanObjects: type compiles", "[kanban_objects]")
{
    static_assert(sizeof(KanbanColumn) > 0);
}
