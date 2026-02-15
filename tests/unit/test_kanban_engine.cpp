/// @file test_kanban_engine.cpp
/// @brief V4 Phase 25 – KanbanEngine unit tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/VaultService.h"
#include "ui/KanbanEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;
using namespace markamp::core;

namespace
{

const std::string kSampleKanbanMarkdown = R"(# Project Board

## Todo
- [ ] Implement login
- [ ] Add tests
- [ ] Write docs

## In Progress
- [/] Design API
- [/] Build database

## Done
- [x] Setup repo
- [x] Create README
)";

const std::string kWipLimitMarkdown = R"(# Board

## Backlog
- [ ] Task 1
- [ ] Task 2
- [ ] Task 3

## In Progress [3]
- [/] Task 4
- [/] Task 5
- [/] Task 6
)";

struct TestFixture
{
    EventBus event_bus;
    Config config;
    VaultService vault_service{event_bus, config};
    KanbanEngine engine{event_bus, vault_service};
};

} // anonymous namespace

// ---------------------------------------------------------------------------
// Test 1: Parse kanban markdown (3 columns with correct card counts)
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: parse kanban markdown", "[kanban]")
{
    auto board = KanbanEngine::parse_kanban_markdown(kSampleKanbanMarkdown);

    CHECK(board.title == "Project Board");
    REQUIRE(board.columns.size() == 3);

    CHECK(board.columns[0].title == "Todo");
    CHECK(board.columns[0].cards.size() == 3);

    CHECK(board.columns[1].title == "In Progress");
    CHECK(board.columns[1].cards.size() == 2);

    CHECK(board.columns[2].title == "Done");
    CHECK(board.columns[2].cards.size() == 2);
}

// ---------------------------------------------------------------------------
// Test 2: Card metadata (status parsed correctly)
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: card metadata", "[kanban]")
{
    auto board = KanbanEngine::parse_kanban_markdown(kSampleKanbanMarkdown);

    // Todo cards
    CHECK(board.columns[0].cards[0].status == TaskStatus::kTodo);

    // In Progress cards
    CHECK(board.columns[1].cards[0].status == TaskStatus::kInProgress);

    // Done cards
    CHECK(board.columns[2].cards[0].status == TaskStatus::kDone);
}

// ---------------------------------------------------------------------------
// Test 3: Move card between columns
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: move card between columns", "[kanban]")
{
    // Move first card from Todo (col 0) to In Progress (col 1)
    auto result = KanbanEngine::move_card(kSampleKanbanMarkdown, 0, 0, 1, 0);

    // Re-parse to verify
    auto board = KanbanEngine::parse_kanban_markdown(result);
    REQUIRE(board.columns.size() == 3);

    // Todo should have 2 cards now
    CHECK(board.columns[0].cards.size() == 2);

    // In Progress should have 3 cards
    CHECK(board.columns[1].cards.size() == 3);
}

// ---------------------------------------------------------------------------
// Test 4: Card toggle
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: card toggle", "[kanban]")
{
    // Toggle first Todo card (should become [x])
    auto result = KanbanEngine::toggle_card(kSampleKanbanMarkdown, 0, 0);

    // Verify the checkbox was toggled
    auto board = KanbanEngine::parse_kanban_markdown(result);
    CHECK(board.columns[0].cards[0].status == TaskStatus::kDone);
}

// ---------------------------------------------------------------------------
// Test 5: Empty column handling
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: empty column", "[kanban]")
{
    std::string markdown = R"(# Board

## Empty Column

## Has Cards
- [ ] Card 1
)";

    auto board = KanbanEngine::parse_kanban_markdown(markdown);
    REQUIRE(board.columns.size() == 2);
    CHECK(board.columns[0].title == "Empty Column");
    CHECK(board.columns[0].cards.empty());
    CHECK(board.columns[1].cards.size() == 1);
}

// ---------------------------------------------------------------------------
// Test 6: WIP limit detection
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: WIP limit", "[kanban]")
{
    auto board = KanbanEngine::parse_kanban_markdown(kWipLimitMarkdown);
    REQUIRE(board.columns.size() == 2);

    CHECK(board.columns[0].max_cards == 0); // No limit
    CHECK(board.columns[1].max_cards == 3); // WIP limit of 3
    CHECK(board.columns[1].title == "In Progress");
}

// ---------------------------------------------------------------------------
// Test 7: Total card count
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: total card count", "[kanban]")
{
    auto board = KanbanEngine::parse_kanban_markdown(kSampleKanbanMarkdown);
    CHECK(board.total_cards() == 7);
}

// ---------------------------------------------------------------------------
// Test 8: Column headings
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: column headings", "[kanban]")
{
    auto board = KanbanEngine::parse_kanban_markdown(kSampleKanbanMarkdown);

    CHECK(board.columns[0].heading_text == "## Todo");
    CHECK(board.columns[1].heading_text == "## In Progress");
    CHECK(board.columns[2].heading_text == "## Done");
}

// ---------------------------------------------------------------------------
// Test 9: Find card
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: find card", "[kanban]")
{
    auto board = KanbanEngine::parse_kanban_markdown(kSampleKanbanMarkdown);

    // First card in first column
    auto found = board.find_card("col0_card0");
    REQUIRE(found.has_value());
    CHECK(found->first == 0);
    CHECK(found->second == 0);

    // Second card in second column
    auto found2 = board.find_card("col1_card1");
    REQUIRE(found2.has_value());
    CHECK(found2->first == 1);
    CHECK(found2->second == 1);

    // Nonexistent
    CHECK_FALSE(board.find_card("nonexistent").has_value());
}

// ---------------------------------------------------------------------------
// Test 10: Toggle done card back to todo
// ---------------------------------------------------------------------------
TEST_CASE("KanbanEngine: toggle done to todo", "[kanban]")
{
    // Toggle first Done card (should become [ ])
    auto result = KanbanEngine::toggle_card(kSampleKanbanMarkdown, 2, 0);

    auto board = KanbanEngine::parse_kanban_markdown(result);
    CHECK(board.columns[2].cards[0].status == TaskStatus::kTodo);
}
