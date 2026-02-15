#include "canvas/CanvasTypes.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("StickyNote default construction", "[canvas][sticky]")
{
    StickyNote note;
    REQUIRE(note.type() == CanvasObjectType::StickyNote);
    REQUIRE(note.text().empty());
    REQUIRE(note.note_color() == StickyNoteColor::kYellow);
    REQUIRE(note.font_size() == Catch::Approx(14.0));
    REQUIRE(note.width() == Catch::Approx(200.0));
    REQUIRE(note.height() == Catch::Approx(200.0));
}

TEST_CASE("StickyNote set text", "[canvas][sticky]")
{
    StickyNote note;
    note.set_text("Hello World");
    REQUIRE(note.text() == "Hello World");
}

TEST_CASE("StickyNote set color", "[canvas][sticky]")
{
    StickyNote note;
    note.set_note_color(StickyNoteColor::kPink);
    REQUIRE(note.note_color() == StickyNoteColor::kPink);
}

TEST_CASE("StickyNote resize with min constraint", "[canvas][sticky]")
{
    StickyNote note;
    note.resize(300.0, 150.0);
    REQUIRE(note.width() == Catch::Approx(300.0));
    REQUIRE(note.height() == Catch::Approx(150.0));

    // Below minimum.
    note.resize(10.0, 20.0);
    REQUIRE(note.width() == Catch::Approx(80.0));
    REQUIRE(note.height() == Catch::Approx(80.0));
}

TEST_CASE("StickyNote local_bounds", "[canvas][sticky]")
{
    StickyNote note;
    note.resize(250.0, 180.0);
    const auto bounds = note.local_bounds();
    REQUIRE(bounds.min_x == Catch::Approx(0.0));
    REQUIRE(bounds.min_y == Catch::Approx(0.0));
    REQUIRE(bounds.width() == Catch::Approx(250.0));
    REQUIRE(bounds.height() == Catch::Approx(180.0));
}

TEST_CASE("StickyNote clone preserves properties", "[canvas][sticky]")
{
    StickyNote note;
    note.set_text("Clone me");
    note.set_note_color(StickyNoteColor::kBlue);
    note.set_font_size(18.0);
    note.resize(300.0, 250.0);

    auto cloned = note.clone();
    const auto* cloned_note = dynamic_cast<StickyNote*>(cloned.get());
    REQUIRE(cloned_note != nullptr);
    REQUIRE(cloned_note->text() == "Clone me");
    REQUIRE(cloned_note->note_color() == StickyNoteColor::kBlue);
    REQUIRE(cloned_note->font_size() == Catch::Approx(18.0));
    REQUIRE(cloned_note->width() == Catch::Approx(300.0));
    REQUIRE(cloned_note->height() == Catch::Approx(250.0));
}

TEST_CASE("StickyNote color_to_rgba", "[canvas][sticky]")
{
    const auto yellow = sticky_color_to_rgba(StickyNoteColor::kYellow);
    REQUIRE(yellow.r == 255);
    REQUIRE(yellow.g == 249);
    REQUIRE(yellow.b == 196);
    REQUIRE(yellow.a == 255);

    const auto cyan = sticky_color_to_rgba(StickyNoteColor::kCyan);
    REQUIRE(cyan.r == 178);
    REQUIRE(cyan.g == 235);
    REQUIRE(cyan.b == 242);
}

TEST_CASE("StickyNote to_json", "[canvas][sticky]")
{
    StickyNote note;
    note.set_text("Test");
    const auto json = note.to_json();
    REQUIRE(json.find("StickyNote") != std::string::npos);
    REQUIRE(json.find("Test") != std::string::npos);
}
