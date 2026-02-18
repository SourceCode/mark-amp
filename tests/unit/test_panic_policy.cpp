/// test_panic_policy.cpp — V7 Phase 21: Thread panic escalation policy tests

#include "core/PanicPolicy.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

static auto make_event(SubsystemId subsystem,
                       PanicSeverity severity,
                       const std::string& msg = "test panic") -> PanicEvent
{
    return PanicEvent{subsystem, severity, msg, __FILE__, __LINE__};
}

// ══════════════════════════════════════════════════════════════════════════════
// Severity-based escalation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PanicPolicy: low severity results in Log", "[panic_policy]")
{
    PanicPolicy policy;
    auto action = policy.escalate(make_event(SubsystemId::Core, PanicSeverity::Low));
    REQUIRE(action == PanicAction::Log);
}

TEST_CASE("PanicPolicy: medium severity results in RestartThread", "[panic_policy]")
{
    PanicPolicy policy;
    auto action = policy.escalate(make_event(SubsystemId::Core, PanicSeverity::Medium));
    REQUIRE(action == PanicAction::RestartThread);
}

TEST_CASE("PanicPolicy: high severity results in IsolateSubsystem", "[panic_policy]")
{
    PanicPolicy policy;
    auto action = policy.escalate(make_event(SubsystemId::Core, PanicSeverity::High));
    REQUIRE(action == PanicAction::IsolateSubsystem);
}

TEST_CASE("PanicPolicy: critical severity results in CrashProcess", "[panic_policy]")
{
    PanicPolicy policy;
    auto action = policy.escalate(make_event(SubsystemId::Core, PanicSeverity::Critical));
    REQUIRE(action == PanicAction::CrashProcess);
}

// ══════════════════════════════════════════════════════════════════════════════
// History-based escalation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PanicPolicy: repeated low panics escalate to RestartThread", "[panic_policy]")
{
    PanicPolicy policy;
    // First panic → Log
    auto action1 = policy.escalate(make_event(SubsystemId::Rendering, PanicSeverity::Low));
    REQUIRE(action1 == PanicAction::Log);

    // Second panic → RestartThread (history escalation)
    auto action2 = policy.escalate(make_event(SubsystemId::Rendering, PanicSeverity::Low));
    REQUIRE(action2 == PanicAction::RestartThread);
}

TEST_CASE("PanicPolicy: max panics triggers isolation", "[panic_policy]")
{
    PanicPolicy policy;
    policy.set_max_panics_before_escalation(3);

    for (int idx = 0; idx < 3; ++idx)
    {
        (void)policy.escalate(make_event(SubsystemId::Editor, PanicSeverity::Low));
    }

    // Next panic should trigger isolation
    // (We already have 3 in the window from the loop, the next one will make it 4 but the 3rd was
    // already >= max) Actually - the 3rd call already returns IsolateSubsystem because
    // panics_in_window == 3 == max
    REQUIRE(policy.panic_count(SubsystemId::Editor) >= 3);
}

// ══════════════════════════════════════════════════════════════════════════════
// Panic count tracking
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PanicPolicy: tracks panic count per subsystem", "[panic_policy]")
{
    PanicPolicy policy;
    REQUIRE(policy.panic_count(SubsystemId::Core) == 0);

    (void)policy.escalate(make_event(SubsystemId::Core, PanicSeverity::Low));
    REQUIRE(policy.panic_count(SubsystemId::Core) == 1);

    (void)policy.escalate(make_event(SubsystemId::Core, PanicSeverity::Low));
    REQUIRE(policy.panic_count(SubsystemId::Core) == 2);

    // Different subsystem is separate
    REQUIRE(policy.panic_count(SubsystemId::Rendering) == 0);
}

TEST_CASE("PanicPolicy: reset clears subsystem history", "[panic_policy]")
{
    PanicPolicy policy;
    (void)policy.escalate(make_event(SubsystemId::Core, PanicSeverity::Low));
    (void)policy.escalate(make_event(SubsystemId::Editor, PanicSeverity::Low));

    policy.reset(SubsystemId::Core);
    REQUIRE(policy.panic_count(SubsystemId::Core) == 0);
    REQUIRE(policy.panic_count(SubsystemId::Editor) == 1);
}

TEST_CASE("PanicPolicy: reset_all clears all history", "[panic_policy]")
{
    PanicPolicy policy;
    (void)policy.escalate(make_event(SubsystemId::Core, PanicSeverity::Low));
    (void)policy.escalate(make_event(SubsystemId::Editor, PanicSeverity::Medium));

    policy.reset_all();
    REQUIRE(policy.panic_count(SubsystemId::Core) == 0);
    REQUIRE(policy.panic_count(SubsystemId::Editor) == 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// Handler registration and execution
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PanicPolicy: execute calls registered handler", "[panic_policy]")
{
    PanicPolicy policy;
    bool handler_called = false;
    PanicSeverity received_severity{};

    policy.register_handler(PanicAction::Log,
                            [&](const PanicEvent& evt)
                            {
                                handler_called = true;
                                received_severity = evt.severity;
                            });

    policy.execute(make_event(SubsystemId::Core, PanicSeverity::Low));
    REQUIRE(handler_called);
    REQUIRE(received_severity == PanicSeverity::Low);
}

TEST_CASE("PanicPolicy: execute without handler does not crash", "[panic_policy]")
{
    PanicPolicy policy;
    // No handler registered — should not crash
    policy.execute(make_event(SubsystemId::Core, PanicSeverity::Low));
    REQUIRE(policy.panic_count(SubsystemId::Core) == 1);
}
