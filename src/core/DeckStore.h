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

    IDeckStore(const IDeckStore&) = delete;
    auto operator=(const IDeckStore&) -> IDeckStore& = delete;
    IDeckStore(IDeckStore&&) = default;
    auto operator=(IDeckStore&&) -> IDeckStore& = default;

    [[nodiscard]] virtual auto load_deck(const std::string& deck_id)
        -> std::expected<FlashcardDeck, std::string> = 0;

    [[nodiscard]] virtual auto save_deck(const FlashcardDeck& deck)
        -> std::expected<void, std::string> = 0;

    [[nodiscard]] virtual auto delete_deck(const std::string& deck_id)
        -> std::expected<void, std::string> = 0;

    [[nodiscard]] virtual auto list_deck_ids()
        -> std::expected<std::vector<std::string>, std::string> = 0;

    [[nodiscard]] virtual auto storage_path() const -> std::filesystem::path = 0;

protected:
    IDeckStore() = default;
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

    [[nodiscard]] static auto escape_json(const std::string& str) -> std::string;
    [[nodiscard]] static auto time_to_iso(std::chrono::system_clock::time_point tp) -> std::string;
};

} // namespace markamp::core
