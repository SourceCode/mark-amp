// test_security_audit_log.cpp — 10 tests for SecurityAuditLog
#include "core/SecurityAuditLog.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SecurityAuditLog starts empty", "[security][audit]")
{
    SecurityAuditLog log;
    CHECK(log.size() == 0);
}

TEST_CASE("SecurityAuditLog record increases size", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Test event");
    CHECK(log.size() == 1);
}

TEST_CASE("SecurityAuditLog record multiple events", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Event 1");
    log.record(SecuritySeverity::kWarning, SecurityCategory::kNetwork, "Event 2");
    log.record(SecuritySeverity::kCritical, SecurityCategory::kAuth, "Event 3");
    CHECK(log.size() == 3);
}

TEST_CASE("SecurityAuditLog recent returns latest events", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "First");
    log.record(SecuritySeverity::kWarning, SecurityCategory::kNetwork, "Second");
    auto recent = log.recent(1);
    CHECK(recent.size() == 1);
}

TEST_CASE("SecurityAuditLog summary counts by severity", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Info event");
    log.record(SecuritySeverity::kWarning, SecurityCategory::kNetwork, "Warning event");
    log.record(SecuritySeverity::kCritical, SecurityCategory::kAuth, "Critical event");
    auto summary = log.summary();
    CHECK(summary.total_events == 3);
    CHECK(summary.info_count >= 1);
    CHECK(summary.warning_count >= 1);
    CHECK(summary.critical_count >= 1);
}

TEST_CASE("SecurityAuditLog clear removes all events", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Event");
    log.clear();
    CHECK(log.size() == 0);
}

TEST_CASE("SecurityAuditLog max_entries respected", "[security][audit]")
{
    SecurityAuditLog log(5);
    CHECK(log.max_entries() == 5);
    for (int idx = 0; idx < 10; ++idx)
    {
        log.record(
            SecuritySeverity::kInfo, SecurityCategory::kInput, "Event " + std::to_string(idx));
    }
    CHECK(log.size() <= 5);
}

TEST_CASE("SecurityAuditLog export_json produces JSON", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Test");
    auto json = log.export_json();
    CHECK_FALSE(json.empty());
}

TEST_CASE("SecurityAuditLog query filters by severity", "[security][audit]")
{
    SecurityAuditLog log;
    log.record(SecuritySeverity::kInfo, SecurityCategory::kInput, "Info");
    log.record(SecuritySeverity::kCritical, SecurityCategory::kAuth, "Critical");
    SecurityEventFilter filter;
    filter.min_severity = SecuritySeverity::kCritical;
    auto results = log.query(filter);
    for (const auto& event : results)
    {
        CHECK(event.severity == SecuritySeverity::kCritical);
    }
}

TEST_CASE("SecurityEvent defaults", "[security][audit]")
{
    SecurityEvent event;
    CHECK(event.timestamp_ms == 0);
    CHECK(event.severity == SecuritySeverity::kInfo);
    CHECK(event.category == SecurityCategory::kInput);
    CHECK(event.description.empty());
}
