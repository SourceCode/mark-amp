/**
 * @file test_control_metrics_instrumentation.cpp
 * @brief Phase 38: Tests for UxMetricsCollector and TelemetryPrivacyController.
 */

#include "ui/TelemetryPrivacyController.h"
#include "ui/UxMetricsCollector.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// MetricEvent
// ═══════════════════════════════════════════════════════

TEST_CASE("MetricEvent - type name", "[metrics][event]")
{
    MetricEvent event;
    event.type = UxMetricType::kMisclick;
    CHECK(event.type_name() == "misclick");

    event.type = UxMetricType::kCommandLatency;
    CHECK(event.type_name() == "command_latency");

    event.type = UxMetricType::kKeyboardNavRate;
    CHECK(event.type_name() == "keyboard_nav_rate");
}

// ═══════════════════════════════════════════════════════
// UxMetricsCollector
// ═══════════════════════════════════════════════════════

TEST_CASE("UxMetricsCollector - empty state", "[metrics][collector]")
{
    UxMetricsCollector collector;
    CHECK(collector.event_count() == 0);
    CHECK(collector.is_enabled());
}

TEST_CASE("UxMetricsCollector - record events", "[metrics][collector]")
{
    UxMetricsCollector collector;
    collector.record(UxMetricType::kMisclick, 1.0, "toolbar");
    collector.record(UxMetricType::kMisclick, 1.0, "editor");
    CHECK(collector.event_count() == 2);

    auto misclicks = collector.events_of_type(UxMetricType::kMisclick);
    CHECK(misclicks.size() == 2);
}

TEST_CASE("UxMetricsCollector - disabled skips recording", "[metrics][collector]")
{
    UxMetricsCollector collector;
    collector.set_enabled(false);
    collector.record(UxMetricType::kMisclick, 1.0);
    CHECK(collector.event_count() == 0);
}

TEST_CASE("UxMetricsCollector - capacity trimming", "[metrics][collector]")
{
    UxMetricsCollector collector;
    collector.set_max_events(3);

    collector.record(UxMetricType::kMisclick, 1.0, "first");
    collector.record(UxMetricType::kMisclick, 2.0, "second");
    collector.record(UxMetricType::kMisclick, 3.0, "third");
    collector.record(UxMetricType::kMisclick, 4.0, "fourth");

    CHECK(collector.event_count() == 3);
    auto events = collector.events_of_type(UxMetricType::kMisclick);
    CHECK(events[0].context == "second"); // First trimmed
}

TEST_CASE("UxMetricsCollector - summary aggregation", "[metrics][collector]")
{
    UxMetricsCollector collector;
    collector.record(UxMetricType::kCommandLatency, 100.0);
    collector.record(UxMetricType::kCommandLatency, 200.0);
    collector.record(UxMetricType::kCommandLatency, 300.0);

    auto sum = collector.summary(UxMetricType::kCommandLatency);
    CHECK(sum.count == 3);
    CHECK(sum.total == 600.0);
    CHECK(sum.average() == 200.0);
    CHECK(sum.min_value == 100.0);
    CHECK(sum.max_value == 300.0);
}

TEST_CASE("UxMetricsCollector - all summaries", "[metrics][collector]")
{
    UxMetricsCollector collector;
    collector.record(UxMetricType::kMisclick, 1.0);

    auto summaries = collector.all_summaries();
    CHECK(summaries.size() == 6); // All 6 metric types
}

TEST_CASE("UxMetricsCollector - clear", "[metrics][collector]")
{
    UxMetricsCollector collector;
    collector.record(UxMetricType::kMisclick, 1.0);
    collector.clear();
    CHECK(collector.event_count() == 0);
}

// ═══════════════════════════════════════════════════════
// RetentionPolicy
// ═══════════════════════════════════════════════════════

TEST_CASE("RetentionPolicy - validity", "[metrics][privacy]")
{
    RetentionPolicy valid;
    valid.max_events = 100;
    valid.max_age_hours = 24;
    CHECK(valid.is_valid());

    RetentionPolicy invalid;
    invalid.max_events = 0;
    CHECK_FALSE(invalid.is_valid());
}

// ═══════════════════════════════════════════════════════
// TelemetryPrivacyController
// ═══════════════════════════════════════════════════════

TEST_CASE("TelemetryPrivacyController - defaults", "[metrics][privacy]")
{
    TelemetryPrivacyController ctrl;
    CHECK(ctrl.tier() == PrivacyTier::kOff);
    CHECK(ctrl.tier_name() == "off");
    CHECK_FALSE(ctrl.is_active());
    CHECK_FALSE(ctrl.has_user_consent());
}

TEST_CASE("TelemetryPrivacyController - tier gating", "[metrics][privacy]")
{
    TelemetryPrivacyController ctrl;
    ctrl.set_user_consent(true);

    ctrl.set_tier(PrivacyTier::kAnonymous);
    CHECK(ctrl.is_active());
    CHECK_FALSE(ctrl.allows_context());

    ctrl.set_tier(PrivacyTier::kDetailed);
    CHECK(ctrl.is_active());
    CHECK(ctrl.allows_context());
}

TEST_CASE("TelemetryPrivacyController - requires consent", "[metrics][privacy]")
{
    TelemetryPrivacyController ctrl;
    ctrl.set_tier(PrivacyTier::kDetailed);
    CHECK_FALSE(ctrl.is_active()); // No consent

    ctrl.set_user_consent(true);
    CHECK(ctrl.is_active());
}

TEST_CASE("TelemetryPrivacyController - PII fields", "[metrics][privacy]")
{
    TelemetryPrivacyController ctrl;
    ctrl.register_field({"user_id", true, true});
    ctrl.register_field({"event_type", true, false});
    ctrl.register_field({"email", false, true});

    CHECK(ctrl.pii_field_count() == 2);
    CHECK(ctrl.is_pii_field("user_id"));
    CHECK_FALSE(ctrl.is_pii_field("event_type"));
}

TEST_CASE("TelemetryPrivacyController - PII tier gating", "[metrics][privacy]")
{
    TelemetryPrivacyController ctrl;

    ctrl.set_tier(PrivacyTier::kAnonymous);
    CHECK_FALSE(ctrl.pii_allowed());

    ctrl.set_tier(PrivacyTier::kDetailed);
    CHECK(ctrl.pii_allowed());
}

TEST_CASE("TelemetryPrivacyController - retention policy", "[metrics][privacy]")
{
    TelemetryPrivacyController ctrl;
    RetentionPolicy policy;
    policy.max_events = 5000;
    policy.max_age_hours = 48;

    ctrl.set_retention(policy);
    CHECK(ctrl.retention().max_events == 5000);
    CHECK(ctrl.retention().max_age_hours == 48);
}
