#pragma once

// V11 Phase 50: Final Node Editor Validation And Rollout Program
// Rollout controller: staged rollout with feature gates and health signals.

#include "NodeEditorTypes.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Strong-typed stage identifier.
struct StageId
{
    uint64_t value{0};

    constexpr StageId() = default;
    constexpr explicit StageId(uint64_t v_arg)
        : value(v_arg)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }
    constexpr auto operator==(const StageId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const StageId&) const noexcept = default;
};

/// Rollout phase.
enum class RolloutPhase : uint8_t
{
    kCanary,
    kBeta,
    kGA
};

/// A rollout stage with features and health threshold.
struct RolloutStage
{
    StageId stage_id;
    std::string name;
    RolloutPhase phase{RolloutPhase::kCanary};
    std::vector<std::string> enabled_features;
    float health_threshold{0.9F};
};

/// A health signal reported by the system.
struct HealthSignal
{
    std::string name;
    float value{1.0F};
    std::chrono::steady_clock::time_point timestamp;
};

/// Result of running the validation suite.
struct ValidationReport
{
    std::size_t total_checks{0};
    std::size_t passed{0};
    std::size_t failed{0};
    std::vector<std::string> details;
};

/// Rollout controller managing staged rollout with health observability.
class NodeRolloutController
{
public:
    NodeRolloutController() = default;

    // --- Stage management ---
    auto add_stage(RolloutStage stage) -> StageId;
    auto remove_stage(StageId stage_id) -> bool;
    [[nodiscard]] auto current_stage() const -> const RolloutStage*;
    auto advance_stage() -> bool;
    auto rollback_stage() -> bool;
    [[nodiscard]] auto stage_count() const -> std::size_t;

    // --- Feature gates ---
    [[nodiscard]] auto is_feature_enabled(const std::string& feature_name) const -> bool;
    void enable_feature(const std::string& feature_name);
    void disable_feature(const std::string& feature_name);
    [[nodiscard]] auto enabled_features() const -> std::vector<std::string>;

    // --- Health ---
    void report_health(const std::string& signal_name, float value);
    [[nodiscard]] auto health_score() const -> float;
    [[nodiscard]] auto health_signals() const -> const std::vector<HealthSignal>&;
    [[nodiscard]] auto is_healthy() const -> bool;

    // --- Validation ---
    [[nodiscard]] auto run_validation_suite() const -> ValidationReport;

    // --- Bulk ---
    void clear();

private:
    std::vector<RolloutStage> stages_;
    std::size_t current_stage_index_{0};
    uint64_t next_stage_id_{1};
    std::vector<std::string> extra_enabled_features_;
    std::vector<HealthSignal> health_signals_;
};

} // namespace markamp::node_editor
