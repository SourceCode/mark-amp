#include "FSRSTypes.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace markamp::core::fsrs
{

// ── CardState conversions ──

auto card_state_to_string(CardState state) -> std::string
{
    switch (state)
    {
        case CardState::New:
            return "new";
        case CardState::Learning:
            return "learning";
        case CardState::Review:
            return "review";
        case CardState::Relearning:
            return "relearning";
    }
    return "new";
}

auto string_to_card_state(const std::string& str) -> CardState
{
    if (str == "learning")
        return CardState::Learning;
    if (str == "review")
        return CardState::Review;
    if (str == "relearning")
        return CardState::Relearning;
    return CardState::New;
}

// ── Rating conversions ──

auto rating_to_string(Rating rating) -> std::string
{
    switch (rating)
    {
        case Rating::Again:
            return "again";
        case Rating::Hard:
            return "hard";
        case Rating::Good:
            return "good";
        case Rating::Easy:
            return "easy";
    }
    return "good";
}

auto string_to_rating(const std::string& str) -> Rating
{
    if (str == "again")
        return Rating::Again;
    if (str == "hard")
        return Rating::Hard;
    if (str == "easy")
        return Rating::Easy;
    return Rating::Good;
}

auto rating_to_int(Rating rating) -> int
{
    return static_cast<int>(rating);
}

// ── SchedulingResult ──

auto SchedulingResult::for_rating(Rating rating) const -> const ScheduledState&
{
    switch (rating)
    {
        case Rating::Again:
            return again;
        case Rating::Hard:
            return hard;
        case Rating::Good:
            return good;
        case Rating::Easy:
            return easy;
    }
    return good;
}

auto SchedulingResult::interval_display(Rating rating) const -> std::string
{
    const auto& state = for_rating(rating);
    const auto days = state.card.scheduled_days;

    if (days < 1.0)
    {
        // Show in minutes
        const auto minutes = static_cast<int>(std::round(days * 24.0 * 60.0));
        if (minutes <= 0)
        {
            return "<1m";
        }
        return std::to_string(minutes) + "m";
    }

    if (days < 30.0)
    {
        return std::to_string(static_cast<int>(std::round(days))) + "d";
    }

    if (days < 365.0)
    {
        const auto months = static_cast<int>(std::round(days / 30.0));
        return std::to_string(months) + "mo";
    }

    const auto years = days / 365.0;
    std::ostringstream oss;
    oss.precision(1);
    oss << std::fixed << years << "yr";
    return oss.str();
}

} // namespace markamp::core::fsrs
