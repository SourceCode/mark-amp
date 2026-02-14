# Phase 26 -- Flashcard Review UI & Session

## Objective

Implement the interactive flashcard review user interface and review session engine. A review session presents cards from a selected deck one at a time: the user sees the card's block content (the "front"), reveals the answer (the "back"), and then rates their recall using one of four buttons -- Again, Hard, Good, or Easy. Each rating feeds into the FSRS scheduler (Phase 24) to compute the next review interval, and the DeckManager (Phase 25) persists the updated card state. The review UI shows real-time progress (cards reviewed vs. total), session statistics, and a preview of the next interval for each rating option so the user can make an informed choice.

SiYuan's implementation in `app/src/card/openCard.ts` (approximately 900 lines) manages the full review lifecycle: shuffling due and new cards into a queue, rendering block content as HTML, handling the reveal/rate cycle, supporting undo of the last rating, and presenting session summary statistics when all cards are reviewed. MarkAmp ports this as a ReviewSession class that encapsulates the session state machine and a FlashcardReviewPanel (wxPanel) that renders the interactive UI. The panel uses MarkAmp's existing HtmlRenderer to convert block markdown content to HTML for display in a wxWebView or wxHtmlWindow embedded within the review panel.

Additionally, this phase implements card creation workflows. Users can turn any block into a flashcard via a "Make Flashcard" context menu entry. The system supports multiple card formats: mark-based cards (content between `==` markers splits front/back), list-based cards (first item is front, remaining items are back), heading-based cards (heading text is front, content under heading is back), and full-block cards (entire block content shown, user self-evaluates). A FlashcardBrowserPanel provides a searchable, filterable list of all cards across decks with bulk operations (reset, remove, move to another deck).

## Prerequisites

- Phase 24 (FSRS Scheduling Algorithm) -- provides the scheduling computations that determine next review intervals
- Phase 25 (Flashcard Deck Management) -- provides DeckManager, FlashcardDeck, FlashcardCard, and persistence

## SiYuan Source Reference

- `app/src/card/openCard.ts` (~900 lines) -- Review dialog lifecycle, card queue management, rating handlers, undo logic, session stats computation, interval preview display
- `app/src/card/makeCard.ts` -- "Make Flashcard" action, block-to-card conversion, mark-based card detection
- `app/src/card/viewCards.ts` -- Card browser/viewer with filtering, sorting, bulk operations
- `kernel/model/flashcard.go` -- Server-side card retrieval, due card computation, review persistence

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/core/ReviewSession.h` | `markamp::core` | ReviewSession class and supporting types |
| `src/core/ReviewSession.cpp` | `markamp::core` | ReviewSession implementation |
| `src/core/CardFactory.h` | `markamp::core` | CardFactory for block-to-card conversion |
| `src/core/CardFactory.cpp` | `markamp::core` | CardFactory implementation |
| `src/ui/FlashcardReviewPanel.h` | `markamp::ui` | Review UI panel declaration |
| `src/ui/FlashcardReviewPanel.cpp` | `markamp::ui` | Review UI panel implementation |
| `src/ui/FlashcardBrowserPanel.h` | `markamp::ui` | Card browser panel declaration |
| `src/ui/FlashcardBrowserPanel.cpp` | `markamp::ui` | Card browser panel implementation |
| `tests/unit/test_review_session.cpp` | (test) | Catch2 test suite for review session logic |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Events.h` | Add ReviewSession and CardCreation events |
| `src/ui/MainFrame.h` | Add menu entries and panel management for flashcard UI |
| `src/ui/MainFrame.cpp` | Wire "Make Flashcard" context menu and review panel activation |
| `src/CMakeLists.txt` | Add new source files |
| `tests/CMakeLists.txt` | Add test_review_session target |

## Data Structures to Implement

```cpp
// ============================================================================
// File: src/core/ReviewSession.h
// ============================================================================
#pragma once

#include "FlashcardTypes.h"

#include <chrono>
#include <deque>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class DeckManager;
class EventBus;

/// A card prepared for review, including rendered content and scheduling previews.
struct ReviewCard
{
    FlashcardCard card;
    std::string block_content_markdown; // Raw markdown of the referenced block
    std::string rendered_front_html;    // Rendered HTML for the "front" (question)
    std::string rendered_back_html;     // Rendered HTML for the "back" (answer)

    /// Preview of next scheduling state for each possible rating.
    struct SchedulePreview
    {
        int interval_days{0};
        double stability{0.0};
        double difficulty{0.0};
        std::string interval_display; // e.g. "1d", "3d", "2w", "3mo"
    };

    std::unordered_map<Rating, SchedulePreview> next_states;

    [[nodiscard]] auto has_separate_back() const -> bool
    {
        return !rendered_back_html.empty() && rendered_back_html != rendered_front_html;
    }
};

/// Statistics for the current review session.
struct SessionStats
{
    int total_cards{0};
    int reviewed{0};
    int remaining{0};
    int new_reviewed{0};
    int review_reviewed{0};
    int correct_count{0};    // Good or Easy
    int again_count{0};      // Again
    int hard_count{0};       // Hard
    int easy_count{0};       // Easy
    double elapsed_seconds{0.0};

    [[nodiscard]] auto completion_fraction() const -> double
    {
        if (total_cards == 0)
        {
            return 0.0;
        }
        return static_cast<double>(reviewed) / static_cast<double>(total_cards);
    }

    [[nodiscard]] auto accuracy_fraction() const -> double
    {
        if (reviewed == 0)
        {
            return 0.0;
        }
        return static_cast<double>(correct_count) / static_cast<double>(reviewed);
    }
};

/// Result returned after rating a card.
struct NextCardResult
{
    bool session_complete{false};       // True if no more cards remain
    std::optional<ReviewCard> next_card; // The next card to review, if any
    SessionStats stats;
};

/// Record of a completed rating, used for undo.
struct ReviewRecord
{
    std::string card_id;
    Rating rating;
    FlashcardCard card_state_before; // Card state before the rating was applied
};

/// Manages the state of an active review session.
/// Owns the review queue, tracks statistics, and supports undo.
class ReviewSession
{
public:
    ReviewSession(DeckManager& deck_manager, EventBus& event_bus);
    ~ReviewSession() = default;

    /// Start a new review session for the given deck.
    /// Fetches due + new cards up to the configured limits.
    /// Returns the session ID and the first card to review.
    [[nodiscard]] auto start_review(const std::string& deck_id,
                                    int new_limit,
                                    int review_limit)
        -> std::expected<ReviewCard, std::string>;

    /// Get the current card being reviewed (without advancing).
    [[nodiscard]] auto get_current_card() const -> std::optional<ReviewCard>;

    /// Rate the current card and advance to the next.
    [[nodiscard]] auto rate_card(Rating rating) -> std::expected<NextCardResult, std::string>;

    /// Skip the current card (move to end of queue).
    [[nodiscard]] auto skip_card() -> std::expected<NextCardResult, std::string>;

    /// Undo the last rating, restoring the previous card state.
    [[nodiscard]] auto undo_last_rating() -> std::expected<ReviewCard, std::string>;

    /// Get current session statistics.
    [[nodiscard]] auto get_session_stats() const -> SessionStats;

    /// End the session and publish summary event.
    void end_session();

    /// Whether a session is currently active.
    [[nodiscard]] auto is_active() const -> bool
    {
        return active_;
    }

    /// The deck ID for this session.
    [[nodiscard]] auto deck_id() const -> const std::string&
    {
        return deck_id_;
    }

private:
    DeckManager& deck_manager_;
    EventBus& event_bus_;

    bool active_{false};
    std::string deck_id_;
    std::string session_id_;

    std::deque<ReviewCard> queue_;
    std::vector<ReviewRecord> history_;
    SessionStats stats_;
    std::chrono::steady_clock::time_point session_start_;

    /// Build a ReviewCard from a FlashcardCard, including content rendering
    /// and schedule previews.
    [[nodiscard]] auto build_review_card(const FlashcardCard& card)
        -> std::expected<ReviewCard, std::string>;

    /// Compute schedule previews for all four ratings.
    [[nodiscard]] auto compute_previews(const FlashcardCard& card)
        -> std::unordered_map<Rating, ReviewCard::SchedulePreview>;

    /// Format an interval in days to a human-readable string.
    [[nodiscard]] static auto format_interval(int days) -> std::string;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/CardFactory.h
// ============================================================================
#pragma once

#include <string>
#include <utility>

namespace markamp::core
{

/// The type of card determined by the block structure.
enum class CardFormat : uint8_t
{
    FullBlock,    // Entire block shown, self-evaluation
    MarkBased,    // Front/back split at ==marker==
    ListBased,    // First list item = front, rest = back
    HeadingBased  // Heading = front, content below = back
};

/// Result of analyzing a block to determine card front/back.
struct CardContent
{
    std::string front_markdown; // The "question" side
    std::string back_markdown;  // The "answer" side (empty for FullBlock)
    CardFormat format{CardFormat::FullBlock};

    [[nodiscard]] auto has_back() const -> bool
    {
        return !back_markdown.empty();
    }
};

/// Factory for converting Markdown block content into flashcard front/back pairs.
/// Analyzes the block structure to determine the best card format.
class CardFactory
{
public:
    /// Analyze a block's markdown content and produce front/back card content.
    [[nodiscard]] auto create_card_content(const std::string& block_markdown) const
        -> CardContent;

    /// Detect if the content contains ==mark== delimiters for front/back split.
    [[nodiscard]] auto detect_mark_split(const std::string& markdown) const
        -> bool;

    /// Detect if the content is a list where first item can serve as front.
    [[nodiscard]] auto detect_list_split(const std::string& markdown) const
        -> bool;

    /// Detect if the content starts with a heading.
    [[nodiscard]] auto detect_heading_split(const std::string& markdown) const
        -> bool;

private:
    [[nodiscard]] auto split_by_mark(const std::string& markdown) const
        -> std::pair<std::string, std::string>;

    [[nodiscard]] auto split_by_list(const std::string& markdown) const
        -> std::pair<std::string, std::string>;

    [[nodiscard]] auto split_by_heading(const std::string& markdown) const
        -> std::pair<std::string, std::string>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`ReviewSession::start_review(deck_id, new_limit, review_limit)`** -- Fetch due cards via `deck_manager_.get_due_cards(deck_id, review_limit)` and new cards via `deck_manager_.get_new_cards(deck_id, new_limit)`. Interleave new cards into the review queue (1 new per 5 reviews). Build ReviewCard objects for each. Initialize session stats. Publish `ReviewSessionStartedEvent`. Return the first ReviewCard or error if deck is empty.

2. **`ReviewSession::rate_card(rating)`** -- Record the current card's pre-rating state in `history_`. Apply the FSRS scheduling algorithm to compute new card fields based on the rating. Call `deck_manager_.update_card_after_review()` to persist. Update `stats_` counters. Pop the current card from the queue. Publish `CardRatedEvent`. Return `NextCardResult` with the next card (or `session_complete=true` if queue is empty).

3. **`ReviewSession::undo_last_rating()`** -- Pop the last entry from `history_`. Restore the card's scheduling fields to `card_state_before`. Call `deck_manager_.update_card_after_review()` with the restored state. Push the card back to the front of the queue. Decrement `stats_` counters. Return the restored ReviewCard.

4. **`ReviewSession::skip_card()`** -- Move the current card from the front to the back of the queue. Do not modify scheduling state. Return the next card at the front.

5. **`ReviewSession::end_session()`** -- Compute final elapsed time. Publish `ReviewSessionEndedEvent` with the final SessionStats. Set `active_ = false`. Clear the queue and history.

6. **`ReviewSession::build_review_card(card)`** -- Retrieve the block's markdown content using the card's `block_id`. Use `CardFactory::create_card_content()` to split into front/back. Render front and back markdown to HTML using MarkAmp's HtmlRenderer. Compute schedule previews for all four ratings. Return the assembled ReviewCard.

7. **`ReviewSession::compute_previews(card)`** -- For each Rating (Again, Hard, Good, Easy), run the FSRS scheduler to compute the resulting interval, stability, and difficulty. Format the interval to a display string (e.g., "10m", "1d", "3d", "2w", "3mo").

8. **`ReviewSession::format_interval(days)`** -- Convert integer days to human-readable string: 0 -> "<10m", 1 -> "1d", 7 -> "1w", 30 -> "1mo", 365 -> "1yr". Use the most appropriate unit.

9. **`FlashcardReviewPanel::OnRevealClicked()`** -- Toggle the back side visibility. When revealed, show the rendered_back_html below the front, enable the four rating buttons, and display the interval preview on each button (e.g., "Again (10m)", "Good (3d)").

10. **`FlashcardReviewPanel::OnRatingClicked(rating)`** -- Call `session_.rate_card(rating)`. If session is complete, show the session summary view. Otherwise, update the card display with the next card, hide the back side, and update the progress bar.

11. **`FlashcardBrowserPanel::populate_card_list()`** -- Fetch all cards from DeckManager, apply current filter (deck, state, search text). Sort by the selected column (due date, created, difficulty). Display in a wxListCtrl with columns: Block Preview, Deck, State, Due Date, Interval, Reviews.

12. **`CardFactory::create_card_content(block_markdown)`** -- Detect card format in priority order: mark-based > list-based > heading-based > full-block. Split content into front/back markdown accordingly. Return CardContent with the detected format.

## Events to Add

Add the following to `src/core/Events.h`:

```cpp
// ============================================================================
// Flashcard review events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReviewSessionStartedEvent)
std::string session_id;
std::string deck_id;
int total_cards{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReviewSessionEndedEvent)
std::string session_id;
std::string deck_id;
int reviewed{0};
int correct{0};
int again{0};
double elapsed_seconds{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardRatedEvent)
std::string card_id;
std::string deck_id;
Rating rating{Rating::Good};
int next_interval_days{0};
std::string next_due_display; // e.g. "3d", "2w"
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MakeFlashcardRequestEvent)
std::string block_id;
std::string block_content;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.flashcard.auto_reveal` | bool | false | Automatically reveal back side after delay |
| `knowledgebase.flashcard.auto_reveal_delay_ms` | int | 3000 | Delay before auto-reveal in milliseconds |
| `knowledgebase.flashcard.show_interval_preview` | bool | true | Show next interval on rating buttons |
| `knowledgebase.flashcard.review_order` | string | "mixed" | Order: "new_first", "review_first", "mixed" |

## Test Cases

All tests in `tests/unit/test_review_session.cpp` using Catch2.

1. **"ReviewSession starts with correct card count from due + new"** -- Create a deck with 5 new cards and 3 due cards. Start a review session with `new_limit=5, review_limit=3`. Verify `get_session_stats().total_cards == 8`.

2. **"rate_card advances to next card and updates stats"** -- Start a session with 3 cards. Rate the first card as Good. Verify `stats.reviewed == 1`, `stats.remaining == 2`, `stats.correct_count == 1`. Verify `get_current_card()` returns a different card.

3. **"rate_card with Again increments again_count"** -- Rate a card as Again. Verify `stats.again_count == 1`, `stats.correct_count == 0`.

4. **"undo_last_rating restores previous card and decrements stats"** -- Rate a card as Good (reviewed=1, correct=1). Call `undo_last_rating()`. Verify `stats.reviewed == 0`, `stats.correct_count == 0`. Verify the undone card is now the current card again.

5. **"skip_card moves card to end of queue"** -- Start with 3 cards, note the first card's ID. Call `skip_card()`. Verify the current card is now different. Verify `stats.reviewed` is still 0 (skip does not count as a review).

6. **"session completes when all cards rated"** -- Start with 2 cards. Rate both as Good. Verify the second `rate_card` returns `NextCardResult.session_complete == true`.

7. **"end_session publishes ReviewSessionEndedEvent with stats"** -- Start a session, review some cards, call `end_session()`. Capture the `ReviewSessionEndedEvent` via EventBus subscription. Verify the event contains correct reviewed count and elapsed time > 0.

8. **"CardFactory detects mark-based card format"** -- Input: `"What is 2+2? ==4==\n\nThe answer is four."`. Verify `detect_mark_split()` returns true. Verify front = "What is 2+2?" and back = "4\n\nThe answer is four."

9. **"CardFactory detects list-based card format"** -- Input: `"- Capital of France?\n- Paris\n- Located on the Seine"`. Verify format is `CardFormat::ListBased`, front = "Capital of France?", back = "Paris\nLocated on the Seine".

10. **"CardFactory falls back to FullBlock for plain paragraphs"** -- Input: `"This is a simple paragraph with no special structure."`. Verify format is `CardFormat::FullBlock` and `has_back()` is false.

## Acceptance Criteria

- [ ] ReviewSession correctly interleaves new and due cards in the review queue
- [ ] rate_card updates FSRS scheduling fields and persists via DeckManager
- [ ] undo_last_rating fully restores the previous card state and session stats
- [ ] skip_card moves the card to the end without modifying scheduling data
- [ ] Session auto-completes when all cards have been rated
- [ ] ReviewSessionStartedEvent and ReviewSessionEndedEvent publish with accurate stats
- [ ] FlashcardReviewPanel renders block content as HTML with reveal/rate cycle
- [ ] Rating buttons display interval previews (e.g., "Again (10m)", "Good (3d)")
- [ ] CardFactory correctly detects mark-based, list-based, heading-based, and full-block formats
- [ ] FlashcardBrowserPanel lists all cards with filtering by deck, state, and search text

## Files to Create/Modify

```
CREATE  src/core/ReviewSession.h
CREATE  src/core/ReviewSession.cpp
CREATE  src/core/CardFactory.h
CREATE  src/core/CardFactory.cpp
CREATE  src/ui/FlashcardReviewPanel.h
CREATE  src/ui/FlashcardReviewPanel.cpp
CREATE  src/ui/FlashcardBrowserPanel.h
CREATE  src/ui/FlashcardBrowserPanel.cpp
CREATE  tests/unit/test_review_session.cpp
MODIFY  src/core/Events.h           -- add review session and card creation events
MODIFY  src/ui/MainFrame.h          -- add flashcard review panel management
MODIFY  src/ui/MainFrame.cpp        -- wire "Make Flashcard" context menu, review panel
MODIFY  src/CMakeLists.txt           -- add new source files
MODIFY  tests/CMakeLists.txt         -- add test_review_session target
MODIFY  resources/config_defaults.json -- add flashcard UI config defaults
```
