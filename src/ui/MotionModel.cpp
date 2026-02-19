#include "MotionModel.h"

namespace markamp::ui
{

void MotionModel::set_tokens(std::vector<MotionToken> tokens)
{
    tokens_ = std::move(tokens);
}

auto MotionModel::tokens() const -> const std::vector<MotionToken>&
{
    return tokens_;
}

auto MotionModel::token_for(MotionContext context) const -> MotionToken
{
    for (const auto& token : tokens_)
    {
        if (token.context == context)
        {
            return token;
        }
    }
    // Default fallback
    return {"default", context, 200, EasingCurve::kEaseOut};
}

auto MotionModel::effective_duration(MotionContext context) const -> int
{
    if (reduced_motion_ && !is_essential(context))
    {
        return 0;
    }
    return token_for(context).duration_ms;
}

void MotionModel::set_reduced_motion(bool enabled)
{
    reduced_motion_ = enabled;
}
auto MotionModel::reduced_motion() const -> bool
{
    return reduced_motion_;
}

auto MotionModel::is_essential(MotionContext context) -> bool
{
    // Reveal and dismiss are essential for user orientation
    return context == MotionContext::kReveal || context == MotionContext::kDismiss;
}

void MotionModel::set_latency_budget_ms(int budget_ms)
{
    latency_budget_ms_ = budget_ms;
}
auto MotionModel::latency_budget_ms() const -> int
{
    return latency_budget_ms_;
}

auto MotionModel::exceeds_budget(MotionContext context) const -> bool
{
    return token_for(context).duration_ms > latency_budget_ms_;
}

} // namespace markamp::ui
