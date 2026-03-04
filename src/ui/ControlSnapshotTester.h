#pragma once

/**
 * @file ControlSnapshotTester.h
 * @brief Phase 39 Task 2: Snapshot tests for custom-drawn controls.
 *
 * Captures rendered state snapshots (default/hover/focus/pressed/disabled),
 * compares against hash-based baselines, and manages baseline updates.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Visual state for snapshot capture.
enum class ControlVisualState : uint8_t
{
    kDefault,
    kHover,
    kFocus,
    kPressed,
    kDisabled,
};

/// A snapshot of a control's rendered state.
struct ControlSnapshot
{
    std::string control_id;
    ControlVisualState state{ControlVisualState::kDefault};
    std::string hash; ///< Hash of rendered output
    int width{0};
    int height{0};

    /// Get state as string.
    [[nodiscard]] auto state_name() const -> std::string;
};

/// Baseline entry for comparison.
struct SnapshotBaseline
{
    std::string control_id;
    ControlVisualState state{ControlVisualState::kDefault};
    std::string expected_hash;
};

/**
 * @brief Captures and compares control state snapshots.
 */
class ControlSnapshotTester
{
public:
    ControlSnapshotTester() = default;

    // ── Baseline management ────────────────────────────────────────

    /// Register a baseline.
    void register_baseline(const SnapshotBaseline& baseline);

    /// Get baseline count.
    [[nodiscard]] auto baseline_count() const -> int;

    /// Find a baseline for a control+state.
    [[nodiscard]] auto find_baseline(const std::string& control_id, ControlVisualState state) const
        -> const SnapshotBaseline*;

    // ── Snapshot comparison ────────────────────────────────────────

    /// Compare a snapshot against its baseline.
    [[nodiscard]] auto compare(const ControlSnapshot& snapshot) const -> bool;

    /// Run all baselines against current snapshots. Returns pass count.
    [[nodiscard]] auto validate_all(const std::vector<ControlSnapshot>& snapshots) const -> int;

    /// Get failed comparisons.
    [[nodiscard]] auto failures(const std::vector<ControlSnapshot>& snapshots) const
        -> std::vector<ControlSnapshot>;

    // ── Update ─────────────────────────────────────────────────────

    /// Update a baseline hash from a snapshot.
    void update_baseline(const std::string& control_id,
                         ControlVisualState state,
                         const std::string& new_hash);

    /// Clear all baselines.
    void clear();

private:
    std::vector<SnapshotBaseline> baselines_;
};

} // namespace markamp::ui
