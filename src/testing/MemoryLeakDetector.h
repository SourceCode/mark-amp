// ============================================================================
// File: src/testing/MemoryLeakDetector.h
// Phase 50: UI Integration Testing Harness — Memory leak detection
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::testing
{

/// Result of a memory leak test cycle.
struct MemoryLeakTestResult
{
    std::string component_name;
    int create_destroy_cycles{0};
    size_t memory_before_bytes{0};
    size_t memory_after_bytes{0};
    double delta_mb{0.0};
    bool leaked{false};

    static constexpr double kLeakThresholdMB = 1.0;
};

/// Memory leak detector using create/destroy cycles.
class MemoryLeakDetector
{
public:
    MemoryLeakDetector() = default;

    /// Record memory before test.
    void record_before(const std::string& component_name, size_t memory_bytes);

    /// Record memory after test cycles.
    void record_after(const std::string& component_name, size_t memory_bytes, int cycles);

    /// Get all results.
    [[nodiscard]] auto results() const -> const std::vector<MemoryLeakTestResult>&
    {
        return results_;
    }

    /// Get leak count (results exceeding threshold).
    [[nodiscard]] auto leak_count() const -> int;

    /// Whether any leaks were detected.
    [[nodiscard]] auto has_leaks() const -> bool
    {
        return leak_count() > 0;
    }

private:
    struct PendingTest
    {
        std::string component_name;
        size_t memory_before{0};
    };
    std::vector<PendingTest> pending_;
    std::vector<MemoryLeakTestResult> results_;
};

} // namespace markamp::testing
