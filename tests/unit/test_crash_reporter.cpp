// test_crash_reporter.cpp — 10 tests for CrashReporter
#include "core/CrashReporter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CrashReporter singleton instance", "[crash]")
{
    auto& r1 = CrashReporter::instance();
    auto& r2 = CrashReporter::instance();
    CHECK(&r1 == &r2);
}

TEST_CASE("CrashReporter default crash dir is /tmp", "[crash]")
{
    auto& reporter = CrashReporter::instance();
    CHECK(reporter.crash_dir() == "/tmp");
}

TEST_CASE("CrashReporter set_crash_dir changes directory", "[crash]")
{
    auto& reporter = CrashReporter::instance();
    auto original = reporter.crash_dir();
    reporter.set_crash_dir("/tmp/test_crashes");
    CHECK(reporter.crash_dir() == "/tmp/test_crashes");
    reporter.set_crash_dir(original); // restore
}

TEST_CASE("CrashReporter set_app_version does not crash", "[crash]")
{
    auto& reporter = CrashReporter::instance();
    REQUIRE_NOTHROW(reporter.set_app_version("v1.2.3"));
}

TEST_CASE("CrashReporter generate_report returns valid report", "[crash]")
{
    CrashContext ctx;
    ctx.signal_number = 11;
    ctx.signal_name = "SIGSEGV";
    ctx.timestamp_epoch_ms = 1234567890;
    ctx.backtrace_depth = 2;
    ctx.backtrace_frames[0] = "frame0";
    ctx.backtrace_frames[1] = "frame1";
    auto report = CrashReporter::generate_report(ctx, "v1.0.0");
    CHECK(report.find("MARKAMP CRASH REPORT") != std::string::npos);
    CHECK(report.find("SIGSEGV") != std::string::npos);
    CHECK(report.find("v1.0.0") != std::string::npos);
    CHECK(report.find("frame0") != std::string::npos);
}

TEST_CASE("CrashReporter build_context captures backtrace", "[crash]")
{
    auto& reporter = CrashReporter::instance();
    auto ctx = reporter.build_context(0);
    CHECK(ctx.signal_number == 0);
    CHECK(ctx.timestamp_epoch_ms > 0);
    CHECK(ctx.backtrace_depth > 0);
}

TEST_CASE("signal_name returns known names", "[crash]")
{
#if defined(__APPLE__) || defined(__linux__)
    CHECK(signal_name(SIGSEGV) == "SIGSEGV");
    CHECK(signal_name(SIGABRT) == "SIGABRT");
    CHECK(signal_name(SIGBUS) == "SIGBUS");
    CHECK(signal_name(SIGFPE) == "SIGFPE");
    CHECK(signal_name(SIGILL) == "SIGILL");
#endif
}

TEST_CASE("signal_name returns UNKNOWN for unrecognized signal", "[crash]")
{
    CHECK(signal_name(999) == "UNKNOWN");
}

TEST_CASE("CrashContext kMaxBacktraceFrames is 64", "[crash]")
{
    CHECK(CrashContext::kMaxBacktraceFrames == 64);
}

TEST_CASE("CrashReporter generate_report handles empty backtrace", "[crash]")
{
    CrashContext ctx;
    ctx.signal_name = "SIGTERM";
    ctx.backtrace_depth = 0;
    auto report = CrashReporter::generate_report(ctx, "v2.0.0");
    CHECK(report.find("Backtrace") != std::string::npos);
    CHECK(report.find("END CRASH REPORT") != std::string::npos);
}
