#include "core/VisualCleanupCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("VisualCleanupCoordinator icon migration complete", "[v22][cleanup]")
{
    VisualCleanupCoordinator coord;
    auto status = coord.icon_migration_status();

    REQUIRE(status.total_icons > 0);
    REQUIRE(status.migrated_icons == status.total_icons);
    REQUIRE(status.legacy_icons_remaining == 0);
    REQUIRE(status.migration_percent == 100.0F);
}

TEST_CASE("VisualCleanupCoordinator no hardcoded literals", "[v22][cleanup]")
{
    VisualCleanupCoordinator coord;
    auto result = coord.audit_hardcoded_literals();

    REQUIRE(result.hardcoded_colors == 0);
    REQUIRE(result.hardcoded_sizes == 0);
    REQUIRE(result.hardcoded_fonts == 0);
    REQUIRE(result.total_violations == 0);
}

TEST_CASE("VisualCleanupCoordinator quality scoreboard", "[v22][cleanup]")
{
    VisualCleanupCoordinator coord;
    auto board = coord.quality_scoreboard();

    REQUIRE(board.overall_score >= 95.0F);
    REQUIRE(board.total_surfaces > 0);
    REQUIRE(board.surfaces_passing == board.total_surfaces);
    REQUIRE(board.blocking_issues == 0);
}

TEST_CASE("VisualCleanupCoordinator exit criteria all pass", "[v22][cleanup]")
{
    VisualCleanupCoordinator coord;
    auto gates = coord.check_exit_criteria();

    REQUIRE(gates.size() >= 5);
    for (const auto& gate : gates)
    {
        REQUIRE(gate.passes);
        REQUIRE_FALSE(gate.gate_name.empty());
        REQUIRE_FALSE(gate.reason.empty());
    }
}
