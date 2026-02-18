#include "LeechDetector.h"

#include <algorithm>

namespace markamp::core::fsrs
{

LeechDetector::LeechDetector(int32_t threshold)
    : threshold_(threshold)
{
}

auto LeechDetector::check_card(const FlashcardCard& card) const -> LeechStatus
{
    LeechStatus status;
    status.card_id = card.id;
    status.lapse_count = card.lapses;
    status.is_leech = card.lapses >= threshold_;
    status.is_suspended = is_suspended(card);
    return status;
}

auto LeechDetector::get_leeches(const FlashcardDeck& deck) const -> std::vector<LeechStatus>
{
    std::vector<LeechStatus> leeches;
    for (const auto& card : deck.cards)
    {
        if (card.lapses >= threshold_)
        {
            leeches.push_back(check_card(card));
        }
    }
    return leeches;
}

auto LeechDetector::suspend_card(FlashcardDeck& deck, const std::string& card_id) const
    -> std::expected<void, std::string>
{
    auto* card = deck.find_card_mut(card_id);
    if (card == nullptr)
    {
        return std::unexpected("Card not found: " + card_id);
    }

    // Mark as suspended by adding the suspended tag to block_id metadata
    // Convention: prefix block_id with "suspended:" marker
    if (!is_suspended(*card))
    {
        card->block_id = std::string(kSuspendedTag) + ":" + card->block_id;
    }

    return {};
}

auto LeechDetector::unsuspend_card(FlashcardDeck& deck, const std::string& card_id) const
    -> std::expected<void, std::string>
{
    auto* card = deck.find_card_mut(card_id);
    if (card == nullptr)
    {
        return std::unexpected("Card not found: " + card_id);
    }

    // Remove the suspended prefix
    const std::string prefix = std::string(kSuspendedTag) + ":";
    if (card->block_id.substr(0, prefix.size()) == prefix)
    {
        card->block_id = card->block_id.substr(prefix.size());
    }

    return {};
}

auto LeechDetector::is_suspended(const FlashcardCard& card) const -> bool
{
    const std::string prefix = std::string(kSuspendedTag) + ":";
    return card.block_id.size() >= prefix.size() &&
           card.block_id.substr(0, prefix.size()) == prefix;
}

auto LeechDetector::get_suspended(const FlashcardDeck& deck) const -> std::vector<std::string>
{
    std::vector<std::string> suspended;
    for (const auto& card : deck.cards)
    {
        if (is_suspended(card))
        {
            suspended.push_back(card.id);
        }
    }
    return suspended;
}

void LeechDetector::set_leech_threshold(int32_t count)
{
    threshold_ = count;
}

auto LeechDetector::leech_threshold() const -> int32_t
{
    return threshold_;
}

} // namespace markamp::core::fsrs
