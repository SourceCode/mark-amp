#include "BenchmarkModel.h"

#include <algorithm>

namespace markamp::canvas
{

void BenchmarkModel::set_fixtures(std::vector<ScenarioFixture> fixtures)
{
    fixtures_ = std::move(fixtures);
}
auto BenchmarkModel::fixtures() const -> const std::vector<ScenarioFixture>&
{
    return fixtures_;
}

auto BenchmarkModel::fixtures_in_category(const std::string& category) const
    -> std::vector<ScenarioFixture>
{
    std::vector<ScenarioFixture> result;
    for (const auto& fixture : fixtures_)
    {
        if (fixture.category == category)
        {
            result.push_back(fixture);
        }
    }
    return result;
}

void BenchmarkModel::add_result(BenchmarkResult result)
{
    result.passed = result.elapsed_ms <= result.threshold_ms;
    results_.push_back(std::move(result));
}

auto BenchmarkModel::results() const -> const std::vector<BenchmarkResult>&
{
    return results_;
}

auto BenchmarkModel::all_passed() const -> bool
{
    return std::all_of(results_.begin(),
                       results_.end(),
                       [](const BenchmarkResult& bench_result) { return bench_result.passed; });
}

auto BenchmarkModel::failed_count() const -> int
{
    return static_cast<int>(std::count_if(results_.begin(),
                                          results_.end(),
                                          [](const BenchmarkResult& bench_result)
                                          { return !bench_result.passed; }));
}

void BenchmarkModel::set_gate_status(bool passed)
{
    gate_passed_ = passed;
}
auto BenchmarkModel::gate_passed() const -> bool
{
    return gate_passed_;
}

} // namespace markamp::canvas
