// Phase 22: Flashcard & Spaced Repetition — Unit Tests
// Tests for FlashcardExtractor, ClozeParser, FlashcardImportExport,
// StudyStreakTracker, LeechDetector, and FlashcardCommandProvider.

#include "core/FlashcardTypes.h"
#include "core/fsrs/ClozeParser.h"
#include "core/fsrs/FlashcardCommandProvider.h"
#include "core/fsrs/FlashcardExtractor.h"
#include "core/fsrs/FlashcardImportExport.h"
#include "core/fsrs/LeechDetector.h"
#include "core/fsrs/StudyStreakTracker.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::fsrs;

// ============================================================================
// Helper: create a sample FlashcardDeck for testing
// ============================================================================

static auto make_sample_deck() -> FlashcardDeck
{
    FlashcardDeck deck;
    deck.id = "test-deck";
    deck.name = "Test Deck";
    deck.created = std::chrono::system_clock::now();
    deck.updated = deck.created;

    FlashcardCard card1;
    card1.id = "card-1";
    card1.block_id = "fc_abc123";
    card1.deck_id = deck.id;
    card1.state = CardState::New;
    card1.stability = 0.0;
    card1.difficulty = 0.0;
    card1.reps = 0;
    card1.lapses = 0;
    card1.due = std::chrono::system_clock::now();
    card1.created = std::chrono::system_clock::now();
    card1.updated = card1.created;
    deck.cards.push_back(card1);

    FlashcardCard card2;
    card2.id = "card-2";
    card2.block_id = "fc_def456";
    card2.deck_id = deck.id;
    card2.state = CardState::Review;
    card2.stability = 5.0;
    card2.difficulty = 3.5;
    card2.reps = 10;
    card2.lapses = 2;
    card2.due = std::chrono::system_clock::now();
    card2.created = std::chrono::system_clock::now();
    card2.updated = card2.created;
    deck.cards.push_back(card2);

    return deck;
}

// ============================================================================
// FlashcardExtractor Tests
// ============================================================================

TEST_CASE("FlashcardExtractor extracts basic cards", "[flashcard][extractor]")
{
    const FlashcardExtractor extractor;
    const std::string line = "Some text ?? What is C++? / A programming language ?? more text";
    const auto card = extractor.extract_from_line(line, 1, "test.md");

    REQUIRE(card.has_value());
    CHECK(card->front == "What is C++?");
    CHECK(card->back == "A programming language");
    CHECK(card->hint.empty());
    CHECK(card->source_line == 1);
    CHECK(card->source_file == "test.md");
    CHECK_FALSE(card->block_id.empty());
}

TEST_CASE("FlashcardExtractor extracts cards with hints", "[flashcard][extractor]")
{
    const FlashcardExtractor extractor;
    const std::string line = "?? Capital of France / Paris / European city ??";
    const auto card = extractor.extract_from_line(line, 5, "geo.md");

    REQUIRE(card.has_value());
    CHECK(card->front == "Capital of France");
    CHECK(card->back == "Paris");
    CHECK(card->hint == "European city");
}

TEST_CASE("FlashcardExtractor returns nullopt for non-flashcard lines", "[flashcard][extractor]")
{
    const FlashcardExtractor extractor;
    const std::string line = "This is just a regular paragraph.";
    const auto card = extractor.extract_from_line(line, 1, "test.md");

    CHECK_FALSE(card.has_value());
}

TEST_CASE("FlashcardExtractor extracts multiple cards from document", "[flashcard][extractor]")
{
    const FlashcardExtractor extractor;
    const std::string doc = "# Chapter 1\n"
                            "?? Q1 / A1 ??\n"
                            "Some text here.\n"
                            "?? Q2 / A2 / Hint ??\n"
                            "More text.\n";

    const auto cards = extractor.extract_from_document(doc, "chapter.md");

    REQUIRE(cards.size() == 2);
    CHECK(cards[0].front == "Q1");
    CHECK(cards[0].back == "A1");
    CHECK(cards[0].source_line == 2);
    CHECK(cards[1].front == "Q2");
    CHECK(cards[1].back == "A2");
    CHECK(cards[1].hint == "Hint");
    CHECK(cards[1].source_line == 4);
}

TEST_CASE("FlashcardExtractor generates deterministic block IDs", "[flashcard][extractor]")
{
    const auto id1 = FlashcardExtractor::generate_block_id("test.md", 10);
    const auto id2 = FlashcardExtractor::generate_block_id("test.md", 10);
    const auto id3 = FlashcardExtractor::generate_block_id("test.md", 20);

    CHECK(id1 == id2); // Same input = same output
    CHECK(id1 != id3); // Different line = different ID
    CHECK(id1.starts_with("fc_"));
}

TEST_CASE("FlashcardExtractor sync identifies new and removed cards", "[flashcard][extractor]")
{
    const FlashcardExtractor extractor;

    // Deck already has card with block_id "fc_abc123"
    auto deck = make_sample_deck();

    // Extract finds different cards
    ExtractedCard ext1;
    ext1.block_id = "fc_abc123"; // existing
    ext1.front = "Q1";
    ext1.back = "A1";

    ExtractedCard ext2;
    ext2.block_id = "fc_newcard"; // new
    ext2.front = "Q2";
    ext2.back = "A2";

    const auto result = extractor.sync_cards({ext1, ext2}, deck);

    CHECK(result.cards_added == 1);     // fc_newcard is new
    CHECK(result.cards_unchanged == 1); // fc_abc123 exists
    CHECK(result.cards_removed == 1);   // fc_def456 removed
}

// ============================================================================
// ClozeParser Tests
// ============================================================================

TEST_CASE("ClozeParser detects cloze syntax", "[flashcard][cloze]")
{
    CHECK(ClozeParser::is_cloze("The {{c1::dog}} is brown"));
    CHECK(ClozeParser::is_cloze("{{simple}}"));
    CHECK_FALSE(ClozeParser::is_cloze("No cloze here"));
    CHECK_FALSE(ClozeParser::is_cloze("Just {{ broken"));
}

TEST_CASE("ClozeParser parses single cloze", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("The {{c1::capital}} of France is important.");

    REQUIRE(card.fields.size() == 1);
    CHECK(card.fields[0].index == 1);
    CHECK(card.fields[0].text == "capital");
    CHECK(card.fields[0].hint.empty());
    CHECK(card.card_count == 1);
}

TEST_CASE("ClozeParser parses cloze with hint", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("The {{c1::dog::animal}} is brown.");

    REQUIRE(card.fields.size() == 1);
    CHECK(card.fields[0].index == 1);
    CHECK(card.fields[0].text == "dog");
    CHECK(card.fields[0].hint == "animal");
}

TEST_CASE("ClozeParser parses multiple cloze indices", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("{{c1::Paris}} is the capital of {{c2::France}}.");

    REQUIRE(card.fields.size() == 2);
    CHECK(card.card_count == 2);

    const auto indices = ClozeParser::unique_indices(card);
    CHECK(indices.size() == 2);
    CHECK(indices[0] == 1);
    CHECK(indices[1] == 2);
}

TEST_CASE("ClozeParser renders front with blank", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("{{c1::Paris}} is the capital of {{c2::France}}.");

    const auto front_c1 = parser.render_front(card, 1);
    CHECK(front_c1.find("[...]") != std::string::npos);
    CHECK(front_c1.find("France") != std::string::npos); // c2 shown as text

    const auto front_c2 = parser.render_front(card, 2);
    CHECK(front_c2.find("Paris") != std::string::npos); // c1 shown as text
    CHECK(front_c2.find("[...]") != std::string::npos);
}

TEST_CASE("ClozeParser renders front with hint", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("The {{c1::dog::animal}} is brown.");
    const auto front = parser.render_front(card, 1);

    CHECK(front.find("[...animal...]") != std::string::npos);
}

TEST_CASE("ClozeParser renders back with highlighted answer", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("{{c1::Paris}} is the capital of {{c2::France}}.");
    const auto back_c1 = parser.render_back(card, 1);

    CHECK(back_c1.find("**Paris**") != std::string::npos);
    CHECK(back_c1.find("France") != std::string::npos);
}

TEST_CASE("ClozeParser generates card pairs", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("{{c1::Paris}} is the capital of {{c2::France}}.");
    const auto pairs = parser.generate_cards(card);

    REQUIRE(pairs.size() == 2);
    // Card for c1
    CHECK(pairs[0].first.find("[...]") != std::string::npos);
    CHECK(pairs[0].second.find("**Paris**") != std::string::npos);
    // Card for c2
    CHECK(pairs[1].first.find("Paris") != std::string::npos);
    CHECK(pairs[1].second.find("**France**") != std::string::npos);
}

TEST_CASE("ClozeParser handles shorthand syntax", "[flashcard][cloze]")
{
    const ClozeParser parser;
    const auto card = parser.parse("The {{word}} is important.");

    REQUIRE(card.fields.size() == 1);
    CHECK(card.fields[0].index == 1); // auto-assigned c1
    CHECK(card.fields[0].text == "word");
}

// ============================================================================
// FlashcardImportExport Tests
// ============================================================================

TEST_CASE("FlashcardImportExport exports to CSV", "[flashcard][import_export]")
{
    const FlashcardImportExport exporter;
    const auto deck = make_sample_deck();
    const auto csv = exporter.export_to_csv(deck);

    CHECK(csv.find("front,back,state,stability,difficulty,reps,lapses,due") != std::string::npos);
    CHECK(csv.find("new") != std::string::npos);
    CHECK(csv.find("review") != std::string::npos);
}

TEST_CASE("FlashcardImportExport exports to JSON", "[flashcard][import_export]")
{
    const FlashcardImportExport exporter;
    const auto deck = make_sample_deck();
    const auto json = exporter.export_to_json(deck);

    CHECK(json.find("\"id\"") != std::string::npos);
    CHECK(json.find("\"block_id\"") != std::string::npos);
    CHECK(json.find("\"state\"") != std::string::npos);
    CHECK(json.front() == '[');
}

TEST_CASE("FlashcardImportExport imports from CSV", "[flashcard][import_export]")
{
    const FlashcardImportExport importer;
    const std::string csv = "front,back\n"
                            "What is C++?,A language\n"
                            "What is Rust?,A language\n";

    const auto result = importer.import_from_csv(csv, "deck-1");

    REQUIRE(result.has_value());
    CHECK(result->cards_imported == 2);
    CHECK(result->cards_failed == 0);
}

TEST_CASE("FlashcardImportExport imports from JSON", "[flashcard][import_export]")
{
    const FlashcardImportExport importer;
    const std::string json = R"([{"front": "Q1", "back": "A1"}, {"front": "Q2", "back": "A2"}])";

    const auto result = importer.import_from_json(json, "deck-1");

    REQUIRE(result.has_value());
    CHECK(result->cards_imported == 2);
}

TEST_CASE("FlashcardImportExport detects format", "[flashcard][import_export]")
{
    CHECK(FlashcardImportExport::detect_format("[{\"front\": \"Q\"}]") ==
          FlashcardExportFormat::JSON);
    CHECK(FlashcardImportExport::detect_format("front,back\nQ,A\n") == FlashcardExportFormat::CSV);
    CHECK(FlashcardImportExport::detect_format("  [{\"front\": \"Q\"}]") ==
          FlashcardExportFormat::JSON);
}

TEST_CASE("FlashcardImportExport rejects empty data", "[flashcard][import_export]")
{
    const FlashcardImportExport importer;

    const auto csv_result = importer.import_from_csv("", "d");
    CHECK_FALSE(csv_result.has_value());

    const auto json_result = importer.import_from_json("", "d");
    CHECK_FALSE(json_result.has_value());
}

// ============================================================================
// StudyStreakTracker Tests
// ============================================================================

TEST_CASE("StudyStreakTracker records and counts reviews", "[flashcard][streak]")
{
    StudyStreakTracker tracker;
    const auto now = std::chrono::system_clock::now();

    tracker.record_review(now);
    tracker.record_review(now);

    CHECK(tracker.total_reviews() == 2);
}

TEST_CASE("StudyStreakTracker computes current streak", "[flashcard][streak]")
{
    StudyStreakTracker tracker;
    const auto now = std::chrono::system_clock::now();

    // Record today
    tracker.record_review(now);

    // Record yesterday
    tracker.record_review(now - std::chrono::hours(24));

    // Record 2 days ago
    tracker.record_review(now - std::chrono::hours(48));

    CHECK(tracker.current_streak() >= 2); // At least 2-3 depending on time of day
}

TEST_CASE("StudyStreakTracker returns zero streak with no reviews", "[flashcard][streak]")
{
    const StudyStreakTracker tracker;
    CHECK(tracker.current_streak() == 0);
    CHECK(tracker.longest_streak() == 0);
}

TEST_CASE("StudyStreakTracker generates heatmap data", "[flashcard][streak]")
{
    StudyStreakTracker tracker;
    const auto now = std::chrono::system_clock::now();
    tracker.record_review(now);

    const auto heatmap = tracker.get_heatmap_data(7);
    CHECK(heatmap.size() == 7);
    CHECK(heatmap[0].has_activity()); // Today should have activity
}

TEST_CASE("StudyStreakTracker serializes and deserializes", "[flashcard][streak]")
{
    StudyStreakTracker tracker;
    const auto now = std::chrono::system_clock::now();
    tracker.record_review(now);
    tracker.record_review(now);
    tracker.record_time(now, 120);

    const auto json = tracker.serialize();
    CHECK_FALSE(json.empty());

    StudyStreakTracker loaded;
    const auto load_result = loaded.deserialize(json);
    REQUIRE(load_result.has_value());
    CHECK(loaded.total_reviews() == 2);
}

TEST_CASE("StudyStreakTracker evaluates milestones", "[flashcard][streak]")
{
    const StudyStreakTracker tracker;
    const auto milestones = tracker.achieved_milestones();
    CHECK(milestones.empty()); // No records = no milestones
}

// ============================================================================
// LeechDetector Tests
// ============================================================================

TEST_CASE("LeechDetector identifies leeches", "[flashcard][leech]")
{
    const LeechDetector detector;
    auto deck = make_sample_deck();

    // Set high lapse count on card-2
    deck.cards[1].lapses = 10;

    const auto status = detector.check_card(deck.cards[1]);
    CHECK(status.is_leech);
    CHECK(status.lapse_count == 10);
    CHECK_FALSE(status.is_suspended);
}

TEST_CASE("LeechDetector does not flag cards below threshold", "[flashcard][leech]")
{
    const LeechDetector detector;
    const auto deck = make_sample_deck();

    const auto status = detector.check_card(deck.cards[0]);
    CHECK_FALSE(status.is_leech);
}

TEST_CASE("LeechDetector finds all leeches in deck", "[flashcard][leech]")
{
    const LeechDetector detector;
    auto deck = make_sample_deck();
    deck.cards[1].lapses = 10;

    const auto leeches = detector.get_leeches(deck);
    REQUIRE(leeches.size() == 1);
    CHECK(leeches[0].card_id == "card-2");
}

TEST_CASE("LeechDetector suspends and unsuspends cards", "[flashcard][leech]")
{
    const LeechDetector detector;
    auto deck = make_sample_deck();

    // Suspend card-1
    const auto suspend_result = detector.suspend_card(deck, "card-1");
    REQUIRE(suspend_result.has_value());
    CHECK(detector.is_suspended(deck.cards[0]));

    // Unsuspend card-1
    const auto unsuspend_result = detector.unsuspend_card(deck, "card-1");
    REQUIRE(unsuspend_result.has_value());
    CHECK_FALSE(detector.is_suspended(deck.cards[0]));
}

TEST_CASE("LeechDetector configurable threshold", "[flashcard][leech]")
{
    LeechDetector detector(3); // Low threshold
    CHECK(detector.leech_threshold() == 3);

    auto deck = make_sample_deck();
    deck.cards[0].lapses = 3;

    CHECK(detector.check_card(deck.cards[0]).is_leech);

    detector.set_leech_threshold(5);
    CHECK(detector.leech_threshold() == 5);
    CHECK_FALSE(detector.check_card(deck.cards[0]).is_leech);
}

TEST_CASE("LeechDetector rejects suspend of unknown card", "[flashcard][leech]")
{
    const LeechDetector detector;
    auto deck = make_sample_deck();

    const auto result = detector.suspend_card(deck, "nonexistent");
    CHECK_FALSE(result.has_value());
}

// ============================================================================
// FlashcardCommandProvider Tests
// ============================================================================

TEST_CASE("FlashcardCommandProvider returns 8 commands", "[flashcard][commands]")
{
    const FlashcardCommandProvider provider;
    const auto commands = provider.get_commands();

    CHECK(commands.size() == 8);
}

TEST_CASE("FlashcardCommandProvider has Start Review command with keybinding",
          "[flashcard][commands]")
{
    const FlashcardCommandProvider provider;
    const auto keybinding = provider.get_keybinding(FlashcardCommandProvider::kStartReview);

    REQUIRE(keybinding.has_value());
    CHECK(*keybinding == "Cmd+Shift+R");
}

TEST_CASE("FlashcardCommandProvider reports enabled state", "[flashcard][commands]")
{
    const FlashcardCommandProvider provider;
    CHECK(provider.is_enabled(FlashcardCommandProvider::kStartReview));
    CHECK(provider.is_enabled(FlashcardCommandProvider::kBrowseDecks));
    CHECK_FALSE(provider.is_enabled("flashcard.nonexistent"));
}

TEST_CASE("FlashcardCommandProvider commands have Flashcard category", "[flashcard][commands]")
{
    const FlashcardCommandProvider provider;
    const auto commands = provider.get_commands();

    for (const auto& command : commands)
    {
        CHECK(command.category == "Flashcard");
        CHECK(command.label.starts_with("Flashcard: "));
    }
}

TEST_CASE("FlashcardCommandProvider get_command returns specific command", "[flashcard][commands]")
{
    const FlashcardCommandProvider provider;
    const auto command = provider.get_command(FlashcardCommandProvider::kExport);

    REQUIRE(command.has_value());
    CHECK(command->id == FlashcardCommandProvider::kExport);
    CHECK(command->label == "Flashcard: Export");
}

TEST_CASE("FlashcardCommandProvider returns nullopt for unknown command", "[flashcard][commands]")
{
    const FlashcardCommandProvider provider;
    const auto command = provider.get_command("flashcard.unknown");

    CHECK_FALSE(command.has_value());
}
