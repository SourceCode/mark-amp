#include "canvas/Board.h"
#include "canvas/GroupObject.h"
#include "canvas/GroupingService.h"
#include "canvas/LockingService.h"
#include "canvas/StickyNote.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("LockingService — lock and unlock batch", "[canvas][locking]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    REQUIRE_FALSE(board.get_object(id1)->is_locked());
    REQUIRE_FALSE(board.get_object(id2)->is_locked());

    LockingService::lock_objects(board, {id1, id2});
    REQUIRE(board.get_object(id1)->is_locked());
    REQUIRE(board.get_object(id2)->is_locked());

    LockingService::unlock_objects(board, {id1, id2});
    REQUIRE_FALSE(board.get_object(id1)->is_locked());
    REQUIRE_FALSE(board.get_object(id2)->is_locked());
}

TEST_CASE("LockingService — toggle_lock", "[canvas][locking]")
{
    Board board;

    auto note = std::make_unique<StickyNote>();
    const auto note_id = note->id();
    board.add_object(std::move(note));

    REQUIRE_FALSE(board.get_object(note_id)->is_locked());

    LockingService::toggle_lock(board, note_id);
    REQUIRE(board.get_object(note_id)->is_locked());

    LockingService::toggle_lock(board, note_id);
    REQUIRE_FALSE(board.get_object(note_id)->is_locked());
}

TEST_CASE("LockingService — is_any_locked", "[canvas][locking]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    const auto id2 = note2->id();
    board.add_object(std::move(note2));

    REQUIRE_FALSE(LockingService::is_any_locked(board, {id1, id2}));

    LockingService::lock_objects(board, {id2});
    REQUIRE(LockingService::is_any_locked(board, {id1, id2}));
}

TEST_CASE("LockingService — locked_ids", "[canvas][locking]")
{
    Board board;

    auto note1 = std::make_unique<StickyNote>();
    const auto id1 = note1->id();
    board.add_object(std::move(note1));

    auto note2 = std::make_unique<StickyNote>();
    const auto id2 = note2->id();
    note2->set_locked(true);
    board.add_object(std::move(note2));

    auto note3 = std::make_unique<StickyNote>();
    board.add_object(std::move(note3));

    const auto locked = LockingService::locked_ids(board);
    REQUIRE(locked.size() == 1);
    REQUIRE(locked[0] == id2);

    // Lock more.
    LockingService::lock_objects(board, {id1});
    const auto locked2 = LockingService::locked_ids(board);
    REQUIRE(locked2.size() == 2);
}

TEST_CASE("LockingService — group-aware locking", "[canvas][locking]")
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

    // Lock the group — should lock children too.
    LockingService::lock_objects(board, {group_id});

    REQUIRE(board.get_object(group_id)->is_locked());
    REQUIRE(board.get_object(id1)->is_locked());
    REQUIRE(board.get_object(id2)->is_locked());

    // Unlock the group.
    LockingService::unlock_objects(board, {group_id});
    REQUIRE_FALSE(board.get_object(id1)->is_locked());
    REQUIRE_FALSE(board.get_object(id2)->is_locked());
}

TEST_CASE("LockingService — toggle on non-existent ID is no-op", "[canvas][locking]")
{
    Board board;
    LockingService::toggle_lock(board, 999999);
    REQUIRE(LockingService::locked_ids(board).empty());
}
