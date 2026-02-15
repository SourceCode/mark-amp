#include "DeckStore.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

JsonDeckStore::JsonDeckStore(std::filesystem::path base_path)
    : base_path_(std::move(base_path))
{
}

auto JsonDeckStore::deck_dir(const std::string& deck_id) const -> std::filesystem::path
{
    return base_path_ / deck_id;
}

auto JsonDeckStore::deck_meta_path(const std::string& deck_id) const -> std::filesystem::path
{
    return deck_dir(deck_id) / "deck.json";
}

auto JsonDeckStore::deck_cards_path(const std::string& deck_id) const -> std::filesystem::path
{
    return deck_dir(deck_id) / "cards.json";
}

auto JsonDeckStore::escape_json(const std::string& str) -> std::string
{
    std::string result;
    result.reserve(str.size() + 16);
    for (const auto ch : str)
    {
        switch (ch)
        {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += ch;
                break;
        }
    }
    return result;
}

auto JsonDeckStore::time_to_iso(std::chrono::system_clock::time_point tp) -> std::string
{
    const auto time_t_val = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_t_val), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// ============================================================================
// Load
// ============================================================================

auto JsonDeckStore::load_deck(const std::string& deck_id)
    -> std::expected<FlashcardDeck, std::string>
{
    const auto meta_path = deck_meta_path(deck_id);
    if (!std::filesystem::exists(meta_path))
    {
        return std::unexpected("Deck not found: " + deck_id);
    }

    FlashcardDeck deck;
    deck.id = deck_id;

    // Read metadata
    {
        std::ifstream ifs(meta_path);
        if (!ifs.is_open())
        {
            return std::unexpected("Failed to open deck metadata: " + meta_path.string());
        }
        // Minimal parsing — extract name from JSON
        const std::string content((std::istreambuf_iterator<char>(ifs)),
                                  std::istreambuf_iterator<char>());
        // Find "name":"..."
        const auto name_pos = content.find("\"name\":\"");
        if (name_pos != std::string::npos)
        {
            const auto start = name_pos + 8;
            const auto end = content.find('"', start);
            if (end != std::string::npos)
            {
                deck.name = content.substr(start, end - start);
            }
        }
    }

    // Cards loaded empty for now (full JSON parser deferred)
    return deck;
}

// ============================================================================
// Save (atomic: write tmp, then rename)
// ============================================================================

auto JsonDeckStore::save_deck(const FlashcardDeck& deck) -> std::expected<void, std::string>
{
    std::error_code err_code;
    const auto dir = deck_dir(deck.id);
    std::filesystem::create_directories(dir, err_code);
    if (err_code)
    {
        return std::unexpected("Failed to create deck directory: " + err_code.message());
    }

    // Write metadata
    {
        const auto tmp_path = deck_meta_path(deck.id).string() + ".tmp";
        std::ofstream ofs(tmp_path);
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to write deck metadata");
        }
        ofs << "{\"id\":\"" << escape_json(deck.id) << "\",\"name\":\"" << escape_json(deck.name)
            << "\",\"created\":\"" << time_to_iso(deck.created) << "\",\"updated\":\""
            << time_to_iso(deck.updated) << "\"}";
        ofs.close();
        std::filesystem::rename(tmp_path, deck_meta_path(deck.id), err_code);
        if (err_code)
        {
            return std::unexpected("Failed to rename deck metadata: " + err_code.message());
        }
    }

    // Write cards
    {
        const auto tmp_path = deck_cards_path(deck.id).string() + ".tmp";
        std::ofstream ofs(tmp_path);
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to write deck cards");
        }
        ofs << "[";
        for (size_t idx = 0; idx < deck.cards.size(); ++idx)
        {
            if (idx > 0)
                ofs << ",";
            const auto& card = deck.cards[idx];
            ofs << "{\"id\":\"" << escape_json(card.id) << "\",\"block_id\":\""
                << escape_json(card.block_id) << "\",\"deck_id\":\"" << escape_json(card.deck_id)
                << "\",\"state\":" << static_cast<int>(card.state)
                << ",\"stability\":" << card.stability << ",\"difficulty\":" << card.difficulty
                << ",\"elapsed_days\":" << card.elapsed_days
                << ",\"scheduled_days\":" << card.scheduled_days << ",\"reps\":" << card.reps
                << ",\"lapses\":" << card.lapses << ",\"due\":\"" << time_to_iso(card.due)
                << "\",\"last_review\":\"" << time_to_iso(card.last_review) << "\",\"created\":\""
                << time_to_iso(card.created) << "\",\"updated\":\"" << time_to_iso(card.updated)
                << "\"}";
        }
        ofs << "]";
        ofs.close();
        std::filesystem::rename(tmp_path, deck_cards_path(deck.id), err_code);
        if (err_code)
        {
            return std::unexpected("Failed to rename deck cards: " + err_code.message());
        }
    }

    return {};
}

// ============================================================================
// Delete
// ============================================================================

auto JsonDeckStore::delete_deck(const std::string& deck_id) -> std::expected<void, std::string>
{
    std::error_code err_code;
    const auto dir = deck_dir(deck_id);
    if (!std::filesystem::exists(dir))
    {
        return std::unexpected("Deck directory not found: " + deck_id);
    }
    std::filesystem::remove_all(dir, err_code);
    if (err_code)
    {
        return std::unexpected("Failed to delete deck: " + err_code.message());
    }
    return {};
}

// ============================================================================
// List
// ============================================================================

auto JsonDeckStore::list_deck_ids() -> std::expected<std::vector<std::string>, std::string>
{
    std::vector<std::string> ids;
    std::error_code err_code;

    if (!std::filesystem::exists(base_path_))
    {
        return ids;
    }

    for (const auto& entry : std::filesystem::directory_iterator(base_path_, err_code))
    {
        if (entry.is_directory())
        {
            ids.push_back(entry.path().filename().string());
        }
    }
    if (err_code)
    {
        return std::unexpected("Failed to list decks: " + err_code.message());
    }
    return ids;
}

} // namespace markamp::core
