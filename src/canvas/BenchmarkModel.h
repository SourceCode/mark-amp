#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Benchmark result entry.
struct BenchmarkResult
{
    std::string name;
    double elapsed_ms{0.0};
    double threshold_ms{0.0};
    bool passed{true};

    // ── Round 4 Batch 9 (#81-83) ────────────────────────────────

    /// (#81) Whether this benchmark failed.
    [[nodiscard]] auto is_failed() const noexcept -> bool
    {
        return !passed;
    }

    /// (#82) Whether a benchmark name is set.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !name.empty();
    }

    /// (#83) How much the elapsed time exceeds the threshold.
    [[nodiscard]] auto overshoot() const noexcept -> double
    {
        return elapsed_ms - threshold_ms;
    }
};

/// Scenario fixture description.
struct ScenarioFixture
{
    std::string fixture_id;
    std::string description;
    int object_count{0};
    std::string category; ///< "drawing", "diagram", "media", "dense"

    // ── Round 4 Batch 9 (#84-85) ────────────────────────────────

    /// (#84) Whether a description is provided.
    [[nodiscard]] auto has_description() const noexcept -> bool
    {
        return !description.empty();
    }

    /// (#85) Whether a category is set.
    [[nodiscard]] auto has_category() const noexcept -> bool
    {
        return !category.empty();
    }
};

/// Testable model for Canvas Test Automation / Benchmarks (Phase 77).
///
/// Encapsulates:
/// - Scenario fixture registry
/// - Benchmark result tracking with pass/fail thresholds
/// - Interaction regression test result tracking
/// - CI gate status
class BenchmarkModel
{
public:
    // ── Fixtures ────────────────────────────────────────────────────

    void set_fixtures(std::vector<ScenarioFixture> fixtures);
    [[nodiscard]] auto fixtures() const -> const std::vector<ScenarioFixture>&;
    [[nodiscard]] auto fixtures_in_category(const std::string& category) const
        -> std::vector<ScenarioFixture>;

    // ── Benchmarks ──────────────────────────────────────────────────

    void add_result(BenchmarkResult result);
    [[nodiscard]] auto results() const -> const std::vector<BenchmarkResult>&;
    [[nodiscard]] auto all_passed() const -> bool;
    [[nodiscard]] auto failed_count() const -> int;

    // ── CI gate ─────────────────────────────────────────────────────

    void set_gate_status(bool passed);
    [[nodiscard]] auto gate_passed() const -> bool;

private:
    std::vector<ScenarioFixture> fixtures_;
    std::vector<BenchmarkResult> results_;
    bool gate_passed_{false};

    // ── Round 4 Batch 9 (#86-87) ────────────────────────────────

    /// (#86) Number of benchmark results.
    [[nodiscard]] auto result_count() const noexcept -> size_t
    {
        return results_.size();
    }

    /// (#87) Number of scenario fixtures.
    [[nodiscard]] auto fixture_count() const noexcept -> size_t
    {
        return fixtures_.size();
    }
};

} // namespace markamp::canvas
