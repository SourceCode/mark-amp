/// @file test_v23_project_completion.cpp
/// @brief V23 Phase 09 — Tests for ProjectWorkflowCompletionAuditor.

#include "core/ProjectWorkflowCompletionAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ProjectCapabilityArea — labels", "[v23][p09][label]")
{
    CHECK(std::string(project_capability_label(ProjectCapabilityArea::kGitOperations)) == "GitOperations");
    CHECK(std::string(project_capability_label(ProjectCapabilityArea::kBuildSystem)) == "BuildSystem");
    CHECK(std::string(project_capability_label(ProjectCapabilityArea::kDebugSession)) == "DebugSession");
}

TEST_CASE("ProjectCompletionItem — completeness", "[v23][p09][item]")
{
    ProjectCompletionItem item;
    CHECK(!item.is_complete());
    item.is_real_backend = true;
    item.has_error_handling = true;
    CHECK(item.is_complete());
}

TEST_CASE("ProjectWorkflowCompletionAuditor — registration", "[v23][p09][auditor]")
{
    ProjectWorkflowCompletionAuditor auditor;
    ProjectCompletionItem item;
    auditor.add_item(std::move(item));
    CHECK(auditor.item_count() == 1);
}

TEST_CASE("ProjectWorkflowCompletionAuditor — query by area", "[v23][p09][query]")
{
    ProjectWorkflowCompletionAuditor auditor;
    ProjectCompletionItem git;
    git.area = ProjectCapabilityArea::kGitOperations;
    auditor.add_item(std::move(git));
    ProjectCompletionItem build;
    build.area = ProjectCapabilityArea::kBuildSystem;
    auditor.add_item(std::move(build));

    CHECK(auditor.items_by_area(ProjectCapabilityArea::kGitOperations).size() == 1);
    CHECK(auditor.items_by_area(ProjectCapabilityArea::kDebugSession).empty());
}

TEST_CASE("ProjectWorkflowCompletionAuditor — complete vs incomplete", "[v23][p09][query]")
{
    ProjectWorkflowCompletionAuditor auditor;
    ProjectCompletionItem ok;
    ok.is_real_backend = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));
    ProjectCompletionItem gap;
    auditor.add_item(std::move(gap));

    CHECK(auditor.complete_items().size() == 1);
    CHECK(auditor.incomplete_items().size() == 1);
}

TEST_CASE("ProjectWorkflowCompletionAuditor — report with gap categories", "[v23][p09][report]")
{
    ProjectWorkflowCompletionAuditor auditor;
    ProjectCompletionItem git_gap;
    git_gap.area = ProjectCapabilityArea::kGitOperations;
    auditor.add_item(std::move(git_gap));
    ProjectCompletionItem build_gap;
    build_gap.area = ProjectCapabilityArea::kBuildSystem;
    auditor.add_item(std::move(build_gap));
    ProjectCompletionItem debug_gap;
    debug_gap.area = ProjectCapabilityArea::kDebugSession;
    auditor.add_item(std::move(debug_gap));
    ProjectCompletionItem ok;
    ok.area = ProjectCapabilityArea::kSCMPanel;
    ok.is_real_backend = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));

    auto r = auditor.report();
    CHECK(r.total == 4);
    CHECK(r.complete == 1);
    CHECK(r.git_gaps == 1);
    CHECK(r.build_gaps == 1);
    CHECK(r.debug_gaps == 1);
}

TEST_CASE("ProjectWorkflowCompletionAuditor — clear", "[v23][p09][auditor]")
{
    ProjectWorkflowCompletionAuditor auditor;
    ProjectCompletionItem item;
    auditor.add_item(std::move(item));
    auditor.clear();
    CHECK(auditor.item_count() == 0);
}

TEST_CASE("ProjectWorkflowCompletionAuditor — export", "[v23][p09][export]")
{
    ProjectWorkflowCompletionAuditor auditor;
    ProjectCompletionItem item;
    item.is_real_backend = true;
    item.has_error_handling = true;
    auditor.add_item(std::move(item));
    CHECK(auditor.export_json().find("\"complete\": 1") != std::string::npos);
    CHECK(auditor.export_markdown().find("Project Workflow") != std::string::npos);
}

TEST_CASE("Integration — project auditor lifecycle", "[v23][p09][integration]")
{
    ProjectWorkflowCompletionAuditor auditor;

    for (auto area : {ProjectCapabilityArea::kGitOperations, ProjectCapabilityArea::kRepositoryDetection})
    {
        ProjectCompletionItem item;
        item.area = area;
        item.is_real_backend = true;
        item.has_error_handling = true;
        auditor.add_item(std::move(item));
    }

    ProjectCompletionItem build_gap;
    build_gap.area = ProjectCapabilityArea::kBuildSystem;
    auditor.add_item(std::move(build_gap));

    ProjectCompletionItem debug_gap;
    debug_gap.area = ProjectCapabilityArea::kDebugSession;
    auditor.add_item(std::move(debug_gap));

    CHECK(auditor.item_count() == 4);
    CHECK(auditor.complete_items().size() == 2);
    auto r = auditor.report();
    CHECK(r.git_gaps == 0);
    CHECK(r.build_gaps == 1);
    CHECK(r.debug_gaps == 1);
    CHECK(r.coverage_pct() == 50.0);
}
