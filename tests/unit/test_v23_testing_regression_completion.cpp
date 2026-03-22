/// @file test_v23_testing_regression_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/TestingRegressionCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P19 testing labels", "[v23][p19]") {
    REQUIRE(std::string(testing_area_label(TestingArea::kPlaceholderTests)) == "PlaceholderTests");
    REQUIRE(std::string(testing_area_label(TestingArea::kFuzzHarness)) == "FuzzHarness");
}
TEST_CASE("P19 item completeness", "[v23][p19]") {
    TestingCompletionItem a; REQUIRE_FALSE(a.is_complete());
    a.is_real_test = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P19 registration", "[v23][p19]") {
    TestingRegressionCompletionAuditor aud;
    aud.add_item({TestingArea::kFuzzHarness, "fuzz-markdown", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P19 queries", "[v23][p19]") {
    TestingRegressionCompletionAuditor aud;
    aud.add_item({TestingArea::kSmokeE2E, "create-save", true, true, "f.cpp", 1});
    aud.add_item({TestingArea::kSmokeE2E, "restart-restore", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P19 report", "[v23][p19]") {
    TestingRegressionCompletionAuditor aud;
    aud.add_item({TestingArea::kFailureModeTests, "offline-retry", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P19 clear", "[v23][p19]") {
    TestingRegressionCompletionAuditor aud;
    aud.add_item({TestingArea::kCorpusCoverage, "corpus", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P19 export json", "[v23][p19]") {
    TestingRegressionCompletionAuditor aud;
    aud.add_item({TestingArea::kRestartTests, "restart", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P19 export markdown", "[v23][p19]") {
    TestingRegressionCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Testing Regression") != std::string::npos);
}
TEST_CASE("P19 integration", "[v23][p19]") {
    TestingRegressionCompletionAuditor aud;
    aud.add_item({TestingArea::kPlaceholderTests, "replace-stub", true, true, "f.cpp", 1});
    aud.add_item({TestingArea::kScoreboard, "completion-board", false, true, "f.cpp", 2});
    aud.add_item({TestingArea::kUnsupportedStateTests, "unsupported-renderer", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
