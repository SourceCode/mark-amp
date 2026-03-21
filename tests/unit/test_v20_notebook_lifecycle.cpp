/// @file test_v20_notebook_lifecycle.cpp
/// @brief V20 Phase 03 – NotebookArtifactLifecycle unit tests.

#include "core/NotebookArtifactLifecycle.h"
#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/UnsavedDocumentBuffer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct NotebookFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    UnsavedDocumentBufferManager buffer_mgr;
    NotebookArtifactLifecycle lifecycle{bus, registry, creation, buffer_mgr, config};
};

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("NotebookLifecycle: construction", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;
    REQUIRE(fix.lifecycle.operation_count() == 0);
    REQUIRE(fix.lifecycle.open_notebook_count() == 0);
}

// ============================================================================
// Create notebook
// ============================================================================

TEST_CASE("NotebookLifecycle: create notebook", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "palette";

    auto result = fix.lifecycle.create_notebook(ctx, "My Notebook", "python");
    REQUIRE(result.ok());
    REQUIRE_FALSE(result.artifact_id.empty());

    REQUIRE(fix.registry.count() == 1);
    REQUIRE(fix.registry.active_artifact() == result.artifact_id);

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->kind == ArtifactKind::kNotebook);
    REQUIRE(record->is_unsaved());
}

TEST_CASE("NotebookLifecycle: new notebook has starter cells", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";

    auto result = fix.lifecycle.create_notebook(ctx, "Test Notebook");
    REQUIRE(result.ok());

    // Should have 2 starter cells: markdown header + code cell
    REQUIRE(fix.lifecycle.cell_count(result.artifact_id) == 2);

    auto& cells = fix.lifecycle.get_cells(result.artifact_id);
    REQUIRE(cells[0].is_markdown());
    REQUIRE(cells[0].source.find("Test Notebook") != std::string::npos);
    REQUIRE(cells[1].is_code());
}

TEST_CASE("NotebookLifecycle: create with default title", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "shortcut";

    auto result = fix.lifecycle.create_notebook(ctx);
    REQUIRE(result.ok());

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->display_name.find("Notebook") != std::string::npos);
}

// ============================================================================
// Open notebook
// ============================================================================

TEST_CASE("NotebookLifecycle: open notebook", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    auto result = fix.lifecycle.open_notebook("/workspace/analysis.ipynb");
    REQUIRE(result.ok());
    REQUIRE(result.resolved_path == "/workspace/analysis.ipynb");

    const auto* record = fix.registry.find(result.artifact_id);
    REQUIRE(record != nullptr);
    REQUIRE(record->display_name == "analysis");
    REQUIRE(record->is_saved());
    REQUIRE(record->kind == ArtifactKind::kNotebook);
}

TEST_CASE("NotebookLifecycle: open same path twice", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    auto r1 = fix.lifecycle.open_notebook("/workspace/nb.ipynb");
    auto r2 = fix.lifecycle.open_notebook("/workspace/nb.ipynb");

    REQUIRE(r1.artifact_id == r2.artifact_id);
    REQUIRE(fix.registry.count() == 1);
}

TEST_CASE("NotebookLifecycle: open empty path fails", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;
    auto result = fix.lifecycle.open_notebook("");
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Save
// ============================================================================

TEST_CASE("NotebookLifecycle: save unsaved triggers request", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_notebook(ctx, "Draft");

    bool save_requested = false;
    auto sub = fix.bus.subscribe<events::ArtifactSaveRequestEvent>(
        [&save_requested](const events::ArtifactSaveRequestEvent&) { save_requested = true; });

    auto save_result = fix.lifecycle.save(create_result.artifact_id);
    REQUIRE(save_result.ok());
    REQUIRE(save_requested);
}

TEST_CASE("NotebookLifecycle: save_as promotes to saved", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "palette";
    auto create_result = fix.lifecycle.create_notebook(ctx, "My NB");

    auto save_result = fix.lifecycle.save_as(create_result.artifact_id, "/workspace/my-nb.ipynb");
    REQUIRE(save_result.ok());

    const auto* record = fix.registry.find(create_result.artifact_id);
    REQUIRE(record->is_saved());
    REQUIRE(record->file_path.value() == "/workspace/my-nb.ipynb");
}

TEST_CASE("NotebookLifecycle: save nonexistent fails", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;
    auto result = fix.lifecycle.save(ArtifactId{"fake"});
    REQUIRE_FALSE(result.ok());
}

// ============================================================================
// Rename
// ============================================================================

TEST_CASE("NotebookLifecycle: rename notebook", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto create_result = fix.lifecycle.create_notebook(ctx, "Old Name");

    auto rename_result = fix.lifecycle.rename(create_result.artifact_id, "New Name");
    REQUIRE(rename_result.ok());

    const auto* record = fix.registry.find(create_result.artifact_id);
    REQUIRE(record->display_name == "New Name");
}

TEST_CASE("NotebookLifecycle: rename with empty title fails", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_notebook(ctx, "Test");

    REQUIRE_FALSE(fix.lifecycle.rename(result.artifact_id, "").ok());
}

// ============================================================================
// Duplicate
// ============================================================================

TEST_CASE("NotebookLifecycle: duplicate notebook", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto original = fix.lifecycle.create_notebook(ctx, "Original");

    // Add a code cell
    fix.lifecycle.add_cell(original.artifact_id, NotebookCellKind::kCode, "print('hello')");

    auto dup = fix.lifecycle.duplicate(original.artifact_id);
    REQUIRE(dup.ok());
    REQUIRE(dup.artifact_id != original.artifact_id);
    REQUIRE(fix.registry.count() == 2);

    // Duplicate should have cells from original
    auto orig_cells = fix.lifecycle.cell_count(original.artifact_id);
    REQUIRE(orig_cells >= 2);
}

// ============================================================================
// Remove / Close
// ============================================================================

TEST_CASE("NotebookLifecycle: remove notebook", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_notebook(ctx, "Temp");

    auto remove_result = fix.lifecycle.remove(result.artifact_id);
    REQUIRE(remove_result.ok());
    REQUIRE(fix.registry.count() == 0);
    REQUIRE(fix.lifecycle.cell_count(result.artifact_id) == 0);
}

TEST_CASE("NotebookLifecycle: close notebook", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_notebook(ctx, "CloseMe");

    auto close_result = fix.lifecycle.close(result.artifact_id);
    REQUIRE(close_result.ok());
    REQUIRE(fix.registry.count() == 0);
}

// ============================================================================
// Cell management
// ============================================================================

TEST_CASE("NotebookLifecycle: add cells", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_notebook(ctx, "Cells");

    int initial = fix.lifecycle.cell_count(result.artifact_id);
    REQUIRE(fix.lifecycle.add_cell(result.artifact_id, NotebookCellKind::kCode, "x = 42"));
    REQUIRE(fix.lifecycle.cell_count(result.artifact_id) == initial + 1);
}

TEST_CASE("NotebookLifecycle: add cell to nonexistent fails", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;
    REQUIRE_FALSE(fix.lifecycle.add_cell(ArtifactId{"fake"}, NotebookCellKind::kCode));
}

// ============================================================================
// Queries
// ============================================================================

TEST_CASE("NotebookLifecycle: query counts", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    fix.lifecycle.create_notebook(ctx, "NB1");
    fix.lifecycle.create_notebook(ctx, "NB2");

    REQUIRE(fix.lifecycle.open_notebook_count() == 2);
    REQUIRE(fix.lifecycle.unsaved_notebook_count() == 2);
}

TEST_CASE("NotebookLifecycle: has_unsaved_changes", "[v20][notebook-lifecycle]")
{
    NotebookFixture fix;

    NotebookPlacementContext ctx;
    ctx.entry_point = "menu";
    auto result = fix.lifecycle.create_notebook(ctx, "Draft");

    REQUIRE(fix.lifecycle.has_unsaved_changes(result.artifact_id));

    fix.lifecycle.save_as(result.artifact_id, "/workspace/draft.ipynb");
    REQUIRE_FALSE(fix.lifecycle.has_unsaved_changes(result.artifact_id));
}
