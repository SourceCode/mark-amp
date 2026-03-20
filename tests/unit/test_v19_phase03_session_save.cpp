/// @file test_v19_phase03_session_save.cpp
/// @brief V19 Phase 03 tests: DirtyStateCoordinator, workspace open pipeline,
///        session persistence contracts.

#include "core/DirtyStateCoordinator.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/WorkspaceLoadStateModel.h"
#include "core/WorkspaceOpenOrchestrator.h"
#include "core/WorkspaceSessionRestore.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// =============================================================================
// DirtyStateCoordinator — registration
// =============================================================================

TEST_CASE("DirtyStateCoordinator starts with no contributors",
          "[v19][phase03][dirty]")
{
    EventBus bus;
    DirtyStateCoordinator coord(bus);

    REQUIRE(coord.contributor_count() == 0);
    REQUIRE_FALSE(coord.is_session_dirty());
}

TEST_CASE("DirtyStateCoordinator registers and unregisters contributors",
          "[v19][phase03][dirty]")
{
    EventBus bus;
    DirtyStateCoordinator coord(bus);

    DirtyContributor contrib;
    contrib.surface_id = "editor:main";
    contrib.display_name = "main.md";
    contrib.is_dirty_fn = []() -> bool { return false; };
    contrib.save_fn = []() -> bool { return true; };
    coord.register_contributor(std::move(contrib));

    REQUIRE(coord.contributor_count() == 1);

    REQUIRE(coord.unregister_contributor("editor:main"));
    REQUIRE(coord.contributor_count() == 0);
    REQUIRE_FALSE(coord.unregister_contributor("nonexistent"));
}

// =============================================================================
// DirtyStateCoordinator — dirty detection
// =============================================================================

TEST_CASE("DirtyStateCoordinator detects dirty contributors",
          "[v19][phase03][dirty]")
{
    EventBus bus;
    DirtyStateCoordinator coord(bus);

    bool surface_dirty = false;

    DirtyContributor contrib;
    contrib.surface_id = "editor:0";
    contrib.display_name = "test.md";
    contrib.is_dirty_fn = [&]() -> bool { return surface_dirty; };
    contrib.save_fn = []() -> bool { return true; };
    coord.register_contributor(std::move(contrib));

    REQUIRE_FALSE(coord.is_session_dirty());

    surface_dirty = true;
    REQUIRE(coord.is_session_dirty());
    auto dirty = coord.get_dirty_contributors();
    REQUIRE(dirty.size() == 1);
    REQUIRE(dirty[0] == "editor:0");
}

// =============================================================================
// DirtyStateCoordinator — save_all
// =============================================================================

TEST_CASE("DirtyStateCoordinator save_all saves dirty contributors",
          "[v19][phase03][dirty]")
{
    EventBus bus;
    DirtyStateCoordinator coord(bus);

    bool dirty = true;
    int save_count = 0;

    DirtyContributor contrib;
    contrib.surface_id = "editor:0";
    contrib.display_name = "test.md";
    contrib.is_dirty_fn = [&]() -> bool { return dirty; };
    contrib.save_fn = [&]() -> bool
    {
        ++save_count;
        dirty = false;
        return true;
    };
    coord.register_contributor(std::move(contrib));

    int failures = coord.save_all();
    REQUIRE(failures == 0);
    REQUIRE(save_count == 1);
    REQUIRE_FALSE(coord.is_session_dirty());
}

// =============================================================================
// DirtyStateCoordinator — close_check
// =============================================================================

TEST_CASE("DirtyStateCoordinator close_check with clean session",
          "[v19][phase03][dirty]")
{
    EventBus bus;
    DirtyStateCoordinator coord(bus);

    REQUIRE(coord.close_check()); // Nothing dirty, safe to close
}

TEST_CASE("DirtyStateCoordinator close_check with dirty uses close policy",
          "[v19][phase03][dirty]")
{
    EventBus bus;
    DirtyStateCoordinator coord(bus);

    DirtyContributor contrib;
    contrib.surface_id = "notebook:0";
    contrib.display_name = "analysis.ipynb";
    contrib.is_dirty_fn = []() -> bool { return true; };
    contrib.save_fn = []() -> bool { return false; }; // can't save
    coord.register_contributor(std::move(contrib));

    // Without close policy, save_all fails → close disallowed
    REQUIRE_FALSE(coord.close_check());

    // With close policy that consents
    coord.set_close_policy([](const std::vector<std::string>& /*dirty*/) -> bool
    {
        return true; // user says "discard changes"
    });

    REQUIRE(coord.close_check());
}

// =============================================================================
// WorkspaceLoadStateModel — state transitions
// =============================================================================

TEST_CASE("WorkspaceLoadStateModel tracks state transitions",
          "[v19][phase03][workspace]")
{
    EventBus bus;
    WorkspaceLoadStateModel model(bus);
    REQUIRE(model.state() == WorkspaceLoadState::kIdle);

    model.set_state(WorkspaceLoadState::kLoading, "Opening workspace");
    REQUIRE(model.is_loading());

    model.set_state(WorkspaceLoadState::kReady);
    REQUIRE(model.is_ready());
}

// =============================================================================
// WorkspaceSessionRestore — basic contracts
// =============================================================================

TEST_CASE("WorkspaceSessionRestore provides snapshot contracts",
          "[v19][phase03][session]")
{
    WorkspaceSessionRestore restore;

    // Session restore should provide basic contracts
    REQUIRE_FALSE(restore.has_snapshot("default"));
}
