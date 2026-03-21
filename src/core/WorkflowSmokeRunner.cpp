/// @file WorkflowSmokeRunner.cpp
/// @brief V20 P10-T02/T03: Workflow smoke runner implementation.

#include "WorkflowSmokeRunner.h"

#include "Events.h"
#include "Logger.h"

#include <chrono>

namespace markamp::core
{

WorkflowSmokeRunner::WorkflowSmokeRunner(EventBus& bus)
    : event_bus_(bus)
{
}

void WorkflowSmokeRunner::register_test(const std::string& test_id,
                                          const std::string& description,
                                          SmokeTestFn test_fn)
{
    SmokeTest test;
    test.test_id = test_id;
    test.description = description;
    test.test_fn = std::move(test_fn);
    tests_.push_back(std::move(test));
    MARKAMP_LOG_DEBUG("Smoke test registered: {}", test_id);
}

auto WorkflowSmokeRunner::run_all() -> std::vector<SmokeTestResult>
{
    last_results_.clear();
    last_results_.reserve(tests_.size());

    for (const auto& test : tests_)
    {
        auto result = run_test(test.test_id);
        last_results_.push_back(result);
    }

    events::SmokeRunCompletedEvent evt;
    evt.total_tests = static_cast<int>(last_results_.size());
    evt.passed = 0;
    evt.failed = 0;
    for (const auto& r : last_results_)
    {
        if (r.passed()) { ++evt.passed; }
        else { ++evt.failed; }
    }
    evt.pass_rate = pass_rate();
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Smoke run complete: {}/{} passed ({:.1f}%)",
                     evt.passed, evt.total_tests, evt.pass_rate * 100.0);
    return last_results_;
}

auto WorkflowSmokeRunner::run_test(const std::string& test_id) -> SmokeTestResult
{
    for (const auto& test : tests_)
    {
        if (test.test_id == test_id)
        {
            auto start = std::chrono::steady_clock::now();
            auto result = test.test_fn();
            auto elapsed = std::chrono::steady_clock::now() - start;
            result.duration_ms = std::chrono::duration<double, std::milli>(elapsed).count();
            result.test_id = test_id;
            result.description = test.description;
            return result;
        }
    }

    SmokeTestResult not_found;
    not_found.test_id = test_id;
    not_found.status = SmokeTestStatus::kFailed;
    not_found.error_message = "Test not found: " + test_id;
    return not_found;
}

void WorkflowSmokeRunner::register_legacy_pathway(const LegacyPathway& pathway)
{
    legacy_pathways_.push_back(pathway);
    MARKAMP_LOG_DEBUG("Legacy pathway registered: {}", pathway.pathway_id);
}

void WorkflowSmokeRunner::retire_pathway(const std::string& pathway_id)
{
    for (auto& p : legacy_pathways_)
    {
        if (p.pathway_id == pathway_id)
        {
            p.retired = true;

            events::LegacyPathwayRetiredEvent evt;
            evt.pathway_id = pathway_id;
            evt.replacement = p.replacement;
            event_bus_.publish(evt);

            MARKAMP_LOG_INFO("Legacy pathway retired: {} -> {}", pathway_id, p.replacement);
            return;
        }
    }
}

auto WorkflowSmokeRunner::active_legacy_count() const -> int
{
    int count = 0;
    for (const auto& p : legacy_pathways_)
    {
        if (!p.retired)
        {
            ++count;
        }
    }
    return count;
}

auto WorkflowSmokeRunner::pass_rate() const -> double
{
    if (last_results_.empty())
    {
        return 0.0;
    }

    int passed = 0;
    for (const auto& r : last_results_)
    {
        if (r.passed())
        {
            ++passed;
        }
    }
    return static_cast<double>(passed) / static_cast<double>(last_results_.size());
}

} // namespace markamp::core
