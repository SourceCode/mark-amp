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
};

/// An empty/error state descriptor.
struct EmptyState
{
    std::string context; ///< e.g., "no-selection", "no-results", "invalid-op"
    std::string message;
    std::string action_hint;
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
};

} // namespace markamp::canvas
