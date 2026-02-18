// ============================================================================
// File: src/core/InputRateLimiter.cpp
// Phase 29: Security & Input Validation — Token-Bucket Rate Limiting
// ============================================================================

#include "InputRateLimiter.h"

#include <algorithm>

namespace markamp::core
{

// ── Construction ──

InputRateLimiter::InputRateLimiter()
{
    initialize_defaults();
}

void InputRateLimiter::initialize_defaults()
{
    // Pre-configure common expensive actions.
    configure_action("search",
                     {.max_tokens = 10, .refill_rate_per_sec = 3.0F, .refill_interval_ms = 333});
    configure_action("ai_completion",
                     {.max_tokens = 5, .refill_rate_per_sec = 1.0F, .refill_interval_ms = 1000});
    configure_action("ai_chat",
                     {.max_tokens = 3, .refill_rate_per_sec = 0.5F, .refill_interval_ms = 2000});
    configure_action("export",
                     {.max_tokens = 3, .refill_rate_per_sec = 1.0F, .refill_interval_ms = 1000});
    configure_action("theme_preview",
                     {.max_tokens = 8, .refill_rate_per_sec = 4.0F, .refill_interval_ms = 250});
}

// ── Token management ──

auto InputRateLimiter::refill_bucket(Bucket& bucket) -> void
{
    const auto kNow = std::chrono::steady_clock::now();
    const auto kElapsed = std::chrono::duration<float>(kNow - bucket.last_refill);
    const float kTokensToAdd = kElapsed.count() * bucket.config.refill_rate_per_sec;

    bucket.current_tokens = std::min(static_cast<float>(bucket.config.max_tokens),
                                     bucket.current_tokens + kTokensToAdd);
    bucket.last_refill = kNow;
}

auto InputRateLimiter::get_or_create_bucket(const std::string& action_name) -> Bucket&
{
    auto iter = buckets_.find(action_name);
    if (iter == buckets_.end())
    {
        Bucket new_bucket;
        new_bucket.config = default_config_;
        new_bucket.current_tokens = static_cast<float>(default_config_.max_tokens);
        new_bucket.last_refill = std::chrono::steady_clock::now();
        auto [inserted_iter, inserted_ok] = buckets_.emplace(action_name, new_bucket);
        return inserted_iter->second;
    }
    return iter->second;
}

auto InputRateLimiter::try_acquire(const std::string& action_name) -> bool
{
    auto& bucket = get_or_create_bucket(action_name);
    refill_bucket(bucket);

    if (bucket.current_tokens >= 1.0F)
    {
        bucket.current_tokens -= 1.0F;
        return true;
    }

    ++bucket.rejection_count;
    ++total_rejections_;
    return false;
}

auto InputRateLimiter::remaining_tokens(const std::string& action_name) const -> int32_t
{
    auto iter = buckets_.find(action_name);
    if (iter == buckets_.end())
    {
        return default_config_.max_tokens;
    }
    return static_cast<int32_t>(iter->second.current_tokens);
}

auto InputRateLimiter::is_limited(const std::string& action_name) const -> bool
{
    auto iter = buckets_.find(action_name);
    if (iter == buckets_.end())
    {
        return false;
    }
    return iter->second.current_tokens < 1.0F;
}

// ── Configuration ──

auto InputRateLimiter::configure_action(const std::string& action_name,
                                        const RateLimitConfig& config) -> void
{
    Bucket bucket;
    bucket.config = config;
    bucket.current_tokens = static_cast<float>(config.max_tokens);
    bucket.last_refill = std::chrono::steady_clock::now();
    buckets_[action_name] = bucket;
}

auto InputRateLimiter::reset(const std::string& action_name) -> void
{
    auto iter = buckets_.find(action_name);
    if (iter != buckets_.end())
    {
        iter->second.current_tokens = static_cast<float>(iter->second.config.max_tokens);
        iter->second.last_refill = std::chrono::steady_clock::now();
    }
}

auto InputRateLimiter::reset_all() -> void
{
    for (auto& [name, bucket] : buckets_)
    {
        bucket.current_tokens = static_cast<float>(bucket.config.max_tokens);
        bucket.last_refill = std::chrono::steady_clock::now();
    }
}

auto InputRateLimiter::get_config(const std::string& action_name) const -> RateLimitConfig
{
    auto iter = buckets_.find(action_name);
    if (iter == buckets_.end())
    {
        return default_config_;
    }
    return iter->second.config;
}

auto InputRateLimiter::configured_actions() const -> std::vector<std::string>
{
    std::vector<std::string> actions;
    actions.reserve(buckets_.size());
    for (const auto& [name, bucket] : buckets_)
    {
        actions.push_back(name);
    }
    return actions;
}

auto InputRateLimiter::total_rejections() const noexcept -> uint64_t
{
    return total_rejections_;
}

auto InputRateLimiter::rejections(const std::string& action_name) const -> uint64_t
{
    auto iter = buckets_.find(action_name);
    if (iter == buckets_.end())
    {
        return 0;
    }
    return iter->second.rejection_count;
}

} // namespace markamp::core
