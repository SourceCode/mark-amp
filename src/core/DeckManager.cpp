#include "DeckManager.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>

namespace markamp::core
{

DeckManager::DeckManager(EventBus& event_bus, Config& config, std::unique_ptr<IDeckStore> store)
    : event_bus_(event_bus)
    , config_(config)
    , store_(std::move(store))
{
}

// ============================================================================
// Initialization
// ============================================================================

auto DeckManager::initialize() -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    const auto deck_ids_result = store_->list_deck_ids();
    if (!deck_ids_result)
    {
        return std::unexpected("Failed to enumerate decks: " + deck_ids_result.error());
    }

    for (const auto& deck_id : deck_ids_result.value())
    {
        auto deck_result = store_->load_deck(deck_id);
        if (deck_result)
        {
            decks_[deck_id] = std::move(deck_result.value());
        }
    }

    ensure_builtin_deck();
    return {};
}

auto DeckManager::save_all() -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    for (const auto& [deck_id, deck] : decks_)
    {
        if (const auto result = store_->save_deck(deck); !result)
        {
            return std::unexpected("Failed to save deck " + deck_id + ": " + result.error());
        }
    }
    return {};
}

// ============================================================================
// Deck Operations
// ============================================================================

auto DeckManager::create_deck(const std::string& name) -> std::expected<std::string, std::string>
{
    std::lock_guard lock(deck_mutex_);

    if (name.empty())
    {
        return std::unexpected("Deck name cannot be empty");
    }

    FlashcardDeck deck;
    deck.id = generate_id();
    deck.name = name;
    deck.created = std::chrono::system_clock::now();
    deck.updated = deck.created;

    const auto assigned_id = deck.id;

    if (const auto result = store_->save_deck(deck); !result)
    {
        return std::unexpected("Failed to persist deck: " + result.error());
    }

    decks_[assigned_id] = std::move(deck);
    return assigned_id;
}

auto DeckManager::remove_deck(const std::string& deck_id) -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    if (deck_id == kBuiltinDeckID)
    {
        return std::unexpected("Cannot delete the built-in deck");
    }

    if (decks_.find(deck_id) == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }

    if (const auto result = store_->delete_deck(deck_id); !result)
    {
        return std::unexpected("Failed to delete deck: " + result.error());
    }

    decks_.erase(deck_id);
    return {};
}

auto DeckManager::rename_deck(const std::string& deck_id, const std::string& new_name)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    if (new_name.empty())
    {
        return std::unexpected("Deck name cannot be empty");
    }

    auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }

    iter->second.name = new_name;
    iter->second.updated = std::chrono::system_clock::now();

    if (const auto result = store_->save_deck(iter->second); !result)
    {
        return std::unexpected("Failed to persist rename: " + result.error());
    }
    return {};
}

auto DeckManager::get_deck(const std::string& deck_id) const
    -> std::expected<FlashcardDeck, std::string>
{
    std::lock_guard lock(deck_mutex_);

    const auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }
    return iter->second;
}

auto DeckManager::list_decks() const -> std::vector<FlashcardDeck>
{
    std::lock_guard lock(deck_mutex_);

    std::vector<FlashcardDeck> result;
    result.reserve(decks_.size());
    for (const auto& [deck_id, deck] : decks_)
    {
        result.push_back(deck);
    }
    return result;
}

auto DeckManager::get_deck_stats(const std::string& deck_id) const
    -> std::expected<DeckStats, std::string>
{
    std::lock_guard lock(deck_mutex_);

    const auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }
    return iter->second.compute_stats();
}

// ============================================================================
// Card Operations
// ============================================================================

auto DeckManager::add_cards(const std::string& deck_id, const std::vector<std::string>& block_ids)
    -> std::expected<std::vector<std::string>, std::string>
{
    std::lock_guard lock(deck_mutex_);

    auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }

    auto& deck = iter->second;
    std::vector<std::string> new_card_ids;
    const auto now = std::chrono::system_clock::now();

    for (const auto& block_id : block_ids)
    {
        // Skip duplicates
        if (deck.has_block(block_id))
        {
            continue;
        }

        FlashcardCard card;
        card.id = generate_id();
        card.block_id = block_id;
        card.deck_id = deck_id;
        card.state = CardState::New;
        card.due = now;
        card.created = now;
        card.updated = now;

        new_card_ids.push_back(card.id);
        deck.cards.push_back(std::move(card));
    }

    deck.updated = now;

    if (const auto result = store_->save_deck(deck); !result)
    {
        return std::unexpected("Failed to persist cards: " + result.error());
    }
    return new_card_ids;
}

auto DeckManager::remove_cards(const std::string& deck_id,
                               const std::vector<std::string>& block_ids)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }

    auto& deck = iter->second;
    std::erase_if(deck.cards,
                  [&](const FlashcardCard& card)
                  {
                      for (const auto& block_id : block_ids)
                      {
                          if (card.block_id == block_id)
                              return true;
                      }
                      return false;
                  });

    deck.updated = std::chrono::system_clock::now();

    if (const auto result = store_->save_deck(deck); !result)
    {
        return std::unexpected("Failed to persist card removal: " + result.error());
    }
    return {};
}

auto DeckManager::get_due_cards(const std::string& deck_id, int limit) const
    -> std::vector<FlashcardCard>
{
    std::lock_guard lock(deck_mutex_);

    const auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
        return {};

    const auto now = std::chrono::system_clock::now();
    std::vector<FlashcardCard> due;

    for (const auto& card : iter->second.cards)
    {
        if (card.state != CardState::New && card.due <= now)
        {
            due.push_back(card);
        }
    }

    // Sort by due date ascending (most overdue first)
    std::sort(due.begin(),
              due.end(),
              [](const FlashcardCard& lhs, const FlashcardCard& rhs) { return lhs.due < rhs.due; });

    if (static_cast<int>(due.size()) > limit)
    {
        due.resize(static_cast<size_t>(limit));
    }
    return due;
}

auto DeckManager::get_new_cards(const std::string& deck_id, int limit) const
    -> std::vector<FlashcardCard>
{
    std::lock_guard lock(deck_mutex_);

    const auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
        return {};

    std::vector<FlashcardCard> new_cards;
    for (const auto& card : iter->second.cards)
    {
        if (card.state == CardState::New)
        {
            new_cards.push_back(card);
        }
    }

    // Sort by creation date ascending
    std::sort(new_cards.begin(),
              new_cards.end(),
              [](const FlashcardCard& lhs, const FlashcardCard& rhs)
              { return lhs.created < rhs.created; });

    if (static_cast<int>(new_cards.size()) > limit)
    {
        new_cards.resize(static_cast<size_t>(limit));
    }
    return new_cards;
}

auto DeckManager::get_all_cards(const std::string& deck_id, int page, int page_size) const
    -> std::expected<std::vector<FlashcardCard>, std::string>
{
    std::lock_guard lock(deck_mutex_);

    const auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }

    const auto& cards = iter->second.cards;
    const auto start = static_cast<size_t>(page * page_size);
    if (start >= cards.size())
    {
        return std::vector<FlashcardCard>{};
    }

    const auto end = std::min(start + static_cast<size_t>(page_size), cards.size());
    return std::vector<FlashcardCard>(cards.begin() + static_cast<ptrdiff_t>(start),
                                      cards.begin() + static_cast<ptrdiff_t>(end));
}

auto DeckManager::reset_cards(const std::vector<std::string>& card_ids)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    const auto now = std::chrono::system_clock::now();

    for (const auto& card_id : card_ids)
    {
        auto* deck = find_deck_for_card(card_id);
        if (deck == nullptr)
            continue;

        auto* card = deck->find_card_mut(card_id);
        if (card == nullptr)
            continue;

        card->state = CardState::New;
        card->stability = 0.0;
        card->difficulty = 0.0;
        card->elapsed_days = 0;
        card->scheduled_days = 0;
        card->reps = 0;
        card->lapses = 0;
        card->due = now;
        card->updated = now;
    }

    return {};
}

auto DeckManager::set_card_due_time(const std::string& card_id,
                                    std::chrono::system_clock::time_point due)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    auto* deck = find_deck_for_card(card_id);
    if (deck == nullptr)
    {
        return std::unexpected("Card not found: " + card_id);
    }

    auto* card = deck->find_card_mut(card_id);
    if (card == nullptr)
    {
        return std::unexpected("Card not found: " + card_id);
    }

    card->due = due;
    card->updated = std::chrono::system_clock::now();
    return {};
}

auto DeckManager::get_cards_by_block_ids(const std::vector<std::string>& block_ids) const
    -> std::vector<FlashcardCard>
{
    std::lock_guard lock(deck_mutex_);

    std::vector<FlashcardCard> result;
    for (const auto& [deck_id, deck] : decks_)
    {
        for (const auto& card : deck.cards)
        {
            for (const auto& block_id : block_ids)
            {
                if (card.block_id == block_id)
                {
                    result.push_back(card);
                    break;
                }
            }
        }
    }
    return result;
}

auto DeckManager::update_card_after_review(const std::string& card_id,
                                           const FlashcardCard& updated_card)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(deck_mutex_);

    auto* deck = find_deck_for_card(card_id);
    if (deck == nullptr)
    {
        return std::unexpected("Card not found: " + card_id);
    }

    auto* card = deck->find_card_mut(card_id);
    if (card == nullptr)
    {
        return std::unexpected("Card not found: " + card_id);
    }

    // Update scheduling fields
    card->state = updated_card.state;
    card->stability = updated_card.stability;
    card->difficulty = updated_card.difficulty;
    card->elapsed_days = updated_card.elapsed_days;
    card->scheduled_days = updated_card.scheduled_days;
    card->reps = updated_card.reps;
    card->lapses = updated_card.lapses;
    card->due = updated_card.due;
    card->last_review = updated_card.last_review;
    card->updated = std::chrono::system_clock::now();

    if (const auto result = store_->save_deck(*deck); !result)
    {
        return std::unexpected("Failed to persist review: " + result.error());
    }
    return {};
}

// ============================================================================
// Configuration
// ============================================================================

auto DeckManager::flashcard_config() const -> FlashcardConfig
{
    // Default config; can be enhanced to read from Config later
    return FlashcardConfig{};
}

// ============================================================================
// Private Helpers
// ============================================================================

auto DeckManager::generate_id() -> std::string
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, 15);
    static const char hex_chars[] = "0123456789abcdef";

    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    for (auto& ch : uuid)
    {
        if (ch == 'x')
        {
            ch = hex_chars[dist(gen)];
        }
        else if (ch == 'y')
        {
            ch = hex_chars[(dist(gen) & 0x3) | 0x8];
        }
    }
    return uuid;
}

void DeckManager::ensure_builtin_deck()
{
    const auto builtin_id = std::string(kBuiltinDeckID);
    if (decks_.find(builtin_id) != decks_.end())
    {
        return;
    }

    FlashcardDeck builtin;
    builtin.id = builtin_id;
    builtin.name = "Default";
    builtin.created = std::chrono::system_clock::now();
    builtin.updated = builtin.created;

    // Persist silently (ignore errors on first creation)
    static_cast<void>(store_->save_deck(builtin));
    decks_[builtin_id] = std::move(builtin);
}

auto DeckManager::find_deck_for_card(const std::string& card_id) -> FlashcardDeck*
{
    for (auto& [deck_id, deck] : decks_)
    {
        if (deck.find_card(card_id) != nullptr)
        {
            return &deck;
        }
    }
    return nullptr;
}

auto DeckManager::find_deck_for_card(const std::string& card_id) const -> const FlashcardDeck*
{
    for (const auto& [deck_id, deck] : decks_)
    {
        if (deck.find_card(card_id) != nullptr)
        {
            return &deck;
        }
    }
    return nullptr;
}

} // namespace markamp::core
