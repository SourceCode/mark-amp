/// FrameBudgetEnforcer.h — Phase 27: Frame budget enforcement and graceful degradation
///
/// Wires FrameBudgetToken + AdaptiveThrottle into a unified paint cycle controller.
/// Tracks consecutive overruns and dynamically adjusts target budget.
///
/// Pattern implemented: #21 Frame-time budgeting (hard cap) with graceful degradation

#pragma once

#include "AdaptiveThrottle.h"
#include "FrameBudgetToken.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Statistics from a single paint frame.
struct FrameStats
{
    std::chrono::microseconds budget{0};
    std::chrono::microseconds elapsed{0};
    DegradationLevel level{DegradationLevel::Full};
    ActivityMode activity{ActivityMode::Idle};
    bool overran{false};
};

/// Subsystem callback that respects frame budget.
using BudgetedWork = std::function<void(const FrameBudgetToken&)>;

/// Unified paint cycle controller combining FrameBudgetToken + AdaptiveThrottle.
///
/// Usage:
///   FrameBudgetEnforcer enforcer;
///   enforcer.register_subsystem("Syntax", [](const auto& tok) { /*...*/ });
///   enforcer.register_subsystem("Layout", [](const auto& tok) { /*...*/ });
///   auto stats = enforcer.run_frame();  // runs subsystems in priority order
class FrameBudgetEnforcer
{
public:
    static constexpr uint32_t kMaxConsecutiveOverruns = 5;

    FrameBudgetEnforcer() = default;

    /// Register a subsystem to run during the paint cycle.
    /// Subsystems run in registration order (priority order).
    void register_subsystem(std::string_view name, BudgetedWork work)
    {
        subsystems_.push_back({std::string(name), std::move(work)});
    }

    /// Signal user input activity (delegates to AdaptiveThrottle).
    void on_input_activity()
    {
        throttle_.update_activity();
    }

    /// Run a single paint frame. Returns statistics.
    [[nodiscard]] auto run_frame() -> FrameStats
    {
        auto budget_us = throttle_.current_budget();
        FrameBudgetToken token(budget_us);

        FrameStats stats;
        stats.budget = budget_us;
        stats.activity = throttle_.current_mode();

        // Run each subsystem, bailing early if budget is exhausted
        for (const auto& subsystem : subsystems_)
        {
            if (token.is_exhausted())
            {
                break;
            }
            subsystem.work(token);
        }

        stats.elapsed = token.elapsed();
        stats.level = token.degradation_level();
        stats.overran = token.is_exhausted();

        // Track consecutive overruns
        if (stats.overran)
        {
            ++consecutive_overruns_;
        }
        else
        {
            consecutive_overruns_ = 0;
        }

        ++total_frames_;
        if (stats.overran)
        {
            ++overrun_frames_;
        }

        return stats;
    }

    /// Number of consecutive frames that exceeded their budget.
    [[nodiscard]] auto consecutive_overruns() const noexcept -> uint32_t
    {
        return consecutive_overruns_;
    }

    /// Whether the system is in sustained overrun (needs backoff).
    [[nodiscard]] auto is_sustained_overrun() const noexcept -> bool
    {
        return consecutive_overruns_ >= kMaxConsecutiveOverruns;
    }

    /// Total frames processed.
    [[nodiscard]] auto total_frames() const noexcept -> uint64_t
    {
        return total_frames_;
    }

    /// Number of frames that overran budget.
    [[nodiscard]] auto overrun_frames() const noexcept -> uint64_t
    {
        return overrun_frames_;
    }

    /// Overrun percentage (0-100).
    [[nodiscard]] auto overrun_percentage() const noexcept -> double
    {
        if (total_frames_ == 0)
        {
            return 0.0;
        }
        return (static_cast<double>(overrun_frames_) / static_cast<double>(total_frames_)) * 100.0;
    }

    /// Access the underlying AdaptiveThrottle for configuration.
    [[nodiscard]] auto throttle() -> AdaptiveThrottle&
    {
        return throttle_;
    }

    /// Number of registered subsystems.
    [[nodiscard]] auto subsystem_count() const noexcept -> std::size_t
    {
        return subsystems_.size();
    }

private:
    struct SubsystemEntry
    {
        std::string name;
        BudgetedWork work;
    };

    std::vector<SubsystemEntry> subsystems_;
    AdaptiveThrottle throttle_;

    uint32_t consecutive_overruns_{0};
    uint64_t total_frames_{0};
    uint64_t overrun_frames_{0};
};

} // namespace markamp::core
