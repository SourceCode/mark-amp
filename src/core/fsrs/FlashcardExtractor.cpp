#include "FlashcardExtractor.h"

#include "ClozeParser.h"

#include <algorithm>
#include <functional>
#include <regex>
#include <sstream>

namespace markamp::core::fsrs
{

// ── Public API ──────────────────────────────────────────────────────────────

auto FlashcardExtractor::extract_from_document(const std::string& content,
                                               const std::string& file_path) const
    -> std::vector<ExtractedCard>
{
    std::vector<ExtractedCard> results;
    std::istringstream stream(content);
    std::string line;
    int32_t line_number = 0;

    while (std::getline(stream, line))
    {
        ++line_number;
        auto card = extract_from_line(line, line_number, file_path);
        if (card.has_value())
        {
            results.push_back(std::move(card.value()));
        }
    }

    return results;
}

auto FlashcardExtractor::extract_from_line(const std::string& line,
                                           int32_t line_number,
                                           const std::string& file_path) const
    -> std::optional<ExtractedCard>
{
    // Pattern: ?? front / back ?? or ?? front / back / hint ??
    // Using lazy matching for the content between ?? markers
    static const std::regex kFlashcardPattern(R"(\?\?\s*(.*?)\s*/\s*(.*?)(?:\s*/\s*(.*?))?\s*\?\?)",
                                              std::regex::ECMAScript);

    std::smatch match;
    if (!std::regex_search(line, match, kFlashcardPattern))
    {
        return std::nullopt;
    }

    ExtractedCard card;
    card.front = trim(match[1].str());
    card.back = trim(match[2].str());

    if (match[3].matched)
    {
        card.hint = trim(match[3].str());
    }

    card.source_file = file_path;
    card.source_line = line_number;
    card.block_id = generate_block_id(file_path, line_number);

    // Check if this is a cloze card
    card.is_cloze = ClozeParser::is_cloze(card.front) || ClozeParser::is_cloze(card.back);

    return card;
}

auto FlashcardExtractor::sync_cards(const std::vector<ExtractedCard>& extracted,
                                    const FlashcardDeck& deck) const -> SyncResult
{
    SyncResult result;

    // Build a set of block IDs from extracted cards
    std::vector<std::string> extracted_block_ids;
    extracted_block_ids.reserve(extracted.size());
    for (const auto& card : extracted)
    {
        extracted_block_ids.push_back(card.block_id);
    }

    // Find new cards (extracted but not in deck)
    for (const auto& ext_card : extracted)
    {
        if (!deck.has_block(ext_card.block_id))
        {
            ++result.cards_added;
            result.added_card_ids.push_back(ext_card.block_id);
        }
        else
        {
            ++result.cards_unchanged;
        }
    }

    // Find removed cards (in deck but not in extracted)
    for (const auto& deck_card : deck.cards)
    {
        const auto found =
            std::find(extracted_block_ids.begin(), extracted_block_ids.end(), deck_card.block_id);
        if (found == extracted_block_ids.end())
        {
            ++result.cards_removed;
            result.removed_card_ids.push_back(deck_card.id);
        }
    }

    return result;
}

auto FlashcardExtractor::generate_block_id(const std::string& file_path, int32_t line_number)
    -> std::string
{
    // Create a deterministic ID from file path + line number using std::hash
    const std::string combined = file_path + ":" + std::to_string(line_number);
    const std::size_t hash_value = std::hash<std::string>{}(combined);

    // Format as hex string with "fc_" prefix
    std::ostringstream oss;
    oss << "fc_" << std::hex << hash_value;
    return oss.str();
}

auto FlashcardExtractor::trim(const std::string& str) -> std::string
{
    const auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
    {
        return "";
    }
    const auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace markamp::core::fsrs
