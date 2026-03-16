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
    // Improvement 31-33: Prepare render data for the host wxPanel.
    // The host panel reads deck_render_items_ and card_render_items_ to populate UI.

    deck_render_items_.clear();
    for (const auto& deck : decks_)
    {
        DeckRenderItem item;
        item.deck_id = deck.id;
        item.name = deck.name;
        item.card_count = static_cast<int>(deck.cards.size());
        item.is_selected = (deck.id == selected_deck_id_);
        deck_render_items_.push_back(std::move(item));
    }

    // Build card grid entries — apply filter and sort
    card_render_items_.clear();
    for (const auto& card : cards_)
    {
        // Improvement 33: Apply filter
        if (!filter_.search_query.empty() &&
            card.block_id.find(filter_.search_query) == std::string::npos)
        {
            continue;
        }

        CardRenderItem entry;
        entry.card_id = card.id;
        entry.front_preview = card.block_id.substr(0, 80);
        entry.is_selected =
            std::find(selected_card_ids_.begin(), selected_card_ids_.end(), card.id) !=
            selected_card_ids_.end();
        card_render_items_.push_back(std::move(entry));
    }

    // Improvement 33: Apply sort
    if (sort_ == CardBrowserSort::CreatedDate)
    {
        // Already in creation order from the data layer
    }
    else if (sort_ == CardBrowserSort::Difficulty)
    {
        std::sort(card_render_items_.begin(),
                  card_render_items_.end(),
                  [](const CardRenderItem& lhs, const CardRenderItem& rhs)
                  { return lhs.front_preview < rhs.front_preview; });
    }
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
