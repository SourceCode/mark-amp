/// test_error_reporting_service.cpp — Comprehensive tests for ErrorReportingService
#include "core/ErrorReportingService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("ErrorReportingService: default construction", "[error_reporting][positive]")
{
    ErrorReportingService svc;
    REQUIRE(svc.unique_error_count() == 0);
    REQUIRE(svc.total_occurrences() == 0);
}

TEST_CASE("ErrorReportingService: report adds error", "[error_reporting][positive]")
{
    ErrorReportingService svc;
    Error err{ErrorCode::Unknown, SubsystemId::Core, "Test error"};
    svc.report(err);
    REQUIRE(svc.unique_error_count() == 1);
    REQUIRE(svc.total_occurrences() == 1);
}

TEST_CASE("ErrorReportingService: duplicate errors deduplicate", "[error_reporting][positive]")
{
    ErrorReportingService svc;
    Error err{ErrorCode::Unknown, SubsystemId::Core, "Same error"};
    svc.report(err);
    svc.report(err);
    REQUIRE(svc.unique_error_count() == 1);
    REQUIRE(svc.total_occurrences() == 2);
}

TEST_CASE("ErrorReportingService: has_error check", "[error_reporting][positive]")
{
    ErrorReportingService svc;
    Error err{ErrorCode::Unknown, SubsystemId::Core, "Test"};
    svc.report(err);
    REQUIRE(svc.has_error(ErrorCode::Unknown, SubsystemId::Core));
}

TEST_CASE("ErrorReportingService: all_reports returns list", "[error_reporting][positive]")
{
    ErrorReportingService svc;
    Error err{ErrorCode::Unknown, SubsystemId::Core, "Test"};
    svc.report(err);
    auto reports = svc.all_reports();
    REQUIRE(reports.size() == 1);
    REQUIRE(reports[0].message == "Test");
}

TEST_CASE("ErrorReportingService: clear removes everything", "[error_reporting][positive]")
{
    ErrorReportingService svc;
    Error err{ErrorCode::Unknown, SubsystemId::Core, "Test"};
    svc.report(err);
    svc.clear();
    REQUIRE(svc.unique_error_count() == 0);
    REQUIRE(svc.total_occurrences() == 0);
    REQUIRE(svc.all_reports().empty());
}

TEST_CASE("ErrorReportingService: toast callback receives report", "[error_reporting][positive]")
{
    ErrorReportingService svc;
    bool callback_called = false;
    svc.set_toast_callback([&](const ErrorReport&) { callback_called = true; });
    Error err{ErrorCode::Unknown, SubsystemId::Core, "Toast error"};
    svc.report(err);
    REQUIRE(callback_called);
}

// ── Negative Tests ──

TEST_CASE("ErrorReportingService: has_error for unreported returns false",
          "[error_reporting][negative]")
{
    ErrorReportingService svc;
    REQUIRE_FALSE(svc.has_error(ErrorCode::Unknown, SubsystemId::Core));
}

TEST_CASE("ErrorReportingService: clear on empty is safe", "[error_reporting][negative]")
{
    ErrorReportingService svc;
    svc.clear();
    REQUIRE(svc.unique_error_count() == 0);
}

// ── Edge Cases ──

TEST_CASE("ErrorReportingService: set_toast_cooldown", "[error_reporting][edge]")
{
    ErrorReportingService svc;
    svc.set_toast_cooldown(std::chrono::seconds(60));
    // Should not crash
}

TEST_CASE("ErrorReportingService: set_max_toasts_per_window", "[error_reporting][edge]")
{
    ErrorReportingService svc;
    svc.set_max_toasts_per_window(10);
    // Should not crash
}

TEST_CASE("ErrorReport struct: default values", "[error_reporting][edge]")
{
    ErrorReport report;
    REQUIRE(report.occurrence_count == 1);
    REQUIRE_FALSE(report.notified);
    REQUIRE(report.message.empty());
}
