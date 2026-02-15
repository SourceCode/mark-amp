# Phase 24 -- FSRS Spaced Repetition Algorithm

**Priority:** Medium (standalone feature, no AV dependency for core algorithm)
**Estimated Scope:** ~8 new files, ~4 modified files
**Dependencies:** None for the core algorithm. Integration with AV (flashcards as AV rows) is optional.

## Objective

Implement the Free Spaced Repetition Scheduler (FSRS) v3 algorithm in C++23. FSRS is a modern spaced repetition algorithm that replaces SM-2 (used by Anki) with better prediction accuracy. It determines optimal review intervals for flashcards based on the user's recall performance, using a mathematically principled model of memory decay.

This phase implements:
- The core FSRS scheduling algorithm with all mathematical formulas
- Card state machine (New -> Learning -> Review, with Relearning on lapses)
- 17-parameter model with configurable weights (defaults from the FSRS research paper)
- Retrievability calculation (probability of recall at any point in time)
- Scheduling output: given a card and a rating, compute the next review state
- Review logging for history tracking
- Flashcard storage and deck management
- Integration with MarkAmp's EventBus for review session events

## Prerequisites

- None for the core algorithm. FSRS is a self-contained mathematical engine.
- For flashcard UI integration: Phase 17 (AttributeView) enables flashcards-as-AV-rows, but the algorithm itself is independent.

## SiYuan Source Reference

| SiYuan File | Purpose | Key Structures |
|---|---|---|
| `go-fsrs/v3` library (github.com/open-spaced-repetition/go-fsrs) | Core FSRS algorithm | `Card`, `ReviewLog`, `Parameters`, `SchedulingInfo`, `SchedulingCards` |
| `kernel/model/flashcard.go` | SiYuan's FSRS integration | Flashcard CRUD, review session, deck management |
| `kernel/model/flashcard_deck.go` | Deck management | Deck struct, due card queries |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/core/fsrs/FSRSTypes.h` | `markamp::core::fsrs` | All enums, structs for the FSRS data model |
| `src/core/fsrs/FSRSTypes.cpp` | `markamp::core::fsrs` | Enum conversion utilities |
| `src/core/fsrs/FSRSEngine.h` | `markamp::core::fsrs` | Core FSRS scheduling algorithm |
| `src/core/fsrs/FSRSEngine.cpp` | `markamp::core::fsrs` | All FSRS mathematical formulas |
| `src/core/fsrs/FlashcardStore.h` | `markamp::core::fsrs` | Flashcard persistence and deck management |
| `src/core/fsrs/FlashcardStore.cpp` | `markamp::core::fsrs` | JSON storage for cards and review logs |
| `src/core/fsrs/ReviewSession.h` | `markamp::core::fsrs` | Review session controller |
| `src/core/fsrs/ReviewSession.cpp` | `markamp::core::fsrs` | Session management, due card ordering |
| `tests/unit/test_fsrs_engine.cpp` | (anonymous) | Catch2 tests for FSRS algorithm |
| `tests/unit/test_flashcard_store.cpp` | (anonymous) | Catch2 tests for persistence |

### Files to Modify

| File | Change |
|---|---|
| `src/core/Events.h` | Add flashcard/review events |
| `src/core/PluginContext.h` | Add `FSRSEngine*` and `FlashcardStore*` forward declarations |
| `src/CMakeLists.txt` | Add new `.cpp` sources |
| `tests/CMakeLists.txt` | Add test targets |

## Data Structures to Implement

### File: `src/core/fsrs/FSRSTypes.h`

```cpp
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Card State Machine
// New -> Learning -> Review (successful) or Relearning (lapse)
// ============================================================================

enum class CardState
{
    New,           // Never reviewed
    Learning,      // In initial learning phase (short intervals)
    Review,        // In long-term review phase (graduated)
    Relearning     // Lapsed: was in Review, answered Again -> re-enter learning
};

[[nodiscard]] auto card_state_to_string(CardState state) -> std::string;
[[nodiscard]] auto string_to_card_state(const std::string& str) -> CardState;

// ============================================================================
// Rating (user's self-assessment of recall quality)
// ============================================================================

enum class Rating : int
{
    Again = 1,     // Complete failure to recall
    Hard  = 2,     // Recalled with significant difficulty
    Good  = 3,     // Recalled with some effort (correct)
    Easy  = 4      // Recalled instantly with no effort
};

[[nodiscard]] auto rating_to_string(Rating rating) -> std::string;
[[nodiscard]] auto string_to_rating(const std::string& str) -> Rating;
[[nodiscard]] auto rating_to_int(Rating rating) -> int;

// ============================================================================
// FSRS Parameters (17 trainable weights)
// Default values from the FSRS-4.5 paper
// ============================================================================

struct FSRSParameters
{
    // w[0..3]: initial stability for each rating (Again, Hard, Good, Easy)
    // w[4..5]: initial difficulty parameters
    // w[6]: difficulty mean reversion rate
    // w[7]: stability after lapse base
    // w[8..10]: recall stability growth parameters
    // w[11..14]: forget stability parameters
    // w[15]: hard penalty factor
    // w[16]: easy bonus factor
    double w[17] = {
        0.4,      // w[0]  - initial stability for Again
        0.6,      // w[1]  - initial stability for Hard
        2.4,      // w[2]  - initial stability for Good
        5.8,      // w[3]  - initial stability for Easy
        4.93,     // w[4]  - initial difficulty base
        0.94,     // w[5]  - initial difficulty scaling
        0.86,     // w[6]  - difficulty mean reversion rate
        0.01,     // w[7]  - (reserved/stability after lapse)
        1.49,     // w[8]  - stability growth exp base
        0.14,     // w[9]  - stability growth difficulty factor
        0.94,     // w[10] - stability growth retrievability factor
        2.18,     // w[11] - forget stability base
        0.05,     // w[12] - forget stability difficulty factor
        0.34,     // w[13] - forget stability previous stability factor
        1.26,     // w[14] - forget stability retrievability factor
        0.29,     // w[15] - hard penalty factor
        2.61      // w[16] - easy bonus factor
    };

    /// Maximum interval in days.
    double maximum_interval{36500.0};  // 100 years

    /// Desired retention rate (0.0 to 1.0). Default 0.9 = 90% target recall.
    double request_retention{0.9};

    [[nodiscard]] auto operator==(const FSRSParameters& other) const -> bool = default;
};

// ============================================================================
// Card: the schedulable unit
// ============================================================================

using TimePoint = std::chrono::system_clock::time_point;

struct Card
{
    std::string id;                    // Unique card ID (UUID)
    std::string deck_id;               // Deck this card belongs to

    // ── Scheduling state ──
    TimePoint due;                     // When this card is next due for review
    double stability{0.0};             // Memory stability (days until R drops to 90%)
    double difficulty{0.0};            // Item difficulty (1.0 = easiest, 10.0 = hardest)
    double elapsed_days{0.0};          // Days since last review
    double scheduled_days{0.0};        // Days until next review (interval)
    int reps{0};                       // Total number of successful reviews
    int lapses{0};                     // Total number of times the card was forgotten
    CardState state{CardState::New};   // Current state in the state machine
    TimePoint last_review;             // Timestamp of last review

    // ── Content reference ──
    std::string block_id;              // Reference to the Markdown block (front/back)
    std::string front_content;         // Cached front text
    std::string back_content;          // Cached back text

    [[nodiscard]] auto operator==(const Card& other) const -> bool = default;
};

// ============================================================================
// Review Log: immutable record of a single review
// ============================================================================

struct ReviewLog
{
    std::string id;                    // Unique log ID
    std::string card_id;               // Which card was reviewed
    Rating rating{Rating::Good};       // User's rating
    CardState state{CardState::New};   // Card state BEFORE the review
    TimePoint due;                     // When the card was due
    double stability{0.0};             // Stability BEFORE the review
    double difficulty{0.0};            // Difficulty BEFORE the review
    double elapsed_days{0.0};          // Days since previous review
    double scheduled_days{0.0};        // Interval that was scheduled
    TimePoint review;                  // When the review actually happened

    [[nodiscard]] auto operator==(const ReviewLog& other) const -> bool = default;
};

// ============================================================================
// Scheduled State: the card state after applying a specific rating
// ============================================================================

struct ScheduledState
{
    Card card;                         // Updated card with new scheduling parameters
    ReviewLog review_log;              // Log entry for this review
};

// ============================================================================
// Scheduling Result: shows what happens for EACH possible rating
// The UI shows all 4 options (Again/Hard/Good/Easy) with their intervals
// ============================================================================

struct SchedulingResult
{
    ScheduledState again;              // What happens if user rates Again
    ScheduledState hard;               // What happens if user rates Hard
    ScheduledState good;               // What happens if user rates Good
    ScheduledState easy;               // What happens if user rates Easy

    /// Get the scheduled state for a specific rating.
    [[nodiscard]] auto for_rating(Rating r) const -> const ScheduledState&;

    /// Get display intervals for the review buttons (e.g. "1m", "10m", "1d", "4d").
    [[nodiscard]] auto interval_display(Rating r) const -> std::string;
};

// ============================================================================
// Deck: a collection of cards
// ============================================================================

struct Deck
{
    std::string id;                    // Unique deck ID
    std::string name;                  // Display name
    std::string description;
    int new_cards_per_day{20};         // Maximum new cards introduced per day
    int max_reviews_per_day{200};      // Maximum reviews per day
    FSRSParameters params;             // Per-deck FSRS parameters (override global)
    bool use_global_params{true};      // If true, use global params instead of per-deck

    [[nodiscard]] auto operator==(const Deck& other) const -> bool = default;
};

// ============================================================================
// Due Card Counts
// ============================================================================

struct DueCardCounts
{
    int new_count{0};                  // Cards in New state
    int learning_count{0};             // Cards in Learning/Relearning state
    int review_count{0};               // Cards in Review state that are due
    int total_due{0};                  // Sum of all due cards

    [[nodiscard]] auto is_empty() const -> bool
    {
        return total_due == 0;
    }
};

} // namespace markamp::core::fsrs
```

### File: `src/core/fsrs/FSRSEngine.h`

```cpp
#pragma once

#include "FSRSTypes.h"

#include <chrono>

namespace markamp::core::fsrs
{

/// Core FSRS v3 scheduling engine.
/// All mathematical formulas are implemented as pure functions.
/// Thread-safe: the engine is stateless (all state is in the Card struct).
class FSRSEngine
{
public:
    explicit FSRSEngine(FSRSParameters params = FSRSParameters{});

    // ── Primary API ──

    /// Schedule a card after a review with the given rating.
    /// Returns all 4 possible outcomes (Again/Hard/Good/Easy).
    [[nodiscard]] auto schedule(const Card& card, TimePoint now) const
        -> SchedulingResult;

    /// Apply a specific rating to a card. Returns the updated card and review log.
    [[nodiscard]] auto review(const Card& card, Rating rating, TimePoint now) const
        -> ScheduledState;

    /// Calculate the retrievability (probability of recall) for a card at a given time.
    /// Returns a value between 0.0 and 1.0.
    [[nodiscard]] auto get_retrievability(const Card& card, TimePoint now) const
        -> double;

    // ── Parameter access ──

    [[nodiscard]] auto parameters() const -> const FSRSParameters&;
    void set_parameters(FSRSParameters params);

    // ── Core FSRS Formulas (public for testing) ──

    /// Calculate initial stability for a new card based on first rating.
    /// Formula: S_0(G) = w[G-1]
    [[nodiscard]] auto initial_stability(Rating rating) const -> double;

    /// Calculate initial difficulty for a new card based on first rating.
    /// Formula: D_0(G) = w[4] - exp(w[5] * (G - 1)) + 1
    /// Clamped to [1, 10].
    [[nodiscard]] auto initial_difficulty(Rating rating) const -> double;

    /// Calculate next difficulty after a review.
    /// Formula: D'(D, G) = w[7] * D_0(4) + (1 - w[7]) * (D - w[6] * (G - 3))
    /// (mean reversion toward initial difficulty of Easy rating)
    /// Clamped to [1, 10].
    [[nodiscard]] auto next_difficulty(double current_difficulty,
                                        Rating rating) const -> double;

    /// Calculate next stability after a SUCCESSFUL recall (Review state).
    /// Formula: S'_r(D, S, R, G) = S * (1 + exp(w[8]) * (11 - D) *
    ///          S^(-w[9]) * (exp((1-R) * w[10]) - 1) *
    ///          (G==Hard ? w[15] : G==Easy ? w[16] : 1))
    [[nodiscard]] auto next_recall_stability(double difficulty,
                                              double stability,
                                              double retrievability,
                                              Rating rating) const -> double;

    /// Calculate next stability after a FAILED recall (lapse).
    /// Formula: S'_f(D, S, R) = w[11] * D^(-w[12]) *
    ///          ((S+1)^w[13] - 1) * exp((1-R) * w[14])
    [[nodiscard]] auto next_forget_stability(double difficulty,
                                              double stability,
                                              double retrievability) const -> double;

    /// Calculate retrievability (recall probability) given elapsed days and stability.
    /// Formula: R(t, S) = (1 + t / (9 * S))^(-1)
    /// This is the power forgetting curve from the FSRS model.
    [[nodiscard]] static auto retrievability(double elapsed_days,
                                              double stability) -> double;

    /// Calculate the interval (in days) to achieve a target retrievability.
    /// Formula: I(R, S) = 9 * S * (1/R - 1)
    /// Used to convert stability to a concrete review interval.
    [[nodiscard]] auto next_interval(double stability) const -> double;

    /// Mean reversion function: blends a value toward a mean.
    /// Formula: mean_reversion(init, current) = w[7] * init + (1 - w[7]) * current
    [[nodiscard]] auto mean_reversion(double init, double current) const -> double;

private:
    FSRSParameters params_;

    /// Clamp difficulty to [1, 10].
    [[nodiscard]] static auto clamp_difficulty(double d) -> double;

    /// Clamp interval to [1, maximum_interval].
    [[nodiscard]] auto clamp_interval(double interval) const -> double;

    /// Schedule a New card.
    [[nodiscard]] auto schedule_new(const Card& card, TimePoint now) const
        -> SchedulingResult;

    /// Schedule a Learning/Relearning card.
    [[nodiscard]] auto schedule_learning(const Card& card, TimePoint now) const
        -> SchedulingResult;

    /// Schedule a Review card.
    [[nodiscard]] auto schedule_review(const Card& card, TimePoint now) const
        -> SchedulingResult;

    /// Build a ScheduledState for a specific rating transition.
    [[nodiscard]] auto build_scheduled_state(
        const Card& card,
        Rating rating,
        CardState new_state,
        double new_difficulty,
        double new_stability,
        double interval_days,
        TimePoint now) const -> ScheduledState;
};

} // namespace markamp::core::fsrs
```

### File: `src/core/fsrs/FlashcardStore.h`

```cpp
#pragma once

#include "FSRSTypes.h"

#include <expected>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::fsrs
{

/// Persistent storage for flashcards, decks, and review logs.
/// Stores data as JSON files in the workspace storage directory.
class FlashcardStore
{
public:
    explicit FlashcardStore(std::filesystem::path storage_root);

    // ── Card CRUD ──

    [[nodiscard]] auto create_card(const std::string& deck_id,
                                    const std::string& block_id,
                                    const std::string& front,
                                    const std::string& back)
        -> std::expected<Card, std::string>;

    [[nodiscard]] auto get_card(const std::string& card_id) const
        -> std::expected<Card, std::string>;

    [[nodiscard]] auto update_card(const Card& card)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto delete_card(const std::string& card_id)
        -> std::expected<void, std::string>;

    // ── Deck CRUD ──

    [[nodiscard]] auto create_deck(const std::string& name,
                                    const std::string& description = "")
        -> std::expected<Deck, std::string>;

    [[nodiscard]] auto get_deck(const std::string& deck_id) const
        -> std::expected<Deck, std::string>;

    [[nodiscard]] auto update_deck(const Deck& deck)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto delete_deck(const std::string& deck_id)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto get_all_decks() const -> std::vector<Deck>;

    // ── Queries ──

    /// Get all cards in a deck.
    [[nodiscard]] auto get_cards_in_deck(const std::string& deck_id) const
        -> std::vector<Card>;

    /// Get cards that are due for review (due <= now).
    [[nodiscard]] auto get_due_cards(const std::string& deck_id,
                                      TimePoint now) const
        -> std::vector<Card>;

    /// Get due card counts for a deck.
    [[nodiscard]] auto get_due_counts(const std::string& deck_id,
                                       TimePoint now) const
        -> DueCardCounts;

    /// Get all new (unreviewed) cards in a deck.
    [[nodiscard]] auto get_new_cards(const std::string& deck_id) const
        -> std::vector<Card>;

    // ── Review Log ──

    /// Append a review log entry.
    [[nodiscard]] auto add_review_log(const ReviewLog& log)
        -> std::expected<void, std::string>;

    /// Get review history for a card.
    [[nodiscard]] auto get_review_logs(const std::string& card_id) const
        -> std::vector<ReviewLog>;

    /// Get all review logs for a date range (for statistics).
    [[nodiscard]] auto get_review_logs_in_range(TimePoint start,
                                                 TimePoint end) const
        -> std::vector<ReviewLog>;

    // ── Persistence ──

    /// Save all data to disk.
    [[nodiscard]] auto save() -> std::expected<void, std::string>;

    /// Load all data from disk.
    [[nodiscard]] auto load() -> std::expected<void, std::string>;

private:
    std::filesystem::path storage_root_;
    mutable std::mutex mutex_;

    std::unordered_map<std::string, Deck> decks_;
    std::unordered_map<std::string, Card> cards_;
    std::vector<ReviewLog> review_logs_;

    [[nodiscard]] static auto generate_uuid() -> std::string;

    // JSON serialization
    [[nodiscard]] auto serialize_decks() const -> std::string;
    [[nodiscard]] auto serialize_cards() const -> std::string;
    [[nodiscard]] auto serialize_review_logs() const -> std::string;
    [[nodiscard]] auto deserialize_decks(const std::string& json)
        -> std::expected<void, std::string>;
    [[nodiscard]] auto deserialize_cards(const std::string& json)
        -> std::expected<void, std::string>;
    [[nodiscard]] auto deserialize_review_logs(const std::string& json)
        -> std::expected<void, std::string>;
};

} // namespace markamp::core::fsrs
```

### File: `src/core/fsrs/ReviewSession.h`

```cpp
#pragma once

#include "FlashcardStore.h"
#include "FSRSEngine.h"
#include "FSRSTypes.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
}

namespace markamp::core::fsrs
{

// ============================================================================
// Review Session State
// ============================================================================

enum class ReviewSessionState
{
    NotStarted,
    InProgress,
    ShowingAnswer,
    Completed
};

// ============================================================================
// Review Session: manages a single study session
// ============================================================================

class ReviewSession
{
public:
    ReviewSession(FlashcardStore* store,
                  FSRSEngine* engine,
                  EventBus* event_bus);

    /// Start a review session for a deck.
    /// Loads due cards + new cards (up to daily limits) and shuffles.
    [[nodiscard]] auto start(const std::string& deck_id)
        -> std::expected<void, std::string>;

    /// Get the current card being reviewed. Returns nullopt if session is complete.
    [[nodiscard]] auto current_card() const -> std::optional<Card>;

    /// Get the scheduling result for the current card (shows all 4 intervals).
    [[nodiscard]] auto current_scheduling() const -> std::optional<SchedulingResult>;

    /// Show the answer for the current card.
    void show_answer();

    /// Rate the current card and advance to the next.
    [[nodiscard]] auto rate(Rating rating) -> std::expected<void, std::string>;

    /// Skip the current card (move to end of queue).
    void skip();

    /// End the session early.
    void end_session();

    // ── Session state ──

    [[nodiscard]] auto state() const -> ReviewSessionState;
    [[nodiscard]] auto deck_id() const -> const std::string&;
    [[nodiscard]] auto cards_reviewed() const -> int;
    [[nodiscard]] auto cards_remaining() const -> int;
    [[nodiscard]] auto total_cards() const -> int;

    // ── Statistics for current session ──

    [[nodiscard]] auto again_count() const -> int;
    [[nodiscard]] auto hard_count() const -> int;
    [[nodiscard]] auto good_count() const -> int;
    [[nodiscard]] auto easy_count() const -> int;

private:
    FlashcardStore* store_{nullptr};
    FSRSEngine* engine_{nullptr};
    EventBus* event_bus_{nullptr};

    std::string deck_id_;
    ReviewSessionState state_{ReviewSessionState::NotStarted};

    std::vector<Card> queue_;          // Cards to review (ordered)
    int current_index_{0};

    // Session statistics
    int again_count_{0};
    int hard_count_{0};
    int good_count_{0};
    int easy_count_{0};
};

} // namespace markamp::core::fsrs
```

## Key Functions to Implement

### FSRSEngine.cpp

1. **`initial_stability(Rating rating)`** -- Return `params_.w[rating_to_int(rating) - 1]`. This gives the initial stability (in days) based on the first rating.

2. **`initial_difficulty(Rating rating)`** -- Compute `d = params_.w[4] - std::exp(params_.w[5] * (rating_to_int(rating) - 1)) + 1`. Return `clamp_difficulty(d)`.

3. **`next_difficulty(current_difficulty, Rating rating)`** -- Compute `new_d = mean_reversion(initial_difficulty(Rating::Easy), current_difficulty - params_.w[6] * (rating_to_int(rating) - 3))`. Return `clamp_difficulty(new_d)`.

4. **`mean_reversion(init, current)`** -- Return `params_.w[7] * init + (1.0 - params_.w[7]) * current`.

5. **`next_recall_stability(difficulty, stability, retrievability, rating)`** -- Compute: `hard_penalty = (rating == Rating::Hard) ? params_.w[15] : 1.0`. `easy_bonus = (rating == Rating::Easy) ? params_.w[16] : 1.0`. Return `stability * (1.0 + std::exp(params_.w[8]) * (11.0 - difficulty) * std::pow(stability, -params_.w[9]) * (std::exp((1.0 - retrievability) * params_.w[10]) - 1.0) * hard_penalty * easy_bonus)`.

6. **`next_forget_stability(difficulty, stability, retrievability)`** -- Return `params_.w[11] * std::pow(difficulty, -params_.w[12]) * (std::pow(stability + 1.0, params_.w[13]) - 1.0) * std::exp((1.0 - retrievability) * params_.w[14])`.

7. **`retrievability(elapsed_days, stability)`** -- If stability <= 0, return 0.0. Return `std::pow(1.0 + elapsed_days / (9.0 * stability), -1.0)`.

8. **`next_interval(stability)`** -- Compute `interval = 9.0 * stability * (1.0 / params_.request_retention - 1.0)`. Return `clamp_interval(std::round(interval))`. Minimum interval is 1 day.

9. **`schedule(card, now)`** -- Dispatch based on `card.state`: `New` -> `schedule_new()`, `Learning`/`Relearning` -> `schedule_learning()`, `Review` -> `schedule_review()`.

10. **`schedule_new(card, now)`** -- For each rating (Again, Hard, Good, Easy): compute `initial_stability(rating)`, `initial_difficulty(rating)`. Again -> state=Learning, interval=1min. Hard -> state=Learning, interval=5min. Good -> state=Learning, interval=10min. Easy -> state=Review, interval=`next_interval(stability)`.

11. **`schedule_learning(card, now)`** -- Compute `elapsed_days`. Compute `retrievability`. For Again: reset stability, state=Relearning, interval=5min. For Hard: slight stability increase, interval=10min. For Good: graduate to Review, stability=`next_recall_stability()`, interval=`next_interval()`. For Easy: graduate to Review with bonus.

12. **`schedule_review(card, now)`** -- Compute `elapsed_days = duration_cast<days>(now - card.last_review)`. Compute `retrievability(elapsed_days, card.stability)`. For Again: `new_stability = next_forget_stability()`, state=Relearning, lapses++, interval=5min. For Hard/Good/Easy: `new_stability = next_recall_stability()`, state=Review, reps++, interval=`next_interval(new_stability)`.

13. **`review(card, rating, now)`** -- Call `schedule(card, now)`. Return the `ScheduledState` for the given rating.

14. **`get_retrievability(card, now)`** -- If card is New, return 0.0. Compute `elapsed_days = duration_cast<hours>(now - card.last_review) / 24.0`. Return `retrievability(elapsed_days, card.stability)`.

### FlashcardStore.cpp

15. **`create_card(deck_id, block_id, front, back)`** -- Generate UUID. Construct Card with `state=New`, `due=now` (immediately available for first review). Store in `cards_`. Return.

16. **`get_due_cards(deck_id, now)`** -- Filter `cards_` by `deck_id` and `due <= now`. Sort by: Learning/Relearning first (shortest interval), then Review by due date, then New.

17. **`get_due_counts(deck_id, now)`** -- Count cards by state where `due <= now`.

18. **`save()`** -- Serialize decks, cards, review_logs to JSON. Write to `storage_root_/flashcards/decks.json`, `cards.json`, `review_logs.json` using `std::error_code` for filesystem operations.

19. **`load()`** -- Read JSON files from storage. Deserialize. Handle missing files gracefully (empty state).

### ReviewSession.cpp

20. **`start(deck_id)`** -- Load due cards and new cards from store. Order: Learning/Relearning first, then due Reviews, then New cards (up to daily new card limit). Store in `queue_`. Set `state_ = InProgress`. Publish `FlashcardReviewStartedEvent`.

21. **`rate(rating)`** -- Call `engine_->review(current_card, rating, now)`. Update card via `store_->update_card()`. Add review log via `store_->add_review_log()`. Increment rating counter. If the card is in Learning and was rated Again, re-insert at a later position in the queue. Advance to next card. If queue exhausted, set `state_ = Completed`. Publish `FlashcardCardReviewedEvent`.

22. **`current_scheduling()`** -- Call `engine_->schedule(current_card, now)`. Return the `SchedulingResult` so the UI can display intervals for each button.

## Events to Add

```cpp
// ============================================================================
// Flashcard / FSRS events (Phase 24)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardReviewStartedEvent)
std::string deck_id;
int total_cards;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardCardReviewedEvent)
std::string card_id;
std::string deck_id;
int rating;        // 1=Again, 2=Hard, 3=Good, 4=Easy
double new_interval_days;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardReviewCompletedEvent)
std::string deck_id;
int cards_reviewed;
int again_count;
int hard_count;
int good_count;
int easy_count;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardDeckCreatedEvent)
std::string deck_id;
std::string deck_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardCardCreatedEvent)
std::string card_id;
std::string deck_id;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|---|---|---|---|
| `fsrs.request_retention` | double | `0.9` | Target recall probability (0.0-1.0) |
| `fsrs.maximum_interval` | int | `36500` | Maximum review interval in days |
| `fsrs.new_cards_per_day` | int | `20` | Default new cards per day per deck |
| `fsrs.max_reviews_per_day` | int | `200` | Maximum reviews per day per deck |
| `fsrs.learning_steps_minutes` | string | `"1,10"` | Comma-separated learning step intervals in minutes |
| `fsrs.relearning_steps_minutes` | string | `"10"` | Comma-separated relearning step intervals in minutes |
| `fsrs.auto_save_after_review` | bool | `true` | Auto-save after each card review |

## Test Cases

### File: `tests/unit/test_fsrs_engine.cpp`

All tests use Catch2 with `[fsrs][engine]` tags.

1. **New card schedule: all 4 ratings produce valid states** -- Create a new card. Call `schedule(card, now)`. Verify `again` state is Learning with short interval. Verify `hard` state is Learning. Verify `good` state is Learning with longer interval. Verify `easy` state is Review with multi-day interval. Verify all stability values are positive. Verify all difficulty values are in [1, 10].

2. **Learning card: Good graduates to Review** -- Create a card in Learning state with `stability=2.4`, `difficulty=5.0`. Rate Good. Verify new state is Review. Verify `scheduled_days > 0`. Verify `stability > 2.4` (stability should increase).

3. **Review card: Again causes lapse** -- Create a card in Review state with `stability=20.0`, `difficulty=5.0`, `reps=5`. Rate Again. Verify new state is Relearning. Verify `lapses` incremented. Verify new stability is less than 20.0 (stability decreases on lapse). Verify new interval is short (learning step).

4. **Review card: Easy gives bonus** -- Create a card in Review state. Schedule. Compare the `easy.card.stability` with `good.card.stability`. Verify Easy stability is greater than Good stability (Easy bonus applied).

5. **Difficulty bounds: clamped to [1, 10]** -- Create cards and rate repeatedly with Again to increase difficulty. Verify difficulty never exceeds 10.0. Rate repeatedly with Easy to decrease difficulty. Verify difficulty never goes below 1.0.

6. **Stability increase on successful recall** -- Create Review card with stability=10.0. Rate Good. Verify new stability > 10.0. The amount of increase should depend on difficulty and retrievability.

7. **Retrievability decay** -- Create a card with stability=10.0. At elapsed_days=0, retrievability should be ~1.0. At elapsed_days=10, retrievability should be ~0.5 (10 days = stability). At elapsed_days=100, retrievability should be very low. Verify monotonic decrease.

8. **Parameter customization** -- Create engine with custom parameters (all w values doubled). Schedule the same card with both default and custom engines. Verify results differ. This proves parameters are being used.

9. **Edge case: 0 elapsed days** -- Create a Review card. Set `last_review = now`. Rate Good (0 elapsed days). Verify no crash or NaN. Verify stability change is minimal.

10. **Batch scheduling: 10000 cards** -- Create 10000 cards in various states. Schedule all of them. Measure wall time. Verify under 100ms. Verify no NaN or infinite values in any output.

11. **Deterministic output** -- Schedule the same card with the same parameters and same timestamp twice. Verify the results are bit-for-bit identical. This ensures no randomness in the scheduling algorithm.

12. **next_interval: achieves target retention** -- For a card with stability=10.0 and request_retention=0.9, compute `next_interval()`. Then verify `retrievability(interval, stability) >= 0.89` and `<= 0.91` (approximately the target retention).

### File: `tests/unit/test_flashcard_store.cpp`

All tests use Catch2 with `[fsrs][store]` tags.

1. **Create and retrieve deck** -- Create deck "Math". Retrieve by ID. Verify name matches.
2. **Create card in deck** -- Create deck. Create card in it. Get cards in deck. Verify card is present.
3. **Due card query** -- Create 5 cards: 2 due now, 2 due tomorrow, 1 new. Query due cards. Verify returns the 2 due + 1 new.
4. **Review log persistence** -- Add review log. Save to disk. Load from disk. Verify log is present with correct fields.
5. **Save/load round-trip** -- Create deck with 10 cards and 5 review logs. Save. Clear memory. Load. Verify all data intact.
6. **Corrupt file handling** -- Write invalid JSON to cards file. Call `load()`. Verify returns error, not crash.

## Acceptance Criteria

- [ ] `FSRSEngine::schedule()` produces valid scheduling results for all 4 card states
- [ ] All FSRS formulas match the go-fsrs reference implementation
- [ ] `initial_stability()` returns `w[rating-1]` for each rating
- [ ] `initial_difficulty()` uses `w[4]` and `w[5]` and clamps to [1, 10]
- [ ] `next_recall_stability()` applies hard penalty (`w[15]`) and easy bonus (`w[16]`)
- [ ] `next_forget_stability()` uses `w[11..14]` parameters
- [ ] `retrievability()` follows the power forgetting curve formula
- [ ] `next_interval()` converts stability to days using `request_retention`
- [ ] Card state transitions are correct: New->Learning->Review, Review->Relearning on lapse
- [ ] `FlashcardStore` saves and loads decks, cards, review logs from JSON files
- [ ] `ReviewSession` manages card queue with correct ordering (learning first, then reviews, then new)
- [ ] All 12 engine test cases pass
- [ ] All 6 store test cases pass
- [ ] No use of `catch(...)` -- all exceptions typed
- [ ] All query methods have `[[nodiscard]]`
- [ ] Output is deterministic (same input = same output, no randomness)

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSTypes.h`
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSTypes.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSEngine.h`
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FSRSEngine.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/ReviewSession.h`
- `/Users/ryanrentfro/code/markamp/src/core/fsrs/ReviewSession.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_fsrs_engine.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_flashcard_store.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/core/Events.h` -- Add 5 flashcard/review events
- `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h` -- Forward-declare `FSRSEngine`, `FlashcardStore`; add pointer fields (optional, for plugin access)
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 4 new `.cpp` source files
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_fsrs_engine` and `test_flashcard_store` test targets
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 7 new `fsrs.*` config keys
