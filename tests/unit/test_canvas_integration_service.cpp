/// test_canvas_integration_service.cpp — Unit tests
#include "core/CanvasIntegrationService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyncJobStatus: enum values", "[canvas_integration_service]")
{
    REQUIRE(static_cast<int>(SyncJobStatus::kPending) != static_cast<int>(SyncJobStatus::kRunning));
}
