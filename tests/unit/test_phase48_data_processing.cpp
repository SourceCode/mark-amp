/// @file test_phase48_data_processing.cpp
/// @brief Phase 48 tests — Data Processing & Computation.

#include "core/ChartDataProvider.h"
#include "core/ComputationCommandProvider.h"
#include "core/DataCommandProvider.h"
#include "core/DataTableEngine.h"
#include "core/DataTransformPipeline.h"
#include "core/Events.h"
#include "core/FormulaEvaluator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// DataTableEngine
// ============================================================================

TEST_CASE("DataTableEngine initial state", "[phase48][table]")
{
    DataTableEngine engine;
    REQUIRE(engine.table_count() == 0);
}

TEST_CASE("DataTableEngine create table", "[phase48][table]")
{
    DataTableEngine engine;
    auto table_id = engine.create_table("Sales");
    REQUIRE_FALSE(table_id.empty());
    REQUIRE(engine.table_count() == 1);
    const auto* tbl = engine.get_table(table_id);
    REQUIRE(tbl != nullptr);
    REQUIRE(tbl->name == "Sales");
}

TEST_CASE("DataTableEngine add columns and rows", "[phase48][table]")
{
    DataTableEngine engine;
    auto table_id = engine.create_table("Test");
    REQUIRE(engine.add_column(table_id, {"Name", ColumnType::kString, 150}));
    REQUIRE(engine.add_column(table_id, {"Value", ColumnType::kNumber, 100}));
    REQUIRE(engine.add_row(table_id, {{"Alice", "100"}}));
    REQUIRE(engine.add_row(table_id, {{"Bob", "200"}}));

    const auto* tbl = engine.get_table(table_id);
    REQUIRE(tbl->rows.size() == 2);
    REQUIRE(tbl->columns.size() == 2);
}

TEST_CASE("DataTableEngine delete table", "[phase48][table]")
{
    DataTableEngine engine;
    auto table_id = engine.create_table("Temp");
    REQUIRE(engine.delete_table(table_id));
    REQUIRE(engine.table_count() == 0);
}

TEST_CASE("DataTableEngine remove row", "[phase48][table]")
{
    DataTableEngine engine;
    auto table_id = engine.create_table("Test");
    engine.add_column(table_id, {"Col", ColumnType::kString});
    engine.add_row(table_id, {{"A"}});
    engine.add_row(table_id, {{"B"}});
    REQUIRE(engine.remove_row(table_id, 0));
    const auto* tbl = engine.get_table(table_id);
    REQUIRE(tbl->rows.size() == 1);
    REQUIRE(tbl->rows[0].cells[0] == "B");
}

TEST_CASE("DataTableEngine sort table", "[phase48][table]")
{
    DataTableEngine engine;
    auto table_id = engine.create_table("Sort");
    engine.add_column(table_id, {"Name", ColumnType::kString});
    engine.add_row(table_id, {{"Charlie"}});
    engine.add_row(table_id, {{"Alice"}});
    engine.add_row(table_id, {{"Bob"}});
    REQUIRE(engine.sort_table(table_id, "Name", true));
    const auto* tbl = engine.get_table(table_id);
    REQUIRE(tbl->rows[0].cells[0] == "Alice");
    REQUIRE(tbl->rows[2].cells[0] == "Charlie");
}

TEST_CASE("DataTableEngine filter table", "[phase48][table]")
{
    DataTableEngine engine;
    auto table_id = engine.create_table("Filter");
    engine.add_column(table_id, {"Status", ColumnType::kString});
    engine.add_row(table_id, {{"active"}});
    engine.add_row(table_id, {{"inactive"}});
    engine.add_row(table_id, {{"active"}});
    auto filtered = engine.filter_table(table_id, "Status", "active");
    REQUIRE(filtered.size() == 2);
}

TEST_CASE("DataTableEngine clear", "[phase48][table]")
{
    DataTableEngine engine;
    engine.create_table("A");
    engine.create_table("B");
    engine.clear();
    REQUIRE(engine.table_count() == 0);
}

// ============================================================================
// FormulaEvaluator
// ============================================================================

TEST_CASE("FormulaEvaluator simple number", "[phase48][formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("42");
    REQUIRE_FALSE(result.is_error);
    REQUIRE(result.value == 42.0);
}

TEST_CASE("FormulaEvaluator SUM", "[phase48][formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("SUM(10, 20, 30)");
    REQUIRE_FALSE(result.is_error);
    REQUIRE(result.value == 60.0);
}

TEST_CASE("FormulaEvaluator AVG", "[phase48][formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("AVG(10, 20, 30)");
    REQUIRE_FALSE(result.is_error);
    REQUIRE(result.value == 20.0);
}

TEST_CASE("FormulaEvaluator MIN MAX", "[phase48][formula]")
{
    FormulaEvaluator eval;
    auto min_res = eval.evaluate("MIN(5, 3, 8)");
    auto max_res = eval.evaluate("MAX(5, 3, 8)");
    REQUIRE(min_res.value == 3.0);
    REQUIRE(max_res.value == 8.0);
}

TEST_CASE("FormulaEvaluator COUNT", "[phase48][formula]")
{
    FormulaEvaluator eval;
    auto result = eval.evaluate("COUNT(1, 2, 3, 4)");
    REQUIRE(result.value == 4.0);
}

TEST_CASE("FormulaEvaluator variable", "[phase48][formula]")
{
    FormulaEvaluator eval;
    eval.register_variable("price", 9.99);
    REQUIRE(eval.has_variable("price"));
    REQUIRE(eval.get_variable("price") == 9.99);
    auto result = eval.evaluate("price");
    REQUIRE(result.value == 9.99);
}

TEST_CASE("FormulaEvaluator validate", "[phase48][formula]")
{
    FormulaEvaluator eval;
    REQUIRE(eval.validate("42"));
    REQUIRE(eval.validate("SUM(1, 2)"));
    REQUIRE_FALSE(eval.validate(""));
}

TEST_CASE("FormulaEvaluator custom function", "[phase48][formula]")
{
    FormulaEvaluator eval;
    eval.register_function("DOUBLE",
                           [](const std::vector<double>& args)
                           { return args.empty() ? 0.0 : args[0] * 2.0; });
    auto result = eval.evaluate("DOUBLE(21)");
    REQUIRE_FALSE(result.is_error);
    REQUIRE(result.value == 42.0);
}

TEST_CASE("FormulaEvaluator clear variables", "[phase48][formula]")
{
    FormulaEvaluator eval;
    eval.register_variable("x", 1.0);
    eval.register_variable("y", 2.0);
    REQUIRE(eval.variable_count() == 2);
    eval.clear_variables();
    REQUIRE(eval.variable_count() == 0);
}

// ============================================================================
// DataTransformPipeline
// ============================================================================

TEST_CASE("DataTransformPipeline initial state", "[phase48][transform]")
{
    DataTransformPipeline pipeline;
    REQUIRE(pipeline.step_count() == 0);
}

TEST_CASE("DataTransformPipeline add/remove steps", "[phase48][transform]")
{
    DataTransformPipeline pipeline;
    pipeline.add_step({TransformType::kFilter, "Status", "active"});
    pipeline.add_step({TransformType::kSort, "Name", "", true});
    REQUIRE(pipeline.step_count() == 2);
    REQUIRE(pipeline.remove_step(0));
    REQUIRE(pipeline.step_count() == 1);
}

TEST_CASE("DataTransformPipeline execute filter", "[phase48][transform]")
{
    DataTransformPipeline pipeline;
    pipeline.add_step({TransformType::kFilter, "Status", "active"});

    std::vector<ColumnDef> cols = {{"Status", ColumnType::kString}};
    std::vector<DataRow> rows = {{{"active"}}, {{"inactive"}}, {{"active"}}};

    auto result = pipeline.execute(rows, cols);
    REQUIRE(result.size() == 2);
}

TEST_CASE("DataTransformPipeline execute sort", "[phase48][transform]")
{
    DataTransformPipeline pipeline;
    pipeline.add_step({TransformType::kSort, "Name", "", true});

    std::vector<ColumnDef> cols = {{"Name", ColumnType::kString}};
    std::vector<DataRow> rows = {{{"Charlie"}}, {{"Alice"}}, {{"Bob"}}};

    auto result = pipeline.execute(rows, cols);
    REQUIRE(result[0].cells[0] == "Alice");
}

TEST_CASE("DataTransformPipeline validate", "[phase48][transform]")
{
    DataTransformPipeline pipeline;
    pipeline.add_step({TransformType::kFilter, "Status", "active"});

    std::vector<ColumnDef> cols = {{"Status", ColumnType::kString}};
    REQUIRE(pipeline.validate(cols));

    std::vector<ColumnDef> wrong_cols = {{"Name", ColumnType::kString}};
    REQUIRE_FALSE(pipeline.validate(wrong_cols));
}

// ============================================================================
// ChartDataProvider
// ============================================================================

TEST_CASE("ChartDataProvider initial state", "[phase48][chart]")
{
    ChartDataProvider provider;
    REQUIRE(provider.dataset_count() == 0);
}

TEST_CASE("ChartDataProvider create dataset", "[phase48][chart]")
{
    ChartDataProvider provider;
    auto dataset_id = provider.create_dataset("Revenue", ChartType::kBar);
    REQUIRE_FALSE(dataset_id.empty());
    const auto* dataset = provider.get_dataset(dataset_id);
    REQUIRE(dataset != nullptr);
    REQUIRE(dataset->name == "Revenue");
    REQUIRE(dataset->type == ChartType::kBar);
}

TEST_CASE("ChartDataProvider add series", "[phase48][chart]")
{
    ChartDataProvider provider;
    auto dataset_id = provider.create_dataset("Sales", ChartType::kLine);
    ChartSeries series;
    series.label = "Q1";
    series.values = {100.0, 200.0, 150.0};
    series.color = "#3498db";
    REQUIRE(provider.add_series(dataset_id, series));

    const auto* dataset = provider.get_dataset(dataset_id);
    REQUIRE(dataset->series.size() == 1);
    REQUIRE(dataset->series[0].label == "Q1");
}

TEST_CASE("ChartDataProvider set labels", "[phase48][chart]")
{
    ChartDataProvider provider;
    auto dataset_id = provider.create_dataset("Test", ChartType::kPie);
    REQUIRE(provider.set_labels(dataset_id, {"Jan", "Feb", "Mar"}));
    const auto* dataset = provider.get_dataset(dataset_id);
    REQUIRE(dataset->labels.size() == 3);
}

TEST_CASE("ChartDataProvider delete dataset", "[phase48][chart]")
{
    ChartDataProvider provider;
    auto dataset_id = provider.create_dataset("Temp", ChartType::kScatter);
    REQUIRE(provider.delete_dataset(dataset_id));
    REQUIRE(provider.dataset_count() == 0);
}

TEST_CASE("ChartDataProvider list datasets", "[phase48][chart]")
{
    ChartDataProvider provider;
    provider.create_dataset("A", ChartType::kBar);
    provider.create_dataset("B", ChartType::kLine);
    auto all = provider.list_datasets();
    REQUIRE(all.size() == 2);
}

// ============================================================================
// DataCommandProvider
// ============================================================================

TEST_CASE("DataCommandProvider provides 8 commands", "[phase48][commands]")
{
    REQUIRE(DataCommandProvider::command_count() == 8);
    REQUIRE(DataCommandProvider::command_ids().size() == 8);
}

TEST_CASE("DataCommandProvider lookup", "[phase48][commands]")
{
    DataCommandProvider provider;
    auto cmd = provider.get_command("data.createTable");
    REQUIRE(cmd.id == "data.createTable");
    REQUIRE(cmd.category == "Data");
}

// ============================================================================
// ComputationCommandProvider
// ============================================================================

TEST_CASE("ComputationCommandProvider provides 8 commands", "[phase48][commands]")
{
    REQUIRE(ComputationCommandProvider::command_count() == 8);
    REQUIRE(ComputationCommandProvider::command_ids().size() == 8);
}

TEST_CASE("ComputationCommandProvider lookup", "[phase48][commands]")
{
    ComputationCommandProvider provider;
    auto cmd = provider.get_command("compute.evaluateFormula");
    REQUIRE(cmd.id == "compute.evaluateFormula");
    REQUIRE(cmd.category == "Computation");
}

// ============================================================================
// Phase 48 Events
// ============================================================================

TEST_CASE("TableCreatedEvent fields", "[phase48][events]")
{
    events::TableCreatedEvent evt;
    evt.table_id = "tbl_1";
    evt.table_name = "Sales";
    REQUIRE(evt.table_name == "Sales");
}

TEST_CASE("RowAddedEvent fields", "[phase48][events]")
{
    events::RowAddedEvent evt;
    evt.table_id = "tbl_1";
    evt.row_index = 5;
    REQUIRE(evt.row_index == 5);
}

TEST_CASE("FormulaEvaluatedEvent fields", "[phase48][events]")
{
    events::FormulaEvaluatedEvent evt;
    evt.expression = "SUM(1,2,3)";
    evt.result = 6.0;
    REQUIRE(evt.result == 6.0);
}

TEST_CASE("TransformExecutedEvent fields", "[phase48][events]")
{
    events::TransformExecutedEvent evt;
    evt.steps_applied = 3;
    evt.rows_input = 100;
    evt.rows_output = 42;
    REQUIRE(evt.rows_output == 42);
}

TEST_CASE("ChartGeneratedEvent fields", "[phase48][events]")
{
    events::ChartGeneratedEvent evt;
    evt.dataset_id = "chart_1";
    evt.chart_type = "bar";
    evt.series_count = 3;
    REQUIRE(evt.series_count == 3);
}

TEST_CASE("DataImportedEvent fields", "[phase48][events]")
{
    events::DataImportedEvent evt;
    evt.source = "data.csv";
    evt.rows_imported = 500;
    evt.columns_imported = 8;
    REQUIRE(evt.rows_imported == 500);
}
