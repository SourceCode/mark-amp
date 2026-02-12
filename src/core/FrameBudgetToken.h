#pragma once

#include "FrameScheduler.h"
#include "Profiler.h"

#include <chrono>
#include <cstdint>

namespace markamp::core
{

/// Degradation quality level — subsystems choose rendering fidelity
/// based on remaining frame budget.
///
/// Pattern implemented: #21 Frame-time budgeting (hard cap) with graceful degradation
enum class DegradationLevel : uint8_t
{
    Full,    // All features enabled — within budget
    Reduced, // Drop non-critical rendering (e.g., skip fancy overlays)
    Minimal  // Emergency — only essential paint, skip highlighting/layout
};

/// Per-frame budget token checked by all UI work before proceeding.
///
/// Constructed at frame start with a target budget (e.g., 8–16ms).
/// All rendering/layout subsystems receive a const ref and bail early
/// when the budget is spent. Instead of blocking input, work degrades
/// quality via the DegradationLevel.
///
/// Pattern implemented: #21 Frame-time budgeting (hard cap) with graceful degradation
class FrameBudgetToken
{
public:
    static constexpr auto kDefaultBudget = std::chrono::microseconds(16000); // 16ms ≈ 60 fps

    explicit FrameBudgetToken(std::chrono::microseconds budget = kDefaultBudget) noexcept
        : budget_(budget)
        , start_(std::chrono::high_resolution_clock::now())
    {
    }

    /// Check if the frame budget has been exhausted.
    [[nodiscard]] auto is_exhausted() const noexcept -> bool
    {
        return elapsed() >= budget_;
    }

    /// Time remaining in the budget. Returns 0 if exhausted.
    [[nodiscard]] auto remaining() const noexcept -> std::chrono::microseconds
    {
        auto used = elapsed();
        return used >= budget_ ? std::chrono::microseconds(0) : budget_ - used;
    }

    /// Time already consumed.
    [[nodiscard]] auto elapsed() const noexcept -> std::chrono::microseconds
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start_);
    }

    /// Current degradation level based on remaining budget.
    ///   > 50% remaining → Full
    ///   > 15% remaining → Reduced
    ///   otherwise        → Minimal
    [[nodiscard]] auto degradation_level() const noexcept -> DegradationLevel
    {
        auto pct = remaining_fraction();
        if (pct > 0.50)
        {
            return DegradationLevel::Full;
        }
        if (pct > 0.15)
        {
            return DegradationLevel::Reduced;
        }
        return DegradationLevel::Minimal;
    }

    /// Whether quality should be degraded (budget < 50% remaining).
    [[nodiscard]] auto should_degrade() const noexcept -> bool
    {
        return degradation_level() != DegradationLevel::Full;
    }

    /// Budget originally allotted.
    [[nodiscard]] auto budget() const noexcept -> std::chrono::microseconds
    {
        return budget_;
    }

private:
    std::chrono::microseconds budget_;
    std::chrono::high_resolution_clock::time_point start_;

    [[nodiscard]] auto remaining_fraction() const noexcept -> double
    {
        auto rem = remaining();
        if (budget_.count() == 0)
        {
            return 0.0;
        }
        return static_cast<double>(rem.count()) / static_cast<double>(budget_.count());
    }
};

/// RAII wrapper that starts a frame budget, runs the FrameScheduler
/// with the remaining time, and logs overruns on destruction.
///
/// Pattern implemented: #21 Frame-time budgeting (hard cap) with graceful degradation
class AutoFrameBudget
{
public:
    explicit AutoFrameBudget(
        FrameScheduler& scheduler,
        std::chrono::microseconds budget = FrameBudgetToken::kDefaultBudget) noexcept
        : scheduler_(scheduler)
        , token_(budget)
    {
    }

    ~AutoFrameBudget()
    {
        // Run scheduler with whatever budget remains
        auto rem = token_.remaining();
        if (rem.count() > 0)
        {
            scheduler_.run_frame(rem);
        }
    }

    AutoFrameBudget(const AutoFrameBudget&) = delete;
    auto operator=(const AutoFrameBudget&) -> AutoFrameBudget& = delete;
    AutoFrameBudget(AutoFrameBudget&&) = delete;
    auto operator=(AutoFrameBudget&&) -> AutoFrameBudget& = delete;

    /// Access the underlying budget token for subsystem checks.
    [[nodiscard]] auto token() const noexcept -> const FrameBudgetToken&
    {
        return token_;
    }

private:
    FrameScheduler& scheduler_;
    FrameBudgetToken token_;
};

} // namespace markamp::core
