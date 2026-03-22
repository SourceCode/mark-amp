/// @file test_v23_surface_completion.cpp
/// @brief V23 Phase 08 — Tests for SpecializedSurfaceCompletionAuditor.

#include "core/SpecializedSurfaceCompletionAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SurfaceCapabilityArea — labels", "[v23][p08][label]")
{
    CHECK(std::string(surface_capability_label(SurfaceCapabilityArea::kPDFViewer)) == "PDFViewer");
    CHECK(std::string(surface_capability_label(SurfaceCapabilityArea::kHistoryTimeline)) == "HistoryTimeline");
    CHECK(std::string(surface_capability_label(SurfaceCapabilityArea::kDiffViewer)) == "DiffViewer");
    CHECK(std::string(surface_capability_label(SurfaceCapabilityArea::kTerminalPanel)) == "TerminalPanel");
}

TEST_CASE("SurfaceCompletionItem — completeness", "[v23][p08][item]")
{
    SurfaceCompletionItem item;
    CHECK(!item.is_complete());
    item.is_implemented = true;
    item.has_error_handling = true;
    CHECK(item.is_complete());
}

TEST_CASE("SpecializedSurfaceCompletionAuditor — registration", "[v23][p08][auditor]")
{
    SpecializedSurfaceCompletionAuditor auditor;
    SurfaceCompletionItem item;
    auditor.add_item(std::move(item));
    CHECK(auditor.item_count() == 1);
}

TEST_CASE("SpecializedSurfaceCompletionAuditor — query by area", "[v23][p08][query]")
{
    SpecializedSurfaceCompletionAuditor auditor;
    SurfaceCompletionItem pdf;
    pdf.area = SurfaceCapabilityArea::kPDFViewer;
    auditor.add_item(std::move(pdf));
    SurfaceCompletionItem diff;
    diff.area = SurfaceCapabilityArea::kDiffViewer;
    auditor.add_item(std::move(diff));

    CHECK(auditor.items_by_area(SurfaceCapabilityArea::kPDFViewer).size() == 1);
    CHECK(auditor.items_by_area(SurfaceCapabilityArea::kTerminalPanel).empty());
}

TEST_CASE("SpecializedSurfaceCompletionAuditor — complete vs incomplete", "[v23][p08][query]")
{
    SpecializedSurfaceCompletionAuditor auditor;
    SurfaceCompletionItem ok;
    ok.is_implemented = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));
    SurfaceCompletionItem gap;
    auditor.add_item(std::move(gap));

    CHECK(auditor.complete_items().size() == 1);
    CHECK(auditor.incomplete_items().size() == 1);
}

TEST_CASE("SpecializedSurfaceCompletionAuditor — report", "[v23][p08][report]")
{
    SpecializedSurfaceCompletionAuditor auditor;
    SurfaceCompletionItem ok;
    ok.is_implemented = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));

    auto r = auditor.report();
    CHECK(r.total == 1);
    CHECK(r.complete == 1);
    CHECK(!r.has_gaps());
}

TEST_CASE("SpecializedSurfaceCompletionAuditor — clear", "[v23][p08][auditor]")
{
    SpecializedSurfaceCompletionAuditor auditor;
    SurfaceCompletionItem item;
    auditor.add_item(std::move(item));
    auditor.clear();
    CHECK(auditor.item_count() == 0);
}

TEST_CASE("SpecializedSurfaceCompletionAuditor — export", "[v23][p08][export]")
{
    SpecializedSurfaceCompletionAuditor auditor;
    SurfaceCompletionItem item;
    item.is_implemented = true;
    item.has_error_handling = true;
    auditor.add_item(std::move(item));
    CHECK(auditor.export_json().find("\"complete\": 1") != std::string::npos);
    CHECK(auditor.export_markdown().find("Specialized Surface") != std::string::npos);
}

TEST_CASE("Integration — surface auditor lifecycle", "[v23][p08][integration]")
{
    SpecializedSurfaceCompletionAuditor auditor;
    for (auto area : {SurfaceCapabilityArea::kPDFViewer, SurfaceCapabilityArea::kDiffViewer})
    {
        SurfaceCompletionItem item;
        item.area = area;
        item.is_implemented = true;
        item.has_error_handling = true;
        auditor.add_item(std::move(item));
    }
    SurfaceCompletionItem gap;
    gap.area = SurfaceCapabilityArea::kTerminalPanel;
    auditor.add_item(std::move(gap));

    CHECK(auditor.item_count() == 3);
    CHECK(auditor.complete_items().size() == 2);
    CHECK(auditor.report().has_gaps());
}
