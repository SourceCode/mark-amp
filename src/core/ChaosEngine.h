/// ChaosEngine.h — Phase 37: Chaos Testing Framework
///
/// Provides controlled failure injection for testing resilience.
/// Supports random allocation failures, event drops, and delay injection.
///
/// Pattern implemented: #33 Chaos testing

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Types of chaos that can be injected.
enum class ChaosType : uint8_t
{
    AllocationFailure, // Simulate malloc returning nullptr
    EventDrop,         // Silently drop events
    Delay,             // Inject random delays
    ExceptionThrow,    // Throw unexpected exceptions
};

/// Configuration for a chaos injection rule.
struct ChaosRule
{
    ChaosType type{ChaosType::Delay};
    double probability{0.0}; // 0.0 to 1.0
    std::string target;      // subsystem name or "*" for all
    int64_t delay_ms{0};     // for Delay type
    bool enabled{true};
};

/// Result of a chaos check.
struct ChaosCheckResult
{
    bool should_inject{false};
    ChaosType type{ChaosType::Delay};
    int64_t param{0};
};

/// Chaos engine for controlled failure injection in tests.
///
/// Usage:
///   ChaosEngine engine;
///   engine.add_rule({ChaosType::Delay, 0.1, "render", 5, true});
///   engine.enable();
///   if (auto result = engine.check("render"); result.should_inject) {
///       std::this_thread::sleep_for(std::chrono::milliseconds(result.param));
///   }
class ChaosEngine
{
public:
    ChaosEngine()
        : rng_(std::random_device{}())
    {
    }

    /// Add a chaos injection rule.
    void add_rule(ChaosRule rule)
    {
        rules_.push_back(std::move(rule));
    }

    /// Enable chaos injection globally.
    void enable()
    {
        enabled_ = true;
    }

    /// Disable chaos injection globally.
    void disable()
    {
        enabled_ = false;
    }

    /// Whether chaos is globally enabled.
    [[nodiscard]] auto is_enabled() const noexcept -> bool
    {
        return enabled_;
    }

    /// Check if chaos should be injected for a given subsystem.
    [[nodiscard]] auto check(std::string_view subsystem) -> ChaosCheckResult
    {
        ChaosCheckResult result;

        if (!enabled_)
        {
            return result;
        }

        for (const auto& rule : rules_)
        {
            if (!rule.enabled)
            {
                continue;
            }
            if (rule.target != "*" && rule.target != subsystem)
            {
                continue;
            }

            std::uniform_real_distribution<double> dist(0.0, 1.0);
            if (dist(rng_) < rule.probability)
            {
                result.should_inject = true;
                result.type = rule.type;
                result.param = rule.delay_ms;
                ++injection_count_;

                auto key = std::string(subsystem);
                subsystem_injections_[key]++;

                return result;
            }
        }

        return result;
    }

    /// Apply a chaos check — if injection triggered, execute the fault.
    void apply(std::string_view subsystem)
    {
        auto result = check(subsystem);
        if (!result.should_inject)
        {
            return;
        }

        switch (result.type)
        {
            case ChaosType::Delay:
                std::this_thread::sleep_for(std::chrono::milliseconds(result.param));
                break;
            case ChaosType::ExceptionThrow:
                throw std::runtime_error("Chaos: injected exception in " + std::string(subsystem));
            case ChaosType::AllocationFailure:
            case ChaosType::EventDrop:
                // These are checked by the caller via check()
                break;
        }
    }

    /// Total injection count.
    [[nodiscard]] auto injection_count() const noexcept -> uint64_t
    {
        return injection_count_;
    }

    /// Injections per subsystem.
    [[nodiscard]] auto subsystem_injections() const
        -> const std::unordered_map<std::string, uint64_t>&
    {
        return subsystem_injections_;
    }

    /// Number of rules registered.
    [[nodiscard]] auto rule_count() const noexcept -> std::size_t
    {
        return rules_.size();
    }

    /// Reset all counters.
    void reset_counters()
    {
        injection_count_ = 0;
        subsystem_injections_.clear();
    }

private:
    std::vector<ChaosRule> rules_;
    std::mt19937 rng_;
    bool enabled_{false};
    uint64_t injection_count_{0};
    std::unordered_map<std::string, uint64_t> subsystem_injections_;
};

} // namespace markamp::core
