/// FxSafetyController.h — Phase 50/55: Safety + Accessibility Controls
///
/// Ensures cinematic effects never degrade core editing usability.
/// Provides auto-degradation under load, reduced-motion mode,
/// low-power mode, text safety, and high-contrast guards.

#pragma once

#include "FxPass.h"

#include <chrono>
#include <cstdint>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::rendering
{

/// Frame timing snapshot for auto-degradation decisions.
struct FrameTimingSnapshot
{
    float frame_time_ms{0.0F};
    float input_latency_ms{0.0F};
    float scroll_latency_ms{0.0F};
    bool is_on_battery{false};
};

/// Controls FX safety modes and adaptive quality.
class FxSafetyController
{
public:
    explicit FxSafetyController(core::EventBus& event_bus);

    // ── Master controls ──

    void set_master_enabled(bool enabled) noexcept;
    [[nodiscard]] auto is_master_enabled() const noexcept -> bool;

    // ── Safety modes ──

    void set_reduced_motion(bool enabled) noexcept;
    [[nodiscard]] auto is_reduced_motion() const noexcept -> bool;

    void set_low_power_mode(bool enabled) noexcept;
    [[nodiscard]] auto is_low_power_mode() const noexcept -> bool;

    void set_text_safety_mode(bool enabled) noexcept;
    [[nodiscard]] auto is_text_safety_mode() const noexcept -> bool;

    void set_high_contrast_guard(bool enabled) noexcept;
    [[nodiscard]] auto is_high_contrast_guard() const noexcept -> bool;

    // ── Frame budget ──

    void set_max_frame_time_budget(float budget_ms) noexcept;
    [[nodiscard]] auto max_frame_time_budget() const noexcept -> float;

    // ── Adaptive degradation ──

    /// Report frame timing for adaptive quality decisions.
    void report_frame_timing(const FrameTimingSnapshot& timing);

    /// Check if the engine should degrade quality based on recent timings.
    [[nodiscard]] auto should_degrade() const noexcept -> bool;

    /// Get the recommended quality tier based on current conditions.
    [[nodiscard]] auto recommended_tier() const noexcept -> QualityTier;

    /// Check if a specific pass type is safe to run given current modes.
    [[nodiscard]] auto is_pass_safe(FxPassType pass_type) const noexcept -> bool;

    /// Number of frame budget violations in the recent window.
    [[nodiscard]] auto violation_count() const noexcept -> uint32_t;

    /// Reset all safety modes to defaults.
    void reset_to_defaults() noexcept;

    /// Number of active safety modes (reduced motion, low power, text safety, high contrast).
    [[nodiscard]] auto active_safety_count() const noexcept -> int
    {
        int count = 0;
        if (reduced_motion_)
        {
            ++count;
        }
        if (low_power_mode_)
        {
            ++count;
        }
        if (text_safety_mode_)
        {
            ++count;
        }
        if (high_contrast_guard_)
        {
            ++count;
        }
        return count;
    }

private:
    core::EventBus& event_bus_;

    bool master_enabled_{true};
    bool reduced_motion_{false};
    bool low_power_mode_{false};
    bool text_safety_mode_{false};
    bool high_contrast_guard_{true};
    float max_frame_time_budget_ms_{16.0F}; ///< ~60fps

    // Adaptive degradation tracking
    static constexpr uint32_t kTimingWindowSize = 30;
    uint32_t violation_count_{0};
    uint32_t frame_count_{0};
    float avg_frame_time_ms_{0.0F};
};

} // namespace markamp::rendering
