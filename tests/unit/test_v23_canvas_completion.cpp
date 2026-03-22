/// @file test_v23_canvas_completion.cpp
/// @brief V23 Phase 07 — Tests for CanvasCompletionAuditor.

#include "core/CanvasCompletionAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CanvasCapabilityArea — labels", "[v23][p07][label]")
{
    CHECK(std::string(canvas_capability_label(CanvasCapabilityArea::kBoardLifecycle)) == "BoardLifecycle");
    CHECK(std::string(canvas_capability_label(CanvasCapabilityArea::kCollaboration)) == "Collaboration");
    CHECK(std::string(canvas_capability_label(CanvasCapabilityArea::kImportExport)) == "ImportExport");
}

TEST_CASE("CanvasCompletionItem — completeness", "[v23][p07][item]")
{
    CanvasCompletionItem item;
    CHECK(!item.is_complete());
    item.is_implemented = true;
    item.has_error_handling = true;
    CHECK(item.is_complete());
}

TEST_CASE("CanvasCompletionAuditor — registration", "[v23][p07][auditor]")
{
    CanvasCompletionAuditor auditor;
    CanvasCompletionItem item;
    auditor.add_item(std::move(item));
    CHECK(auditor.item_count() == 1);
}

TEST_CASE("CanvasCompletionAuditor — query by area", "[v23][p07][query]")
{
    CanvasCompletionAuditor auditor;
    CanvasCompletionItem board;
    board.area = CanvasCapabilityArea::kBoardLifecycle;
    auditor.add_item(std::move(board));
    CanvasCompletionItem collab;
    collab.area = CanvasCapabilityArea::kCollaboration;
    auditor.add_item(std::move(collab));

    CHECK(auditor.items_by_area(CanvasCapabilityArea::kBoardLifecycle).size() == 1);
    CHECK(auditor.items_by_area(CanvasCapabilityArea::kWidgets).empty());
}

TEST_CASE("CanvasCompletionAuditor — complete vs incomplete", "[v23][p07][query]")
{
    CanvasCompletionAuditor auditor;
    CanvasCompletionItem ok;
    ok.is_implemented = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));
    CanvasCompletionItem gap;
    auditor.add_item(std::move(gap));

    CHECK(auditor.complete_items().size() == 1);
    CHECK(auditor.incomplete_items().size() == 1);
}

TEST_CASE("CanvasCompletionAuditor — report", "[v23][p07][report]")
{
    CanvasCompletionAuditor auditor;
    CanvasCompletionItem ok;
    ok.is_implemented = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));
    CanvasCompletionItem gap;
    auditor.add_item(std::move(gap));

    auto r = auditor.report();
    CHECK(r.total == 2);
    CHECK(r.complete == 1);
    CHECK(r.incomplete == 1);
    CHECK(r.coverage_pct() == 50.0);
}

TEST_CASE("CanvasCompletionAuditor — clear", "[v23][p07][auditor]")
{
    CanvasCompletionAuditor auditor;
    CanvasCompletionItem item;
    auditor.add_item(std::move(item));
    auditor.clear();
    CHECK(auditor.item_count() == 0);
}

TEST_CASE("CanvasCompletionAuditor — export", "[v23][p07][export]")
{
    CanvasCompletionAuditor auditor;
    CanvasCompletionItem item;
    item.is_implemented = true;
    item.has_error_handling = true;
    auditor.add_item(std::move(item));
    CHECK(auditor.export_json().find("\"complete\": 1") != std::string::npos);
    CHECK(auditor.export_markdown().find("Canvas Completion") != std::string::npos);
}

TEST_CASE("Integration — canvas auditor lifecycle", "[v23][p07][integration]")
{
    CanvasCompletionAuditor auditor;
    for (auto area : {CanvasCapabilityArea::kBoardLifecycle, CanvasCapabilityArea::kToolIntegration,
                      CanvasCapabilityArea::kObjectSerialization})
    {
        CanvasCompletionItem item;
        item.area = area;
        item.is_implemented = true;
        item.has_error_handling = true;
        auditor.add_item(std::move(item));
    }
    CanvasCompletionItem gap;
    gap.area = CanvasCapabilityArea::kCollaboration;
    auditor.add_item(std::move(gap));

    CHECK(auditor.item_count() == 4);
    CHECK(auditor.complete_items().size() == 3);
    CHECK(auditor.report().coverage_pct() == 75.0);
}
