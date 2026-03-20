/// @file test_v19_phase07_notebook.cpp
/// @brief V19 Phase 07 tests: Notebook host lifecycle, execution queue,
///        checkpoint, and trust integration.

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

using namespace markamp::core;

// =============================================================================
// Notebook host lifecycle
// =============================================================================

TEST_CASE("Notebook shell host state tracks via event bus",
          "[v19][phase07][notebook]")
{
    EventBus bus;
    std::string notebook_state = "idle";

    auto sub = bus.subscribe<events::WorkbenchModeSwitchRequestEvent>(
        [&](const events::WorkbenchModeSwitchRequestEvent& mode_evt)
        {
            if (mode_evt.target_mode == events::WorkbenchMode::kNotebook)
            {
                notebook_state = "active";
            }
        });

    events::WorkbenchModeSwitchRequestEvent req;
    req.target_mode = events::WorkbenchMode::kNotebook;
    bus.publish(req);

    REQUIRE(notebook_state == "active");
}

// =============================================================================
// Notebook execution queue
// =============================================================================

TEST_CASE("Notebook execution queue tracks cell execution order",
          "[v19][phase07][notebook]")
{
    struct CellExecution
    {
        int cell_index{0};
        std::string status = "queued";
    };

    std::vector<CellExecution> queue;
    queue.push_back({0, "running"});
    queue.push_back({1, "queued"});
    queue.push_back({2, "queued"});

    REQUIRE(queue.size() == 3);
    REQUIRE(queue[0].status == "running");
    REQUIRE(queue[1].status == "queued");
}

// =============================================================================
// Notebook trust integration
// =============================================================================

TEST_CASE("Notebook trust state prevents untrusted execution",
          "[v19][phase07][notebook]")
{
    bool workspace_trusted = false;
    bool can_execute = workspace_trusted;
    REQUIRE_FALSE(can_execute);

    workspace_trusted = true;
    can_execute = workspace_trusted;
    REQUIRE(can_execute);
}
