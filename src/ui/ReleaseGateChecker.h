#pragma once

/**
 * @file ReleaseGateChecker.h
 * @brief Phase 40 Task 5: Release gates with go/no-go enforcement.
 *
 * Defines release gates (interaction, performance, accessibility, regression),
 * tracks gate status, and generates go/no-go summary.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Gate status.
enum class GateStatus : uint8_t
{
    kPass,
    kFail,
    kBlocked,
    kPending,
};

/// A release gate definition.
struct ReleaseGate
{
    std::string gate_id;
    std::string name;
    std::string description;
    GateStatus status{GateStatus::kPending};
    std::string notes;

    /// Get status as string.
    [[nodiscard]] auto status_name() const -> std::string;

    /// Check if gate is blocking release.
    [[nodiscard]] auto is_blocking() const -> bool;
};

/**
 * @brief Release gate enforcement and go/no-go checking.
 */
class ReleaseGateChecker
{
public:
    ReleaseGateChecker() = default;

    // ── Gate management ────────────────────────────────────────────

    /// Add a release gate.
    void add_gate(const ReleaseGate& gate);

    /// Update a gate's status.
    void set_status(const std::string& gate_id, GateStatus status, const std::string& notes = "");

    /// Get a gate by ID.
    [[nodiscard]] auto find_gate(const std::string& gate_id) const -> const ReleaseGate*;

    /// Get all gates.
    [[nodiscard]] auto all_gates() const -> const std::vector<ReleaseGate>&;

    /// Get gate count.
    [[nodiscard]] auto gate_count() const -> int;

    // ── Go/No-Go ───────────────────────────────────────────────────

    /// Check if all gates pass (go decision).
    [[nodiscard]] auto is_go() const -> bool;

    /// Get count of blocking gates.
    [[nodiscard]] auto blocking_count() const -> int;

    /// Get blocking gate IDs.
    [[nodiscard]] auto blocking_gates() const -> std::vector<std::string>;

    /// Get pass count.
    [[nodiscard]] auto pass_count() const -> int;

    /// Get pending count.
    [[nodiscard]] auto pending_count() const -> int;

    // ── Standard gates ─────────────────────────────────────────────

    /// Register standard release gates.
    void register_standard_gates();

private:
    std::vector<ReleaseGate> gates_;
};

} // namespace markamp::ui
