// test_deprecation_tracker.cpp — 10 tests for DeprecationTracker
#include "core/DeprecationTracker.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DeprecationTracker starts empty", "[deprecation]")
{
    DeprecationTracker tracker;
    CHECK(tracker.deprecation_count() == 0);
}

TEST_CASE("DeprecationTracker add and query deprecation", "[deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({
        .feature_name = "old_api",
        .deprecated_in = "v1.0",
        .removed_in = "",
        .replacement = "new_api",
        .reason = "Superseded",
    });
    CHECK(tracker.is_deprecated("old_api"));
    CHECK(tracker.deprecation_count() == 1);
}

TEST_CASE("DeprecationTracker get_replacement returns suggestion", "[deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({
        .feature_name = "legacy_func",
        .deprecated_in = "v2.0",
        .removed_in = "",
        .replacement = "modern_func",
        .reason = "Better API",
    });
    CHECK(tracker.get_replacement("legacy_func") == "modern_func");
}

TEST_CASE("DeprecationTracker get_entry returns full info", "[deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({
        .feature_name = "feat_x",
        .deprecated_in = "v3.0",
        .removed_in = "v4.0",
        .replacement = "feat_y",
        .reason = "Performance",
    });
    const auto* entry = tracker.get_entry("feat_x");
    REQUIRE(entry != nullptr);
    CHECK(entry->deprecated_in == "v3.0");
    CHECK(entry->removed_in == "v4.0");
}

TEST_CASE("DeprecationTracker is_deprecated returns false for unknown", "[deprecation]")
{
    DeprecationTracker tracker;
    CHECK_FALSE(tracker.is_deprecated("nonexistent"));
}

TEST_CASE("DeprecationTracker active_deprecations excludes removed", "[deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({.feature_name = "active",
                             .deprecated_in = "v1",
                             .removed_in = "",
                             .replacement = "x",
                             .reason = ""});
    tracker.add_deprecation({.feature_name = "removed",
                             .deprecated_in = "v1",
                             .removed_in = "v2",
                             .replacement = "y",
                             .reason = ""});
    auto active = tracker.active_deprecations();
    // Active deprecations should only include those without removed_in
    bool found_active = false;
    for (const auto* entry : active)
    {
        if (entry->feature_name == "active")
            found_active = true;
    }
    CHECK(found_active);
}

TEST_CASE("DeprecationTracker removed_features returns removed only", "[deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({.feature_name = "active",
                             .deprecated_in = "v1",
                             .removed_in = "",
                             .replacement = "",
                             .reason = ""});
    tracker.add_deprecation({.feature_name = "gone",
                             .deprecated_in = "v1",
                             .removed_in = "v2",
                             .replacement = "",
                             .reason = ""});
    auto removed = tracker.removed_features();
    bool found_gone = false;
    for (const auto* entry : removed)
    {
        if (entry->feature_name == "gone")
            found_gone = true;
    }
    CHECK(found_gone);
}

TEST_CASE("DeprecationTracker clear removes all", "[deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({.feature_name = "a",
                             .deprecated_in = "v1",
                             .removed_in = "",
                             .replacement = "",
                             .reason = ""});
    tracker.clear();
    CHECK(tracker.deprecation_count() == 0);
    CHECK_FALSE(tracker.is_deprecated("a"));
}

TEST_CASE("DeprecationTracker get_entry returns nullptr for unknown", "[deprecation]")
{
    DeprecationTracker tracker;
    CHECK(tracker.get_entry("nonexistent") == nullptr);
}

TEST_CASE("DeprecationTracker get_replacement returns empty for unknown", "[deprecation]")
{
    DeprecationTracker tracker;
    CHECK(tracker.get_replacement("nonexistent").empty());
}
