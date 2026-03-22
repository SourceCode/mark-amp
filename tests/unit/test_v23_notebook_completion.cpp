/// @file test_v23_notebook_completion.cpp
/// @brief V23 Phase 06 — Tests for NotebookExecutionCompletionAuditor.

#include "core/NotebookExecutionCompletionAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("NotebookCapabilityArea — labels", "[v23][p06][label]")
{
    CHECK(std::string(notebook_capability_label(NotebookCapabilityArea::kKernelLifecycle)) == "KernelLifecycle");
    CHECK(std::string(notebook_capability_label(NotebookCapabilityArea::kCellExecution)) == "CellExecution");
    CHECK(std::string(notebook_capability_label(NotebookCapabilityArea::kAICompletion)) == "AICompletion");
}

TEST_CASE("NotebookCompletionItem — completeness", "[v23][p06][item]")
{
    NotebookCompletionItem item;
    CHECK(!item.is_complete());
    item.is_real_transport = true;
    item.has_error_handling = true;
    CHECK(item.is_complete());
}

TEST_CASE("NotebookExecutionCompletionAuditor — registration", "[v23][p06][auditor]")
{
    NotebookExecutionCompletionAuditor auditor;
    CHECK(auditor.item_count() == 0);
    NotebookCompletionItem item;
    auditor.add_item(std::move(item));
    CHECK(auditor.item_count() == 1);
}

TEST_CASE("NotebookExecutionCompletionAuditor — query by area", "[v23][p06][query]")
{
    NotebookExecutionCompletionAuditor auditor;
    NotebookCompletionItem kernel;
    kernel.area = NotebookCapabilityArea::kKernelLifecycle;
    auditor.add_item(std::move(kernel));
    NotebookCompletionItem ai;
    ai.area = NotebookCapabilityArea::kAICompletion;
    auditor.add_item(std::move(ai));

    CHECK(auditor.items_by_area(NotebookCapabilityArea::kKernelLifecycle).size() == 1);
    CHECK(auditor.items_by_area(NotebookCapabilityArea::kAICompletion).size() == 1);
    CHECK(auditor.items_by_area(NotebookCapabilityArea::kCellExecution).empty());
}

TEST_CASE("NotebookExecutionCompletionAuditor — complete vs incomplete", "[v23][p06][query]")
{
    NotebookExecutionCompletionAuditor auditor;
    NotebookCompletionItem complete;
    complete.is_real_transport = true;
    complete.has_error_handling = true;
    auditor.add_item(std::move(complete));
    NotebookCompletionItem incomplete;
    auditor.add_item(std::move(incomplete));

    CHECK(auditor.complete_items().size() == 1);
    CHECK(auditor.incomplete_items().size() == 1);
}

TEST_CASE("NotebookExecutionCompletionAuditor — report", "[v23][p06][report]")
{
    NotebookExecutionCompletionAuditor auditor;
    NotebookCompletionItem kernel_gap;
    kernel_gap.area = NotebookCapabilityArea::kKernelTransport;
    auditor.add_item(std::move(kernel_gap));
    NotebookCompletionItem ai_gap;
    ai_gap.area = NotebookCapabilityArea::kAIDocumentGen;
    auditor.add_item(std::move(ai_gap));
    NotebookCompletionItem ok;
    ok.area = NotebookCapabilityArea::kCellExecution;
    ok.is_real_transport = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));

    auto r = auditor.report();
    CHECK(r.total == 3);
    CHECK(r.complete == 1);
    CHECK(r.kernel_gaps == 1);
    CHECK(r.ai_gaps == 1);
}

TEST_CASE("NotebookExecutionCompletionAuditor — clear", "[v23][p06][auditor]")
{
    NotebookExecutionCompletionAuditor auditor;
    NotebookCompletionItem item;
    auditor.add_item(std::move(item));
    auditor.clear();
    CHECK(auditor.item_count() == 0);
}

TEST_CASE("NotebookExecutionCompletionAuditor — export", "[v23][p06][export]")
{
    NotebookExecutionCompletionAuditor auditor;
    NotebookCompletionItem item;
    item.is_real_transport = true;
    item.has_error_handling = true;
    auditor.add_item(std::move(item));

    CHECK(!auditor.export_json().empty());
    CHECK(auditor.export_markdown().find("Notebook Execution") != std::string::npos);
}

TEST_CASE("Integration — notebook auditor lifecycle", "[v23][p06][integration]")
{
    NotebookExecutionCompletionAuditor auditor;

    for (auto area : {NotebookCapabilityArea::kKernelLifecycle, NotebookCapabilityArea::kCellExecution})
    {
        NotebookCompletionItem item;
        item.area = area;
        item.is_real_transport = true;
        item.has_error_handling = true;
        auditor.add_item(std::move(item));
    }

    NotebookCompletionItem gap;
    gap.area = NotebookCapabilityArea::kKernelTransport;
    auditor.add_item(std::move(gap));

    CHECK(auditor.item_count() == 3);
    CHECK(auditor.complete_items().size() == 2);
    auto r = auditor.report();
    CHECK(r.kernel_gaps == 1);
    CHECK(r.coverage_pct() < 70.0);
}
