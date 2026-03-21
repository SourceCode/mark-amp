/// @file test_v20_notebook_integration.cpp
/// @brief V20 Phase 03 – End-to-end notebook integration tests.

#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/NotebookArtifactLifecycle.h"
#include "core/NotebookCommandRouter.h"
#include "core/NotebookStorageResolver.h"
#include "core/UnsavedDocumentBuffer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct NotebookIntegrationFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    UnsavedDocumentBufferManager buffer_mgr;
    NotebookArtifactLifecycle lifecycle{bus, registry, creation, buffer_mgr, config};
    NotebookStorageResolver storage_resolver;
    NotebookCommandRouter command_router{bus};
};

// ============================================================================
// Full create → add cells → save flow
// ============================================================================

TEST_CASE("Integration: Create → Add cells → Save As", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    // 1. Resolve storage
    auto location = fix.storage_resolver.resolve("/workspace");
    REQUIRE(location.is_workspace);

    // 2. Create notebook
    NotebookPlacementContext ctx;
    ctx.entry_point = "palette";
    ctx.target_directory = location.resolved_path;

    auto result = fix.lifecycle.create_notebook(ctx, "Data Analysis", "python");
    REQUIRE(result.ok());

    // 3. Add cells
    fix.lifecycle.add_cell(result.artifact_id, NotebookCellKind::kCode, "import pandas as pd");
    fix.lifecycle.add_cell(result.artifact_id, NotebookCellKind::kMarkdown, "## Results");
    REQUIRE(fix.lifecycle.cell_count(result.artifact_id) == 4); // 2 starter + 2 added

    // 4. Set command context
    NotebookContext nb_ctx;
    nb_ctx.active_notebook_id = result.artifact_id;
    nb_ctx.is_notebook_focused = true;
    fix.command_router.set_context(nb_ctx);
    REQUIRE(fix.command_router.should_save_notebook());

    // 5. Save
    auto save_result = fix.lifecycle.save_as(result.artifact_id,
                                               "/workspace/notebooks/analysis.ipynb");
    REQUIRE(save_result.ok());

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record->is_saved());
    REQUIRE(record->file_path.value() == "/workspace/notebooks/analysis.ipynb");
}

// ============================================================================
// Open → Focus → Save
// ============================================================================

TEST_CASE("Integration: Open → Focus → Save", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    // Open existing notebook
    auto result = fix.lifecycle.open_notebook("/workspace/ml-model.ipynb");
    REQUIRE(result.ok());

    // Focus the notebook
    NotebookContext ctx;
    ctx.active_notebook_id = result.artifact_id;
    ctx.is_notebook_focused = true;
    ctx.active_cell_id = "cell-1";
    fix.command_router.set_context(ctx);

    // Edit → dirty
    fix.registry.set_state(result.artifact_id, ArtifactLifecycleState::kDirty);

    // Save
    auto save_result = fix.lifecycle.save(result.artifact_id);
    REQUIRE(save_result.ok());
    REQUIRE(fix.registry.find(result.artifact_id)->is_saved());
}

// ============================================================================
// Rename open notebook
// ============================================================================

TEST_CASE("Integration: Rename notebook updates display", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_notebook(ctx, "Draft");

    auto rename_result = fix.lifecycle.rename(result.artifact_id, "Final Report");
    REQUIRE(rename_result.ok());
    REQUIRE(fix.registry.find(result.artifact_id)->display_name == "Final Report");
}

// ============================================================================
// Duplicate with cells
// ============================================================================

TEST_CASE("Integration: Duplicate preserves cells", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto original = fix.lifecycle.create_notebook(ctx, "Template");

    fix.lifecycle.add_cell(original.artifact_id, NotebookCellKind::kCode, "# template code");

    int orig_cell_count = fix.lifecycle.cell_count(original.artifact_id);

    auto dup = fix.lifecycle.duplicate(original.artifact_id);
    REQUIRE(dup.ok());

    // Both should have cells
    REQUIRE(fix.lifecycle.cell_count(original.artifact_id) == orig_cell_count);
    REQUIRE(fix.lifecycle.cell_count(dup.artifact_id) >= 2); // at least starter cells
}

// ============================================================================
// Context switch: editor ↔ notebook  
// ============================================================================

TEST_CASE("Integration: Context switch editor ↔ notebook", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    // Start in editor mode
    REQUIRE_FALSE(fix.command_router.is_notebook_active());

    // Switch to notebook
    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto nb = fix.lifecycle.create_notebook(ctx, "Notebook");

    NotebookContext nb_ctx;
    nb_ctx.active_notebook_id = nb.artifact_id;
    nb_ctx.is_notebook_focused = true;
    fix.command_router.set_context(nb_ctx);
    REQUIRE(fix.command_router.is_notebook_active());
    REQUIRE(fix.command_router.should_save_notebook());

    // Switch back to editor
    fix.command_router.clear_context();
    REQUIRE_FALSE(fix.command_router.is_notebook_active());
    REQUIRE_FALSE(fix.command_router.should_save_notebook());
}

// ============================================================================
// Storage resolution integration
// ============================================================================

TEST_CASE("Integration: Storage resolution for notebook placement", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    // Workspace notebook
    auto ws_loc = fix.storage_resolver.resolve("/my-project");
    REQUIRE(ws_loc.is_workspace);
    REQUIRE(ws_loc.resolved_path == "/my-project/notebooks");

    // Legacy notebook
    auto legacy_loc = fix.storage_resolver.resolve("", "", "/home/.markamp/kb");
    REQUIRE(legacy_loc.is_legacy);

    // Workspace path check
    REQUIRE(fix.storage_resolver.is_workspace_path("/my-project/notebooks/nb.ipynb", "/my-project"));
    REQUIRE_FALSE(fix.storage_resolver.is_legacy_path("/my-project/notebooks/nb.ipynb", "/home/.markamp/kb"));
}

// ============================================================================
// Close flow with dirty check
// ============================================================================

TEST_CASE("Integration: Close dirty notebook", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_notebook(ctx, "Temp");

    // Has unsaved changes
    REQUIRE(fix.lifecycle.has_unsaved_changes(result.artifact_id));

    // Close anyway
    auto close_result = fix.lifecycle.close(result.artifact_id);
    REQUIRE(close_result.ok());
    REQUIRE(fix.registry.count() == 0);
}

// ============================================================================
// Multiple notebooks
// ============================================================================

TEST_CASE("Integration: Multiple notebooks in session", "[v20][notebook-integration]")
{
    NotebookIntegrationFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";

    auto nb1 = fix.lifecycle.create_notebook(ctx, "NB1");
    auto nb2 = fix.lifecycle.create_notebook(ctx, "NB2");
    auto nb3 = fix.lifecycle.open_notebook("/workspace/existing.ipynb");

    REQUIRE(fix.lifecycle.open_notebook_count() == 3);
    REQUIRE(fix.lifecycle.unsaved_notebook_count() == 2);

    // Close one
    fix.lifecycle.close(nb2.artifact_id);
    REQUIRE(fix.lifecycle.open_notebook_count() == 2);
}
