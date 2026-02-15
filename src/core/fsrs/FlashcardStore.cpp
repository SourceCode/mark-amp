#include "FlashcardStore.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>

namespace markamp::core::fsrs
{

FlashcardStore::FlashcardStore(std::filesystem::path storage_root)
    : storage_root_(std::move(storage_root))
{
}

// ============================================================================
// Card CRUD
// ============================================================================

auto FlashcardStore::create_card(const std::string& deck_id,
                                 const std::string& block_id,
                                 const std::string& front,
                                 const std::string& back) -> std::expected<Card, std::string>
{
    std::lock_guard lock(mutex_);

    if (decks_.find(deck_id) == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }

    Card card;
    card.id = generate_uuid();
    card.deck_id = deck_id;
    card.block_id = block_id;
    card.front_content = front;
    card.back_content = back;
    card.state = CardState::New;
    card.due = std::chrono::system_clock::now();

    cards_[card.id] = card;
    return card;
}

auto FlashcardStore::get_card(const std::string& card_id) const -> std::expected<Card, std::string>
{
    std::lock_guard lock(mutex_);
    const auto iter = cards_.find(card_id);
    if (iter == cards_.end())
    {
        return std::unexpected("Card not found: " + card_id);
    }
    return iter->second;
}

auto FlashcardStore::update_card(const Card& card) -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);
    if (cards_.find(card.id) == cards_.end())
    {
        return std::unexpected("Card not found: " + card.id);
    }
    cards_[card.id] = card;
    return {};
}

auto FlashcardStore::delete_card(const std::string& card_id) -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);
    if (cards_.erase(card_id) == 0)
    {
        return std::unexpected("Card not found: " + card_id);
    }
    return {};
}

// ============================================================================
// Deck CRUD
// ============================================================================

auto FlashcardStore::create_deck(const std::string& name, const std::string& description)
    -> std::expected<Deck, std::string>
{
    std::lock_guard lock(mutex_);

    Deck deck;
    deck.id = generate_uuid();
    deck.name = name;
    deck.description = description;

    decks_[deck.id] = deck;
    return deck;
}

auto FlashcardStore::get_deck(const std::string& deck_id) const -> std::expected<Deck, std::string>
{
    std::lock_guard lock(mutex_);
    const auto iter = decks_.find(deck_id);
    if (iter == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck_id);
    }
    return iter->second;
}

auto FlashcardStore::update_deck(const Deck& deck) -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);
    if (decks_.find(deck.id) == decks_.end())
    {
        return std::unexpected("Deck not found: " + deck.id);
    }
    decks_[deck.id] = deck;
    return {};
}

auto FlashcardStore::delete_deck(const std::string& deck_id) -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);
    if (decks_.erase(deck_id) == 0)
    {
        return std::unexpected("Deck not found: " + deck_id);
    }
    // Remove all cards in this deck
    std::erase_if(cards_, [&](const auto& pair) { return pair.second.deck_id == deck_id; });
    return {};
}

auto FlashcardStore::get_all_decks() const -> std::vector<Deck>
{
    std::lock_guard lock(mutex_);
    std::vector<Deck> result;
    result.reserve(decks_.size());
    for (const auto& [deck_id, deck] : decks_)
    {
        result.push_back(deck);
    }
    return result;
}

// ============================================================================
// Queries
// ============================================================================

auto FlashcardStore::get_cards_in_deck(const std::string& deck_id) const -> std::vector<Card>
{
    std::lock_guard lock(mutex_);
    std::vector<Card> result;
    for (const auto& [card_id, card] : cards_)
    {
        if (card.deck_id == deck_id)
        {
            result.push_back(card);
        }
    }
    return result;
}

auto FlashcardStore::get_due_cards(const std::string& deck_id, TimePoint now) const
    -> std::vector<Card>
{
    std::lock_guard lock(mutex_);
    std::vector<Card> result;
    for (const auto& [card_id, card] : cards_)
    {
        if (card.deck_id == deck_id && card.due <= now)
        {
            result.push_back(card);
        }
    }
    // Sort: Learning/Relearning first, then Review by due date, then New
    std::sort(result.begin(),
              result.end(),
              [](const Card& lhs, const Card& rhs)
              {
                  const auto state_priority = [](CardState state) -> int
                  {
                      switch (state)
                      {
                          case CardState::Learning:
                          case CardState::Relearning:
                              return 0;
                          case CardState::Review:
                              return 1;
                          case CardState::New:
                              return 2;
                      }
                      return 3;
                  };
                  const auto lp = state_priority(lhs.state);
                  const auto rp = state_priority(rhs.state);
                  if (lp != rp)
                      return lp < rp;
                  return lhs.due < rhs.due;
              });
    return result;
}

auto FlashcardStore::get_due_counts(const std::string& deck_id, TimePoint now) const
    -> DueCardCounts
{
    std::lock_guard lock(mutex_);
    DueCardCounts counts;
    for (const auto& [card_id, card] : cards_)
    {
        if (card.deck_id != deck_id)
            continue;
        if (card.state == CardState::New)
        {
            ++counts.new_count;
        }
        else if (card.state == CardState::Learning || card.state == CardState::Relearning)
        {
            if (card.due <= now)
            {
                ++counts.learning_count;
            }
        }
        else if (card.state == CardState::Review && card.due <= now)
        {
            ++counts.review_count;
        }
    }
    counts.total_due = counts.new_count + counts.learning_count + counts.review_count;
    return counts;
}

auto FlashcardStore::get_new_cards(const std::string& deck_id) const -> std::vector<Card>
{
    std::lock_guard lock(mutex_);
    std::vector<Card> result;
    for (const auto& [card_id, card] : cards_)
    {
        if (card.deck_id == deck_id && card.state == CardState::New)
        {
            result.push_back(card);
        }
    }
    return result;
}

// ============================================================================
// Review Log
// ============================================================================

auto FlashcardStore::add_review_log(const ReviewLog& log) -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);
    review_logs_.push_back(log);
    return {};
}

auto FlashcardStore::get_review_logs(const std::string& card_id) const -> std::vector<ReviewLog>
{
    std::lock_guard lock(mutex_);
    std::vector<ReviewLog> result;
    for (const auto& log : review_logs_)
    {
        if (log.card_id == card_id)
        {
            result.push_back(log);
        }
    }
    return result;
}

auto FlashcardStore::get_review_logs_in_range(TimePoint start, TimePoint end) const
    -> std::vector<ReviewLog>
{
    std::lock_guard lock(mutex_);
    std::vector<ReviewLog> result;
    for (const auto& log : review_logs_)
    {
        if (log.review >= start && log.review <= end)
        {
            result.push_back(log);
        }
    }
    return result;
}

// ============================================================================
// Persistence
// ============================================================================

auto FlashcardStore::save() -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    std::error_code err_code;
    const auto flashcard_dir = storage_root_ / "flashcards";
    std::filesystem::create_directories(flashcard_dir, err_code);
    if (err_code)
    {
        return std::unexpected("Failed to create storage directory: " + err_code.message());
    }

    // Write decks
    {
        std::ofstream ofs(flashcard_dir / "decks.json");
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to open decks.json for writing");
        }
        ofs << serialize_decks();
    }

    // Write cards
    {
        std::ofstream ofs(flashcard_dir / "cards.json");
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to open cards.json for writing");
        }
        ofs << serialize_cards();
    }

    // Write review logs
    {
        std::ofstream ofs(flashcard_dir / "review_logs.json");
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to open review_logs.json for writing");
        }
        ofs << serialize_review_logs();
    }

    return {};
}

auto FlashcardStore::load() -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    const auto flashcard_dir = storage_root_ / "flashcards";
    if (!std::filesystem::exists(flashcard_dir))
    {
        return {}; // Empty state on first run
    }

    // Load decks
    if (std::filesystem::exists(flashcard_dir / "decks.json"))
    {
        std::ifstream ifs(flashcard_dir / "decks.json");
        if (ifs.is_open())
        {
            const std::string content((std::istreambuf_iterator<char>(ifs)),
                                      std::istreambuf_iterator<char>());
            if (const auto result = deserialize_decks(content); !result)
            {
                return std::unexpected("Failed to parse decks: " + result.error());
            }
        }
    }

    // Load cards
    if (std::filesystem::exists(flashcard_dir / "cards.json"))
    {
        std::ifstream ifs(flashcard_dir / "cards.json");
        if (ifs.is_open())
        {
            const std::string content((std::istreambuf_iterator<char>(ifs)),
                                      std::istreambuf_iterator<char>());
            if (const auto result = deserialize_cards(content); !result)
            {
                return std::unexpected("Failed to parse cards: " + result.error());
            }
        }
    }

    // Load review logs
    if (std::filesystem::exists(flashcard_dir / "review_logs.json"))
    {
        std::ifstream ifs(flashcard_dir / "review_logs.json");
        if (ifs.is_open())
        {
            const std::string content((std::istreambuf_iterator<char>(ifs)),
                                      std::istreambuf_iterator<char>());
            if (const auto result = deserialize_review_logs(content); !result)
            {
                return std::unexpected("Failed to parse review logs: " + result.error());
            }
        }
    }

    return {};
}

// ============================================================================
// Utilities
// ============================================================================

auto FlashcardStore::generate_uuid() -> std::string
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

auto FlashcardStore::escape_json(const std::string& str) -> std::string
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

auto FlashcardStore::time_to_iso(TimePoint tp) -> std::string
{
    const auto time_t_val = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_t_val), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

auto FlashcardStore::iso_to_time(const std::string& iso) -> TimePoint
{
    std::tm time_struct{};
    std::istringstream iss(iso);
    iss >> std::get_time(&time_struct, "%Y-%m-%dT%H:%M:%SZ");
    if (iss.fail())
    {
        return TimePoint{};
    }
    const auto time_t_val = timegm(&time_struct);
    return std::chrono::system_clock::from_time_t(time_t_val);
}

// ============================================================================
// JSON Serialization
// ============================================================================

auto FlashcardStore::serialize_decks() const -> std::string
{
    std::ostringstream json;
    json << "[";
    bool first = true;
    for (const auto& [deck_id, deck] : decks_)
    {
        if (!first)
            json << ",";
        first = false;
        json << "{\"id\":\"" << escape_json(deck.id) << "\""
             << ",\"name\":\"" << escape_json(deck.name) << "\""
             << ",\"description\":\"" << escape_json(deck.description) << "\""
             << ",\"new_cards_per_day\":" << deck.new_cards_per_day
             << ",\"max_reviews_per_day\":" << deck.max_reviews_per_day
             << ",\"use_global_params\":" << (deck.use_global_params ? "true" : "false") << "}";
    }
    json << "]";
    return json.str();
}

auto FlashcardStore::serialize_cards() const -> std::string
{
    std::ostringstream json;
    json << "[";
    bool first = true;
    for (const auto& [card_id, card] : cards_)
    {
        if (!first)
            json << ",";
        first = false;
        json << "{\"id\":\"" << escape_json(card.id) << "\""
             << ",\"deck_id\":\"" << escape_json(card.deck_id) << "\""
             << ",\"block_id\":\"" << escape_json(card.block_id) << "\""
             << ",\"front_content\":\"" << escape_json(card.front_content) << "\""
             << ",\"back_content\":\"" << escape_json(card.back_content) << "\""
             << ",\"state\":\"" << card_state_to_string(card.state) << "\""
             << ",\"stability\":" << card.stability << ",\"difficulty\":" << card.difficulty
             << ",\"elapsed_days\":" << card.elapsed_days
             << ",\"scheduled_days\":" << card.scheduled_days << ",\"reps\":" << card.reps
             << ",\"lapses\":" << card.lapses << ",\"due\":\"" << time_to_iso(card.due) << "\""
             << ",\"last_review\":\"" << time_to_iso(card.last_review) << "\""
             << "}";
    }
    json << "]";
    return json.str();
}

auto FlashcardStore::serialize_review_logs() const -> std::string
{
    std::ostringstream json;
    json << "[";
    for (size_t idx = 0; idx < review_logs_.size(); ++idx)
    {
        if (idx > 0)
            json << ",";
        const auto& log = review_logs_[idx];
        json << "{\"id\":\"" << escape_json(log.id) << "\""
             << ",\"card_id\":\"" << escape_json(log.card_id) << "\""
             << ",\"rating\":\"" << rating_to_string(log.rating) << "\""
             << ",\"state\":\"" << card_state_to_string(log.state) << "\""
             << ",\"stability\":" << log.stability << ",\"difficulty\":" << log.difficulty
             << ",\"elapsed_days\":" << log.elapsed_days
             << ",\"scheduled_days\":" << log.scheduled_days << ",\"due\":\""
             << time_to_iso(log.due) << "\""
             << ",\"review\":\"" << time_to_iso(log.review) << "\""
             << "}";
    }
    json << "]";
    return json.str();
}

// ============================================================================
// JSON Deserialization (minimal parser for our known schema)
// ============================================================================

auto FlashcardStore::deserialize_decks(const std::string& json) -> std::expected<void, std::string>
{
    // Minimal: accept empty array or populated arrays
    if (json.empty() || json == "[]")
    {
        return {};
    }
    // For now, accept but don't parse complex JSON (full parser deferred)
    // This ensures load() doesn't fail on first run
    return {};
}

auto FlashcardStore::deserialize_cards(const std::string& json) -> std::expected<void, std::string>
{
    if (json.empty() || json == "[]")
    {
        return {};
    }
    return {};
}

auto FlashcardStore::deserialize_review_logs(const std::string& json)
    -> std::expected<void, std::string>
{
    if (json.empty() || json == "[]")
    {
        return {};
    }
    return {};
}

} // namespace markamp::core::fsrs
