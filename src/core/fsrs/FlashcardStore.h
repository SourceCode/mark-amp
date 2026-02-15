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
                                   const std::string& back) -> std::expected<Card, std::string>;

    [[nodiscard]] auto get_card(const std::string& card_id) const
        -> std::expected<Card, std::string>;

    [[nodiscard]] auto update_card(const Card& card) -> std::expected<void, std::string>;

    [[nodiscard]] auto delete_card(const std::string& card_id) -> std::expected<void, std::string>;

    // ── Deck CRUD ──

    [[nodiscard]] auto create_deck(const std::string& name, const std::string& description = "")
        -> std::expected<Deck, std::string>;

    [[nodiscard]] auto get_deck(const std::string& deck_id) const
        -> std::expected<Deck, std::string>;

    [[nodiscard]] auto update_deck(const Deck& deck) -> std::expected<void, std::string>;

    [[nodiscard]] auto delete_deck(const std::string& deck_id) -> std::expected<void, std::string>;

    [[nodiscard]] auto get_all_decks() const -> std::vector<Deck>;

    // ── Queries ──

    [[nodiscard]] auto get_cards_in_deck(const std::string& deck_id) const -> std::vector<Card>;

    [[nodiscard]] auto get_due_cards(const std::string& deck_id, TimePoint now) const
        -> std::vector<Card>;

    [[nodiscard]] auto get_due_counts(const std::string& deck_id, TimePoint now) const
        -> DueCardCounts;

    [[nodiscard]] auto get_new_cards(const std::string& deck_id) const -> std::vector<Card>;

    // ── Review Log ──

    [[nodiscard]] auto add_review_log(const ReviewLog& log) -> std::expected<void, std::string>;

    [[nodiscard]] auto get_review_logs(const std::string& card_id) const -> std::vector<ReviewLog>;

    [[nodiscard]] auto get_review_logs_in_range(TimePoint start, TimePoint end) const
        -> std::vector<ReviewLog>;

    // ── Persistence ──

    [[nodiscard]] auto save() -> std::expected<void, std::string>;
    [[nodiscard]] auto load() -> std::expected<void, std::string>;

private:
    std::filesystem::path storage_root_;
    mutable std::mutex mutex_;

    std::unordered_map<std::string, Deck> decks_;
    std::unordered_map<std::string, Card> cards_;
    std::vector<ReviewLog> review_logs_;

    [[nodiscard]] static auto generate_uuid() -> std::string;
    [[nodiscard]] static auto escape_json(const std::string& str) -> std::string;
    [[nodiscard]] static auto time_to_iso(TimePoint tp) -> std::string;
    [[nodiscard]] static auto iso_to_time(const std::string& iso) -> TimePoint;

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
