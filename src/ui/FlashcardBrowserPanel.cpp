#include "FlashcardBrowserPanel.h"

#include <algorithm>

namespace markamp::ui
{

void FlashcardBrowserPanel::initialize()
{
    decks_.clear();
    cards_.clear();
    selected_card_ids_.clear();
}

void FlashcardBrowserPanel::layout(float x, float y, float width, float height)
{
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
}

void FlashcardBrowserPanel::render()
{
    // Stub: actual UI rendering deferred to UI integration phase
}

void FlashcardBrowserPanel::set_decks(const std::vector<core::FlashcardDeck>& decks)
{
    decks_ = decks;
}

void FlashcardBrowserPanel::set_cards(const std::vector<core::FlashcardCard>& cards)
{
    cards_ = cards;
}

void FlashcardBrowserPanel::set_deck_stats(const core::DeckStats& stats)
{
    current_stats_ = stats;
}

void FlashcardBrowserPanel::set_selected_deck(const std::string& deck_id)
{
    selected_deck_id_ = deck_id;
}

void FlashcardBrowserPanel::set_sort(CardBrowserSort sort)
{
    sort_ = sort;
}

void FlashcardBrowserPanel::set_filter(const CardBrowserFilter& filter)
{
    filter_ = filter;
}

auto FlashcardBrowserPanel::selected_card_ids() const -> std::vector<std::string>
{
    return selected_card_ids_;
}

void FlashcardBrowserPanel::select_all()
{
    selected_card_ids_.clear();
    for (const auto& card : cards_)
    {
        selected_card_ids_.push_back(card.id);
    }
}

void FlashcardBrowserPanel::deselect_all()
{
    selected_card_ids_.clear();
}

void FlashcardBrowserPanel::set_on_card_selected(OnCardSelectedCallback callback)
{
    on_card_selected_ = std::move(callback);
}

void FlashcardBrowserPanel::set_on_deck_selected(OnDeckSelectedCallback callback)
{
    on_deck_selected_ = std::move(callback);
}

void FlashcardBrowserPanel::set_on_review_deck(OnActionCallback callback)
{
    on_review_deck_ = std::move(callback);
}

void FlashcardBrowserPanel::set_on_delete_cards(OnActionCallback callback)
{
    on_delete_cards_ = std::move(callback);
}

void FlashcardBrowserPanel::set_on_reset_cards(OnActionCallback callback)
{
    on_reset_cards_ = std::move(callback);
}

void FlashcardBrowserPanel::set_on_create_deck(OnActionCallback callback)
{
    on_create_deck_ = std::move(callback);
}

void FlashcardBrowserPanel::set_on_delete_deck(OnActionCallback callback)
{
    on_delete_deck_ = std::move(callback);
}

} // namespace markamp::ui
