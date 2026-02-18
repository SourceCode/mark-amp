/// test_sticky_note_object.cpp — Unit tests
#include "canvas/StickyNote.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("StickyNoteColor: enum values", "[sticky_note]")
{
    REQUIRE(static_cast<uint8_t>(StickyNoteColor::kYellow) != static_cast<uint8_t>(StickyNoteColor::kGreen));
}

TEST_CASE("StickyNote: compiles", "[sticky_note]")
{
    static_assert(sizeof(StickyNote) > 0);
}
