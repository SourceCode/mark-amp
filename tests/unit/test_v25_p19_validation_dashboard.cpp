/// @file test_v25_p19_validation_dashboard.cpp
/// @brief V25 Phase 19: Release validation dashboard tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P19: Dashboard report generation", "[v25][p19]")
{
    ReleaseValidationDashboard dashboard;
    DashboardSection section;
    section.section_name = "Governance";
    section.total_items = 10;
    section.passing_items = 10;
    section.failing_items = 0;
    dashboard.add_section(section);

    auto report = dashboard.generate_report();
    REQUIRE(report.is_release_ready);
    REQUIRE(report.total_blockers == 0);
}

TEST_CASE("V25 P19: Dashboard blocks on blockers", "[v25][p19]")
{
    ReleaseValidationDashboard dashboard;
    DashboardSection section;
    section.section_name = "Canvas";
    section.total_items = 5;
    section.passing_items = 3;
    section.failing_items = 2;
    section.blocker_ids.push_back("P09-T01");
    dashboard.add_section(section);

    auto report = dashboard.generate_report();
    REQUIRE_FALSE(report.is_release_ready);
    REQUIRE(report.total_blockers == 1);
}

TEST_CASE("V25 P19: Dashboard export markdown", "[v25][p19]")
{
    ReleaseValidationDashboard dashboard;
    DashboardSection s;
    s.section_name = "Test";
    s.total_items = 1;
    s.passing_items = 1;
    dashboard.add_section(s);

    auto md = dashboard.export_markdown();
    REQUIRE(md.find("Release Validation Dashboard") != std::string::npos);
}
