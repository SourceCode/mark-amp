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
};

} // namespace markamp::canvas
