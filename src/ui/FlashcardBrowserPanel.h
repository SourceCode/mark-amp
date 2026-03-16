#pragma once

#include "../core/FlashcardTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Sorting options for the card browser.
enum class CardBrowserSort
{
    DueDate,
    CreatedDate,
    Difficulty,
    Stability,
    State,
    Reps,
    Lapses
};

/// Filter options for the card browser.
struct CardBrowserFilter
{
    std::string search_query;
    core::CardState state_filter{core::CardState::New};
    bool filter_by_state{false};
    bool show_due_only{false};
};

/// Improvement 31: Render data for a single deck entry.
struct DeckRenderItem
{
    std::string deck_id;
    std::string name;
    int card_count{0};
    bool is_selected{false};
};

/// Improvement 32: Render data for a single card entry.
struct CardRenderItem
{
    std::string card_id;
    std::string front_preview;
    bool is_selected{false};
};

/// UI panel for browsing and managing all flashcards across decks.
/// Shows card list, deck selector, bulk operations, and statistics.
class FlashcardBrowserPanel
{
public:
    using OnCardSelectedCallback = std::function<void(const std::string& card_id)>;
    using OnDeckSelectedCallback = std::function<void(const std::string& deck_id)>;
    using OnActionCallback = std::function<void()>;

    FlashcardBrowserPanel() = default;

    // ── Panel lifecycle ──

    void initialize();
    void layout(float x, float y, float width, float height);
    void render();

    // ── Data display ──

    void set_decks(const std::vector<core::FlashcardDeck>& decks);
    void set_cards(const std::vector<core::FlashcardCard>& cards);
    void set_deck_stats(const core::DeckStats& stats);
    void set_selected_deck(const std::string& deck_id);

    // ── Sorting & filtering ──

    void set_sort(CardBrowserSort sort);
    void set_filter(const CardBrowserFilter& filter);

    // ── Selection ──

    [[nodiscard]] auto selected_card_ids() const -> std::vector<std::string>;
    void select_all();
    void deselect_all();

    // ── Callbacks ──

    void set_on_card_selected(OnCardSelectedCallback callback);
    void set_on_deck_selected(OnDeckSelectedCallback callback);
    void set_on_review_deck(OnActionCallback callback);
    void set_on_delete_cards(OnActionCallback callback);
    void set_on_reset_cards(OnActionCallback callback);
    void set_on_create_deck(OnActionCallback callback);
    void set_on_delete_deck(OnActionCallback callback);

private:
    float x_{0}, y_{0}, width_{0}, height_{0};

    std::vector<core::FlashcardDeck> decks_;
    std::vector<core::FlashcardCard> cards_;
    core::DeckStats current_stats_;
    std::string selected_deck_id_;

    CardBrowserSort sort_{CardBrowserSort::DueDate};
    CardBrowserFilter filter_;
    std::vector<std::string> selected_card_ids_;

    // Improvement 31-32: Render data
    std::vector<DeckRenderItem> deck_render_items_;
    std::vector<CardRenderItem> card_render_items_;

    OnCardSelectedCallback on_card_selected_;
    OnDeckSelectedCallback on_deck_selected_;
    OnActionCallback on_review_deck_;
    OnActionCallback on_delete_cards_;
    OnActionCallback on_reset_cards_;
    OnActionCallback on_create_deck_;
    OnActionCallback on_delete_deck_;
};

} // namespace markamp::ui
