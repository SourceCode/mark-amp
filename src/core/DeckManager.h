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

    [[nodiscard]] auto remove_deck(const std::string& deck_id) -> std::expected<void, std::string>;

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
    [[maybe_unused]] EventBus& event_bus_;
    [[maybe_unused]] Config& config_;
    std::unique_ptr<IDeckStore> store_;

    mutable std::mutex deck_mutex_;
    std::unordered_map<std::string, FlashcardDeck> decks_;

    /// Generate a new unique ID for a deck or card.
    [[nodiscard]] static auto generate_id() -> std::string;

    /// Ensure the built-in deck exists.
    void ensure_builtin_deck();

    /// Find the deck containing a specific card_id. Returns nullptr if not found.
    [[nodiscard]] auto find_deck_for_card(const std::string& card_id) -> FlashcardDeck*;
    [[nodiscard]] auto find_deck_for_card(const std::string& card_id) const -> const FlashcardDeck*;
};

} // namespace markamp::core
