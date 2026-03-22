/// @file test_v25_p20_release_signoff.cpp
/// @brief V25 Phase 20: Release signoff runner tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P20: Closure report generation", "[v25][p20]")
{
    ReleaseSignoffRunner runner;
    SubsystemSignoff s;
    s.subsystem_name = "Governance";
    s.verdict = SubsystemVerdict::kGreen;
    runner.add_signoff(s);

    auto report = runner.generate_closure_report();
    REQUIRE(report.green_count == 1);
    REQUIRE(report.is_release_candidate);
}

TEST_CASE("V25 P20: Blocked subsystem prevents release", "[v25][p20]")
{
    ReleaseSignoffRunner runner;
    SubsystemSignoff g;
    g.subsystem_name = "Canvas";
    g.verdict = SubsystemVerdict::kGreen;
    runner.add_signoff(g);

    SubsystemSignoff b;
    b.subsystem_name = "Cloud";
    b.verdict = SubsystemVerdict::kBlocked;
    runner.add_signoff(b);

    auto report = runner.generate_closure_report();
    REQUIRE_FALSE(report.is_release_candidate);
    REQUIRE(report.blocked_count == 1);
}

TEST_CASE("V25 P20: Gated subsystem with green allows release", "[v25][p20]")
{
    ReleaseSignoffRunner runner;
    SubsystemSignoff g;
    g.subsystem_name = "Persistence";
    g.verdict = SubsystemVerdict::kGreen;
    runner.add_signoff(g);

    SubsystemSignoff ga;
    ga.subsystem_name = "Cloud";
    ga.verdict = SubsystemVerdict::kGated;
    runner.add_signoff(ga);

    auto report = runner.generate_closure_report();
    REQUIRE(report.is_release_candidate);
    REQUIRE(report.gated_count == 1);
}

TEST_CASE("V25 P20: Export markdown closure report", "[v25][p20]")
{
    ReleaseSignoffRunner runner;
    SubsystemSignoff s;
    s.subsystem_name = "Test";
    s.verdict = SubsystemVerdict::kGreen;
    runner.add_signoff(s);

    auto md = runner.export_markdown();
    REQUIRE(md.find("Release Closure Report") != std::string::npos);
    REQUIRE(md.find("Green") != std::string::npos);
}

TEST_CASE("V25 P20: Total subsystems in report", "[v25][p20]")
{
    ReleaseSignoffRunner runner;
    for (int i = 0; i < 5; ++i) {
        SubsystemSignoff s;
        s.subsystem_name = "Sub" + std::to_string(i);
        s.verdict = SubsystemVerdict::kGreen;
        runner.add_signoff(s);
    }
    auto report = runner.generate_closure_report();
    REQUIRE(report.total_subsystems() == 5);
}
