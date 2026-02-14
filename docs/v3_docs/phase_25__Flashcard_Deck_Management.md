# Phase 25 -- Flashcard Deck Management

## Objective

Implement flashcard deck management for MarkAmp, porting SiYuan's riff deck system into the C++23 architecture. Decks are named collections of flashcards where each card is linked to a specific block in a Markdown document. SiYuan maintains a built-in deck (always present, cannot be deleted) plus user-created decks. Cards connect a block_id to FSRS (Free Spaced Repetition Scheduler) scheduling data, enabling spaced repetition learning directly within the knowledge base. Deck operations include creating, removing, and renaming decks, as well as adding, removing, and querying cards within decks.

The DeckManager class serves as the central orchestration point for all deck and card operations. It owns a thread-safe map of FlashcardDeck objects, each containing a vector of FlashcardCard entries. Persistence uses a storage directory structure at `storage/riff/{deck_id}/` containing `deck.json` for metadata and `cards.json` for the card collection. The manager must be fully thread-safe, guarding all mutations with a deck_mutex to support concurrent access from the review UI, background sync, and event handlers. All filesystem operations must use `std::error_code` overloads to avoid exceptions from corrupt or missing storage files.

This phase establishes the data layer only -- no UI. It provides the foundation that Phase 26 (Flashcard Review UI) builds upon to create the interactive review experience. The DeckManager integrates with MarkAmp's EventBus to publish deck lifecycle events, and with Config for user-configurable limits (new card limit, review card limit, retention target, maximum interval). Cards reference blocks by their block_id, which corresponds to a heading, paragraph, or list item in the Markdown AST.

## Prerequisites

- Phase 24 (FSRS Scheduling Algorithm) -- provides `FSRSCard`, `FSRSParameters`, `FSRSScheduler`, `Rating` enum, and scheduling state types that FlashcardCard wraps

## SiYuan Source Reference

- `kernel/model/flashcard.go` -- `Decks` map, `builtinDeckID`, `deckLock` mutex, `CreateDeck()`, `RemoveDeck()`, `RenameDeck()`, `AddFlashcards()`, `RemoveFlashcards()`, `GetFlashcards()`, `GetNotebookFlashcards()`, `GetTreeFlashcards()`, `GetAllDueFlashcards()`, `ResetFlashcards()`, `SetFlashcardsDueTime()`, `GetFlashcardsByBlockIDs()`
- `go-riff` library -- `Deck` struct with Card management, `Store` interface for JSON persistence, `Card` struct linking block ID to scheduling data
- SiYuan stores decks in `storage/riff/{deck_id}/` with `deck.json` metadata and card data files

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/core/FlashcardTypes.h` | `markamp::core` | FlashcardDeck, FlashcardCard, DeckStats structs and CardState enum |
| `src/core/DeckManager.h` | `markamp::core` | DeckManager class declaration |
| `src/core/DeckManager.cpp` | `markamp::core` | DeckManager implementation |
| `src/core/DeckStore.h` | `markamp::core` | IDeckStore interface and JsonDeckStore implementation |
| `src/core/DeckStore.cpp` | `markamp::core` | JsonDeckStore persistence implementation |
| `tests/unit/test_deck_manager.cpp` | (test) | Catch2 test suite for deck management |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Events.h` | Add 6 new event declarations for deck/card lifecycle |
| `src/core/PluginContext.h` | Add `DeckManager*` pointer to PluginContext |
| `src/core/Config.h` | Add flashcard config keys to CachedValues |
| `src/CMakeLists.txt` | Add new source files to build |
| `tests/CMakeLists.txt` | Add test_deck_manager target |
| `resources/config_defaults.json` | Add flashcard default config values |

## Data Structures to Implement

```cpp
// ============================================================================
// File: src/core/FlashcardTypes.h
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// Forward declaration from Phase 24
struct FSRSCard;

/// State of a flashcard relative to the scheduling algorithm.
enum class CardState : uint8_t
{
    New = 0,       // Never reviewed
    Learning = 1,  // In initial learning steps
    Review = 2,    // Graduated to review queue
    Relearning = 3 // Failed review, relearning
};

/// Scheduling rating for a flashcard review.
enum class Rating : uint8_t
{
    Again = 1,
    Hard = 2,
    Good = 3,
    Easy = 4
};

/// A single flashcard linking a document block to FSRS scheduling data.
struct FlashcardCard
{
    std::string id;        // Unique card ID (UUID)
    std::string block_id;  // ID of the Markdown block this card references
    std::string deck_id;   // ID of the deck containing this card
    CardState state{CardState::New};

    // FSRS scheduling fields
    double stability{0.0};
    double difficulty{0.0};
    int elapsed_days{0};
    int scheduled_days{0};
    int reps{0};
    int lapses{0};

    std::chrono::system_clock::time_point due;
    std::chrono::system_clock::time_point last_review;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point updated;

    [[nodiscard]] auto is_due() const -> bool
    {
        return due <= std::chrono::system_clock::now();
    }

    [[nodiscard]] auto is_new() const -> bool
    {
        return state == CardState::New;
    }

    [[nodiscard]] auto days_until_due() const -> int
    {
        auto now = std::chrono::system_clock::now();
        if (due <= now)
        {
            return 0;
        }
        return static_cast<int>(
            std::chrono::duration_cast<std::chrono::hours>(due - now).count() / 24);
    }
};

/// Aggregate statistics for a deck.
struct DeckStats
{
    int total_cards{0};
    int new_cards{0};
    int learning_cards{0};
    int review_cards{0};
    int due_cards{0};

    [[nodiscard]] auto is_empty() const -> bool
    {
        return total_cards == 0;
    }
};

/// A flashcard deck -- a named collection of cards.
struct FlashcardDeck
{
    std::string id;   // Unique deck ID (UUID or "builtin" for the default deck)
    std::string name; // Human-readable deck name
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point updated;
    std::vector<FlashcardCard> cards;

    [[nodiscard]] auto card_count() const -> int
    {
        return static_cast<int>(cards.size());
    }

    [[nodiscard]] auto compute_stats() const -> DeckStats
    {
        DeckStats stats;
        stats.total_cards = card_count();
        for (const auto& card : cards)
        {
            switch (card.state)
            {
            case CardState::New:
                ++stats.new_cards;
                break;
            case CardState::Learning:
            case CardState::Relearning:
                ++stats.learning_cards;
                break;
            case CardState::Review:
                ++stats.review_cards;
                break;
            }
            if (card.is_due())
            {
                ++stats.due_cards;
            }
        }
        return stats;
    }

    [[nodiscard]] auto find_card(const std::string& card_id) const -> const FlashcardCard*
    {
        for (const auto& card : cards)
        {
            if (card.id == card_id)
            {
                return &card;
            }
        }
        return nullptr;
    }

    [[nodiscard]] auto find_card_mut(const std::string& card_id) -> FlashcardCard*
    {
        for (auto& card : cards)
        {
            if (card.id == card_id)
            {
                return &card;
            }
        }
        return nullptr;
    }

    [[nodiscard]] auto has_block(const std::string& block_id) const -> bool
    {
        for (const auto& card : cards)
        {
            if (card.block_id == block_id)
            {
                return true;
            }
        }
        return false;
    }
};

/// Configuration parameters for the FSRS scheduler.
struct FlashcardConfig
{
    int new_card_limit{20};
    int review_card_limit{200};
    double request_retention{0.9};
    int maximum_interval{36500}; // days (~100 years)
};

/// Identifier for the built-in default deck that always exists.
inline constexpr std::string_view kBuiltinDeckID = "builtin";

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/DeckStore.h
// ============================================================================
#pragma once

#include "FlashcardTypes.h"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

/// Abstract interface for flashcard deck persistence.
class IDeckStore
{
public:
    virtual ~IDeckStore() = default;

    [[nodiscard]] virtual auto load_deck(const std::string& deck_id)
        -> std::expected<FlashcardDeck, std::string> = 0;

    [[nodiscard]] virtual auto save_deck(const FlashcardDeck& deck)
        -> std::expected<void, std::string> = 0;

    [[nodiscard]] virtual auto delete_deck(const std::string& deck_id)
        -> std::expected<void, std::string> = 0;

    [[nodiscard]] virtual auto list_deck_ids()
        -> std::expected<std::vector<std::string>, std::string> = 0;

    [[nodiscard]] virtual auto storage_path() const -> std::filesystem::path = 0;
};

/// JSON-based deck store. Stores each deck in storage/riff/{deck_id}/ with
/// deck.json (metadata) and cards.json (card collection).
class JsonDeckStore : public IDeckStore
{
public:
    explicit JsonDeckStore(std::filesystem::path base_path);

    [[nodiscard]] auto load_deck(const std::string& deck_id)
        -> std::expected<FlashcardDeck, std::string> override;

    [[nodiscard]] auto save_deck(const FlashcardDeck& deck)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto delete_deck(const std::string& deck_id)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto list_deck_ids()
        -> std::expected<std::vector<std::string>, std::string> override;

    [[nodiscard]] auto storage_path() const -> std::filesystem::path override
    {
        return base_path_;
    }

private:
    std::filesystem::path base_path_; // e.g. ~/.markamp/storage/riff/

    [[nodiscard]] auto deck_dir(const std::string& deck_id) const -> std::filesystem::path;
    [[nodiscard]] auto deck_meta_path(const std::string& deck_id) const -> std::filesystem::path;
    [[nodiscard]] auto deck_cards_path(const std::string& deck_id) const -> std::filesystem::path;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/DeckManager.h
// ============================================================================
#pragma once

#include "DeckStore.h"
#include "FlashcardTypes.h"

#include <expected>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// Central manager for flashcard decks and cards.
/// Thread-safe: all public methods acquire deck_mutex_ before accessing state.
/// Owns a DeckStore for persistence and publishes events via EventBus.
class DeckManager
{
public:
    DeckManager(EventBus& event_bus, Config& config, std::unique_ptr<IDeckStore> store);
    ~DeckManager() = default;

    // Non-copyable, non-movable (owns mutex)
    DeckManager(const DeckManager&) = delete;
    auto operator=(const DeckManager&) -> DeckManager& = delete;
    DeckManager(DeckManager&&) = delete;
    auto operator=(DeckManager&&) -> DeckManager& = delete;

    /// Load all decks from persistent storage. Creates the built-in deck if absent.
    [[nodiscard]] auto initialize() -> std::expected<void, std::string>;

    /// Persist all modified decks to storage.
    [[nodiscard]] auto save_all() -> std::expected<void, std::string>;

    // ── Deck Operations ──

    [[nodiscard]] auto create_deck(const std::string& name)
        -> std::expected<std::string, std::string>;

    [[nodiscard]] auto remove_deck(const std::string& deck_id)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto rename_deck(const std::string& deck_id, const std::string& new_name)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto get_deck(const std::string& deck_id) const
        -> std::expected<FlashcardDeck, std::string>;

    [[nodiscard]] auto list_decks() const -> std::vector<FlashcardDeck>;

    [[nodiscard]] auto get_deck_stats(const std::string& deck_id) const
        -> std::expected<DeckStats, std::string>;

    // ── Card Operations ──

    [[nodiscard]] auto add_cards(const std::string& deck_id,
                                 const std::vector<std::string>& block_ids)
        -> std::expected<std::vector<std::string>, std::string>;

    [[nodiscard]] auto remove_cards(const std::string& deck_id,
                                    const std::vector<std::string>& block_ids)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto get_due_cards(const std::string& deck_id, int limit) const
        -> std::vector<FlashcardCard>;

    [[nodiscard]] auto get_new_cards(const std::string& deck_id, int limit) const
        -> std::vector<FlashcardCard>;

    [[nodiscard]] auto get_all_cards(const std::string& deck_id, int page, int page_size) const
        -> std::expected<std::vector<FlashcardCard>, std::string>;

    [[nodiscard]] auto reset_cards(const std::vector<std::string>& card_ids)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto set_card_due_time(const std::string& card_id,
                                         std::chrono::system_clock::time_point due)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto get_cards_by_block_ids(const std::vector<std::string>& block_ids) const
        -> std::vector<FlashcardCard>;

    [[nodiscard]] auto update_card_after_review(const std::string& card_id,
                                                 const FlashcardCard& updated_card)
        -> std::expected<void, std::string>;

    // ── Configuration ──

    [[nodiscard]] auto flashcard_config() const -> FlashcardConfig;

private:
    EventBus& event_bus_;
    Config& config_;
    std::unique_ptr<IDeckStore> store_;

    mutable std::mutex deck_mutex_;
    std::unordered_map<std::string, FlashcardDeck> decks_;

    /// Generate a new unique ID for a deck or card.
    [[nodiscard]] static auto generate_id() -> std::string;

    /// Ensure the built-in deck exists.
    void ensure_builtin_deck();

    /// Find the deck containing a specific card_id. Returns nullptr if not found.
    [[nodiscard]] auto find_deck_for_card(const std::string& card_id) -> FlashcardDeck*;
    [[nodiscard]] auto find_deck_for_card(const std::string& card_id) const
        -> const FlashcardDeck*;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`DeckManager::initialize()`** -- Load all decks from the IDeckStore by enumerating deck IDs, deserializing each, and populating the in-memory `decks_` map. Call `ensure_builtin_deck()` to create the default deck if it does not exist in storage. Return error if storage is inaccessible.

2. **`DeckManager::create_deck(name)`** -- Generate a UUID for the new deck, construct a FlashcardDeck with empty card list, insert into `decks_` map, persist via `store_->save_deck()`, publish `DeckCreatedEvent` with the deck ID and name. Return the new deck ID.

3. **`DeckManager::remove_deck(deck_id)`** -- Reject removal of the built-in deck (return error). Remove from `decks_` map, delete from persistent store via `store_->delete_deck()`, publish `DeckRemovedEvent`. Return error if deck_id not found.

4. **`DeckManager::rename_deck(deck_id, new_name)`** -- Validate the new name is non-empty and the deck exists. Update the deck's `name` and `updated` timestamp, persist, publish `DeckStatsChangedEvent`.

5. **`DeckManager::add_cards(deck_id, block_ids)`** -- For each block_id, check it is not already in the deck (skip duplicates). Create a new FlashcardCard with generated ID, `CardState::New`, and current timestamp. Append to the deck's cards vector. Persist. Publish `CardAddedEvent` for each new card. Return the list of newly created card IDs.

6. **`DeckManager::remove_cards(deck_id, block_ids)`** -- Erase cards from the deck's cards vector where `block_id` matches any in the input set. Persist. Publish `CardRemovedEvent` for each removed card. Publish `DeckStatsChangedEvent`.

7. **`DeckManager::get_due_cards(deck_id, limit)`** -- Filter the deck's cards to those where `is_due()` returns true and `state != CardState::New`. Sort by due date ascending (most overdue first). Return up to `limit` cards.

8. **`DeckManager::get_new_cards(deck_id, limit)`** -- Filter the deck's cards to those with `state == CardState::New`. Sort by creation date ascending. Return up to `limit` cards.

9. **`DeckManager::reset_cards(card_ids)`** -- For each card_id, find the card across all decks, reset its FSRS fields to initial values (stability=0, difficulty=0, reps=0, lapses=0, state=New), set due to now. Persist affected decks. Publish `DeckStatsChangedEvent`.

10. **`DeckManager::set_card_due_time(card_id, due)`** -- Find the card, update its `due` timestamp, persist. Used for manual scheduling adjustments.

11. **`DeckManager::get_cards_by_block_ids(block_ids)`** -- Search across all decks for cards whose `block_id` is in the input set. Return collected cards. Used to check if a block already has a flashcard.

12. **`DeckManager::update_card_after_review(card_id, updated_card)`** -- Replace the card's scheduling fields with the updated values from a review session. Update `last_review` and `updated` timestamps. Persist. Publish `CardReviewedEvent`.

13. **`JsonDeckStore::load_deck(deck_id)`** -- Read `deck.json` and `cards.json` from `storage/riff/{deck_id}/`. Parse JSON using nlohmann/json or YAML-cpp. Deserialize timestamps from ISO 8601 strings. Return `std::unexpected` with descriptive error on parse failure.

14. **`JsonDeckStore::save_deck(deck)`** -- Create the deck directory if absent (using `std::error_code` overload). Serialize deck metadata to `deck.json` and cards to `cards.json`. Write atomically (write to temp file, then rename) to prevent corruption on crash.

15. **`DeckManager::flashcard_config()`** -- Read `knowledgebase.flashcard.*` keys from Config, constructing a FlashcardConfig struct with validated values. Clamp `request_retention` to [0.5, 0.99], `maximum_interval` to [1, 36500].

## Events to Add

Add the following to `src/core/Events.h` in a new `Flashcard events` section:

```cpp
// ============================================================================
// Flashcard events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeckCreatedEvent)
std::string deck_id;
std::string deck_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeckRemovedEvent)
std::string deck_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardAddedEvent)
std::string deck_id;
std::string card_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardRemovedEvent)
std::string deck_id;
std::string card_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardReviewedEvent)
std::string card_id;
std::string deck_id;
Rating rating{Rating::Good};
int new_interval_days{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeckStatsChangedEvent)
std::string deck_id;
DeckStats stats;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.flashcard.new_card_limit` | int | 20 | Maximum new cards per review session |
| `knowledgebase.flashcard.review_card_limit` | int | 200 | Maximum review cards per session |
| `knowledgebase.flashcard.request_retention` | double | 0.9 | Target retention rate for FSRS (0.5-0.99) |
| `knowledgebase.flashcard.maximum_interval` | int | 36500 | Maximum interval in days between reviews |

Add these defaults to `resources/config_defaults.json`:

```json
{
    "knowledgebase.flashcard.new_card_limit": 20,
    "knowledgebase.flashcard.review_card_limit": 200,
    "knowledgebase.flashcard.request_retention": 0.9,
    "knowledgebase.flashcard.maximum_interval": 36500
}
```

## Test Cases

All tests in `tests/unit/test_deck_manager.cpp` using Catch2.

1. **"DeckManager initializes with built-in deck"** -- After `initialize()`, `list_decks()` returns at least one deck with ID matching `kBuiltinDeckID`. The built-in deck has zero cards and a non-empty name.

2. **"create_deck produces unique IDs and publishes event"** -- Create two decks with different names. Verify they have distinct IDs, both appear in `list_decks()`, and `DeckCreatedEvent` was published for each (use an EventBus subscription to capture).

3. **"remove_deck rejects built-in deck deletion"** -- Attempt `remove_deck(kBuiltinDeckID)`. Verify it returns an error. The built-in deck still exists in `list_decks()`.

4. **"remove_deck deletes user deck and publishes event"** -- Create a deck, then remove it. Verify `list_decks()` no longer contains it. Verify `DeckRemovedEvent` was published.

5. **"add_cards creates new cards with CardState::New"** -- Add 3 block IDs to the built-in deck. Verify `get_deck(kBuiltinDeckID)` shows 3 cards, all with `state == CardState::New` and `is_new() == true`.

6. **"add_cards skips duplicate block IDs"** -- Add block "b1" to a deck, then add ["b1", "b2"]. Verify the deck has exactly 2 cards (not 3). The duplicate "b1" was silently skipped.

7. **"remove_cards removes by block_id and publishes events"** -- Add 3 cards, then remove 2 by block_id. Verify 1 card remains. Verify 2 `CardRemovedEvent` instances were published.

8. **"get_due_cards returns only due review cards sorted by urgency"** -- Manually set card due times: one past-due by 2 days, one past-due by 5 days, one future. Verify `get_due_cards()` returns 2 cards with the 5-day-overdue card first.

9. **"reset_cards restores cards to New state"** -- Add a card, simulate a review (set state=Review, reps=5, stability=10.0). Call `reset_cards()`. Verify state is `CardState::New`, reps is 0, stability is 0.0.

10. **"get_cards_by_block_ids searches across all decks"** -- Create two decks, add "blockA" to deck1 and "blockB" to deck2. Call `get_cards_by_block_ids({"blockA", "blockB"})`. Verify 2 cards returned from different decks.

## Acceptance Criteria

- [ ] DeckManager initializes cleanly and creates the built-in deck on first run
- [ ] create_deck, remove_deck, rename_deck all persist to storage and publish events
- [ ] Built-in deck cannot be removed (returns error)
- [ ] add_cards creates FlashcardCard entries with CardState::New and deduplicates
- [ ] remove_cards removes by block_id and updates deck stats
- [ ] get_due_cards correctly filters and sorts by due date
- [ ] get_new_cards correctly filters New-state cards
- [ ] reset_cards restores all FSRS fields to initial values
- [ ] All operations are thread-safe (deck_mutex_ acquired in every public method)
- [ ] JsonDeckStore writes atomically (temp file + rename) to prevent corruption

## Files to Create/Modify

```
CREATE  src/core/FlashcardTypes.h
CREATE  src/core/DeckManager.h
CREATE  src/core/DeckManager.cpp
CREATE  src/core/DeckStore.h
CREATE  src/core/DeckStore.cpp
CREATE  tests/unit/test_deck_manager.cpp
MODIFY  src/core/Events.h              -- add 6 flashcard event declarations
MODIFY  src/core/PluginContext.h        -- add DeckManager* deck_manager{nullptr};
MODIFY  src/CMakeLists.txt              -- add DeckManager.cpp, DeckStore.cpp
MODIFY  tests/CMakeLists.txt            -- add test_deck_manager target
MODIFY  resources/config_defaults.json  -- add knowledgebase.flashcard.* defaults
```
