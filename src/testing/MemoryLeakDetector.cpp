// ============================================================================
// File: src/testing/MemoryLeakDetector.cpp
// Phase 50: UI Integration Testing Harness — Memory leak detection
// ============================================================================
#include "MemoryLeakDetector.h"

#include <algorithm>
#include <cmath>

namespace markamp::testing
{

void MemoryLeakDetector::record_before(const std::string& component_name, size_t memory_bytes)
{
    pending_.push_back({.component_name = component_name, .memory_before = memory_bytes});
}

void MemoryLeakDetector::record_after(const std::string& component_name,
                                      size_t memory_bytes,
                                      int cycles)
{
    auto it = std::ranges::find_if(
        pending_, [&](const auto& p) { return p.component_name == component_name; });

    if (it == pending_.end())
    {
        return;
    }

    double delta = static_cast<double>(memory_bytes) - static_cast<double>(it->memory_before);
    double delta_mb = delta / (1024.0 * 1024.0);

    results_.push_back({.component_name = component_name,
                        .create_destroy_cycles = cycles,
                        .memory_before_bytes = it->memory_before,
                        .memory_after_bytes = memory_bytes,
                        .delta_mb = delta_mb,
                        .leaked = std::abs(delta_mb) > MemoryLeakTestResult::kLeakThresholdMB});

    pending_.erase(it);
}

auto MemoryLeakDetector::leak_count() const -> int
{
    return static_cast<int>(
        std::ranges::count_if(results_, [](const auto& r) { return r.leaked; }));
}

} // namespace markamp::testing
