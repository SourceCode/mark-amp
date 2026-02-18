/// test_daily_note_service.cpp — Unit tests

#include "core/DailyNoteService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DailyNoteService: compiles", "[daily_note_service]")
{
    static_assert(sizeof(DailyNoteService) > 0);
}
