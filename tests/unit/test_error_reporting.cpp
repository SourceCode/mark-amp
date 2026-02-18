/// test_error_reporting.cpp — V7 Phase 30: Error reporting service tests

#include "core/ErrorReportingService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

static auto make_test_error(ErrorCode code,
                            SubsystemId subsystem,
                            const std::string& msg = "test error") -> Error
{
    return Error{
        .code = code,
        .subsystem = subsystem,
        .message = msg,
    };
}

// ══════════════════════════════════════════════════════════════════════════════
// Basic reporting
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ErrorReportingService: no errors initially", "[error_reporting]")
{
    ErrorReportingService service;
    REQUIRE(service.unique_error_count() == 0);
    REQUIRE(service.total_occurrences() == 0);
}

TEST_CASE("ErrorReportingService: report increments counts", "[error_reporting]")
{
    ErrorReportingService service;
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace));

    REQUIRE(service.unique_error_count() == 1);
    REQUIRE(service.total_occurrences() == 1);
}

TEST_CASE("ErrorReportingService: deduplicates same error", "[error_reporting]")
{
    ErrorReportingService service;
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace, "first"));
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace, "second"));

    REQUIRE(service.unique_error_count() == 1);
    REQUIRE(service.total_occurrences() == 2);
}

TEST_CASE("ErrorReportingService: different codes are separate", "[error_reporting]")
{
    ErrorReportingService service;
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace));
    service.report(make_test_error(ErrorCode::ParseError, SubsystemId::Core));

    REQUIRE(service.unique_error_count() == 2);
    REQUIRE(service.total_occurrences() == 2);
}

TEST_CASE("ErrorReportingService: different subsystems are separate", "[error_reporting]")
{
    ErrorReportingService service;
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace));
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Core));

    REQUIRE(service.unique_error_count() == 2);
}

TEST_CASE("ErrorReportingService: has_error check", "[error_reporting]")
{
    ErrorReportingService service;
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace));

    REQUIRE(service.has_error(ErrorCode::IoError, SubsystemId::Workspace));
    REQUIRE_FALSE(service.has_error(ErrorCode::ParseError, SubsystemId::Core));
}

// ══════════════════════════════════════════════════════════════════════════════
// Reports
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ErrorReportingService: all_reports returns details", "[error_reporting]")
{
    ErrorReportingService service;
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace, "disk full"));

    auto reports = service.all_reports();
    REQUIRE(reports.size() == 1);
    REQUIRE(reports[0].code == ErrorCode::IoError);
    REQUIRE(reports[0].message == "disk full");
    REQUIRE(reports[0].occurrence_count == 1);
    REQUIRE(reports[0].first_seen_ms > 0);
}

TEST_CASE("ErrorReportingService: clear removes all", "[error_reporting]")
{
    ErrorReportingService service;
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace));
    service.report(make_test_error(ErrorCode::ParseError, SubsystemId::Core));

    service.clear();
    REQUIRE(service.unique_error_count() == 0);
    REQUIRE(service.total_occurrences() == 0);
}

// ══════════════════════════════════════════════════════════════════════════════
// Toast callback
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ErrorReportingService: toast callback fires on report", "[error_reporting]")
{
    ErrorReportingService service;
    bool called = false;
    ErrorCode received_code{};

    service.set_toast_callback(
        [&](const ErrorReport& report)
        {
            called = true;
            received_code = report.code;
        });

    service.report(make_test_error(ErrorCode::RenderError, SubsystemId::Rendering));
    REQUIRE(called);
    REQUIRE(received_code == ErrorCode::RenderError);
}

TEST_CASE("ErrorReportingService: toast rate limiting", "[error_reporting]")
{
    ErrorReportingService service;
    service.set_max_toasts_per_window(2);
    service.set_toast_cooldown(std::chrono::seconds(60));

    int toast_count = 0;
    service.set_toast_callback([&](const ErrorReport& /*report*/) { toast_count++; });

    // Report many errors — only 2 toasts should fire
    for (int idx = 0; idx < 10; ++idx)
    {
        service.report(make_test_error(
            static_cast<ErrorCode>(idx), SubsystemId::Core, "error " + std::to_string(idx)));
    }

    REQUIRE(toast_count == 2);
}

TEST_CASE("ErrorReportingService: no callback means no crash", "[error_reporting]")
{
    ErrorReportingService service;
    // No callback set — should not crash
    service.report(make_test_error(ErrorCode::IoError, SubsystemId::Workspace));
    REQUIRE(service.unique_error_count() == 1);
}
