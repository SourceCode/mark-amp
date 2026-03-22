/// @file test_v25_p02_artifact_lifecycle.cpp
/// @brief V25 Phase 02: Artifact creation controller tests.
#include <catch2/catch_test_macros.hpp>
#include "core/ArtifactCreationController.h"
#include "core/Config.h"

using namespace markamp::core;

TEST_CASE("V25 P02: Controller routes creation through service", "[v25][p02]")
{
    EventBus bus;
    Config cfg;
    ArtifactRegistry registry(bus);
    ArtifactCreationService service(bus, registry, cfg);
    ArtifactCreationController controller(service);

    auto result = controller.create_from_surface("TabBar", ArtifactKind::kTextFile);
    REQUIRE(result.success);
    REQUIRE(controller.routed_count() == 1);
}

TEST_CASE("V25 P02: Controller tracks bypass count", "[v25][p02]")
{
    EventBus bus;
    Config cfg;
    ArtifactRegistry registry(bus);
    ArtifactCreationService service(bus, registry, cfg);
    ArtifactCreationController controller(service);

    REQUIRE(controller.all_routed());
    controller.record_bypass("MainFrame");
    REQUIRE_FALSE(controller.all_routed());
    REQUIRE(controller.bypass_count() == 1);
}

TEST_CASE("V25 P02: Controller counts by surface", "[v25][p02]")
{
    EventBus bus;
    Config cfg;
    ArtifactRegistry registry(bus);
    ArtifactCreationService service(bus, registry, cfg);
    ArtifactCreationController controller(service);

    controller.create_from_surface("TabBar", ArtifactKind::kTextFile);
    controller.create_from_surface("TabBar", ArtifactKind::kNotebook);
    controller.create_from_surface("FileTree", ArtifactKind::kCanvas);

    REQUIRE(controller.count_by_surface("TabBar") == 2);
    REQUIRE(controller.count_by_surface("FileTree") == 1);
}

TEST_CASE("V25 P02: Route history preserves artifact kinds", "[v25][p02]")
{
    EventBus bus;
    Config cfg;
    ArtifactRegistry registry(bus);
    ArtifactCreationService service(bus, registry, cfg);
    ArtifactCreationController controller(service);

    controller.create_from_surface("TabBar", ArtifactKind::kNotebook);
    auto& history = controller.route_history();
    REQUIRE(history.size() == 1);
    REQUIRE(history[0].kind == ArtifactKind::kNotebook);
    REQUIRE(history[0].source_surface == "TabBar");
}
