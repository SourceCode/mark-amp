/// @file test_v20_lifecycle_tracer.cpp
/// @brief V20 Phase 10 – LifecycleTracer unit tests.

#include "core/LifecycleTracer.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("LifecycleTracer: construction", "[v20][lifecycle-tracer]")
{
    EventBus bus;
    LifecycleTracer tracer(bus);
    REQUIRE(tracer.trace_count() == 0);
    REQUIRE(tracer.error_count() == 0);
}

TEST_CASE("LifecycleTracer: trace operations", "[v20][lifecycle-tracer]")
{
    EventBus bus;
    LifecycleTracer tracer(bus);

    tracer.trace("art-1", TraceEventType::kCreate, "command", "new file");
    tracer.trace("art-1", TraceEventType::kSave, "user", "first save");
    tracer.trace("art-1", TraceEventType::kDirty, "edit", "content changed");

    REQUIRE(tracer.trace_count() == 3);
    REQUIRE(tracer.error_count() == 0);
}

TEST_CASE("LifecycleTracer: traces for artifact", "[v20][lifecycle-tracer]")
{
    EventBus bus;
    LifecycleTracer tracer(bus);

    tracer.trace("art-1", TraceEventType::kCreate, "shell");
    tracer.trace("art-2", TraceEventType::kCreate, "shell");
    tracer.trace("art-1", TraceEventType::kSave, "user");

    auto art1_traces = tracer.traces_for("art-1");
    REQUIRE(art1_traces.size() == 2);

    auto art2_traces = tracer.traces_for("art-2");
    REQUIRE(art2_traces.size() == 1);
}

TEST_CASE("LifecycleTracer: error tracking", "[v20][lifecycle-tracer]")
{
    EventBus bus;
    LifecycleTracer tracer(bus);

    tracer.trace("art-1", TraceEventType::kSave, "user", "save OK", true);
    tracer.trace("art-1", TraceEventType::kError, "system", "disk full", false);

    REQUIRE(tracer.error_count() == 1);
    auto errors = tracer.traces_by_type(TraceEventType::kError);
    REQUIRE(errors.size() == 1);
    REQUIRE_FALSE(errors[0].success);
}

TEST_CASE("LifecycleTracer: recent traces and clear", "[v20][lifecycle-tracer]")
{
    EventBus bus;
    LifecycleTracer tracer(bus);

    for (int i = 0; i < 10; ++i)
    {
        tracer.trace("art-" + std::to_string(i), TraceEventType::kCreate, "test");
    }

    auto recent = tracer.recent_traces(3);
    REQUIRE(recent.size() == 3);

    tracer.clear();
    REQUIRE(tracer.trace_count() == 0);
    REQUIRE(tracer.error_count() == 0);
}

TEST_CASE("LifecycleTracer: event name generation", "[v20][lifecycle-tracer]")
{
    LifecycleTraceEntry entry;
    entry.event_type = TraceEventType::kCreate;
    REQUIRE(entry.event_name() == "create");

    entry.event_type = TraceEventType::kAutosave;
    REQUIRE(entry.event_name() == "autosave");

    entry.event_type = TraceEventType::kDelete;
    REQUIRE(entry.event_name() == "delete");
}
