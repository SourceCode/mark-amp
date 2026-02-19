#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Audit check status.
enum class AuditStatus : uint8_t
{
    kPass,
    kFail,
    kSkipped,
};

/// A single audit check result.
struct AuditCheck
{
    std::string check_id;
    std::string category; ///< e.g., "interaction", "style", "accessibility", "performance"
    std::string description;
    AuditStatus status{AuditStatus::kSkipped};
    std::string notes;
};

/// Release gate definition.
struct ReleaseGate
{
    std::string gate_id;
    std::string name;
    bool is_required{true};
    bool is_passing{false};
};

/// Testable model for Final UX Audit & Release Gates (Phase 40).
///
/// Encapsulates:
/// - Full control surface audit checklist
/// - Per-category pass/fail counts
/// - Release gate evaluation
/// - Overall readiness determination
class ReleaseAuditModel
{
public:
    // ── Audit checks ────────────────────────────────────────────────

    void set_checks(std::vector<AuditCheck> checks);
    [[nodiscard]] auto checks() const -> const std::vector<AuditCheck>&;
    [[nodiscard]] auto by_category(const std::string& category) const -> std::vector<AuditCheck>;
    [[nodiscard]] auto pass_count() const -> int;
    [[nodiscard]] auto fail_count() const -> int;
    [[nodiscard]] auto skip_count() const -> int;

    // ── Release gates ───────────────────────────────────────────────

    void set_gates(std::vector<ReleaseGate> gates);
    [[nodiscard]] auto gates() const -> const std::vector<ReleaseGate>&;

    /// Returns true if all required gates are passing.
    [[nodiscard]] auto all_required_gates_passing() const -> bool;

    /// Returns names of failing required gates.
    [[nodiscard]] auto failing_gates() const -> std::vector<std::string>;

    // ── Overall readiness ───────────────────────────────────────────

    /// Ready = no audit failures AND all required gates passing.
    [[nodiscard]] auto is_release_ready() const -> bool;

private:
    std::vector<AuditCheck> checks_;
    std::vector<ReleaseGate> gates_;
};

} // namespace markamp::ui
