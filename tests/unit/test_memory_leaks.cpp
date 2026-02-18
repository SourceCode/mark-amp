/// @file test_memory_leaks.cpp
/// Phase 34 Batch 34C – Task 12: Memory leak tests.
/// Tests create/delete cycles for canvas objects, EventBus subscribe/unsubscribe,
/// Config snapshot/restore cycles, and Board object churn.

#include "canvas/Board.h"
#include "canvas/StickyNote.h"
#include "canvas/TextBox.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

using namespace markamp::canvas;
using namespace markamp::core;

// ---------------------------------------------------------------------------
// Canvas object create/delete cycles
// ---------------------------------------------------------------------------

TEST_CASE("Memory — create/delete 1000 canvas objects", "[memory][canvas]")
{
    Board board;

    for (int idx = 0; idx < 1000; ++idx)
    {
        auto note = std::make_unique<StickyNote>();
        note->set_text("Note " + std::to_string(idx));
        board.add_object(std::move(note));
    }

    REQUIRE(board.object_count() == 1000);

    // Remove all by collecting IDs first
    auto ids = board.z_ordered_ids();
    for (auto obj_id : ids)
    {
        board.remove_object(obj_id);
    }

    REQUIRE(board.object_count() == 0);
}

TEST_CASE("Memory — Board clear_all_objects releases everything", "[memory][canvas]")
{
    Board board;

    for (int idx = 0; idx < 500; ++idx)
    {
        auto obj = std::make_unique<TextBox>();
        obj->set_text("Box " + std::to_string(idx));
        board.add_object(std::move(obj));
    }

    REQUIRE(board.object_count() == 500);
    board.clear_all_objects();
    REQUIRE(board.object_count() == 0);
}

TEST_CASE("Memory — Board deep_clone produces independent copy", "[memory][canvas]")
{
    Board board;

    for (int idx = 0; idx < 100; ++idx)
    {
        auto note = std::make_unique<StickyNote>();
        note->set_text("Original " + std::to_string(idx));
        board.add_object(std::move(note));
    }

    auto cloned = board.deep_clone();
    REQUIRE(cloned.object_count() == board.object_count());

    // Modifying clone shouldn't affect original
    cloned.clear_all_objects();
    REQUIRE(cloned.object_count() == 0);
    REQUIRE(board.object_count() == 100);
}

// ---------------------------------------------------------------------------
// EventBus subscribe/unsubscribe cycles
// ---------------------------------------------------------------------------

struct MemoryTestEvent : markamp::core::Event
{
    int value{0};
    explicit MemoryTestEvent(int val = 0)
        : value(val)
    {
    }
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "MemoryTestEvent";
    }
};

TEST_CASE("Memory — EventBus subscribe/unsubscribe 1000 cycles", "[memory][eventbus]")
{
    EventBus bus;
    int count = 0;

    for (int idx = 0; idx < 1000; ++idx)
    {
        {
            // RAII subscription — automatically unsubscribes when scope exits
            auto sub = bus.subscribe<MemoryTestEvent>([&count](const MemoryTestEvent& /*evt*/)
                                                      { ++count; });

            bus.publish(MemoryTestEvent{idx});
        }
        // Subscription destroyed here — handler should no longer fire
    }

    // Each iteration publishes once while subscribed
    REQUIRE(count == 1000);

    // After all unsubscribed, publishing should not increment
    bus.publish(MemoryTestEvent{-1});
    REQUIRE(count == 1000);
}

// ---------------------------------------------------------------------------
// Config snapshot/restore cycles
// ---------------------------------------------------------------------------

TEST_CASE("Memory — Config snapshot-restore 100 cycles", "[memory][config]")
{
    Config config;
    config.set("mem.test", 42);

    for (int idx = 0; idx < 100; ++idx)
    {
        auto snap = config.snapshot();
        config.set("mem.test", idx);
        config = snap; // Copy assignment restores state
    }

    // After all restores, should be back to 42
    REQUIRE(config.get_int("mem.test") == 42);
}

// ---------------------------------------------------------------------------
// Repeated Board add/remove churn
// ---------------------------------------------------------------------------

TEST_CASE("Memory — Board add/remove churn 500 rounds", "[memory][churn]")
{
    Board board;

    for (int round = 0; round < 500; ++round)
    {
        auto note = std::make_unique<StickyNote>();
        note->set_text("churn " + std::to_string(round));
        auto obj_id = board.add_object(std::move(note));
        board.remove_object(obj_id);
    }

    // Board should be empty after all add/remove cycles
    REQUIRE(board.object_count() == 0);
}

TEST_CASE("Memory — Board statistics after heavy churn", "[memory][stats]")
{
    Board board;

    // Add 200 objects
    for (int idx = 0; idx < 200; ++idx)
    {
        auto note = std::make_unique<StickyNote>();
        board.add_object(std::move(note));
    }

    auto stats = board.statistics();
    REQUIRE(stats.total_objects == 200);

    board.clear_all_objects();
    stats = board.statistics();
    REQUIRE(stats.total_objects == 0);
}
