#include "canvas/Board.h"
#include "canvas/GroupObject.h"
#include "canvas/GroupingService.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::canvas;

TEST_CASE("GroupObject — default construction", "[canvas][group]")
{
    GroupObject group;
    REQUIRE(group.type() == CanvasObjectType::Group);
    REQUIRE(group.child_count() == 0);
    REQUIRE(group.children().empty());
}

TEST_CASE("GroupObject — add and remove children", "[canvas][group]")
{
    GroupObject group;
    group.add_child(10);
    group.add_child(20);
    group.add_child(30);

    REQUIRE(group.child_count() == 3);
    REQUIRE(group.contains_child(10));
    REQUIRE(group.contains_child(20));
    REQUIRE(group.contains_child(30));
    REQUIRE_FALSE(group.contains_child(99));

    // Duplicate add is ignored.
    group.add_child(10);
    REQUIRE(group.child_count() == 3);

    // Invalid ID is ignored.
    group.add_child(kInvalidObjectId);
    REQUIRE(group.child_count() == 3);

    // Remove one.
    group.remove_child(20);
    REQUIRE(group.child_count() == 2);
    REQUIRE_FALSE(group.contains_child(20));

    // Remove non-existent is no-op.
    group.remove_child(999);
    REQUIRE(group.child_count() == 2);
}

TEST_CASE("GroupObject — update_bounds from Board", "[canvas][group]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    note1->set_position(0.0, 0.0);
    note1->resize(100.0, 100.0);
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    note2->set_position(200.0, 200.0);
    note2->resize(100.0, 100.0);
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    GroupObject group;
    group.add_child(id1);
    group.add_child(id2);
    group.update_bounds(board);

    const auto bounds = group.local_bounds();
    // Should encompass both notes: (0,0) to (300,300).
    REQUIRE_THAT(bounds.min_x, Catch::Matchers::WithinAbs(0.0, 0.1));
    REQUIRE_THAT(bounds.min_y, Catch::Matchers::WithinAbs(0.0, 0.1));
    REQUIRE_THAT(bounds.max_x, Catch::Matchers::WithinAbs(300.0, 0.1));
    REQUIRE_THAT(bounds.max_y, Catch::Matchers::WithinAbs(300.0, 0.1));
}

TEST_CASE("GroupObject — empty group bounds", "[canvas][group]")
{
    GroupObject group;
    const auto bounds = group.local_bounds();
    REQUIRE_THAT(bounds.min_x, Catch::Matchers::WithinAbs(0.0, 0.01));
    REQUIRE_THAT(bounds.max_x, Catch::Matchers::WithinAbs(0.0, 0.01));
}

TEST_CASE("GroupObject — clone", "[canvas][group]")
{
    GroupObject group;
    group.set_name("TestGroup");
    group.add_child(10);
    group.add_child(20);

    auto cloned = group.clone();
    REQUIRE(cloned->type() == CanvasObjectType::Group);
    REQUIRE(cloned->name() == "TestGroup");

    const auto& cloned_group = static_cast<const GroupObject&>(*cloned);
    REQUIRE(cloned_group.child_count() == 2);
    REQUIRE(cloned_group.contains_child(10));
    REQUIRE(cloned_group.contains_child(20));
}

TEST_CASE("GroupObject — JSON serialization", "[canvas][group]")
{
    GroupObject group;
    group.add_child(42);
    group.add_child(99);

    const auto json = group.to_json();
    REQUIRE(json.find("\"type\":\"Group\"") != std::string::npos);
    REQUIRE(json.find("\"children\":[42,99]") != std::string::npos);
}

TEST_CASE("GroupingService — group and ungroup", "[canvas][group]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    // Group them.
    const auto group_id = GroupingService::group_objects(board, {id1, id2});
    REQUIRE(group_id != kInvalidObjectId);

    // Children should have parent_id set.
    REQUIRE(board.get_object(id1)->parent_id() == group_id);
    REQUIRE(board.get_object(id2)->parent_id() == group_id);

    // Group should exist on the board.
    const auto* group_obj = board.get_object(group_id);
    REQUIRE(group_obj != nullptr);
    REQUIRE(group_obj->type() == CanvasObjectType::Group);

    // Ungroup.
    const auto children = GroupingService::ungroup(board, group_id);
    REQUIRE(children.size() == 2);
    REQUIRE(board.get_object(group_id) == nullptr); // Group removed.
    REQUIRE(board.get_object(id1)->parent_id() == kInvalidObjectId);
    REQUIRE(board.get_object(id2)->parent_id() == kInvalidObjectId);
}

TEST_CASE("GroupingService — empty group request", "[canvas][group]")
{
    Board board;
    const auto result = GroupingService::group_objects(board, {});
    REQUIRE(result == kInvalidObjectId);
}

TEST_CASE("GroupingService — get_top_level_group", "[canvas][group]")
{
    Board board;

    auto note = std::make_unique<StickyNote>();
    const auto note_id = note->id();
    board.add_object(std::move(note));

    // Create inner group.
    const auto inner_group_id = GroupingService::group_objects(board, {note_id});

    // Create outer group.
    const auto outer_group_id = GroupingService::group_objects(board, {inner_group_id});

    // Top-level group of note should be the outer group.
    const auto top = GroupingService::get_top_level_group(board, note_id);
    REQUIRE(top == outer_group_id);

    // Top-level group of outer group should be invalid (no parent).
    const auto outer_top = GroupingService::get_top_level_group(board, outer_group_id);
    REQUIRE(outer_top == kInvalidObjectId);
}

TEST_CASE("GroupingService — flatten nested groups", "[canvas][group]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    // Create nested groups: outer { inner { note1 }, note2 }.
    const auto inner_id = GroupingService::group_objects(board, {id1});
    const auto outer_id = GroupingService::group_objects(board, {inner_id, id2});

    // Flatten outer.
    GroupingService::flatten_group(board, outer_id);

    // Both groups should be removed.
    REQUIRE(board.get_object(outer_id) == nullptr);
    REQUIRE(board.get_object(inner_id) == nullptr);

    // Notes should have no parent.
    REQUIRE(board.get_object(id1)->parent_id() == kInvalidObjectId);
    REQUIRE(board.get_object(id2)->parent_id() == kInvalidObjectId);
}
