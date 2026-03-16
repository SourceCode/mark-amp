#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Microinteraction type.
enum class MicroInteraction : uint8_t
{
    kHover,
    kSelect,
    kSnap,
    kToolSwitch,
    kDrop,
};

/// An easing preset.
struct EasingPreset
{
    std::string name;
    double duration_ms{200.0};
    std::string curve; ///< e.g., "ease-out", "ease-in-out", "spring"

    // ── Round 6 Batch 8 (#71-73) ────────────────────────────────

    /// (#71) Whether a name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !name.empty();
    }

    /// (#72) Whether a curve is set.
    [[nodiscard]] auto has_curve() const noexcept -> bool
    {
        return !curve.empty();
    }

    /// (#73) Whether the easing is fast (<= 100ms).
    [[nodiscard]] auto is_fast() const noexcept -> bool
    {
        return duration_ms <= 100.0;
    }
};

/// An empty/error state descriptor.
struct EmptyState
{
    std::string context; ///< e.g., "no-selection", "no-results", "invalid-op"
    std::string message;
    std::string action_hint;

    // ── Round 6 Batch 8 (#74-76) ────────────────────────────────

    /// (#74) Whether a message is set.
    [[nodiscard]] auto has_message() const noexcept -> bool
    {
        return !message.empty();
    }

    /// (#75) Whether a context is set.
    [[nodiscard]] auto has_context() const noexcept -> bool
    {
        return !context.empty();
    }

    /// (#76) Whether an action hint is set.
    [[nodiscard]] auto has_action_hint() const noexcept -> bool
    {
        return !action_hint.empty();
    }
};

/// Testable model for Canvas Polish / Microinteractions (Phase 79).
///
/// Encapsulates:
/// - Microinteraction enablement per type
/// - Easing preset registry
/// - Empty/error state descriptors
/// - Label consistency tracking
class PolishModel
{
public:
    // ── Microinteractions ───────────────────────────────────────────

    void enable_interaction(MicroInteraction interaction);
    void disable_interaction(MicroInteraction interaction);
    [[nodiscard]] auto is_enabled(MicroInteraction interaction) const -> bool;

    // ── Easing ──────────────────────────────────────────────────────

    void set_easings(std::vector<EasingPreset> easings);
    [[nodiscard]] auto easings() const -> const std::vector<EasingPreset>&;
    [[nodiscard]] auto easing_for(const std::string& name) const -> EasingPreset;

    // ── Empty states ────────────────────────────────────────────────

    void set_empty_states(std::vector<EmptyState> states);
    [[nodiscard]] auto empty_states() const -> const std::vector<EmptyState>&;

    // ── Label audit ─────────────────────────────────────────────────

    void set_label_issues(int count);
    [[nodiscard]] auto label_issues() const -> int;
    [[nodiscard]] auto labels_consistent() const -> bool;

private:
    std::vector<MicroInteraction> enabled_;
    std::vector<EasingPreset> easings_;
    std::vector<EmptyState> empty_states_;
    int label_issues_{0};

    // ── Round 6 Batch 8 (#77-80) ────────────────────────────────

    /// (#77) Whether easing presets exist.
    [[nodiscard]] auto has_easings() const noexcept -> bool
    {
        return !easings_.empty();
    }

    /// (#78) Number of easing presets.
    [[nodiscard]] auto easing_count() const noexcept -> size_t
    {
        return easings_.size();
    }

    /// (#79) Whether empty state descriptors exist.
    [[nodiscard]] auto has_empty_states() const noexcept -> bool
    {
        return !empty_states_.empty();
    }

    /// (#80) Whether label issues exist.
    [[nodiscard]] auto has_label_issues() const noexcept -> bool
    {
        return label_issues_ > 0;
    }
};

} // namespace markamp::canvas
