/// PanicPolicy.h — V7 Phase 21: Thread panic escalation policy
///
/// Defines escalation actions when a subsystem encounters a panic-level fault.
/// The policy determines whether to log, restart the thread, isolate the
/// subsystem, or crash the process based on severity and fault history.

#pragma once

#include "Result.h"
#include "StructuredLogger.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Panic Action
// ══════════════════════════════════════════════════════════════════════════════

/// Actions to take when a panic is detected.
enum class PanicAction : uint8_t
{
    Log = 0,              // Log the error and continue
    RestartThread = 1,    // Attempt to restart the faulted thread
    IsolateSubsystem = 2, // Disable the faulted subsystem
    CrashProcess = 3,     // Terminate the process with crash dump
};

// ══════════════════════════════════════════════════════════════════════════════
// Panic Severity
// ══════════════════════════════════════════════════════════════════════════════

/// Severity levels for classifying panics.
enum class PanicSeverity : uint8_t
{
    Low = 0,      // Recoverable, log and continue
    Medium = 1,   // May need thread restart
    High = 2,     // Subsystem should be isolated
    Critical = 3, // Process cannot continue safely
};

// ══════════════════════════════════════════════════════════════════════════════
// Panic Event
// ══════════════════════════════════════════════════════════════════════════════

/// Describes a panic event for policy evaluation.
struct PanicEvent
{
    SubsystemId subsystem{SubsystemId::Core};
    PanicSeverity severity{PanicSeverity::Low};
    std::string message;
    std::string source_file;
    int source_line{0};
};

// ══════════════════════════════════════════════════════════════════════════════
// Panic Policy
// ══════════════════════════════════════════════════════════════════════════════

/// Determines the appropriate action when a subsystem panics.
class PanicPolicy
{
public:
    PanicPolicy() = default;

    /// Evaluate the appropriate action based on the panic event and history.
    [[nodiscard]] auto escalate(const PanicEvent& event) -> PanicAction;

    /// Register a handler for a specific action.
    void register_handler(PanicAction action, std::function<void(const PanicEvent&)> handler);

    /// Execute the escalation: evaluate action, call handler, log.
    void execute(const PanicEvent& event);

    /// Get the total number of panics for a subsystem.
    [[nodiscard]] auto panic_count(SubsystemId subsystem) const -> size_t;

    /// Reset panic history for a subsystem.
    void reset(SubsystemId subsystem);

    /// Reset all panic history.
    void reset_all();

    /// Configure the maximum panics before escalation.
    void set_max_panics_before_escalation(size_t count)
    {
        max_panics_ = count;
    }

    /// Configure the time window for counting panics.
    void set_escalation_window(std::chrono::seconds window)
    {
        escalation_window_ = window;
    }

private:
    struct SubsystemHistory
    {
        size_t total_panics{0};
        std::chrono::steady_clock::time_point first_panic;
        std::chrono::steady_clock::time_point last_panic;
        size_t panics_in_window{0};
    };

    size_t max_panics_{5};
    std::chrono::seconds escalation_window_{300}; // 5 minutes
    mutable std::mutex mutex_;
    std::unordered_map<int, SubsystemHistory> history_;
    std::unordered_map<int, std::function<void(const PanicEvent&)>> handlers_;
    StructuredLogger logger_{"PanicPolicy"};
};

} // namespace markamp::core
