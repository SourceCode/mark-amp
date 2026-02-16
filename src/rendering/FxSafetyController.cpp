/// FxSafetyController.cpp — Phase 50/55: Safety Controller Implementation

#include "FxSafetyController.h"

#include "core/EventBus.h"

#include <spdlog/spdlog.h>

namespace markamp::rendering
{

FxSafetyController::FxSafetyController(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
    static_cast<void>(event_bus_); // Reserved for future event publishing.
}

// ── Master controls ──

void FxSafetyController::set_master_enabled(bool enabled) noexcept
{
    master_enabled_ = enabled;
}

auto FxSafetyController::is_master_enabled() const noexcept -> bool
{
    return master_enabled_;
}

// ── Safety modes ──

void FxSafetyController::set_reduced_motion(bool enabled) noexcept
{
    reduced_motion_ = enabled;
    spdlog::info("FxSafety: reduced motion {}", enabled ? "enabled" : "disabled");
}

auto FxSafetyController::is_reduced_motion() const noexcept -> bool
{
    return reduced_motion_;
}

void FxSafetyController::set_low_power_mode(bool enabled) noexcept
{
    low_power_mode_ = enabled;
    spdlog::info("FxSafety: low power mode {}", enabled ? "enabled" : "disabled");
}

auto FxSafetyController::is_low_power_mode() const noexcept -> bool
{
    return low_power_mode_;
}

void FxSafetyController::set_text_safety_mode(bool enabled) noexcept
{
    text_safety_mode_ = enabled;
}

auto FxSafetyController::is_text_safety_mode() const noexcept -> bool
{
    return text_safety_mode_;
}

void FxSafetyController::set_high_contrast_guard(bool enabled) noexcept
{
    high_contrast_guard_ = enabled;
}

auto FxSafetyController::is_high_contrast_guard() const noexcept -> bool
{
    return high_contrast_guard_;
}

// ── Frame budget ──

void FxSafetyController::set_max_frame_time_budget(float budget_ms) noexcept
{
    max_frame_time_budget_ms_ = budget_ms;
}

auto FxSafetyController::max_frame_time_budget() const noexcept -> float
{
    return max_frame_time_budget_ms_;
}

// ── Adaptive degradation ──

void FxSafetyController::report_frame_timing(const FrameTimingSnapshot& timing)
{
    ++frame_count_;

    // Running average
    const float kAlpha = 0.1F;
    avg_frame_time_ms_ = (kAlpha * timing.frame_time_ms) + ((1.0F - kAlpha) * avg_frame_time_ms_);

    if (timing.frame_time_ms > max_frame_time_budget_ms_)
    {
        ++violation_count_;
    }
    else if (violation_count_ > 0 && frame_count_ % kTimingWindowSize == 0)
    {
        // Decay violations over time
        --violation_count_;
    }

    // Auto-enable low power on battery
    if (timing.is_on_battery && !low_power_mode_)
    {
        set_low_power_mode(true);
        spdlog::info("FxSafety: auto-enabled low power mode (battery detected)");
    }
}

auto FxSafetyController::should_degrade() const noexcept -> bool
{
    // Degrade if we've had 3+ violations in recent frames
    return violation_count_ >= 3 || low_power_mode_;
}

auto FxSafetyController::recommended_tier() const noexcept -> QualityTier
{
    if (!master_enabled_)
    {
        return QualityTier::kMinimal;
    }

    if (low_power_mode_ || violation_count_ >= 5)
    {
        return QualityTier::kMinimal;
    }

    if (violation_count_ >= 3)
    {
        return QualityTier::kEfficient;
    }

    if (violation_count_ >= 1)
    {
        return QualityTier::kBalanced;
    }

    return QualityTier::kCinematic;
}

auto FxSafetyController::is_pass_safe(FxPassType pass_type) const noexcept -> bool
{
    if (!master_enabled_)
    {
        return false;
    }

    // Reduced motion blocks distortion and motion-heavy passes
    if (reduced_motion_)
    {
        if (pass_type == FxPassType::kDistortion)
        {
            return false;
        }
    }

    // Text safety blocks distortion and chroma on text
    if (text_safety_mode_)
    {
        if (pass_type == FxPassType::kDistortion || pass_type == FxPassType::kChromaShift)
        {
            return false;
        }
    }

    // Low power blocks expensive passes
    if (low_power_mode_)
    {
        if (pass_type == FxPassType::kBlur || pass_type == FxPassType::kBloom ||
            pass_type == FxPassType::kReflection || pass_type == FxPassType::kNoiseGrain)
        {
            return false;
        }
    }

    return true;
}

auto FxSafetyController::violation_count() const noexcept -> uint32_t
{
    return violation_count_;
}

void FxSafetyController::reset_to_defaults() noexcept
{
    master_enabled_ = true;
    reduced_motion_ = false;
    low_power_mode_ = false;
    text_safety_mode_ = false;
    high_contrast_guard_ = true;
    max_frame_time_budget_ms_ = 16.0F;
    violation_count_ = 0;
    frame_count_ = 0;
    avg_frame_time_ms_ = 0.0F;
}

} // namespace markamp::rendering
