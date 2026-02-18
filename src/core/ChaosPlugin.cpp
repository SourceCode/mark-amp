/// ChaosPlugin.cpp — V7 Phase 40: Chaos plugin implementation

#include "ChaosPlugin.h"

#include <stdexcept>

namespace markamp::core
{

auto behavior_name(ChaosBehavior behavior) -> std::string
{
    switch (behavior)
    {
        case ChaosBehavior::kNone:
            return "None";
        case ChaosBehavior::kThrowOnActivate:
            return "Throw on Activate";
        case ChaosBehavior::kThrowOnDeactivate:
            return "Throw on Deactivate";
        case ChaosBehavior::kThrowOnEvent:
            return "Throw on Event";
        case ChaosBehavior::kBusyLoop:
            return "Busy Loop";
        case ChaosBehavior::kMemoryPressure:
            return "Memory Pressure";
        case ChaosBehavior::kEventFlood:
            return "Event Flood";
    }
    return "Unknown";
}

auto ChaosPlugin::activate() -> Result<void>
{
    activation_attempts_++;

    if (behavior_ == ChaosBehavior::kThrowOnActivate)
    {
        return std::unexpected(make_error(ErrorCode::PluginError,
                                          SubsystemId::ExtensionHost,
                                          "ChaosPlugin: deliberate throw on activate"));
    }

    return {};
}

auto ChaosPlugin::deactivate() -> Result<void>
{
    if (behavior_ == ChaosBehavior::kThrowOnDeactivate)
    {
        return std::unexpected(make_error(ErrorCode::PluginError,
                                          SubsystemId::ExtensionHost,
                                          "ChaosPlugin: deliberate throw on deactivate"));
    }

    return {};
}

auto ChaosPlugin::on_event(const std::string& event_name) -> Result<void>
{
    if (behavior_ == ChaosBehavior::kThrowOnEvent)
    {
        return std::unexpected(make_error(ErrorCode::PluginError,
                                          SubsystemId::ExtensionHost,
                                          "ChaosPlugin: deliberate throw on event: " + event_name));
    }

    if (behavior_ == ChaosBehavior::kEventFlood)
    {
        // Simulate flooding: count events that would be emitted
        flood_count_ += 1000;
        return {};
    }

    if (behavior_ == ChaosBehavior::kBusyLoop)
    {
        // Simulate bounded busy work (not infinite — we count iterations)
        volatile int counter = 0;
        for (int iteration = 0; iteration < 1000000; ++iteration)
        {
            counter++;
        }
        return {};
    }

    return {};
}

} // namespace markamp::core
