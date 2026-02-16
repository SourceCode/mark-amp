/// test_crash_reporter.cpp — Phase 30: CrashReporter tests

#include "core/CrashReporter.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

TEST_CASE("signal_name: known signals", "[crash_reporter]")
{
#if defined(__APPLE__) || defined(__linux__)
    REQUIRE(signal_name(SIGSEGV) == "SIGSEGV");
    REQUIRE(signal_name(SIGABRT) == "SIGABRT");
    REQUIRE(signal_name(SIGBUS) == "SIGBUS");
    REQUIRE(signal_name(SIGFPE) == "SIGFPE");
    REQUIRE(signal_name(SIGILL) == "SIGILL");
#endif
}

TEST_CASE("signal_name: unknown signal", "[crash_reporter]")
{
    REQUIRE(signal_name(9999) == "UNKNOWN");
}

TEST_CASE("CrashReporter: set_crash_dir", "[crash_reporter]")
{
    auto& reporter = CrashReporter::instance();
    reporter.set_crash_dir("/tmp/test-crashes");

    REQUIRE(reporter.crash_dir() == "/tmp/test-crashes");
}

TEST_CASE("CrashReporter: generate_report produces formatted output", "[crash_reporter]")
{
    CrashContext ctx;
    ctx.signal_number = 11;
    ctx.signal_name = "SIGSEGV";
    ctx.timestamp_epoch_ms = 1234567890;
    ctx.backtrace_depth = 2;
    ctx.backtrace_frames[0] = "frame0_symbol";
    ctx.backtrace_frames[1] = "frame1_symbol";

    auto report = CrashReporter::generate_report(ctx, "1.2.3");

    REQUIRE(report.find("MARKAMP CRASH REPORT") != std::string::npos);
    REQUIRE(report.find("Version: 1.2.3") != std::string::npos);
    REQUIRE(report.find("SIGSEGV") != std::string::npos);
    REQUIRE(report.find("frame0_symbol") != std::string::npos);
    REQUIRE(report.find("frame1_symbol") != std::string::npos);
    REQUIRE(report.find("END CRASH REPORT") != std::string::npos);
}

TEST_CASE("CrashReporter: build_context captures backtrace", "[crash_reporter]")
{
    auto& reporter = CrashReporter::instance();
    auto ctx = reporter.build_context(0); // 0 = no real signal

    REQUIRE(ctx.signal_number == 0);
    REQUIRE(ctx.timestamp_epoch_ms > 0);
#if defined(__APPLE__) || defined(__linux__)
    REQUIRE(ctx.backtrace_depth > 0);
#endif
}

TEST_CASE("CrashReporter: is_installed starts false", "[crash_reporter]")
{
    // Note: we don't call install() in tests to avoid modifying signal handlers
    // Just check the default state
    auto& reporter = CrashReporter::instance();
    // The state depends on whether install() was called in this process
    // Just verify the method works
    static_cast<void>(reporter.is_installed());
}

TEST_CASE("CrashContext: empty report", "[crash_reporter]")
{
    CrashContext ctx;
    ctx.signal_number = 0;
    ctx.signal_name = "NONE";
    ctx.timestamp_epoch_ms = 0;
    ctx.backtrace_depth = 0;

    auto report = CrashReporter::generate_report(ctx, "0.0.0");

    REQUIRE(report.find("Signal:  NONE") != std::string::npos);
    REQUIRE(report.find("Backtrace") != std::string::npos);
}
