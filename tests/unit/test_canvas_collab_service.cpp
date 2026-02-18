/// test_canvas_collab_service.cpp — Unit tests
#include "core/CanvasCollabService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PresenceStatus: enum values", "[canvas_collab_service]")
{
    REQUIRE(static_cast<int>(PresenceStatus::kActive) != static_cast<int>(PresenceStatus::kIdle));
}

TEST_CASE("PresenceStatus: compiles", "[canvas_collab_service]")
{
    static_assert(sizeof(PresenceStatus) > 0);
}
