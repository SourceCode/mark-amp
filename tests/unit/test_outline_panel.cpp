#include "canvas/Board.h"
#include "canvas/FrameObject.h"
#include "canvas/OutlinePanel.h"
#include "canvas/SectionObject.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("OutlineBuilder: build entries from board", "[outline_panel]")
{
    Board board;
    auto frame = std::make_unique<FrameObject>();
    frame->set_title("Slide 1");
    frame->set_position(0.0, 0.0);
    frame->set_scale(500.0, 500.0);
    board.add_object(std::move(frame));

    auto sticky = std::make_unique<StickyNote>();
    sticky->set_name("Note 1");
    sticky->set_position(100.0, 100.0);
    board.add_object(std::move(sticky));

    auto entries = OutlineBuilder::build_entries(board);
    // Frame + contained sticky = at least 2 entries.
    REQUIRE(entries.size() >= 2);
    REQUIRE(entries[0].type == CanvasObjectType::Frame);
}

TEST_CASE("OutlineBuilder: frame children are indented", "[outline_panel]")
{
    Board board;
    auto frame = std::make_unique<FrameObject>();
    frame->set_title("Container");
    frame->set_position(0.0, 0.0);
    frame->set_scale(1000.0, 1000.0);
    board.add_object(std::move(frame));

    auto sticky = std::make_unique<StickyNote>();
    sticky->set_name("Inside");
    sticky->set_position(100.0, 100.0);
    board.add_object(std::move(sticky));

    auto entries = OutlineBuilder::build_entries(board);
    REQUIRE(entries.size() >= 2);
    REQUIRE(entries[0].indent_level == 0);
    REQUIRE(entries[1].indent_level == 1);
}

TEST_CASE("OutlinePanel: collapse hides children", "[outline_panel]")
{
    OutlinePanel panel;
    std::vector<OutlineEntry> entries = {
        {.id = 1,
         .label = "Frame",
         .type = CanvasObjectType::Frame,
         .indent_level = 0,
         .is_expanded = true,
         .is_container = true,
         .child_count = 2},
        {.id = 2,
         .label = "Child 1",
         .type = CanvasObjectType::StickyNote,
         .indent_level = 1,
         .is_expanded = true,
         .is_container = false,
         .child_count = 0},
        {.id = 3,
         .label = "Child 2",
         .type = CanvasObjectType::TextBox,
         .indent_level = 1,
         .is_expanded = true,
         .is_container = false,
         .child_count = 0},
        {.id = 4,
         .label = "Top level",
         .type = CanvasObjectType::Shape,
         .indent_level = 0,
         .is_expanded = true,
         .is_container = false,
         .child_count = 0},
    };
    panel.set_entries(entries);

    // All visible initially.
    REQUIRE(panel.visible_entries().size() == 4);

    // Collapse the frame.
    panel.toggle_expand(1);
    auto visible = panel.visible_entries();
    REQUIRE(visible.size() == 2); // Frame + top level
    REQUIRE(visible[0]->id == 1);
    REQUIRE(visible[1]->id == 4);
}

TEST_CASE("OutlinePanel: navigate callback fires", "[outline_panel]")
{
    OutlinePanel panel;
    ObjectId navigated = kInvalidObjectId;
    panel.set_on_navigate([&](ObjectId target) { navigated = target; });

    panel.navigate_to(42);
    REQUIRE(navigated == 42);
}

TEST_CASE("OutlineBuilder: frames ordered by z-index", "[outline_panel]")
{
    Board board;
    auto frame1 = std::make_unique<FrameObject>();
    frame1->set_title("Second");
    frame1->set_z_index(10);
    board.add_object(std::move(frame1));

    auto frame2 = std::make_unique<FrameObject>();
    frame2->set_title("First");
    frame2->set_z_index(5);
    board.add_object(std::move(frame2));

    auto entries = OutlineBuilder::build_entries(board);
    size_t frame_count = 0;
    for (const auto& entry : entries)
    {
        if (entry.type == CanvasObjectType::Frame)
        {
            ++frame_count;
        }
    }
    REQUIRE(frame_count == 2);
    // First frame entry should be "First" (lower z-index).
    REQUIRE(entries[0].label == "First");
}

TEST_CASE("OutlineBuilder: uncontained objects at root level", "[outline_panel]")
{
    Board board;
    auto sticky = std::make_unique<StickyNote>();
    sticky->set_name("Orphan");
    sticky->set_position(5000.0, 5000.0); // Far from any frame
    board.add_object(std::move(sticky));

    auto entries = OutlineBuilder::build_entries(board);
    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].indent_level == 0);
    REQUIRE(entries[0].label == "Orphan");
}
