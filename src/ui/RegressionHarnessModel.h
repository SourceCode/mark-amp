#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Control state for snapshot testing.
enum class ControlState : uint8_t
{
    kDefault,
    kHover,
    kFocus,
    kPressed,
    kDisabled,
};

/// Snapshot baseline entry.
struct SnapshotBaseline
{
    std::string control_id;
    ControlState state{ControlState::kDefault};
    std::string hash; ///< Content hash of the rendered snapshot
    bool is_current{true};
};

/// Keyboard workflow step.
struct KeyboardStep
{
    std::string step_id;
    std::string description;
    std::string key_sequence; ///< e.g., "Tab", "Enter", "Ctrl+S"
    bool passed{false};
};

/// Testable model for Regression Harness & QA Automation (Phase 39).
///
/// Encapsulates:
/// - Snapshot baselines for all control states
/// - Keyboard workflow test steps
/// - Accessibility audit results
/// - CI gate status
class RegressionHarnessModel
{
public:
    // ── Snapshots ───────────────────────────────────────────────────

    void set_baselines(std::vector<SnapshotBaseline> baselines);
    [[nodiscard]] auto baselines() const -> const std::vector<SnapshotBaseline>&;

    /// Check if a control's current hash matches its baseline.
    [[nodiscard]] auto is_snapshot_current(const std::string& control_id,
                                           ControlState state,
                                           const std::string& current_hash) const -> bool;

    /// Returns all stale baselines.
    [[nodiscard]] auto stale_baselines() const -> std::vector<SnapshotBaseline>;

    // ── Keyboard workflows ──────────────────────────────────────────

    void set_keyboard_steps(std::vector<KeyboardStep> steps);
    [[nodiscard]] auto keyboard_steps() const -> const std::vector<KeyboardStep>&;
    void mark_step_passed(const std::string& step_id);
    [[nodiscard]] auto all_keyboard_steps_passed() const -> bool;
    [[nodiscard]] auto keyboard_pass_rate() const -> double;

    // ── CI gate ─────────────────────────────────────────────────────

    void set_ci_gate_passing(bool passing);
    [[nodiscard]] auto ci_gate_passing() const -> bool;

private:
    std::vector<SnapshotBaseline> baselines_;
    std::vector<KeyboardStep> keyboard_steps_;
    bool ci_gate_passing_{false};
};

} // namespace markamp::ui
