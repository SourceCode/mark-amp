/// test_daily_note_service.cpp — Unit tests for DailyNoteService
#include "core/DailyNoteService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("DailyNoteService: type compiles", "[daily_note_service]")
{
    static_assert(sizeof(DailyNoteService) > 0);
}
