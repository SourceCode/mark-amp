/// PanicPolicy.cpp — V7 Phase 21: Thread panic escalation policy

#include "PanicPolicy.h"

namespace markamp::core
{

auto PanicPolicy::escalate(const PanicEvent& event) -> PanicAction
{
    std::lock_guard lock(mutex_);

    auto key = static_cast<int>(event.subsystem);
    auto& history = history_[key];
    auto now = std::chrono::steady_clock::now();

    // Track panic
    if (history.total_panics == 0)
    {
        history.first_panic = now;
    }
    history.last_panic = now;
    history.total_panics++;

    // Count panics within escalation window
    auto window_start = now - escalation_window_;
    if (history.first_panic >= window_start)
    {
        history.panics_in_window = history.total_panics;
    }
    else
    {
        history.panics_in_window++;
    }

    // Critical severity always crashes
    if (event.severity == PanicSeverity::Critical)
    {
        return PanicAction::CrashProcess;
    }

    // High severity isolates immediately
    if (event.severity == PanicSeverity::High)
    {
        return PanicAction::IsolateSubsystem;
    }

    // Escalate based on repeated panics
    if (history.panics_in_window >= max_panics_)
    {
        return PanicAction::IsolateSubsystem;
    }

    if (event.severity == PanicSeverity::Medium || history.panics_in_window >= 2)
    {
        return PanicAction::RestartThread;
    }

    return PanicAction::Log;
}

void PanicPolicy::register_handler(PanicAction action,
                                   std::function<void(const PanicEvent&)> handler)
{
    std::lock_guard lock(mutex_);
    handlers_[static_cast<int>(action)] = std::move(handler);
}

void PanicPolicy::execute(const PanicEvent& event)
{
    auto action = escalate(event);

    // Log regardless of action
    logger_.error("Panic in " + std::string(subsystem_name(event.subsystem)) + ": " + event.message,
                  {{"severity", std::to_string(static_cast<int>(event.severity))},
                   {"action", std::to_string(static_cast<int>(action))},
                   {"source", event.source_file + ":" + std::to_string(event.source_line)}});

    // Execute registered handler
    std::lock_guard lock(mutex_);
    auto handler_it = handlers_.find(static_cast<int>(action));
    if (handler_it != handlers_.end())
    {
        handler_it->second(event);
    }
}

auto PanicPolicy::panic_count(SubsystemId subsystem) const -> size_t
{
    std::lock_guard lock(mutex_);
    auto key = static_cast<int>(subsystem);
    auto history_it = history_.find(key);
    if (history_it == history_.end())
    {
        return 0;
    }
    return history_it->second.total_panics;
}

void PanicPolicy::reset(SubsystemId subsystem)
{
    std::lock_guard lock(mutex_);
    history_.erase(static_cast<int>(subsystem));
}

void PanicPolicy::reset_all()
{
    std::lock_guard lock(mutex_);
    history_.clear();
}

} // namespace markamp::core
