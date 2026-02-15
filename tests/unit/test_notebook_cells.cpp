/// @file test_notebook_cells.cpp
/// @brief V4 Phase 34 – NotebookCellManager and VariableInspector tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/KernelManager.h"
#include "core/NotebookCellManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Test fixture
// ============================================================================

struct CellFixture
{
    EventBus event_bus;
    Config config;
    NotebookCellManager manager{event_bus};
    KernelManager kernel_manager{event_bus, config};
};

// ============================================================================
// Cell CRUD
// ============================================================================

TEST_CASE("NotebookCellManager adds code cell at end", "[phase34][cells]")
{
    CellFixture ctx;
    auto id = ctx.manager.add_cell(CellType::kCode);

    REQUIRE(ctx.manager.cell_count() == 1);
    auto cell = ctx.manager.get_cell(id);
    REQUIRE(cell.has_value());
    REQUIRE(cell->metadata.type == CellType::kCode);
}

TEST_CASE("NotebookCellManager adds cell at specific position", "[phase34][cells]")
{
    CellFixture ctx;
    auto id1 = ctx.manager.add_cell(CellType::kCode);
    auto id2 = ctx.manager.add_cell(CellType::kMarkdown);
    auto id3 = ctx.manager.add_cell(CellType::kCode, 1); // Insert at position 1.

    REQUIRE(ctx.manager.cell_count() == 3);
    REQUIRE(ctx.manager.cell_at(0).metadata.cell_id == id1);
    REQUIRE(ctx.manager.cell_at(1).metadata.cell_id == id3);
    REQUIRE(ctx.manager.cell_at(2).metadata.cell_id == id2);
}

TEST_CASE("NotebookCellManager removes cell", "[phase34][cells]")
{
    CellFixture ctx;
    auto id = ctx.manager.add_cell(CellType::kCode);
    ctx.manager.remove_cell(id);

    REQUIRE(ctx.manager.cell_count() == 0);
    REQUIRE_FALSE(ctx.manager.get_cell(id).has_value());
}

TEST_CASE("NotebookCellManager moves cell to new position", "[phase34][cells]")
{
    CellFixture ctx;
    auto id1 = ctx.manager.add_cell(CellType::kCode);
    auto id2 = ctx.manager.add_cell(CellType::kMarkdown);
    auto id3 = ctx.manager.add_cell(CellType::kCode);

    ctx.manager.move_cell(id3, 0); // Move last cell to beginning.
    REQUIRE(ctx.manager.cell_at(0).metadata.cell_id == id3);
    REQUIRE(ctx.manager.cell_at(1).metadata.cell_id == id1);
    REQUIRE(ctx.manager.cell_at(2).metadata.cell_id == id2);
}

TEST_CASE("NotebookCellManager sets cell source", "[phase34][cells]")
{
    CellFixture ctx;
    auto id = ctx.manager.add_cell(CellType::kCode);
    ctx.manager.set_cell_source(id, "print('hello')");

    auto cell = ctx.manager.get_cell(id);
    REQUIRE(cell->source == "print('hello')");
}

// ============================================================================
// Cell Metadata
// ============================================================================

TEST_CASE("NotebookCellManager manages cell tags", "[phase34][cells]")
{
    CellFixture ctx;
    auto id = ctx.manager.add_cell(CellType::kCode);
    ctx.manager.add_cell_tag(id, "slow");
    ctx.manager.add_cell_tag(id, "important");
    ctx.manager.add_cell_tag(id, "slow"); // Duplicate should be ignored.

    auto cell = ctx.manager.get_cell(id);
    REQUIRE(cell->metadata.tags.size() == 2);
}

// ============================================================================
// Execution tracking
// ============================================================================

TEST_CASE("NotebookCellManager tracks execution counts", "[phase34][cells]")
{
    CellFixture ctx;
    auto id = ctx.manager.add_cell(CellType::kCode);

    int count = ctx.manager.next_execution_count();
    REQUIRE(count == 1);

    ctx.manager.record_execution(id, count, 123.45);

    auto cell = ctx.manager.get_cell(id);
    REQUIRE(cell->metadata.execution_count == 1);
    REQUIRE(cell->metadata.execution_label() == "[1]");
    REQUIRE(cell->metadata.execution_time_ms == 123.45);
    REQUIRE(ctx.manager.execution_order().size() == 1);
}

// ============================================================================
// Serialization
// ============================================================================

TEST_CASE("NotebookCellManager serializes and deserializes", "[phase34][cells]")
{
    CellFixture ctx;
    auto id1 = ctx.manager.add_cell(CellType::kCode);
    ctx.manager.set_cell_source(id1, "x = 42");
    ctx.manager.record_execution(id1, 1, 10.0);

    auto id2 = ctx.manager.add_cell(CellType::kMarkdown);
    ctx.manager.set_cell_source(id2, "# Title");

    auto json = ctx.manager.serialize_to_json();
    REQUIRE(json.find("\"cell_type\":\"code\"") != std::string::npos);
    REQUIRE(json.find("\"cell_type\":\"markdown\"") != std::string::npos);
    REQUIRE(json.find("x = 42") != std::string::npos);

    // Deserialize into a new manager.
    CellFixture ctx2;
    ctx2.manager.deserialize_from_json(json);
    REQUIRE(ctx2.manager.cell_count() == 2);
    REQUIRE(ctx2.manager.cell_at(0).source == "x = 42");
    REQUIRE(ctx2.manager.cell_at(1).source == "# Title");
    REQUIRE(ctx2.manager.cell_at(0).metadata.execution_count == 1);
}

// ============================================================================
// Cell metadata label
// ============================================================================

TEST_CASE("CellMetadata execution_label returns bracket format", "[phase34][cells]")
{
    CellMetadata meta;
    REQUIRE(meta.execution_label() == "[ ]");

    meta.execution_count = 5;
    REQUIRE(meta.execution_label() == "[5]");
}

// ============================================================================
// Variable Inspector
// ============================================================================

TEST_CASE("VariableInspector manages variable list", "[phase34][vars]")
{
    CellFixture ctx;
    VariableInspector inspector(ctx.event_bus, ctx.kernel_manager);

    VariableInfo var;
    var.name = "x";
    var.type_name = "int";
    var.value_repr = "42";
    inspector.add_variable(var);

    REQUIRE(inspector.variables().size() == 1);
    REQUIRE(inspector.variables()[0].name == "x");
}

TEST_CASE("VariableInspector refresh fails for unknown kernel", "[phase34][vars]")
{
    CellFixture ctx;
    VariableInspector inspector(ctx.event_bus, ctx.kernel_manager);

    auto result = inspector.refresh("nonexistent");
    REQUIRE_FALSE(result.has_value());
}
