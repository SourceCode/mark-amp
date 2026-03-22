/// @file test_v23_editor_completion.cpp
/// @brief V23 Phase 05 — Tests for EditorFeatureCompletionAuditor.

#include "core/EditorFeatureCompletionAuditor.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Label tests
// ============================================================================

TEST_CASE("EditorFeatureArea — labels", "[v23][p05][label]")
{
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kSearch)) == "Search");
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kPreview)) == "Preview");
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kNavigation)) == "Navigation");
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kDiagnostics)) == "Diagnostics");
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kCodeLens)) == "CodeLens");
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kPeek)) == "Peek");
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kSymbolLookup)) == "SymbolLookup");
    CHECK(std::string(editor_feature_area_label(EditorFeatureArea::kQuickFix)) == "QuickFix");
}

// ============================================================================
// Coverage item
// ============================================================================

TEST_CASE("EditorFeatureCoverageItem — completeness", "[v23][p05][item]")
{
    EditorFeatureCoverageItem item;

    SECTION("Complete when implemented + error handling")
    {
        item.is_implemented = true;
        item.has_error_handling = true;
        CHECK(item.is_complete());
    }

    SECTION("Incomplete without implementation")
    {
        item.has_error_handling = true;
        CHECK(!item.is_complete());
    }

    SECTION("Incomplete without error handling")
    {
        item.is_implemented = true;
        CHECK(!item.is_complete());
    }
}

// ============================================================================
// Registration and queries
// ============================================================================

TEST_CASE("EditorFeatureCompletionAuditor — registration", "[v23][p05][auditor]")
{
    EditorFeatureCompletionAuditor auditor;
    CHECK(auditor.item_count() == 0);

    EditorFeatureCoverageItem item;
    item.area = EditorFeatureArea::kSearch;
    item.feature_name = "keyword_search";
    auditor.add_item(std::move(item));

    CHECK(auditor.item_count() == 1);
}

TEST_CASE("EditorFeatureCompletionAuditor — query by area", "[v23][p05][query]")
{
    EditorFeatureCompletionAuditor auditor;

    EditorFeatureCoverageItem search;
    search.area = EditorFeatureArea::kSearch;
    auditor.add_item(std::move(search));

    EditorFeatureCoverageItem preview;
    preview.area = EditorFeatureArea::kPreview;
    auditor.add_item(std::move(preview));

    EditorFeatureCoverageItem preview2;
    preview2.area = EditorFeatureArea::kPreview;
    auditor.add_item(std::move(preview2));

    CHECK(auditor.items_by_area(EditorFeatureArea::kSearch).size() == 1);
    CHECK(auditor.items_by_area(EditorFeatureArea::kPreview).size() == 2);
    CHECK(auditor.items_by_area(EditorFeatureArea::kDiagnostics).empty());
}

TEST_CASE("EditorFeatureCompletionAuditor — complete vs incomplete", "[v23][p05][query]")
{
    EditorFeatureCompletionAuditor auditor;

    EditorFeatureCoverageItem complete;
    complete.is_implemented = true;
    complete.has_error_handling = true;
    auditor.add_item(std::move(complete));

    EditorFeatureCoverageItem placeholder;
    placeholder.is_implemented = false;
    auditor.add_item(std::move(placeholder));

    CHECK(auditor.complete_items().size() == 1);
    CHECK(auditor.incomplete_items().size() == 1);
}

TEST_CASE("EditorFeatureCompletionAuditor — placeholder items", "[v23][p05][query]")
{
    EditorFeatureCompletionAuditor auditor;

    EditorFeatureCoverageItem impl;
    impl.is_implemented = true;
    auditor.add_item(std::move(impl));

    EditorFeatureCoverageItem placeholder;
    placeholder.is_implemented = false;
    auditor.add_item(std::move(placeholder));

    CHECK(auditor.placeholder_items().size() == 1);
}

// ============================================================================
// Gap report
// ============================================================================

TEST_CASE("EditorFeatureCompletionAuditor — gap report", "[v23][p05][report]")
{
    EditorFeatureCompletionAuditor auditor;

    EditorFeatureCoverageItem search;
    search.area = EditorFeatureArea::kSearch;
    search.is_implemented = true;
    search.has_error_handling = true;
    auditor.add_item(std::move(search));

    EditorFeatureCoverageItem preview_gap;
    preview_gap.area = EditorFeatureArea::kPreview;
    preview_gap.is_implemented = false;
    auditor.add_item(std::move(preview_gap));

    EditorFeatureCoverageItem nav_gap;
    nav_gap.area = EditorFeatureArea::kNavigation;
    nav_gap.is_implemented = true;
    // Missing error handling
    auditor.add_item(std::move(nav_gap));

    auto report = auditor.gap_report();
    CHECK(report.total_features == 3);
    CHECK(report.implemented == 1);
    CHECK(report.placeholder == 2);
    CHECK(report.preview_gaps == 1);
    CHECK(report.navigation_gaps == 1);
    CHECK(report.search_gaps == 0);
    CHECK(report.has_gaps());
}

TEST_CASE("EditorFeatureGapReport — empty is 100%", "[v23][p05][report]")
{
    EditorFeatureGapReport report;
    CHECK(report.coverage_pct() == 100.0);
    CHECK(!report.has_gaps());
}

// ============================================================================
// Clear and export
// ============================================================================

TEST_CASE("EditorFeatureCompletionAuditor — clear", "[v23][p05][auditor]")
{
    EditorFeatureCompletionAuditor auditor;
    EditorFeatureCoverageItem item;
    auditor.add_item(std::move(item));
    auditor.clear();
    CHECK(auditor.item_count() == 0);
}

TEST_CASE("EditorFeatureCompletionAuditor — JSON export", "[v23][p05][export]")
{
    EditorFeatureCompletionAuditor auditor;

    EditorFeatureCoverageItem item;
    item.area = EditorFeatureArea::kSearch;
    item.feature_name = "keyword_search";
    item.is_implemented = true;
    item.has_error_handling = true;
    auditor.add_item(std::move(item));

    auto json = auditor.export_json();
    CHECK(json.find("\"implemented\": 1") != std::string::npos);
    CHECK(json.find("keyword_search") != std::string::npos);
}

TEST_CASE("EditorFeatureCompletionAuditor — Markdown export", "[v23][p05][export]")
{
    EditorFeatureCompletionAuditor auditor;

    EditorFeatureCoverageItem gap;
    gap.area = EditorFeatureArea::kPreview;
    gap.feature_name = "mermaid_preview";
    auditor.add_item(std::move(gap));

    auto md = auditor.export_markdown();
    CHECK(md.find("V23 Editor Feature Completion Report") != std::string::npos);
    CHECK(md.find("Incomplete Features") != std::string::npos);
    CHECK(md.find("mermaid_preview") != std::string::npos);
}

// ============================================================================
// Integration
// ============================================================================

TEST_CASE("Integration — full editor feature auditor lifecycle", "[v23][p05][integration]")
{
    EditorFeatureCompletionAuditor auditor;

    // Complete search features
    for (const auto& name : {"keyword_search", "regex_search", "phrase_search"})
    {
        EditorFeatureCoverageItem item;
        item.area = EditorFeatureArea::kSearch;
        item.feature_name = name;
        item.is_implemented = true;
        item.is_indexed = true;
        item.has_error_handling = true;
        item.has_async_support = true;
        auditor.add_item(std::move(item));
    }

    // Complete preview
    EditorFeatureCoverageItem markdown_preview;
    markdown_preview.area = EditorFeatureArea::kPreview;
    markdown_preview.feature_name = "markdown_preview";
    markdown_preview.is_implemented = true;
    markdown_preview.has_error_handling = true;
    auditor.add_item(std::move(markdown_preview));

    // Incomplete preview (placeholder)
    EditorFeatureCoverageItem mermaid_gap;
    mermaid_gap.area = EditorFeatureArea::kPreview;
    mermaid_gap.feature_name = "mermaid_preview";
    mermaid_gap.is_implemented = false;
    auditor.add_item(std::move(mermaid_gap));

    // Incomplete peek
    EditorFeatureCoverageItem peek_gap;
    peek_gap.area = EditorFeatureArea::kPeek;
    peek_gap.feature_name = "peek_definition";
    peek_gap.is_implemented = true;
    // Missing error handling
    auditor.add_item(std::move(peek_gap));

    CHECK(auditor.item_count() == 6);
    CHECK(auditor.complete_items().size() == 4);
    CHECK(auditor.incomplete_items().size() == 2);
    CHECK(auditor.placeholder_items().size() == 1);

    auto report = auditor.gap_report();
    CHECK(report.implemented == 4);
    CHECK(report.placeholder == 2);
    CHECK(report.preview_gaps == 1);
    CHECK(report.peek_gaps == 1);
    CHECK(report.search_gaps == 0);

    auto json = auditor.export_json();
    CHECK(!json.empty());
    auto md = auditor.export_markdown();
    CHECK(!md.empty());
}
