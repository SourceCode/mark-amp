# Phase 22: Flashcard and Spaced Repetition

## Overview
The FSRS (Free Spaced Repetition Scheduler) system has core infrastructure: FSRSEngine, FSRSReviewSession, FSRSTypes, and FlashcardStore in `src/core/fsrs/`. UI components FlashcardBrowserPanel and FlashcardReviewPanel exist in `src/ui/`. ReviewSession exists in core. However, this spaced repetition system is not accessible as a workspace feature. This phase connects the FSRS engine to a functional flashcard experience.

## Prerequisites
- Phase 06 (Workbench navigation for Flashcard mode)
- Phase 07 (Editor for flashcard markup)
- Phase 03 (Theme engine for flashcard styling)

## Tasks

### Task 1: Wire FlashcardStore to Workspace
**Files:** `src/core/fsrs/FlashcardStore.cpp`, `src/core/VaultService.cpp`
**Description:** FlashcardStore exists. Wire it to persist flashcards in `.markamp/flashcards/`. Each deck stored as a JSON file with card data and review history.
**Acceptance Criteria:**
- Flashcards stored in `.markamp/flashcards/{deck_id}.json`
- Auto-save on card create/edit/review
- Load all decks on workspace open
- `FlashcardDeckLoadedEvent` emitted per deck
- Deck metadata: name, card count, due count, last reviewed

### Task 2: Wire FlashcardBrowserPanel as Deck Manager
**Files:** `src/ui/FlashcardBrowserPanel.cpp`, `src/ui/FlashcardBrowserPanel.h`
**Description:** FlashcardBrowserPanel exists. Wire it as the deck management UI: list decks, create/delete decks, show deck statistics.
**Acceptance Criteria:**
- Panel lists all decks with name, card count, due count
- "New Deck" creates empty deck
- "Delete Deck" with confirmation
- Deck statistics: total cards, new, learning, review, mature
- Click deck opens deck editor
- Due indicator for decks with cards due today

### Task 3: Wire FlashcardReviewPanel for Study Sessions
**Files:** `src/ui/FlashcardReviewPanel.cpp`, `src/ui/FlashcardReviewPanel.h`
**Description:** FlashcardReviewPanel exists. Wire it as the review interface: show front, reveal back on click/space, rate difficulty (Again, Hard, Good, Easy).
**Acceptance Criteria:**
- Front of card shown
- Space/click reveals back
- Rating buttons: Again, Hard, Good, Easy
- Next interval shown for each rating (preview)
- Progress bar: cards remaining in session
- Session summary on completion

### Task 4: Wire FSRSEngine Scheduling
**Files:** `src/core/fsrs/FSRSEngine.cpp`, `src/core/fsrs/FSRSTypes.cpp`
**Description:** FSRSEngine implements the FSRS algorithm. Wire scheduling: compute next review date based on rating, update card stability and difficulty parameters.
**Acceptance Criteria:**
- FSRS-5 algorithm implementation
- Rating maps: Again (1), Hard (2), Good (3), Easy (4)
- Next interval computed per rating
- Card state transitions: New -> Learning -> Review -> Relearning
- Stability and difficulty parameters updated
- Intervals grow with successful reviews (spaced repetition)

### Task 5: Wire FSRSReviewSession for Session Management
**Files:** `src/core/fsrs/FSRSReviewSession.cpp`
**Description:** FSRSReviewSession manages review sessions. Wire session lifecycle: start session with due cards, track progress, save results.
**Acceptance Criteria:**
- Session starts with all due cards from selected deck(s)
- Cards presented in order: overdue first, then due today
- New card limit configurable (default: 20 per day)
- Review limit configurable (default: 200 per day)
- Session results saved to review history
- `ReviewSessionCompletedEvent` emitted

### Task 6: Wire Markdown Flashcard Extraction
**Files:** `src/core/fsrs/FlashcardStore.cpp`, `src/core/MarkdownParser.cpp`
**Description:** Extract flashcards from Markdown documents using special syntax: `?? front / back ??` creates a basic card, `?? front / back / hint ??` creates a card with hint.
**Acceptance Criteria:**
- `?? front / back ??` parsed as flashcard
- `?? front / back / hint ??` parsed with hint
- Cards linked to source document and line number
- Re-scan on document save updates cards
- Deleted syntax removes card (with confirmation)
- `FlashcardExtractedEvent` emitted

### Task 7: Wire Cloze Deletion Cards
**Files:** `src/core/fsrs/FlashcardStore.cpp`, `src/ui/FlashcardReviewPanel.cpp`
**Description:** Support cloze deletion: `??c1 {{word}} in a sentence ??` creates a card where `{{word}}` is hidden during review.
**Acceptance Criteria:**
- `{{text}}` syntax marks cloze deletions
- Multiple cloze deletions per card: `{{c1::word1}}`, `{{c2::word2}}`
- Each cloze generates a separate review card
- Cloze hint: `{{c1::word::hint}}`
- Review shows sentence with blank for active cloze

### Task 8: Wire Card Editor
**Files:** `src/ui/FlashcardBrowserPanel.cpp`
**Description:** Card editor for manual flashcard creation: front (Markdown), back (Markdown), tags, deck assignment. Rich editor with Markdown preview.
**Acceptance Criteria:**
- Front field: Markdown editor with preview
- Back field: Markdown editor with preview
- Tags: tag input with autocomplete from existing tags
- Deck selector: dropdown of existing decks
- Save and "Save and Add Another" buttons
- Edit existing cards from browser

### Task 9: Wire Deck Statistics Dashboard
**Files:** `src/ui/FlashcardBrowserPanel.cpp`
**Description:** Deck detail view shows statistics: review forecast (cards due per day for next 30 days), retention rate, review history chart, card maturity distribution.
**Acceptance Criteria:**
- Review forecast: bar chart of due cards per day
- Retention rate: percentage of correct reviews
- Review history: reviews per day for last 30 days
- Maturity distribution: new, learning, young, mature
- Statistics update after each review session

### Task 10: Wire Study Streak and Heatmap
**Files:** `src/ui/FlashcardBrowserPanel.cpp`
**Description:** Track daily review streaks and show a GitHub-style activity heatmap of review days. Motivational feature for consistent study.
**Acceptance Criteria:**
- Current streak count shown on dashboard
- Heatmap shows review activity for last 365 days
- Color intensity proportional to cards reviewed
- Streak milestone badges: 7, 30, 100, 365 days
- "Review today to maintain streak" reminder

### Task 11: Wire Card Tagging and Filtering
**Files:** `src/core/fsrs/FlashcardStore.cpp`, `src/ui/FlashcardBrowserPanel.cpp`
**Description:** Cards can be tagged. Browser filters by tag, deck, state (new, learning, review, suspended), and due date.
**Acceptance Criteria:**
- Cards can have multiple tags
- Filter by tag, deck, card state
- Search cards by front/back content
- Sort by due date, difficulty, created date
- Bulk operations: tag, move to deck, suspend, delete

### Task 12: Wire Card Suspension and Leech Detection
**Files:** `src/core/fsrs/FSRSEngine.cpp`, `src/ui/FlashcardReviewPanel.cpp`
**Description:** Cards can be suspended (removed from review queue). Leech detection: cards with excessive "Again" ratings flagged as leeches with suggestion to edit or suspend.
**Acceptance Criteria:**
- Suspend card: right-click context menu
- Unsuspend card: from browser
- Leech threshold: 8 lapses (configurable)
- Leech detected: notification with "Edit" and "Suspend" actions
- Leech indicator in browser
- `FlashcardLeechDetectedEvent` emitted

### Task 13: Wire Review Undo
**Files:** `src/ui/FlashcardReviewPanel.cpp`, `src/core/fsrs/FSRSReviewSession.cpp`
**Description:** During review, undo last rating to re-review the card. Useful for accidental button presses.
**Acceptance Criteria:**
- "Undo" button or Cmd+Z during review
- Returns to previous card
- Previous rating reverted
- Card scheduling restored to pre-rating state
- Undo available for last 5 cards in session

### Task 14: Wire Flashcard Import/Export
**Files:** `src/core/fsrs/FlashcardStore.cpp`
**Description:** Import from Anki (.apkg), CSV, and JSON. Export to CSV, JSON. Preserves review history on import.
**Acceptance Criteria:**
- Import CSV: front, back, tags columns
- Import JSON: structured card data
- Import Anki: .apkg with media extraction (best effort)
- Export CSV and JSON
- Review history preserved where possible

### Task 15: Wire Flashcard Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register flashcard commands: "Flashcard: Start Review", "Flashcard: Browse Decks", "Flashcard: Add Card", "Flashcard: Import", "Flashcard: Statistics".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Flashcard:" prefix
- "Start Review" begins session with due cards across all decks
- "Add Card" opens card editor
- Keyboard shortcut for "Start Review"

### Task 16: Wire Flashcard Theme Integration
**Files:** `src/ui/FlashcardReviewPanel.cpp`, `src/ui/FlashcardBrowserPanel.cpp`
**Description:** Flashcard UI uses theme tokens: card background, rating button colors, progress bar, statistics charts.
**Acceptance Criteria:**
- Card background from `card_bg` token
- Rating buttons: Again (red), Hard (orange), Good (green), Easy (blue)
- Progress bar uses accent color
- Statistics charts use theme-aware colors
- Dark mode: card content readable

### Task 17: Wire Review Notifications
**Files:** `src/core/NotificationService.cpp`, `src/core/fsrs/FlashcardStore.cpp`
**Description:** Daily notification when cards are due for review. Notification includes deck name and count.
**Acceptance Criteria:**
- Notification on app start if cards are due
- Notification content: "X cards due in Y decks"
- Click notification opens review session
- Configurable: enable/disable, time of day
- Badge on activity bar for due cards

### Task 18: Wire Cross-Reference from Cards to Documents
**Files:** `src/core/fsrs/FlashcardStore.cpp`, `src/ui/FlashcardReviewPanel.cpp`
**Description:** Cards extracted from Markdown maintain a link to their source document. During review, "Show Source" navigates to the originating document.
**Acceptance Criteria:**
- "Show Source" button during review
- Click navigates to source document at card's line
- Source document opens in split view alongside review
- Source link shown in card browser
- Broken source link (deleted document) handled gracefully

### Task 19: Wire Custom Card Templates
**Files:** `src/core/fsrs/FlashcardStore.cpp`
**Description:** Card templates define additional fields beyond front/back: vocabulary (word, reading, meaning, example), Q&A (question, answer, explanation).
**Acceptance Criteria:**
- Built-in templates: Basic, Basic (with hint), Cloze, Vocabulary, Q&A
- Custom templates definable in config
- Template specifies fields and review layout
- Fields support Markdown rendering
- Template selectable when creating cards

### Task 20: Add Flashcard Tests
**Files:** `tests/unit/test_flashcard_system.cpp`
**Description:** Test FSRS system: scheduling, session management, Markdown extraction, serialization, and review lifecycle.
**Acceptance Criteria:**
- FSRS scheduling produces correct intervals
- Session respects new/review card limits
- Markdown extraction: basic and cloze cards
- Leech detection after threshold lapses
- Serialization round-trip preserves all state

## Testing Requirements
- FSRS algorithm: interval computation for all ratings and states
- Session management: card ordering, limits, completion
- Markdown extraction: parse all card syntaxes
- Serialization: save/load preserves review history

## Phase Completion Criteria
- Flashcard decks creatable and manageable
- Review sessions with FSRS scheduling
- Markdown flashcard extraction (basic and cloze)
- Deck statistics and study streak tracking
- Card editor with Markdown support
- All tests pass
