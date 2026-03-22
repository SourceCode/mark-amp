/// @file test_v23_structured_data_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/StructuredDataCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P15 structured data labels", "[v23][p15]") {
    REQUIRE(std::string(structured_data_label(StructuredDataArea::kAVTableEditor)) == "AVTableEditor");
    REQUIRE(std::string(structured_data_label(StructuredDataArea::kKnowledgeGraph)) == "KnowledgeGraph");
}
TEST_CASE("P15 item completeness", "[v23][p15]") {
    StructuredDataItem a; REQUIRE_FALSE(a.is_complete());
    a.is_real_data_flow = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P15 registration", "[v23][p15]") {
    StructuredDataCompletionAuditor aud;
    aud.add_item({StructuredDataArea::kBacklinks, "backlink-nav", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P15 queries", "[v23][p15]") {
    StructuredDataCompletionAuditor aud;
    aud.add_item({StructuredDataArea::kDeckFlashcard, "deck-parse", true, true, "f.cpp", 1});
    aud.add_item({StructuredDataArea::kDeckFlashcard, "deck-persist", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P15 report", "[v23][p15]") {
    StructuredDataCompletionAuditor aud;
    aud.add_item({StructuredDataArea::kSearchIndexing, "av-index", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P15 clear", "[v23][p15]") {
    StructuredDataCompletionAuditor aud;
    aud.add_item({StructuredDataArea::kActivityTimeline, "timeline", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P15 export json", "[v23][p15]") {
    StructuredDataCompletionAuditor aud;
    aud.add_item({StructuredDataArea::kTaskCalendar, "task", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P15 export markdown", "[v23][p15]") {
    StructuredDataCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Structured Data") != std::string::npos);
}
TEST_CASE("P15 integration", "[v23][p15]") {
    StructuredDataCompletionAuditor aud;
    aud.add_item({StructuredDataArea::kAVTableEditor, "cell-edit", true, true, "f.cpp", 1});
    aud.add_item({StructuredDataArea::kKnowledgeGraph, "graph-query", false, true, "f.cpp", 2});
    aud.add_item({StructuredDataArea::kDataMigration, "migrate-v2", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
