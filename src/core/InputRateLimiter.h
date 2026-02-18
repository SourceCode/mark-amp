// ============================================================================
// File: src/core/InputRateLimiter.h
// Phase 29: Security & Input Validation — Token-Bucket Rate Limiting
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Configuration for a single rate-limited action.
struct RateLimitConfig
{
    int32_t max_tokens{10};          ///< Maximum burst capacity
    float refill_rate_per_sec{2.0F}; ///< Tokens refilled per second
    int32_t refill_interval_ms{500}; ///< Refill check interval
};

/// Token-bucket rate limiter for user actions that trigger expensive operations.
///
/// Prevents abuse and protects performance by limiting how frequently
/// costly operations (search, AI, export) can be triggered. Each action
/// has its own independent token bucket.
class InputRateLimiter
{
public:
    InputRateLimiter();

    /// Try to acquire a token for the given action. Returns true if allowed.
    [[nodiscard]] auto try_acquire(const std::string& action_name) -> bool;

    /// Get remaining tokens for an action.
    [[nodiscard]] auto remaining_tokens(const std::string& action_name) const -> int32_t;

    /// Check if an action is currently rate-limited (zero tokens).
    [[nodiscard]] auto is_limited(const std::string& action_name) const -> bool;

    /// Configure a specific action's rate limit.
    auto configure_action(const std::string& action_name, const RateLimitConfig& config) -> void;

    /// Reset tokens for a specific action to max.
    auto reset(const std::string& action_name) -> void;

    /// Reset all actions to their max tokens.
    auto reset_all() -> void;

    /// Get the configuration for an action (returns default if not configured).
    [[nodiscard]] auto get_config(const std::string& action_name) const -> RateLimitConfig;

    /// List all configured action names.
    [[nodiscard]] auto configured_actions() const -> std::vector<std::string>;

    /// Get the total number of rate-limited rejections across all actions.
    [[nodiscard]] auto total_rejections() const noexcept -> uint64_t;

    /// Get rejections for a specific action.
    [[nodiscard]] auto rejections(const std::string& action_name) const -> uint64_t;

private:
    /// Internal state for a single action bucket.
    struct Bucket
    {
        RateLimitConfig config;
        float current_tokens{0.0F};
        std::chrono::steady_clock::time_point last_refill;
        uint64_t rejection_count{0};
    };

    /// Refill tokens for a bucket based on elapsed time.
    static auto refill_bucket(Bucket& bucket) -> void;

    /// Get or create a bucket for the given action.
    auto get_or_create_bucket(const std::string& action_name) -> Bucket&;

    /// Initialize default actions.
    void initialize_defaults();

    std::unordered_map<std::string, Bucket> buckets_;
    RateLimitConfig default_config_;
    uint64_t total_rejections_{0};
};

} // namespace markamp::core
