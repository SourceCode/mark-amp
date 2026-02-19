// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/AuditModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Audit gates pass and fail counting", "[audit][gates]")
{
    AuditModel model;
    model.set_gates({
        {"g1", "Draw tools", "fundamentals", GateStatus::kPassed, ""},
        {"g2", "Keyboard nav", "accessibility", GateStatus::kPassed, ""},
        {"g3", "Frame time", "performance", GateStatus::kFailed, "Over budget"},
    });
    REQUIRE(model.passed_count() == 2);
    REQUIRE(model.failed_count() == 1);
    REQUIRE_FALSE(model.all_passed());
}

TEST_CASE("Update gate status", "[audit][update]")
{
    AuditModel model;
    model.set_gates({{"g1", "Draw", "fundamentals", GateStatus::kNotRun, ""}});
    REQUIRE(model.not_run_count() == 1);
    model.update_gate("g1", GateStatus::kPassed, "All checks OK");
    REQUIRE(model.passed_count() == 1);
    REQUIRE(model.not_run_count() == 0);
}

TEST_CASE("Category filtering", "[audit][category]")
{
    AuditModel model;
    model.set_gates({
        {"g1", "Draw", "fundamentals", GateStatus::kPassed, ""},
        {"g2", "Style", "fundamentals", GateStatus::kPassed, ""},
        {"g3", "macOS", "platform", GateStatus::kPassed, ""},
    });
    REQUIRE(model.gates_in_category("fundamentals").size() == 2);
    REQUIRE(model.gates_in_category("platform").size() == 1);
}

TEST_CASE("Release readiness requires all passed", "[audit][release]")
{
    AuditModel model;
    model.set_gates({
        {"g1", "Draw", "fundamentals", GateStatus::kPassed, ""},
        {"g2", "Render", "performance", GateStatus::kPassed, ""},
    });
    REQUIRE(model.is_release_ready());
}

TEST_CASE("Release not ready with not-run gates", "[audit][release]")
{
    AuditModel model;
    model.set_gates({
        {"g1", "Draw", "fundamentals", GateStatus::kPassed, ""},
        {"g2", "Pending", "performance", GateStatus::kNotRun, ""},
    });
    REQUIRE_FALSE(model.is_release_ready());
}

TEST_CASE("Empty gates not release ready", "[audit][release]")
{
    AuditModel model;
    REQUIRE_FALSE(model.is_release_ready());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
