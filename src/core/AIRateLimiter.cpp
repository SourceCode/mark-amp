// ============================================================================
// File: src/core/AIRateLimiter.cpp
// Phase 26: AI Integration — API usage tracking implementation
// ============================================================================

#include "AIRateLimiter.h"

#include "Events.h"

namespace markamp::core
{

AIRateLimiter::AIRateLimiter(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto AIRateLimiter::record_usage(int32_t prompt_tokens,
                                 int32_t completion_tokens,
                                 AIProvider provider) -> void
{
    const double prompt_cost = static_cast<double>(prompt_tokens) * cost_per_token(provider, false);
    const double completion_cost =
        static_cast<double>(completion_tokens) * cost_per_token(provider, true);
    const double request_cost = prompt_cost + completion_cost;

    // Update session stats.
    session_stats_.prompt_tokens += prompt_tokens;
    session_stats_.completion_tokens += completion_tokens;
    session_stats_.total_tokens += (prompt_tokens + completion_tokens);
    session_stats_.request_count++;
    session_stats_.estimated_cost += request_cost;

    // Update total stats.
    total_stats_.prompt_tokens += prompt_tokens;
    total_stats_.completion_tokens += completion_tokens;
    total_stats_.total_tokens += (prompt_tokens + completion_tokens);
    total_stats_.request_count++;
    total_stats_.estimated_cost += request_cost;

    // Check for warning threshold.
    if (monthly_limit_ > 0.0 && !warning_sent_)
    {
        const double pct = usage_percentage();
        if (pct >= 0.8)
        {
            warning_sent_ = true;
            events::AIRateLimitWarningEvent evt;
            evt.usage_percentage = pct;
            evt.limit = monthly_limit_;
            event_bus_.publish(evt);
        }
    }
}

auto AIRateLimiter::session_usage() const -> AIUsageStats
{
    return session_stats_;
}

auto AIRateLimiter::total_usage() const -> AIUsageStats
{
    return total_stats_;
}

auto AIRateLimiter::estimated_cost() const -> double
{
    return total_stats_.estimated_cost;
}

auto AIRateLimiter::check_limit() const -> bool
{
    if (monthly_limit_ <= 0.0)
    {
        return true; // No limit set
    }
    if (!hard_stop_)
    {
        return true; // Soft limit only
    }
    return total_stats_.estimated_cost < monthly_limit_;
}

auto AIRateLimiter::set_monthly_limit(double limit) -> void
{
    monthly_limit_ = limit;
    warning_sent_ = false; // Reset warning on limit change
}

auto AIRateLimiter::monthly_limit() const -> double
{
    return monthly_limit_;
}

auto AIRateLimiter::usage_percentage() const -> double
{
    if (monthly_limit_ <= 0.0)
    {
        return 0.0;
    }
    return total_stats_.estimated_cost / monthly_limit_;
}

auto AIRateLimiter::set_hard_stop(bool enabled) -> void
{
    hard_stop_ = enabled;
}

auto AIRateLimiter::hard_stop_enabled() const -> bool
{
    return hard_stop_;
}

auto AIRateLimiter::reset_session() -> void
{
    session_stats_ = AIUsageStats{};
}

auto AIRateLimiter::reset_all() -> void
{
    session_stats_ = AIUsageStats{};
    total_stats_ = AIUsageStats{};
    warning_sent_ = false;
}

auto AIRateLimiter::cost_per_token(AIProvider provider, bool is_completion) -> double
{
    // Approximate pricing per token (USD).
    switch (provider)
    {
        case AIProvider::OpenAI:
            return is_completion ? 0.00003 : 0.00001; // GPT-4 pricing
        case AIProvider::Anthropic:
            return is_completion ? 0.000015 : 0.000003; // Claude 3 Sonnet pricing
        case AIProvider::Local:
            return 0.0; // Local models are free
        case AIProvider::Custom:
            return is_completion ? 0.00001 : 0.000005; // Conservative estimate
    }
    return 0.0;
}

} // namespace markamp::core
