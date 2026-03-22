/// @file test_v23_node_editor_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/NodeEditorCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P14 node editor labels", "[v23][p14]") {
    REQUIRE(std::string(node_editor_area_label(NodeEditorArea::kDomainRuntime)) == "DomainRuntime");
    REQUIRE(std::string(node_editor_area_label(NodeEditorArea::kHitTesting)) == "HitTesting");
}
TEST_CASE("P14 item completeness", "[v23][p14]") {
    NodeEditorCompletionItem a; REQUIRE_FALSE(a.is_complete());
    a.is_real_execution = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P14 registration", "[v23][p14]") {
    NodeEditorCompletionAuditor aud;
    aud.add_item({NodeEditorArea::kDomainRuntime, "audio-runtime", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P14 queries", "[v23][p14]") {
    NodeEditorCompletionAuditor aud;
    aud.add_item({NodeEditorArea::kTransformEvaluation, "expr-eval", true, true, "f.cpp", 1});
    aud.add_item({NodeEditorArea::kTransformEvaluation, "data-transform", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P14 report", "[v23][p14]") {
    NodeEditorCompletionAuditor aud;
    aud.add_item({NodeEditorArea::kPersistenceExport, "graph-save", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P14 clear", "[v23][p14]") {
    NodeEditorCompletionAuditor aud;
    aud.add_item({NodeEditorArea::kPreviewSystem, "preview", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P14 export json", "[v23][p14]") {
    NodeEditorCompletionAuditor aud;
    aud.add_item({NodeEditorArea::kGraphSerialization, "serialize", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P14 export markdown", "[v23][p14]") {
    NodeEditorCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Node Editor") != std::string::npos);
}
TEST_CASE("P14 integration", "[v23][p14]") {
    NodeEditorCompletionAuditor aud;
    aud.add_item({NodeEditorArea::kDomainRuntime, "data-gen", true, true, "f.cpp", 1});
    aud.add_item({NodeEditorArea::kInteractionFidelity, "bezier-hit", false, true, "f.cpp", 2});
    aud.add_item({NodeEditorArea::kCompletionGates, "graph-fixture", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
