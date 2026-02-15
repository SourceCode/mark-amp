#include "canvas/Board.h"
#include "canvas/GroupObject.h"
#include "canvas/GroupingService.h"
#include "canvas/LayeringService.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("LayeringService — bring_to_front single", "[canvas][layering]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    note1->set_z_index(0);
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    note2->set_z_index(1);
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    auto note3 = std::make_unique<StickyNote>();
    note3->set_z_index(2);
    board.add_object(std::move(note3));

    LayeringService::bring_to_front(board, {id1});

    // id1 should now have the highest z_index.
    const auto* obj1 = board.get_object(id1);
    const auto* obj2 = board.get_object(id2);
    REQUIRE(obj1->z_index() > obj2->z_index());
}

TEST_CASE("LayeringService — send_to_back single", "[canvas][layering]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    note1->set_z_index(5);
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    note2->set_z_index(1);
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    LayeringService::send_to_back(board, {id1});

    const auto* obj1 = board.get_object(id1);
    const auto* obj2 = board.get_object(id2);
    REQUIRE(obj1->z_index() <= obj2->z_index());
}

TEST_CASE("LayeringService — z_range", "[canvas][layering]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    note1->set_z_index(-3);
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    note2->set_z_index(10);
    board.add_object(std::move(note2));

    auto note3 = std::make_unique<StickyNote>();
    note3->set_z_index(5);
    board.add_object(std::move(note3));

    const auto [min_z, max_z] = LayeringService::z_range(board);
    REQUIRE(min_z == -3);
    REQUIRE(max_z == 10);
}

TEST_CASE("LayeringService — z_range empty board", "[canvas][layering]")
{
    Board board;
    const auto [min_z, max_z] = LayeringService::z_range(board);
    REQUIRE(min_z == 0);
    REQUIRE(max_z == 0);
}

TEST_CASE("LayeringService — auto_distribute_z", "[canvas][layering]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    note1->set_z_index(100);
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    note2->set_z_index(-50);
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    auto note3 = std::make_unique<StickyNote>();
    note3->set_z_index(42);
    const auto id3 = note3->id();
    board.add_object(std::move(note3));

    LayeringService::auto_distribute_z(board);

    // Should be reassigned to 0, 1, 2 in ascending original z order.
    // Original order: id2(-50) < id3(42) < id1(100).
    REQUIRE(board.get_object(id2)->z_index() == 0);
    REQUIRE(board.get_object(id3)->z_index() == 1);
    REQUIRE(board.get_object(id1)->z_index() == 2);
}

TEST_CASE("LayeringService — group-aware bring_to_front", "[canvas][layering]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    note1->set_z_index(0);
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    note2->set_z_index(1);
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    auto note3 = std::make_unique<StickyNote>();
    note3->set_z_index(10);
    board.add_object(std::move(note3));

    // Group note1 and note2.
    const auto group_id = GroupingService::group_objects(board, {id1, id2});

    // Bring group to front — should move both children too.
    LayeringService::bring_to_front(board, {group_id});

    // All should now be at high z.
    const auto* obj1 = board.get_object(id1);
    const auto* obj2 = board.get_object(id2);
    REQUIRE(obj1->z_index() >= 10);
    REQUIRE(obj2->z_index() >= 10);
}
