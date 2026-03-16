// ============================================================================
// File: src/rendering/FxAccessibility.h
// Phase 28: FX Visual Effects System — Accessibility guards
// ============================================================================
#pragma once

#include "FxPass.h"

#include <string>
#include <vector>

namespace markamp::rendering
{

/// Screen reader announcements and accessibility guards for FX effects.
/// Detects OS reduced-motion preferences and generates human-readable
/// descriptions of FX state changes.
class FxAccessibility
{
public:
    FxAccessibility() = default;

    // ── Announcements ──

    /// Generate announcement text for a preset change.
    [[nodiscard]] auto announce_preset_change(const std::string& from_preset,
                                              const std::string& to_preset) const -> std::string;

    /// Generate announcement text for a quality tier change.
    [[nodiscard]] auto announce_quality_change(QualityTier tier) const -> std::string;

    /// Generate announcement text for a safety mode toggle.
    [[nodiscard]] auto announce_safety_mode(const std::string& mode_name, bool enabled) const
        -> std::string;

    /// Generate announcement text for FX master toggle.
    [[nodiscard]] auto announce_master_toggle(bool enabled) const -> std::string;

    // ── OS preference detection ──

    /// Check if the OS prefers reduced motion (macOS:
    /// NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification).
    [[nodiscard]] auto prefers_reduced_motion() const -> bool;

    /// Check if the OS is running in high-contrast mode.
    [[nodiscard]] auto prefers_high_contrast() const -> bool;

    // ── Summary ──

    /// Get a human-readable summary of the current FX accessibility state.
    [[nodiscard]] auto get_accessibility_summary(bool fx_enabled,
                                                 bool reduced_motion,
                                                 bool low_power,
                                                 bool text_safety,
                                                 QualityTier tier) const -> std::string;

    /// Get recommendations based on the current accessibility state.
    [[nodiscard]] auto get_recommendations(bool fx_enabled, bool reduced_motion) const
        -> std::vector<std::string>;

    // ── Announcement history ──

    /// Record an announcement (stored for testing/auditing).
    auto record_announcement(const std::string& message) -> void;

    /// Get all recorded announcements.
    [[nodiscard]] auto announcement_history() const -> const std::vector<std::string>&;

    /// Clear announcement history.
    auto clear_history() -> void;

    /// Number of announcements made.
    [[nodiscard]] auto announcement_count() const noexcept -> std::size_t;

    /// Whether any announcements have been recorded.
    [[nodiscard]] auto has_history() const noexcept -> bool
    {
        return !history_.empty();
    }

    /// Whether the history is at maximum capacity.
    [[nodiscard]] auto is_at_capacity() const noexcept -> bool
    {
        return history_.size() >= kMaxHistory;
    }

private:
    static constexpr std::size_t kMaxHistory = 100;
    std::vector<std::string> history_;
};

} // namespace markamp::rendering
