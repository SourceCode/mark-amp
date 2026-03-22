/// @file WorkflowSmokeRunner.h
/// @brief V20 P10-T02/T03: Workflow smoke testing and legacy retirement.
///
/// Encodes top product workflows as pass/fail checks. Tracks legacy
/// pathway retirement and validates no shadow paths remain.
#pragma once

#include "EventBus.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Smoke test result status.
enum class SmokeTestStatus
{
    kPassed,
    kFailed,
    kSkipped,
    kNotRun
};

/// A single smoke test result.
struct SmokeTestResult
{
    std::string test_id;             ///< e.g. "smoke.file.create"
    std::string description;         ///< Human-readable description
    SmokeTestStatus status{SmokeTestStatus::kNotRun};
    std::string error_message;       ///< On failure
    double duration_ms{0.0};         ///< Execution time

    [[nodiscard]] auto passed() const noexcept -> bool
    {
        return status == SmokeTestStatus::kPassed;
    }
};

/// Legacy pathway entry for retirement tracking.
struct LegacyPathway
{
    std::string pathway_id;          ///< e.g. "mainframe.untitled-path"
    std::string file_path;           ///< File containing the legacy code
    std::string description;         ///< What it does
    bool retired{false};             ///< Whether it's been removed
    std::string replacement;         ///< What replaces it
};

/// Smoke test callback type.
using SmokeTestFn = std::function<SmokeTestResult()>;

/// Manages workflow smoke tests and legacy pathway retirement.
class WorkflowSmokeRunner
{
public:
    explicit WorkflowSmokeRunner(EventBus& bus);

    /// Register a smoke test.
    void register_test(const std::string& test_id, const std::string& description,
                       SmokeTestFn test_fn);

    /// Run all registered smoke tests.
    auto run_all() -> std::vector<SmokeTestResult>;

    /// Run a specific smoke test.
    auto run_test(const std::string& test_id) -> SmokeTestResult;

    /// Get last results.
    [[nodiscard]] auto last_results() const -> const std::vector<SmokeTestResult>&
    {
        return last_results_;
    }

    /// Register a legacy pathway for retirement tracking.
    void register_legacy_pathway(const LegacyPathway& pathway);

    /// Mark a legacy pathway as retired.
    void retire_pathway(const std::string& pathway_id);

    /// All legacy pathways.
    [[nodiscard]] auto all_legacy_pathways() const -> const std::vector<LegacyPathway>&
    {
        return legacy_pathways_;
    }

    /// Unretired pathway count.
    [[nodiscard]] auto active_legacy_count() const -> int;

    /// Total registered tests.
    [[nodiscard]] auto test_count() const noexcept -> int
    {
        return static_cast<int>(tests_.size());
    }

    /// Pass rate from last run (0.0-1.0).
    [[nodiscard]] auto pass_rate() const -> double;

    // ── V24 P01-T05: Phase Readiness Checks ──

    /// Phase readiness evaluation result.
    struct PhaseReadiness
    {
        std::string phase_id;
        bool is_ready{false};
        int total_tests{0};
        int passed_tests{0};
        int failed_tests{0};
        std::vector<std::string> blocking_test_ids;

        [[nodiscard]] auto pass_ratio() const noexcept -> double
        {
            return total_tests > 0
                ? static_cast<double>(passed_tests) / static_cast<double>(total_tests)
                : 0.0;
        }

        [[nodiscard]] auto has_blockers() const noexcept -> bool
        {
            return !blocking_test_ids.empty();
        }
    };

    /// Check if a phase is ready by running all smoke tests that match the phase prefix.
    [[nodiscard]] auto check_phase_readiness(const std::string& phase_id) -> PhaseReadiness;

private:
    struct SmokeTest
    {
        std::string test_id;
        std::string description;
        SmokeTestFn test_fn;
    };

    EventBus& event_bus_;
    std::vector<SmokeTest> tests_;
    std::vector<SmokeTestResult> last_results_;
    std::vector<LegacyPathway> legacy_pathways_;
};

} // namespace markamp::core
