#include "canvas/Board.h"
#include "canvas/FrameObject.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ── Construction ────────────────────────────────────────────────

TEST_CASE("FrameObject default construction", "[frame]")
{
    FrameObject frame;
    REQUIRE(frame.type() == CanvasObjectType::Frame);
    REQUIRE(frame.width() == Catch::Approx(800.0));
    REQUIRE(frame.height() == Catch::Approx(600.0));
    REQUIRE(frame.title() == "Frame 1");
    REQUIRE(frame.show_title() == true);
    REQUIRE(frame.sequence_number() == 1);
}

// ── Dimensions ──────────────────────────────────────────────────

TEST_CASE("FrameObject resize with minimum", "[frame]")
{
    FrameObject frame;
    frame.resize(50.0, 50.0);
    REQUIRE(frame.width() == Catch::Approx(100.0)); // Min 100
    REQUIRE(frame.height() == Catch::Approx(100.0));

    frame.resize(1200.0, 900.0);
    REQUIRE(frame.width() == Catch::Approx(1200.0));
    REQUIRE(frame.height() == Catch::Approx(900.0));
}

TEST_CASE("FrameObject local_bounds", "[frame]")
{
    FrameObject frame;
    const auto bounds = frame.local_bounds();
    REQUIRE(bounds.min_x == Catch::Approx(0.0));
    REQUIRE(bounds.min_y == Catch::Approx(0.0));
    REQUIRE(bounds.max_x == Catch::Approx(800.0));
    REQUIRE(bounds.max_y == Catch::Approx(600.0));
}

// ── Title & Sequence ────────────────────────────────────────────

TEST_CASE("FrameObject title and sequence", "[frame]")
{
    FrameObject frame;
    frame.set_title("Slide 1");
    frame.set_sequence_number(3);
    frame.set_show_title(false);

    REQUIRE(frame.title() == "Slide 1");
    REQUIRE(frame.sequence_number() == 3);
    REQUIRE(frame.show_title() == false);
}

// ── Colors ──────────────────────────────────────────────────────

TEST_CASE("FrameObject colors", "[frame]")
{
    FrameObject frame;
    frame.set_background_color({240, 240, 255, 255});
    frame.set_border_color({0, 0, 128, 255});

    REQUIRE(frame.background_color().b == 255);
    REQUIRE(frame.border_color().b == 128);
}

// ── Containment ─────────────────────────────────────────────────

TEST_CASE("FrameObject contains_object", "[frame]")
{
    FrameObject frame;
    frame.set_position(0.0, 0.0);

    StickyNote note;
    // Place note fully inside frame (frame is 800x600 at origin).
    note.set_position(100.0, 100.0);
    note.resize(100.0, 100.0);

    REQUIRE(frame.contains_object(note) == true);
}

TEST_CASE("FrameObject does not contain object outside", "[frame]")
{
    FrameObject frame;
    frame.set_position(0.0, 0.0);

    StickyNote note;
    // Place note outside the frame.
    note.set_position(900.0, 700.0);
    note.resize(100.0, 100.0);

    REQUIRE(frame.contains_object(note) == false);
}

TEST_CASE("FrameObject does not contain itself", "[frame]")
{
    FrameObject frame;
    REQUIRE(frame.contains_object(frame) == false);
}

// ── collect_contained_ids ───────────────────────────────────────

TEST_CASE("FrameObject collect_contained_ids", "[frame]")
{
    Board board;
    auto frame = std::make_unique<FrameObject>();
    frame->set_position(0.0, 0.0);
    const auto frame_id = board.add_object(std::move(frame));

    auto note_inside = std::make_unique<StickyNote>();
    note_inside->set_position(100.0, 100.0);
    note_inside->resize(80.0, 80.0);
    const auto inside_id = board.add_object(std::move(note_inside));

    auto note_outside = std::make_unique<StickyNote>();
    note_outside->set_position(900.0, 100.0);
    note_outside->resize(80.0, 80.0);
    board.add_object(std::move(note_outside));

    const auto* frame_ptr = static_cast<const FrameObject*>(board.get_object(frame_id));
    const auto contained = frame_ptr->collect_contained_ids(board);

    REQUIRE(contained.size() == 1);
    REQUIRE(contained[0] == inside_id);
}

// ── Clone ───────────────────────────────────────────────────────

TEST_CASE("FrameObject clone", "[frame]")
{
    FrameObject frame;
    frame.set_title("Presentation 1");
    frame.set_sequence_number(5);
    frame.set_name("slide");
    frame.resize(1024.0, 768.0);

    auto cloned = frame.clone();
    const auto& copy = static_cast<const FrameObject&>(*cloned);

    REQUIRE(copy.title() == "Presentation 1");
    REQUIRE(copy.sequence_number() == 5);
    REQUIRE(copy.name() == "slide");
    REQUIRE(copy.width() == Catch::Approx(1024.0));
    REQUIRE(copy.id() != frame.id());
}

// ── JSON ────────────────────────────────────────────────────────

TEST_CASE("FrameObject to_json", "[frame]")
{
    FrameObject frame;
    frame.set_title("Slide A");

    const auto json = frame.to_json();
    REQUIRE(json.find("Frame") != std::string::npos);
    REQUIRE(json.find("Slide A") != std::string::npos);
    REQUIRE(json.find("sequence") != std::string::npos);
}
