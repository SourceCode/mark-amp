/// @file test_v23_rendering_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/RenderingOutputCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P13 rendering labels", "[v23][p13]") {
    REQUIRE(std::string(rendering_area_label(RenderingArea::kDiagramRenderer)) == "DiagramRenderer");
    REQUIRE(std::string(rendering_area_label(RenderingArea::kPDFExtraction)) == "PDFExtraction");
}
TEST_CASE("P13 item completeness", "[v23][p13]") {
    RenderingCompletionItem a; REQUIRE_FALSE(a.is_complete());
    a.is_real_renderer = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P13 auditor registration", "[v23][p13]") {
    RenderingOutputCompletionAuditor aud;
    aud.add_item({RenderingArea::kDiagramRenderer, "mermaid", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P13 queries", "[v23][p13]") {
    RenderingOutputCompletionAuditor aud;
    aud.add_item({RenderingArea::kExportTemplate, "html-export", true, true, "f.cpp", 1});
    aud.add_item({RenderingArea::kExportTemplate, "pdf-export", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P13 report", "[v23][p13]") {
    RenderingOutputCompletionAuditor aud;
    aud.add_item({RenderingArea::kPrintPrep, "print", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P13 clear", "[v23][p13]") {
    RenderingOutputCompletionAuditor aud;
    aud.add_item({RenderingArea::kTokenizerPipeline, "tok", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P13 export json", "[v23][p13]") {
    RenderingOutputCompletionAuditor aud;
    aud.add_item({RenderingArea::kMediaFallback, "img", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P13 export markdown", "[v23][p13]") {
    RenderingOutputCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Rendering Output") != std::string::npos);
}
TEST_CASE("P13 integration", "[v23][p13]") {
    RenderingOutputCompletionAuditor aud;
    aud.add_item({RenderingArea::kDiagramRenderer, "mermaid", true, true, "f.cpp", 1});
    aud.add_item({RenderingArea::kContentFidelity, "round-trip", false, true, "f.cpp", 2});
    aud.add_item({RenderingArea::kRegressionGates, "snapshot", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
