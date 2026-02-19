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
};

/// Scenario fixture description.
struct ScenarioFixture
{
    std::string fixture_id;
    std::string description;
    int object_count{0};
    std::string category; ///< "drawing", "diagram", "media", "dense"
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
};

} // namespace markamp::canvas
