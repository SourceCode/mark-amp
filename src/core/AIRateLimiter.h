// ============================================================================
// File: src/core/AIRateLimiter.h
// Phase 26: AI Integration — API usage tracking and rate limiting
// ============================================================================
#pragma once

#include "AITypes.h"

#include <string>

namespace markamp::core
{

class EventBus;

/// Tracks AI API usage (tokens, requests, cost) and enforces spending limits.
/// Warning at 80% of limit, hard stop at 100% (configurable).
class AIRateLimiter
{
public:
    explicit AIRateLimiter(EventBus& event_bus);

    /// Record usage from a completed AI request.
    auto record_usage(int32_t prompt_tokens, int32_t completion_tokens, AIProvider provider)
        -> void;

    /// Get session usage statistics (resets each app launch).
    [[nodiscard]] auto session_usage() const -> AIUsageStats;

    /// Get total (all-time) usage statistics.
    [[nodiscard]] auto total_usage() const -> AIUsageStats;

    /// Get estimated cost for total usage.
    [[nodiscard]] auto estimated_cost() const -> double;

    /// Check if usage is under the monthly limit. Returns true if allowed.
    [[nodiscard]] auto check_limit() const -> bool;

    /// Set monthly spending limit in dollars. 0 = unlimited.
    auto set_monthly_limit(double limit) -> void;

    /// Get current monthly spending limit.
    [[nodiscard]] auto monthly_limit() const -> double;

    /// Get usage as a percentage of the monthly limit (0.0–1.0+).
    [[nodiscard]] auto usage_percentage() const -> double;

    /// Enable or disable the hard stop at limit.
    auto set_hard_stop(bool enabled) -> void;

    /// Check if hard stop is enabled.
    [[nodiscard]] auto hard_stop_enabled() const -> bool;

    /// Reset session stats.
    auto reset_session() -> void;

    /// Reset all stats.
    auto reset_all() -> void;

private:
    EventBus& event_bus_;
    AIUsageStats session_stats_;
    AIUsageStats total_stats_;
    double monthly_limit_{0.0}; // 0 = unlimited
    bool hard_stop_{true};
    bool warning_sent_{false};

    /// Calculate cost per token for a given provider.
    [[nodiscard]] static auto cost_per_token(AIProvider provider, bool is_completion) -> double;
};

} // namespace markamp::core
