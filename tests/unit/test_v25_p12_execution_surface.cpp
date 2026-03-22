/// @file test_v25_p12_execution_surface.cpp
/// @brief V25 Phase 12: Execution surface auditor tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P12: Dead controls detected", "[v25][p12]")
{
    ExecutionSurfaceAuditor auditor;
    ExecSurfaceItem item;
    item.control_id = "run.button";
    item.status = ExecSurfaceStatus::kDead;
    auditor.add_item(item);
    REQUIRE(auditor.dead_count() == 1);
    REQUIRE(auditor.dead_controls().size() == 1);
}

TEST_CASE("V25 P12: Live controls detected", "[v25][p12]")
{
    ExecutionSurfaceAuditor auditor;
    ExecSurfaceItem item;
    item.control_id = "save.button";
    item.status = ExecSurfaceStatus::kLive;
    auditor.add_item(item);
    REQUIRE(auditor.live_controls().size() == 1);
    REQUIRE(auditor.dead_count() == 0);
}

TEST_CASE("V25 P12: Item count", "[v25][p12]")
{
    ExecutionSurfaceAuditor auditor;
    REQUIRE(auditor.item_count() == 0);
    ExecSurfaceItem item;
    item.control_id = "test";
    auditor.add_item(item);
    REQUIRE(auditor.item_count() == 1);
}
