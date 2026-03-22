/// @file test_v23_release_gate_completion.cpp
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/ReleaseGateCompletionAuditor.h"
using namespace markamp::core;
TEST_CASE("P20 release gate labels", "[v23][p20]") {
    REQUIRE(std::string(release_gate_label(ReleaseGateArea::kPlaceholderEradication)) == "PlaceholderEradication");
    REQUIRE(std::string(release_gate_label(ReleaseGateArea::kDeadPathRemoval)) == "DeadPathRemoval");
}
TEST_CASE("P20 item completeness", "[v23][p20]") {
    ReleaseGateItem a; REQUIRE_FALSE(a.is_complete());
    a.is_resolved = true; a.has_error_handling = true; REQUIRE(a.is_complete());
}
TEST_CASE("P20 registration", "[v23][p20]") {
    ReleaseGateCompletionAuditor aud;
    aud.add_item({ReleaseGateArea::kLegacyDeletion, "old-toolbar", true, true, "f.cpp", 1});
    REQUIRE(aud.item_count() == 1);
}
TEST_CASE("P20 queries", "[v23][p20]") {
    ReleaseGateCompletionAuditor aud;
    aud.add_item({ReleaseGateArea::kCompletionGates, "gate-pass", true, true, "f.cpp", 1});
    aud.add_item({ReleaseGateArea::kCompletionGates, "gate-fail", false, false, "f.cpp", 2});
    REQUIRE(aud.complete_items().size() == 1); REQUIRE(aud.incomplete_items().size() == 1);
}
TEST_CASE("P20 report", "[v23][p20]") {
    ReleaseGateCompletionAuditor aud;
    aud.add_item({ReleaseGateArea::kFinalSweep, "sweep-src", true, true, "f.cpp", 1});
    auto r = aud.report(); REQUIRE(r.total == 1); REQUIRE_FALSE(r.has_gaps());
}
TEST_CASE("P20 clear", "[v23][p20]") {
    ReleaseGateCompletionAuditor aud;
    aud.add_item({ReleaseGateArea::kWaiverProcess, "waiver", true, true, "f.cpp", 1});
    aud.clear(); REQUIRE(aud.item_count() == 0);
}
TEST_CASE("P20 export json", "[v23][p20]") {
    ReleaseGateCompletionAuditor aud;
    aud.add_item({ReleaseGateArea::kScoreboard, "board", true, true, "f.cpp", 1});
    REQUIRE(aud.export_json().find("\"total\": 1") != std::string::npos);
}
TEST_CASE("P20 export markdown", "[v23][p20]") {
    ReleaseGateCompletionAuditor aud;
    REQUIRE(aud.export_markdown().find("Release Gate") != std::string::npos);
}
TEST_CASE("P20 integration", "[v23][p20]") {
    ReleaseGateCompletionAuditor aud;
    aud.add_item({ReleaseGateArea::kPlaceholderEradication, "remove-stubs", true, true, "f.cpp", 1});
    aud.add_item({ReleaseGateArea::kDeadPathRemoval, "delete-legacy", false, true, "f.cpp", 2});
    aud.add_item({ReleaseGateArea::kCertification, "final-cert", true, true, "f.cpp", 3});
    auto r = aud.report(); REQUIRE(r.complete == 2); REQUIRE(r.incomplete == 1);
}
