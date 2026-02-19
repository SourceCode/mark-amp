// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ReleaseAuditModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_audit() -> ReleaseAuditModel
{
    ReleaseAuditModel model;
    model.set_checks({
        {"c1", "interaction", "Keyboard navigation", AuditStatus::kPass, ""},
        {"c2", "interaction", "Touch targets", AuditStatus::kPass, ""},
        {"c3", "style", "Theme consistency", AuditStatus::kFail, "Sidebar mismatch"},
        {"c4", "accessibility", "Screen reader", AuditStatus::kPass, ""},
        {"c5", "performance", "Frame budget", AuditStatus::kSkipped, "Not tested"},
    });
    model.set_gates({
        {"g1", "Unit Tests", true, true},
        {"g2", "A11y Audit", true, true},
        {"g3", "Performance", true, false},
        {"g4", "Documentation", false, false},
    });
    return model;
}

TEST_CASE("Pass/fail/skip counts", "[audit][counts]")
{
    auto model = make_test_audit();
    REQUIRE(model.pass_count() == 3);
    REQUIRE(model.fail_count() == 1);
    REQUIRE(model.skip_count() == 1);
}

TEST_CASE("Filter by category", "[audit][filter]")
{
    auto model = make_test_audit();
    REQUIRE(model.by_category("interaction").size() == 2);
    REQUIRE(model.by_category("style").size() == 1);
}

TEST_CASE("Failing required gates", "[audit][gate]")
{
    auto model = make_test_audit();
    REQUIRE_FALSE(model.all_required_gates_passing());
    const auto failing = model.failing_gates();
    REQUIRE(failing.size() == 1);
    REQUIRE(failing[0] == "Performance");
}

TEST_CASE("Not release ready with failures", "[audit][readiness]")
{
    auto model = make_test_audit();
    REQUIRE_FALSE(model.is_release_ready());
}

TEST_CASE("Release ready when all pass", "[audit][readiness]")
{
    ReleaseAuditModel model;
    model.set_checks({
        {"c1", "interaction", "OK", AuditStatus::kPass, ""},
    });
    model.set_gates({
        {"g1", "Tests", true, true},
    });
    REQUIRE(model.is_release_ready());
}

TEST_CASE("Optional gate failure does not block", "[audit][gate]")
{
    ReleaseAuditModel model;
    model.set_checks({
        {"c1", "style", "OK", AuditStatus::kPass, ""},
    });
    model.set_gates({
        {"g1", "Required Gate", true, true},
        {"g2", "Optional Gate", false, false},
    });
    REQUIRE(model.all_required_gates_passing());
    REQUIRE(model.is_release_ready());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
