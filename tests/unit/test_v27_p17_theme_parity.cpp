/// @file test_v27_p17_theme_parity.cpp
/// @brief V27 Phase 17: Theme audit, hardcoded detection, parity, surface filter.
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
    auto summary = audit.summary();
    REQUIRE(summary.total_checks == 2);
    REQUIRE(summary.pass_count == 1);
    REQUIRE(summary.fail_count == 1);
    REQUIRE(summary.hardcoded_colors == 1);
    REQUIRE(summary.pass_rate_pct() == 50);
}
TEST_CASE("V27 P17: Theme audit full audit baseline", "[v27][p17]") {
    V27ThemeAudit audit;
    audit.run_full_audit();
    REQUIRE(audit.entry_count() >= 10);
    auto summary = audit.summary();
    REQUIRE(summary.pass_count == summary.total_checks);
}
TEST_CASE("V27 P17: Hardcoded entries filter", "[v27][p17]") {
    V27ThemeAudit audit;
    audit.record({"A", "t1", V27ThemeAuditSeverity::kPass, "OK", false});
    audit.record({"B", "t2", V27ThemeAuditSeverity::kFail, "Hardcoded", true});
    auto hardcoded = audit.hardcoded_entries();
    REQUIRE(hardcoded.size() == 1);
    REQUIRE(hardcoded[0].surface == "B");
}
TEST_CASE("V27 P17: Surface-specific entry filter", "[v27][p17]") {
    V27ThemeAudit audit;
    audit.record({"Shell", "BgColor", V27ThemeAuditSeverity::kPass, "OK", false});
    audit.record({"Shell", "FgColor", V27ThemeAuditSeverity::kPass, "OK", false});
    audit.record({"Editor", "BgColor", V27ThemeAuditSeverity::kPass, "OK", false});
    auto shell_entries = audit.entries_for_surface("Shell");
    REQUIRE(shell_entries.size() == 2);
    auto editor_entries = audit.entries_for_surface("Editor");
    REQUIRE(editor_entries.size() == 1);
}
TEST_CASE("V27 P17: Empty audit has 100% pass rate", "[v27][p17]") {
    V27ThemeAudit audit;
    REQUIRE(audit.summary().pass_rate_pct() == 100);
}
