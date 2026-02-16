/// test_profiler_v2.cpp — Phase 16 Profiler data structure tests
///
/// Validates ProfileId-based recording, RingBuffer correctness,
/// and backward compatibility of string-based slow path.

#include "core/Profiler.h"
#include "core/ProfilerIds.h"
#include "core/RingBuffer.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE("RingBuffer: push and read", "[profiler]")
{
    markamp::core::RingBuffer<int, 4> ring;
    REQUIRE(ring.empty());
    REQUIRE(ring.size() == 0);

    ring.push(10);
    ring.push(20);
    ring.push(30);

    REQUIRE(ring.size() == 3);
    REQUIRE(ring.at(0) == 10);
    REQUIRE(ring.at(1) == 20);
    REQUIRE(ring.at(2) == 30);
    REQUIRE(ring.back() == 30);
}

TEST_CASE("RingBuffer: wrapping overwrites oldest", "[profiler]")
{
    markamp::core::RingBuffer<int, 3> ring;
    ring.push(1);
    ring.push(2);
    ring.push(3);
    REQUIRE(ring.size() == 3);
    REQUIRE_FALSE(ring.has_wrapped());

    // Push beyond capacity — overwrites oldest
    ring.push(4);
    REQUIRE(ring.size() == 3);
    REQUIRE(ring.has_wrapped());

    // Oldest should now be 2, newest 4
    REQUIRE(ring.at(0) == 2);
    REQUIRE(ring.at(1) == 3);
    REQUIRE(ring.at(2) == 4);
    REQUIRE(ring.back() == 4);
}

TEST_CASE("RingBuffer: clear resets state", "[profiler]")
{
    markamp::core::RingBuffer<double, 8> ring;
    ring.push(1.0);
    ring.push(2.0);
    REQUIRE(ring.size() == 2);

    ring.clear();
    REQUIRE(ring.empty());
    REQUIRE(ring.size() == 0);
    REQUIRE(ring.total_pushes() == 0);
}

TEST_CASE("RingBuffer: total_pushes tracks all writes", "[profiler]")
{
    markamp::core::RingBuffer<int, 2> ring;
    ring.push(1);
    ring.push(2);
    ring.push(3);
    ring.push(4);

    REQUIRE(ring.total_pushes() == 4);
    REQUIRE(ring.size() == 2); // Only 2 stored
}

TEST_CASE("ProfileId: name lookup returns correct name", "[profiler]")
{
    using markamp::core::profile_id_name;
    using markamp::core::ProfileId;

    REQUIRE(profile_id_name(ProfileId::EventBus_Publish) == "EventBus::publish");
    REQUIRE(profile_id_name(ProfileId::MarkdownParser_Parse) == "MarkdownParser::parse");
    REQUIRE(profile_id_name(ProfileId::Startup_Total) == "Startup::total");
}

TEST_CASE("ProfileId: out-of-range returns unknown", "[profiler]")
{
    using markamp::core::profile_id_name;
    using markamp::core::ProfileId;

    auto bad_id = static_cast<ProfileId>(9999);
    REQUIRE(profile_id_name(bad_id) == "unknown");
}

TEST_CASE("Profiler: record by ProfileId stores in ring buffer", "[profiler]")
{
    markamp::core::Profiler profiler;
    profiler.record(markamp::core::ProfileId::Config_Get, 1.5);
    profiler.record(markamp::core::ProfileId::Config_Get, 2.5);
    profiler.record(markamp::core::ProfileId::Config_Get, 3.5);

    auto results = profiler.results();
    bool found = false;
    for (const auto& result : results)
    {
        if (result.name == "Config::get")
        {
            found = true;
            REQUIRE(result.call_count == 3);
            REQUIRE(result.min_ms == 1.5);
            REQUIRE(result.max_ms == 3.5);
            REQUIRE(result.avg_ms == 2.5);
        }
    }
    REQUIRE(found);
}

TEST_CASE("Profiler: string-based backward compatibility", "[profiler]")
{
    markamp::core::Profiler profiler;
    profiler.record("legacy_timer", 10.0);
    profiler.record("legacy_timer", 20.0);

    auto results = profiler.results();
    bool found = false;
    for (const auto& result : results)
    {
        if (result.name == "legacy_timer")
        {
            found = true;
            REQUIRE(result.call_count == 2);
            REQUIRE(result.avg_ms == 15.0);
        }
    }
    REQUIRE(found);
}

TEST_CASE("Profiler: reset clears both paths", "[profiler]")
{
    markamp::core::Profiler profiler;
    profiler.record("string_timer", 5.0);
    profiler.record(markamp::core::ProfileId::EventBus_Publish, 3.0);

    profiler.reset();

    auto results = profiler.results();
    REQUIRE(results.empty());
}
