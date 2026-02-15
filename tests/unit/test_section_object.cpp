#include "canvas/Board.h"
#include "canvas/SectionObject.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ── Construction ────────────────────────────────────────────────

TEST_CASE("SectionObject default construction", "[section]")
{
    SectionObject section;
    REQUIRE(section.type() == CanvasObjectType::Section);
    REQUIRE(section.width() == Catch::Approx(600.0));
    REQUIRE(section.height() == Catch::Approx(400.0));
    REQUIRE(section.title() == "Section 1");
    REQUIRE(section.is_collapsed() == false);
    REQUIRE(section.tint_color().r == 227);
    REQUIRE(section.tint_color().a == 80);
}

// ── Dimensions ──────────────────────────────────────────────────

TEST_CASE("SectionObject resize with minimum", "[section]")
{
    SectionObject section;
    section.resize(50.0, 30.0);
    REQUIRE(section.width() == Catch::Approx(150.0));  // Min 150
    REQUIRE(section.height() == Catch::Approx(100.0)); // Min 100

    section.resize(800.0, 500.0);
    REQUIRE(section.width() == Catch::Approx(800.0));
    REQUIRE(section.height() == Catch::Approx(500.0));
}

TEST_CASE("SectionObject local_bounds", "[section]")
{
    SectionObject section;
    const auto bounds = section.local_bounds();
    REQUIRE(bounds.min_x == Catch::Approx(0.0));
    REQUIRE(bounds.max_x == Catch::Approx(600.0));
    REQUIRE(bounds.max_y == Catch::Approx(400.0));
}

// ── Title ───────────────────────────────────────────────────────

TEST_CASE("SectionObject title", "[section]")
{
    SectionObject section;
    section.set_title("Design Ideas");
    REQUIRE(section.title() == "Design Ideas");
}

// ── Tint Color ──────────────────────────────────────────────────

TEST_CASE("SectionObject tint_color", "[section]")
{
    SectionObject section;
    section.set_tint_color({200, 255, 200, 100});
    REQUIRE(section.tint_color().g == 255);
    REQUIRE(section.tint_color().a == 100);
}

// ── Collapse ────────────────────────────────────────────────────

TEST_CASE("SectionObject collapse", "[section]")
{
    SectionObject section;
    REQUIRE(section.is_collapsed() == false);

    section.set_collapsed(true);
    REQUIRE(section.is_collapsed() == true);

    section.set_collapsed(false);
    REQUIRE(section.is_collapsed() == false);
}

// ── Containment ─────────────────────────────────────────────────

TEST_CASE("SectionObject contains_object inside", "[section]")
{
    SectionObject section;
    section.set_position(0.0, 0.0);

    StickyNote note;
    note.set_position(50.0, 50.0);
    note.resize(100.0, 100.0);

    REQUIRE(section.contains_object(note) == true);
}

TEST_CASE("SectionObject does not contain object outside", "[section]")
{
    SectionObject section;
    section.set_position(0.0, 0.0);

    StickyNote note;
    note.set_position(700.0, 500.0);
    note.resize(100.0, 100.0);

    REQUIRE(section.contains_object(note) == false);
}

TEST_CASE("SectionObject does not contain itself", "[section]")
{
    SectionObject section;
    REQUIRE(section.contains_object(section) == false);
}

// ── collect_contained_ids ───────────────────────────────────────

TEST_CASE("SectionObject collect_contained_ids", "[section]")
{
    Board board;
    auto section = std::make_unique<SectionObject>();
    section->set_position(0.0, 0.0);
    const auto section_id = board.add_object(std::move(section));

    auto note_inside = std::make_unique<StickyNote>();
    note_inside->set_position(50.0, 50.0);
    note_inside->resize(80.0, 80.0);
    const auto inside_id = board.add_object(std::move(note_inside));

    auto note_outside = std::make_unique<StickyNote>();
    note_outside->set_position(700.0, 500.0);
    note_outside->resize(80.0, 80.0);
    board.add_object(std::move(note_outside));

    const auto* sec_ptr = static_cast<const SectionObject*>(board.get_object(section_id));
    const auto contained = sec_ptr->collect_contained_ids(board);

    REQUIRE(contained.size() == 1);
    REQUIRE(contained[0] == inside_id);
}

// ── Clone ───────────────────────────────────────────────────────

TEST_CASE("SectionObject clone", "[section]")
{
    SectionObject section;
    section.set_title("Research");
    section.set_collapsed(true);
    section.set_name("research_section");
    section.set_tint_color({100, 200, 150, 60});

    auto cloned = section.clone();
    const auto& copy = static_cast<const SectionObject&>(*cloned);

    REQUIRE(copy.title() == "Research");
    REQUIRE(copy.is_collapsed() == true);
    REQUIRE(copy.name() == "research_section");
    REQUIRE(copy.tint_color().r == 100);
    REQUIRE(copy.id() != section.id());
}

// ── JSON ────────────────────────────────────────────────────────

TEST_CASE("SectionObject to_json", "[section]")
{
    SectionObject section;
    section.set_title("Ideas");
    section.set_collapsed(true);

    const auto json = section.to_json();
    REQUIRE(json.find("Section") != std::string::npos);
    REQUIRE(json.find("Ideas") != std::string::npos);
    REQUIRE(json.find("\"collapsed\":true") != std::string::npos);
}
