/// LoadTestHarness.h — Phase 39: Load Testing Framework
///
/// Provides a harness for running load test scenarios with configurable
/// concurrency, duration, and throughput measurement.
///
/// Pattern implemented: #35 Load testing

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace markamp::core
{

/// Results from a load test run.
struct LoadTestResult
{
    std::string scenario_name;
    uint64_t total_operations{0};
    int64_t duration_ms{0};
    double ops_per_second{0.0};
    uint64_t errors{0};
    double avg_latency_us{0.0};
    double p99_latency_us{0.0};
    uint32_t thread_count{0};
};

/// A load test scenario definition.
using LoadTestWork = std::function<bool()>; // returns true on success

/// Harness for running load tests with configurable concurrency.
///
/// Usage:
///   LoadTestHarness harness("EventBus stress test");
///   harness.set_threads(4);
///   harness.set_duration(std::chrono::seconds(5));
///   auto result = harness.run([]() { bus.publish(...); return true; });
class LoadTestHarness
{
public:
    explicit LoadTestHarness(std::string scenario_name)
        : scenario_name_(std::move(scenario_name))
    {
    }

    /// Set the number of concurrent threads.
    void set_threads(uint32_t count)
    {
        thread_count_ = count;
    }

    /// Set the test duration.
    void set_duration(std::chrono::milliseconds duration)
    {
        duration_ = duration;
    }

    /// Run the load test.
    [[nodiscard]] auto run(LoadTestWork work) -> LoadTestResult
    {
        std::atomic<uint64_t> total_ops{0};
        std::atomic<uint64_t> total_errors{0};
        std::atomic<int64_t> total_latency_us{0};
        std::atomic<bool> running{true};

        auto start = std::chrono::steady_clock::now();

        std::vector<std::thread> threads;
        threads.reserve(thread_count_);

        for (uint32_t thr_idx = 0; thr_idx < thread_count_; ++thr_idx)
        {
            threads.emplace_back(
                [&]()
                {
                    while (running.load(std::memory_order_acquire))
                    {
                        auto op_start = std::chrono::steady_clock::now();
                        bool success = work();
                        auto op_end = std::chrono::steady_clock::now();

                        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
                            op_end - op_start);
                        total_latency_us.fetch_add(latency.count(), std::memory_order_relaxed);

                        if (success)
                        {
                            total_ops.fetch_add(1, std::memory_order_relaxed);
                        }
                        else
                        {
                            total_errors.fetch_add(1, std::memory_order_relaxed);
                        }
                    }
                });
        }

        // Wait for test duration
        std::this_thread::sleep_for(duration_);
        running.store(false, std::memory_order_release);

        for (auto& thr : threads)
        {
            thr.join();
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        LoadTestResult result;
        result.scenario_name = scenario_name_;
        result.total_operations = total_ops.load();
        result.errors = total_errors.load();
        result.duration_ms = elapsed_ms;
        result.thread_count = thread_count_;

        auto total_ops_val = result.total_operations + result.errors;
        result.ops_per_second = (elapsed_ms > 0) ? (static_cast<double>(result.total_operations) *
                                                    1000.0 / static_cast<double>(elapsed_ms))
                                                 : 0.0;
        result.avg_latency_us = (total_ops_val > 0) ? static_cast<double>(total_latency_us.load()) /
                                                          static_cast<double>(total_ops_val)
                                                    : 0.0;

        return result;
    }

private:
    std::string scenario_name_;
    uint32_t thread_count_{1};
    std::chrono::milliseconds duration_{std::chrono::milliseconds(1000)};
};

} // namespace markamp::core
