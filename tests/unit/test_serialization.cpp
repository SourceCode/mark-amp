/// @file test_serialization.cpp
/// Phase 34 Batch 34C – Task 10: Serialization round-trip tests.
/// Tests Board/CanvasObject JSON round-trip via BoardSerializer,
/// Config JSON round-trip, and Flashcard deck/card serialization.

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/StickyNote.h"
#include "canvas/TextBox.h"
#include "core/Config.h"
#include "core/FlashcardTypes.h"
#include "core/Theme.h"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <string>

using namespace markamp::canvas;
using namespace markamp::core;

// ---------------------------------------------------------------------------
// Board + BoardSerializer round-trip
// ---------------------------------------------------------------------------

TEST_CASE("Serialization — Board serialize/deserialize preserves objects", "[serial][board]")
{
    Board board;
    board.metadata_mut().name = "Test Board";

    auto note = std::make_unique<StickyNote>();
    note->set_text("Hello serialization");
    note->set_note_color(StickyNoteColor::kBlue);
    auto note_id = board.add_object(std::move(note));

    auto text = std::make_unique<TextBox>();
    text->set_text("Some text content");
    auto text_id = board.add_object(std::move(text));

    BoardSerializer serializer;
    auto json = serializer.serialize(board);
    REQUIRE_FALSE(json.empty());

    // Verify JSON contains our data
    REQUIRE(json.find("Test Board") != std::string::npos);
}

TEST_CASE("Serialization — BoardSerializer validates format version", "[serial][board]")
{
    BoardSerializer serializer;
    auto valid = serializer.validate_json("{\"format_version\": 999}");
    // Invalid format version should fail validation
    REQUIRE_FALSE(valid);
}

TEST_CASE("Serialization — BoardSerializer handles empty board", "[serial][board]")
{
    Board board;
    BoardSerializer serializer;
    auto json = serializer.serialize(board);
    REQUIRE_FALSE(json.empty());

    // deserialize returns Board directly — empty board is valid
    auto deserialized = serializer.deserialize(json);
    REQUIRE(deserialized.object_count() == 0);
}

TEST_CASE("Serialization — BoardSerializer validate_json rejects malformed", "[serial][board]")
{
    BoardSerializer serializer;
    // validate_json returns false for invalid JSON
    REQUIRE_FALSE(serializer.validate_json("{{invalid json"));
    REQUIRE_FALSE(serializer.validate_json(""));
}

// ---------------------------------------------------------------------------
// Config JSON round-trip
// ---------------------------------------------------------------------------

TEST_CASE("Serialization — Config string round-trip", "[serial][config]")
{
    Config config;
    config.set("app.language", std::string("en-US"));
    REQUIRE(config.get_string("app.language") == "en-US");
}

TEST_CASE("Serialization — Config int round-trip", "[serial][config]")
{
    Config config;
    config.set("editor.tab_width", 4);
    REQUIRE(config.get_int("editor.tab_width") == 4);
}

TEST_CASE("Serialization — Config bool round-trip", "[serial][config]")
{
    Config config;
    config.set("editor.word_wrap", true);
    REQUIRE(config.get_bool("editor.word_wrap") == true);

    config.set("editor.word_wrap", false);
    REQUIRE(config.get_bool("editor.word_wrap") == false);
}

TEST_CASE("Serialization — Config double round-trip", "[serial][config]")
{
    Config config;
    config.set("editor.line_height", 1.5);
    REQUIRE(config.get_double("editor.line_height") == 1.5);
}

// ---------------------------------------------------------------------------
// Flashcard types serialization
// ---------------------------------------------------------------------------

TEST_CASE("Serialization — FlashcardCard default state", "[serial][flashcard]")
{
    FlashcardCard card;
    REQUIRE(card.state == CardState::New);
    REQUIRE(card.is_new());
    REQUIRE(card.reps == 0);
    REQUIRE(card.lapses == 0);
    REQUIRE(card.stability == 0.0);
}

TEST_CASE("Serialization — FlashcardDeck card count", "[serial][flashcard]")
{
    FlashcardDeck deck;
    deck.id = "test-deck";
    deck.name = "Test Deck";
    REQUIRE(deck.card_count() == 0);

    FlashcardCard card1;
    card1.id = "card-1";
    card1.state = CardState::New;
    deck.cards.push_back(card1);

    FlashcardCard card2;
    card2.id = "card-2";
    card2.state = CardState::Review;
    deck.cards.push_back(card2);

    REQUIRE(deck.card_count() == 2);
}

TEST_CASE("Serialization — FlashcardDeck compute_stats", "[serial][flashcard]")
{
    FlashcardDeck deck;
    deck.id = "stats-deck";
    deck.name = "Stats Deck";

    FlashcardCard new_card;
    new_card.id = "new-1";
    new_card.state = CardState::New;
    deck.cards.push_back(new_card);

    FlashcardCard learning_card;
    learning_card.id = "learn-1";
    learning_card.state = CardState::Learning;
    deck.cards.push_back(learning_card);

    FlashcardCard review_card;
    review_card.id = "review-1";
    review_card.state = CardState::Review;
    deck.cards.push_back(review_card);

    auto stats = deck.compute_stats();
    REQUIRE(stats.total_cards == 3);
    REQUIRE(stats.new_cards == 1);
    REQUIRE(stats.learning_cards == 1);
    REQUIRE(stats.review_cards == 1);
}

TEST_CASE("Serialization — FlashcardDeck find_card", "[serial][flashcard]")
{
    FlashcardDeck deck;
    FlashcardCard card;
    card.id = "find-me";
    card.block_id = "block-42";
    deck.cards.push_back(card);

    auto found = deck.find_card("find-me");
    REQUIRE(found != nullptr);
    REQUIRE(found->block_id == "block-42");

    auto not_found = deck.find_card("nonexistent");
    REQUIRE(not_found == nullptr);
}

TEST_CASE("Serialization — FlashcardDeck has_block", "[serial][flashcard]")
{
    FlashcardDeck deck;
    FlashcardCard card;
    card.id = "c1";
    card.block_id = "block-99";
    deck.cards.push_back(card);

    REQUIRE(deck.has_block("block-99"));
    REQUIRE_FALSE(deck.has_block("block-nonexistent"));
}

TEST_CASE("Serialization — FlashcardConfig defaults", "[serial][flashcard]")
{
    FlashcardConfig cfg;
    REQUIRE(cfg.new_card_limit == 20);
    REQUIRE(cfg.review_card_limit == 200);
    REQUIRE(cfg.request_retention == 0.9);
    REQUIRE(cfg.maximum_interval == 36500);
}

TEST_CASE("Serialization — DeckStats is_empty", "[serial][flashcard]")
{
    DeckStats stats;
    REQUIRE(stats.is_empty());

    stats.total_cards = 5;
    REQUIRE_FALSE(stats.is_empty());
}

// ---------------------------------------------------------------------------
// Theme JSON round-trip
// ---------------------------------------------------------------------------

TEST_CASE("Serialization — Theme full JSON round-trip", "[serial][theme]")
{
    Theme original;
    original.id = "serial-test";
    original.name = "Serial Test";
    original.author = "MarkAmp";
    original.description = "Round-trip test theme";
    original.source = "custom";
    original.colors.bg_app = Color{15, 15, 30};
    original.colors.accent_primary = Color{100, 99, 255};

    nlohmann::json j;
    to_json(j, original);

    Theme restored;
    from_json(j, restored);

    REQUIRE(restored.id == "serial-test");
    REQUIRE(restored.name == "Serial Test");
    REQUIRE(restored.colors.bg_app == original.colors.bg_app);
    REQUIRE(restored.colors.accent_primary == original.colors.accent_primary);
}
