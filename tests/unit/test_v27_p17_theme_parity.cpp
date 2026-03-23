/// @file test_v27_p17_theme_parity.cpp
/// @brief V27 Phase 17: Theme audit, hardcoded detection, parity.
#include <catch2/catch_test_macros.hpp>
#include "core/V27ThemeAudit.h"
using namespace markamp::core;
TEST_CASE("V27 P17: Theme audit records entries", "[v27][p17]") {
    V27ThemeAudit audit;
    REQUIRE(audit.entry_count() == 0);
    audit.record({"Shell", "BgColor", V27ThemeAuditSeverity::kPass, "Correct", false});
    REQUIRE(audit.entry_count() == 1);
}
TEST_CASE("V27 P17: Theme audit summary", "[v27][p17]") {
    V27ThemeAudit audit;
    audit.record({"Shell", "BgColor", V27ThemeAuditSeverity::kPass, "OK", false});
    audit.record({"Editor", "FgColor", V27ThemeAuditSeverity::kFail, "Hardcoded", true});
    auto s = audit.summary();
    REQUIRE(s.total_checks == 2);
    REQUIRE(s.pass_count == 1);
    REQUIRE(s.fail_count == 1);
    REQUIRE(s.hardcoded_colors == 1);
    REQUIRE(s.pass_rate_pct() == 50);
}
TEST_CASE("V27 P17: Theme audit full audit baseline", "[v27][p17]") {
    V27ThemeAudit audit;
    audit.run_full_audit();
    REQUIRE(audit.entry_count() >= 10);
    auto s = audit.summary();
    REQUIRE(s.pass_count == s.total_checks); // baseline all pass
}
TEST_CASE("V27 P17: Hardcoded entries filter", "[v27][p17]") {
    V27ThemeAudit audit;
    audit.record({"A", "t1", V27ThemeAuditSeverity::kPass, "OK", false});
    audit.record({"B", "t2", V27ThemeAuditSeverity::kFail, "Hardcoded", true});
    auto hc = audit.hardcoded_entries();
    REQUIRE(hc.size() == 1);
    REQUIRE(hc[0].surface == "B");
}
