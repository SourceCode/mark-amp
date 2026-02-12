#pragma once

#include <chrono>
#include <cstdint>

namespace markamp::core
{

/// Activity mode for adaptive throttling.
///
/// In Typing mode, frame budget is allocated primarily to input+paint.
/// In Idle mode, more budget goes to highlighting, lint, prefetch.
///
/// Pattern implemented: #32 Adaptive throttling (typing vs idle modes)
enum class ActivityMode : uint8_t
{
    Typing, // User actively typing — minimum background work
    Idle    // No recent input — allow catch-up work
};

/// Switches between "typing mode" and "idle mode" based on input activity.
///
/// Each keystroke calls update_activity(). current_mode() returns the
/// current mode based on elapsed time since last activity.
///
/// Pattern implemented: #32 Adaptive throttling (typing vs idle modes)
class AdaptiveThrottle
{
public:
    using Clock = std::chrono::steady_clock;

    static constexpr auto kDefaultIdleThreshold = std::chrono::milliseconds(300);
    static constexpr auto kDefaultTypingBudget = std::chrono::microseconds(4000); // 4ms
    static constexpr auto kDefaultIdleBudget = std::chrono::microseconds(16000);  // 16ms

    explicit AdaptiveThrottle(
        std::chrono::milliseconds idle_threshold = kDefaultIdleThreshold) noexcept
        : idle_threshold_(idle_threshold)
        , last_activity_(Clock::now() - idle_threshold) // start idle
    {
    }

    /// Signal user activity (called on each keystroke/mouse event).
    void update_activity() noexcept
    {
        last_activity_ = Clock::now();
    }

    /// Get the current activity mode.
    [[nodiscard]] auto current_mode() const noexcept -> ActivityMode
    {
        auto elapsed = Clock::now() - last_activity_;
        return (elapsed < idle_threshold_) ? ActivityMode::Typing : ActivityMode::Idle;
    }

    /// Get the appropriate frame budget for the current mode.
    [[nodiscard]] auto current_budget() const noexcept -> std::chrono::microseconds
    {
        return (current_mode() == ActivityMode::Typing) ? kDefaultTypingBudget : kDefaultIdleBudget;
    }

    /// Time since last input activity.
    [[nodiscard]] auto time_since_activity() const noexcept -> std::chrono::milliseconds
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - last_activity_);
    }

    /// Whether the user is currently typing.
    [[nodiscard]] auto is_typing() const noexcept -> bool
    {
        return current_mode() == ActivityMode::Typing;
    }

    /// Whether the system is idle and can do catch-up work.
    [[nodiscard]] auto is_idle() const noexcept -> bool
    {
        return current_mode() == ActivityMode::Idle;
    }

    /// Configure the idle threshold.
    void set_idle_threshold(std::chrono::milliseconds threshold) noexcept
    {
        idle_threshold_ = threshold;
    }

    /// Get the configured idle threshold.
    [[nodiscard]] auto idle_threshold() const noexcept -> std::chrono::milliseconds
    {
        return idle_threshold_;
    }

private:
    std::chrono::milliseconds idle_threshold_;
    Clock::time_point last_activity_;
};

} // namespace markamp::core
