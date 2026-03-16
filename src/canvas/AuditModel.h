#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Audit gate status.
enum class GateStatus : uint8_t
{
    kNotRun,
    kPassed,
    kFailed,
};

/// An audit gate check.
struct AuditGate
{
    std::string gate_id;
    std::string name;
    std::string category; ///< "fundamentals", "performance", "accessibility", "platform"
    GateStatus status{GateStatus::kNotRun};
    std::string notes;

    // ── Round 4 Batch 8 (#74-77) ────────────────────────────────

    /// (#74) Whether this gate passed.
    [[nodiscard]] auto is_passed() const noexcept -> bool
    {
        return status == GateStatus::kPassed;
    }

    /// (#75) Whether this gate failed.
    [[nodiscard]] auto is_failed() const noexcept -> bool
    {
        return status == GateStatus::kFailed;
    }

    /// (#76) Whether this gate has not been run.
    [[nodiscard]] auto is_not_run() const noexcept -> bool
    {
        return status == GateStatus::kNotRun;
    }

    /// (#77) Whether notes are attached.
    [[nodiscard]] auto has_notes() const noexcept -> bool
    {
        return !notes.empty();
    }
};

/// Testable model for Final Canvas Audit (Phase 80).
///
/// Encapsulates:
/// - Fundamentals checklist gates
/// - Cross-platform UX validation status
/// - Performance gate results
/// - Accessibility gate results
/// - Overall readiness verdict
class AuditModel
{
public:
    // ── Gates ───────────────────────────────────────────────────────

    void set_gates(std::vector<AuditGate> gates);
    [[nodiscard]] auto gates() const -> const std::vector<AuditGate>&;

    void update_gate(const std::string& gate_id, GateStatus status, const std::string& notes);
    [[nodiscard]] auto gates_in_category(const std::string& category) const
        -> std::vector<AuditGate>;

    // ── Summary ─────────────────────────────────────────────────────

    [[nodiscard]] auto passed_count() const -> int;
    [[nodiscard]] auto failed_count() const -> int;
    [[nodiscard]] auto not_run_count() const -> int;
    [[nodiscard]] auto all_passed() const -> bool;
    [[nodiscard]] auto is_release_ready() const -> bool;

private:
    std::vector<AuditGate> gates_;

    // ── Round 4 Batch 8 (#78) ───────────────────────────────────

    /// (#78) Total number of gates.
    [[nodiscard]] auto gate_count() const noexcept -> size_t
    {
        return gates_.size();
    }
};

} // namespace markamp::canvas
