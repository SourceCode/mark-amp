/// ChaosPlugin.h — V7 Phase 40: Deliberately misbehaving plugin for chaos testing
///
/// Throws on activate/deactivate/event, infinite loop simulation,
/// memory pressure, and event flood scenarios.

#pragma once

#include "Result.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Chaos Behaviors
// ══════════════════════════════════════════════════════════════════════════════

/// Types of misbehavior a chaos plugin can exhibit.
enum class ChaosBehavior : uint8_t
{
    kNone = 0,
    kThrowOnActivate = 1,
    kThrowOnDeactivate = 2,
    kThrowOnEvent = 3,
    kBusyLoop = 4,       // Simulate CPU-bound work
    kMemoryPressure = 5, // Allocate excessive memory
    kEventFlood = 6,     // Emit excessive events
};

/// Get display name for a chaos behavior.
[[nodiscard]] auto behavior_name(ChaosBehavior behavior) -> std::string;

// ══════════════════════════════════════════════════════════════════════════════
// Chaos Plugin
// ══════════════════════════════════════════════════════════════════════════════

/// A deliberately misbehaving plugin for testing resilience.
class ChaosPlugin
{
public:
    explicit ChaosPlugin(ChaosBehavior behavior = ChaosBehavior::kNone)
        : behavior_(behavior)
    {
    }

    /// Simulate plugin activation.
    [[nodiscard]] auto activate() -> Result<void>;

    /// Simulate plugin deactivation.
    [[nodiscard]] auto deactivate() -> Result<void>;

    /// Simulate handling an event.
    [[nodiscard]] auto on_event(const std::string& event_name) -> Result<void>;

    /// Get the configured behavior.
    [[nodiscard]] auto behavior() const noexcept -> ChaosBehavior
    {
        return behavior_;
    }

    /// Set the behavior.
    void set_behavior(ChaosBehavior behavior)
    {
        behavior_ = behavior;
    }

    /// Get the number of events that would have been flooded.
    [[nodiscard]] auto flood_count() const noexcept -> size_t
    {
        return flood_count_;
    }

    /// Get the number of activations attempted.
    [[nodiscard]] auto activation_attempts() const noexcept -> size_t
    {
        return activation_attempts_;
    }

private:
    ChaosBehavior behavior_;
    size_t flood_count_{0};
    size_t activation_attempts_{0};
};

} // namespace markamp::core
