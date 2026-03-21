/// @file test_v20_notebook_command_router.cpp
/// @brief V20 Phase 03 – NotebookCommandRouter unit tests.

#include "core/NotebookCommandRouter.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("NotebookCommandRouter: construction", "[v20][notebook-router]")
{
    EventBus bus;
    NotebookCommandRouter router(bus);

    REQUIRE(router.update_count() == 0);
    REQUIRE_FALSE(router.is_notebook_active());
    REQUIRE(router.active_notebook_id().empty());
}

// ============================================================================
// Set context
// ============================================================================

TEST_CASE("NotebookCommandRouter: set context", "[v20][notebook-router]")
{
    EventBus bus;
    NotebookCommandRouter router(bus);

    NotebookContext ctx;
    ctx.active_notebook_id = ArtifactId{"nb-1"};
    ctx.active_cell_id = "cell-1";
    ctx.is_notebook_focused = true;
    ctx.is_cell_editing = true;

    router.set_context(ctx);

    REQUIRE(router.is_notebook_active());
    REQUIRE(router.active_notebook_id() == ArtifactId{"nb-1"});
    REQUIRE(router.active_cell_id() == "cell-1");
    REQUIRE(router.should_save_notebook());
    REQUIRE(router.update_count() == 1);
}

// ============================================================================
// Clear context
// ============================================================================

TEST_CASE("NotebookCommandRouter: clear context", "[v20][notebook-router]")
{
    EventBus bus;
    NotebookCommandRouter router(bus);

    NotebookContext ctx;
    ctx.active_notebook_id = ArtifactId{"nb-1"};
    ctx.is_notebook_focused = true;
    router.set_context(ctx);

    router.clear_context();

    REQUIRE_FALSE(router.is_notebook_active());
    REQUIRE(router.active_notebook_id().empty());
    REQUIRE_FALSE(router.should_save_notebook());
    REQUIRE(router.update_count() == 2);
}

// ============================================================================
// Active cell
// ============================================================================

TEST_CASE("NotebookCommandRouter: set active cell", "[v20][notebook-router]")
{
    EventBus bus;
    NotebookCommandRouter router(bus);

    NotebookContext ctx;
    ctx.active_notebook_id = ArtifactId{"nb-1"};
    ctx.is_notebook_focused = true;
    router.set_context(ctx);

    router.set_active_cell("cell-3");
    REQUIRE(router.active_cell_id() == "cell-3");
}

// ============================================================================
// NotebookContext helpers
// ============================================================================

TEST_CASE("NotebookContext: has_active queries", "[v20][notebook-router]")
{
    NotebookContext ctx;
    REQUIRE_FALSE(ctx.has_active_notebook());
    REQUIRE_FALSE(ctx.has_active_cell());

    ctx.active_notebook_id = ArtifactId{"nb"};
    REQUIRE(ctx.has_active_notebook());

    ctx.active_cell_id = "cell-1";
    REQUIRE(ctx.has_active_cell());
}

// ============================================================================
// Save targeting
// ============================================================================

TEST_CASE("NotebookCommandRouter: save targets notebook when focused", "[v20][notebook-router]")
{
    EventBus bus;
    NotebookCommandRouter router(bus);

    // Not focused — save should NOT target notebook
    NotebookContext unfocused;
    unfocused.active_notebook_id = ArtifactId{"nb-1"};
    unfocused.is_notebook_focused = false;
    router.set_context(unfocused);
    REQUIRE_FALSE(router.should_save_notebook());

    // Focused — save SHOULD target notebook
    NotebookContext focused;
    focused.active_notebook_id = ArtifactId{"nb-1"};
    focused.is_notebook_focused = true;
    router.set_context(focused);
    REQUIRE(router.should_save_notebook());
}

// ============================================================================
// Phase 03 events
// ============================================================================

TEST_CASE("Phase 03 events: NotebookArtifactCreatedEvent", "[v20][notebook-events]")
{
    events::NotebookArtifactCreatedEvent evt;
    evt.artifact_id = "nb-1";
    evt.title = "My Notebook";
    evt.kernel_language = "python";
    evt.initial_cell_count = 2;

    REQUIRE(evt.artifact_id == "nb-1");
    REQUIRE(evt.title == "My Notebook");
    REQUIRE(evt.initial_cell_count == 2);
}

TEST_CASE("Phase 03 events: NotebookContextChangedEvent", "[v20][notebook-events]")
{
    events::NotebookContextChangedEvent evt;
    evt.artifact_id = "nb-1";
    evt.active_cell_id = "cell-2";
    evt.is_focused = true;

    REQUIRE(evt.is_focused);
}

TEST_CASE("Phase 03 events: NotebookKernelStateChangedEvent", "[v20][notebook-events]")
{
    events::NotebookKernelStateChangedEvent evt;
    evt.artifact_id = "nb-1";
    evt.kernel_state = "busy";

    REQUIRE(evt.kernel_state == "busy");
}
