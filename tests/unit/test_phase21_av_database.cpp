#include "core/av/AVAggregateRow.h"
#include "core/av/AVFormulaEngine.h"
#include "core/av/AVImportExport.h"
#include "core/av/AVLinkedDatabase.h"
#include "core/av/AVTimelineView.h"
#include "core/av/AVUndoManager.h"
#include "core/av/AttributeViewStore.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::core::av;

// ════════════════════════════════════════════════════════════
// Test helper: build a sample AttributeView
// ════════════════════════════════════════════════════════════

namespace
{

auto make_sample_av() -> AttributeView
{
    AttributeView attribute_view;
    attribute_view.id = "test_av";
    attribute_view.name = "Test AV";

    // Add keys
    AVKey name_key;
    name_key.id = "k_name";
    name_key.name = "Name";
    name_key.type = AVKeyType::Text;
    static_cast<void>(attribute_view.add_key(name_key));

    AVKey price_key;
    price_key.id = "k_price";
    price_key.name = "Price";
    price_key.type = AVKeyType::Number;
    static_cast<void>(attribute_view.add_key(price_key));

    AVKey done_key;
    done_key.id = "k_done";
    done_key.name = "Done";
    done_key.type = AVKeyType::Checkbox;
    static_cast<void>(attribute_view.add_key(done_key));

    // Add rows
    static_cast<void>(attribute_view.add_row("row1"));
    static_cast<void>(attribute_view.add_row("row2"));
    static_cast<void>(attribute_view.add_row("row3"));

    // Set values
    AVValueText txt1;
    txt1.content = "Widget A";
    attribute_view.set_value("k_name", "row1", txt1);

    AVValueText txt2;
    txt2.content = "Widget B";
    attribute_view.set_value("k_name", "row2", txt2);

    AVValueText txt3;
    txt3.content = "Widget C";
    attribute_view.set_value("k_name", "row3", txt3);

    AVValueNumber num1;
    num1.content = 10.0;
    num1.is_not_empty = true;
    attribute_view.set_value("k_price", "row1", num1);

    AVValueNumber num2;
    num2.content = 20.0;
    num2.is_not_empty = true;
    attribute_view.set_value("k_price", "row2", num2);

    AVValueNumber num3;
    num3.content = 30.0;
    num3.is_not_empty = true;
    attribute_view.set_value("k_price", "row3", num3);

    AVValueCheckbox chk1;
    chk1.checked = true;
    attribute_view.set_value("k_done", "row1", chk1);

    AVValueCheckbox chk2;
    chk2.checked = false;
    attribute_view.set_value("k_done", "row2", chk2);

    AVValueCheckbox chk3;
    chk3.checked = true;
    attribute_view.set_value("k_done", "row3", chk3);

    return attribute_view;
}

} // namespace

// ════════════════════════════════════════════════════════════
// AVFormulaEngine Tests
// ════════════════════════════════════════════════════════════

TEST_CASE("AVFormulaEngine: parse simple expression", "[av][formula]")
{
    AVFormulaEngine engine;
    auto formula = engine.parse("=Price + 5");

    REQUIRE(formula.valid);
    REQUIRE(formula.expression == "=Price + 5");
    REQUIRE(formula.dependent_key_ids.contains("Price"));
}

TEST_CASE("AVFormulaEngine: evaluate arithmetic", "[av][formula]")
{
    AVFormulaEngine engine;
    auto attribute_view = make_sample_av();

    auto formula = engine.parse("=Price * 2");
    REQUIRE(formula.valid);

    auto result = engine.evaluate(formula, attribute_view, "row1");
    REQUIRE(result.has_value());
    REQUIRE_THAT(formula_result_to_double(*result), Catch::Matchers::WithinAbs(20.0, 0.001));
}

TEST_CASE("AVFormulaEngine: SUM function", "[av][formula]")
{
    AVFormulaEngine engine;
    auto attribute_view = make_sample_av();

    auto formula = engine.parse("=SUM(Price)");
    REQUIRE(formula.valid);

    auto result = engine.evaluate(formula, attribute_view, "row1");
    REQUIRE(result.has_value());
    REQUIRE_THAT(formula_result_to_double(*result), Catch::Matchers::WithinAbs(60.0, 0.001));
}

TEST_CASE("AVFormulaEngine: IF function", "[av][formula]")
{
    AVFormulaEngine engine;
    auto attribute_view = make_sample_av();

    auto formula = engine.parse("=IF(Price > 15, \"expensive\", \"cheap\")");
    REQUIRE(formula.valid);

    auto result1 = engine.evaluate(formula, attribute_view, "row1");
    REQUIRE(result1.has_value());
    REQUIRE(formula_result_to_string(*result1) == "cheap");

    auto result2 = engine.evaluate(formula, attribute_view, "row2");
    REQUIRE(result2.has_value());
    REQUIRE(formula_result_to_string(*result2) == "expensive");
}

TEST_CASE("AVFormulaEngine: CONCAT function", "[av][formula]")
{
    AVFormulaEngine engine;
    auto attribute_view = make_sample_av();

    auto formula = engine.parse("=CONCAT(Name, \" - done\")");
    REQUIRE(formula.valid);

    auto result = engine.evaluate(formula, attribute_view, "row1");
    REQUIRE(result.has_value());
    REQUIRE(formula_result_to_string(*result) == "Widget A - done");
}

TEST_CASE("AVFormulaEngine: evaluate_all", "[av][formula]")
{
    AVFormulaEngine engine;
    auto attribute_view = make_sample_av();

    auto formula = engine.parse("=Price + 1");
    auto results = engine.evaluate_all(formula, attribute_view);

    REQUIRE(results.size() == 3);
    REQUIRE_THAT(formula_result_to_double(results[0].second),
                 Catch::Matchers::WithinAbs(11.0, 0.001));
}

TEST_CASE("AVFormulaEngine: get_dependencies", "[av][formula]")
{
    AVFormulaEngine engine;
    auto formula = engine.parse("=Price * 2 + SUM(Quantity)");

    auto deps = engine.get_dependencies(formula);
    REQUIRE(deps.contains("Price"));
    REQUIRE(deps.contains("Quantity"));
}

TEST_CASE("AVFormulaEngine: list/register functions", "[av][formula]")
{
    AVFormulaEngine engine;
    auto funcs = engine.list_functions();
    REQUIRE(funcs.size() >= 10);

    // Register a custom function
    engine.register_function("DOUBLE",
                             [](const std::vector<AVFormulaResult>& args) -> AVFormulaResult
                             {
                                 if (args.empty())
                                 {
                                     return 0.0;
                                 }
                                 return formula_result_to_double(args[0]) * 2.0;
                             });
    auto new_funcs = engine.list_functions();
    REQUIRE(new_funcs.size() == funcs.size() + 1);
}

TEST_CASE("AVFormulaEngine: division by zero", "[av][formula]")
{
    AVFormulaEngine engine;
    auto attribute_view = make_sample_av();

    auto formula = engine.parse("=Price / 0");
    auto result = engine.evaluate(formula, attribute_view, "row1");
    REQUIRE_FALSE(result.has_value());
}

// ════════════════════════════════════════════════════════════
// AVImportExport Tests
// ════════════════════════════════════════════════════════════

TEST_CASE("AVImportExport: CSV export", "[av][import_export]")
{
    auto attribute_view = make_sample_av();
    auto csv = AVImportExport::export_to_string(attribute_view, AVExportFormat::CSV);

    REQUIRE(!csv.empty());
    REQUIRE(csv.find("Name") != std::string::npos);
    REQUIRE(csv.find("Price") != std::string::npos);
    REQUIRE(csv.find("Widget A") != std::string::npos);
}

TEST_CASE("AVImportExport: JSON export", "[av][import_export]")
{
    auto attribute_view = make_sample_av();
    auto json = AVImportExport::export_to_string(attribute_view, AVExportFormat::JSON);

    REQUIRE(!json.empty());
    REQUIRE(json.find('[') != std::string::npos);
    REQUIRE(json.find("Name") != std::string::npos);
}

TEST_CASE("AVImportExport: Markdown export", "[av][import_export]")
{
    auto attribute_view = make_sample_av();
    auto markdown_output =
        AVImportExport::export_to_string(attribute_view, AVExportFormat::MarkdownTable);

    REQUIRE(!markdown_output.empty());
    REQUIRE(markdown_output.find("| Name |") != std::string::npos);
    REQUIRE(markdown_output.find("| --- |") != std::string::npos);
}

TEST_CASE("AVImportExport: CSV import", "[av][import_export]")
{
    const std::string csv_data = "Product,Amount,Active\n"
                                 "Apple,1.50,true\n"
                                 "Banana,0.75,false\n"
                                 "Cherry,3.00,true\n";

    AttributeView attribute_view;
    attribute_view.id = "import_test";
    attribute_view.name = "Import Test";

    auto result = AVImportExport::import_from_csv(csv_data, attribute_view);
    REQUIRE(result.has_value());
    REQUIRE(result->rows_imported == 3);
    REQUIRE(result->columns_created == 3);
}

TEST_CASE("AVImportExport: JSON import", "[av][import_export]")
{
    const std::string json_data = R"([
        {"name": "Item 1", "value": "100"},
        {"name": "Item 2", "value": "200"}
    ])";

    AttributeView attribute_view;
    attribute_view.id = "json_import_test";
    attribute_view.name = "JSON Import Test";

    auto result = AVImportExport::import_from_json(json_data, attribute_view);
    REQUIRE(result.has_value());
    REQUIRE(result->rows_imported == 2);
}

TEST_CASE("AVImportExport: detect_column_types", "[av][import_export]")
{
    const std::vector<std::string> headers = {"Name", "Count", "URL"};
    const std::vector<std::vector<std::string>> rows = {
        {"Widget", "42", "https://example.com"},
        {"Gadget", "17", "https://test.org"},
        {"Doohickey", "99", "https://foo.bar"},
    };

    auto mappings = AVImportExport::detect_column_types(rows, headers);
    REQUIRE(mappings.size() == 3);
    REQUIRE(mappings[0].inferred_type == AVKeyType::Text);
    REQUIRE(mappings[1].inferred_type == AVKeyType::Number);
    REQUIRE(mappings[2].inferred_type == AVKeyType::URL);
}

// ════════════════════════════════════════════════════════════
// AVUndoManager Tests
// ════════════════════════════════════════════════════════════

TEST_CASE("AVUndoManager: basic undo/redo", "[av][undo]")
{
    AVUndoManager undo_mgr(50);

    REQUIRE_FALSE(undo_mgr.can_undo());
    REQUIRE_FALSE(undo_mgr.can_redo());

    AVOperation operation;
    operation.type = AVOperationType::SetValue;
    operation.key_id = "k1";
    operation.block_id = "b1";
    operation.old_value = AVValueText{"old"};
    operation.new_value = AVValueText{"new"};
    operation.description = "Edit cell";

    undo_mgr.record(operation);

    REQUIRE(undo_mgr.can_undo());
    REQUIRE_FALSE(undo_mgr.can_redo());
    REQUIRE(undo_mgr.undo_count() == 1);
    REQUIRE(undo_mgr.undo_description() == "Edit cell");

    auto undone = undo_mgr.undo();
    REQUIRE(undone.operations.size() == 1);
    REQUIRE(undo_mgr.can_redo());
    REQUIRE_FALSE(undo_mgr.can_undo());

    auto redone = undo_mgr.redo();
    REQUIRE(redone.operations.size() == 1);
    REQUIRE(undo_mgr.can_undo());
}

TEST_CASE("AVUndoManager: max history truncation", "[av][undo]")
{
    AVUndoManager undo_mgr(5);

    for (int idx = 0; idx < 10; ++idx)
    {
        AVOperation operation;
        operation.type = AVOperationType::SetValue;
        operation.description = "Op " + std::to_string(idx);
        undo_mgr.record(operation);
    }

    REQUIRE(undo_mgr.undo_count() == 5);
}

TEST_CASE("AVUndoManager: composite operations", "[av][undo]")
{
    AVUndoManager undo_mgr;

    AVCompositeOperation composite;
    composite.description = "Paste 3 cells";
    for (int idx = 0; idx < 3; ++idx)
    {
        AVOperation operation;
        operation.type = AVOperationType::SetValue;
        operation.key_id = "k" + std::to_string(idx);
        composite.operations.push_back(operation);
    }

    undo_mgr.record_composite(composite);
    REQUIRE(undo_mgr.undo_count() == 1);

    auto undone = undo_mgr.undo();
    REQUIRE(undone.operations.size() == 3);
}

TEST_CASE("AVUndoManager: clear", "[av][undo]")
{
    AVUndoManager undo_mgr;

    AVOperation operation;
    operation.type = AVOperationType::AddRow;
    undo_mgr.record(operation);
    undo_mgr.record(operation);

    REQUIRE(undo_mgr.undo_count() == 2);
    undo_mgr.clear();
    REQUIRE(undo_mgr.undo_count() == 0);
    REQUIRE(undo_mgr.redo_count() == 0);
}

// ════════════════════════════════════════════════════════════
// AVAggregateRow Tests
// ════════════════════════════════════════════════════════════

TEST_CASE("AVAggregateRow: sum", "[av][aggregate]")
{
    auto attribute_view = make_sample_av();
    AVAggregateRow agg;
    agg.set_aggregate("k_price", AVAggregateType::Sum);

    auto results = agg.compute(attribute_view);
    REQUIRE(results.size() == 1);
    REQUIRE_THAT(results[0].numeric_result, Catch::Matchers::WithinAbs(60.0, 0.001));
}

TEST_CASE("AVAggregateRow: average", "[av][aggregate]")
{
    auto attribute_view = make_sample_av();
    AVAggregateRow agg;
    agg.set_aggregate("k_price", AVAggregateType::Average);

    auto result = agg.compute_single(attribute_view, "k_price");
    REQUIRE_THAT(result.numeric_result, Catch::Matchers::WithinAbs(20.0, 0.001));
}

TEST_CASE("AVAggregateRow: min/max/range", "[av][aggregate]")
{
    auto attribute_view = make_sample_av();
    AVAggregateRow agg;

    agg.set_aggregate("k_price", AVAggregateType::Min);
    auto min_result = agg.compute_single(attribute_view, "k_price");
    REQUIRE_THAT(min_result.numeric_result, Catch::Matchers::WithinAbs(10.0, 0.001));

    agg.set_aggregate("k_price", AVAggregateType::Max);
    auto max_result = agg.compute_single(attribute_view, "k_price");
    REQUIRE_THAT(max_result.numeric_result, Catch::Matchers::WithinAbs(30.0, 0.001));

    agg.set_aggregate("k_price", AVAggregateType::Range);
    auto range_result = agg.compute_single(attribute_view, "k_price");
    REQUIRE_THAT(range_result.numeric_result, Catch::Matchers::WithinAbs(20.0, 0.001));
}

TEST_CASE("AVAggregateRow: count and percent", "[av][aggregate]")
{
    auto attribute_view = make_sample_av();
    AVAggregateRow agg;

    agg.set_aggregate("k_price", AVAggregateType::Count);
    auto count_result = agg.compute_single(attribute_view, "k_price");
    REQUIRE_THAT(count_result.numeric_result, Catch::Matchers::WithinAbs(3.0, 0.001));

    agg.set_aggregate("k_price", AVAggregateType::PercentFilled);
    auto pf_result = agg.compute_single(attribute_view, "k_price");
    REQUIRE_THAT(pf_result.numeric_result, Catch::Matchers::WithinAbs(100.0, 0.001));
}

TEST_CASE("AVAggregateRow: median", "[av][aggregate]")
{
    auto attribute_view = make_sample_av();
    AVAggregateRow agg;
    agg.set_aggregate("k_price", AVAggregateType::Median);

    auto result = agg.compute_single(attribute_view, "k_price");
    REQUIRE_THAT(result.numeric_result, Catch::Matchers::WithinAbs(20.0, 0.001));
}

TEST_CASE("AVAggregateRow: format_result", "[av][aggregate]")
{
    AVAggregateResult result;
    result.aggregate = AVAggregateType::Count;
    result.numeric_result = 42.0;

    auto formatted = AVAggregateRow::format_result(result);
    REQUIRE(formatted == "42");

    result.aggregate = AVAggregateType::PercentFilled;
    result.numeric_result = 66.666;
    formatted = AVAggregateRow::format_result(result);
    REQUIRE(formatted.find('%') != std::string::npos);
}

TEST_CASE("AVAggregateRow: string conversion", "[av][aggregate]")
{
    REQUIRE(aggregate_type_to_string(AVAggregateType::Sum) == "sum");
    REQUIRE(string_to_aggregate_type("average") == AVAggregateType::Average);
}

// ════════════════════════════════════════════════════════════
// AVTimelineView Tests
// ════════════════════════════════════════════════════════════

TEST_CASE("AVTimelineView: bucket_rows by month", "[av][timeline]")
{
    AttributeView attribute_view;
    attribute_view.id = "timeline_test";
    attribute_view.name = "Timeline Test";

    AVKey date_key;
    date_key.id = "k_date";
    date_key.name = "Due Date";
    date_key.type = AVKeyType::Date;
    static_cast<void>(attribute_view.add_key(date_key));

    static_cast<void>(attribute_view.add_row("row1"));
    static_cast<void>(attribute_view.add_row("row2"));
    static_cast<void>(attribute_view.add_row("row3"));

    // Jan 15, 2025 (1736899200000 ms)
    AVValueDate date1;
    date1.content = 1736899200000;
    attribute_view.set_value("k_date", "row1", date1);

    // Jan 20, 2025 (1737331200000 ms)
    AVValueDate date2;
    date2.content = 1737331200000;
    attribute_view.set_value("k_date", "row2", date2);

    // Feb 10, 2025 (1739145600000 ms)
    AVValueDate date3;
    date3.content = 1739145600000;
    attribute_view.set_value("k_date", "row3", date3);

    AVTimelineView timeline;
    timeline.set_date_key("k_date");
    timeline.set_granularity(AVTimelineGranularity::Month);

    auto buckets = timeline.bucket_rows(attribute_view);
    REQUIRE(buckets.size() == 2);              // Jan and Feb
    REQUIRE(buckets[0].block_ids.size() == 2); // 2 in Jan
    REQUIRE(buckets[1].block_ids.size() == 1); // 1 in Feb
}

TEST_CASE("AVTimelineView: rows_in_range", "[av][timeline]")
{
    AttributeView attribute_view;
    attribute_view.id = "range_test";
    attribute_view.name = "Range Test";

    AVKey date_key;
    date_key.id = "k_date";
    date_key.name = "Date";
    date_key.type = AVKeyType::Date;
    static_cast<void>(attribute_view.add_key(date_key));

    static_cast<void>(attribute_view.add_row("row1"));
    AVValueDate date1;
    date1.content = 1736899200000; // Jan 15
    attribute_view.set_value("k_date", "row1", date1);

    AVTimelineView timeline;
    timeline.set_date_key("k_date");

    auto rows = timeline.rows_in_range(attribute_view, 1735689600000, 1738368000000);
    REQUIRE(rows.size() == 1);
    REQUIRE(rows[0] == "row1");
}

TEST_CASE("AVTimelineView: unscheduled_rows", "[av][timeline]")
{
    AttributeView attribute_view;
    attribute_view.id = "unsched_test";
    attribute_view.name = "Unscheduled Test";

    AVKey date_key;
    date_key.id = "k_date";
    date_key.name = "Date";
    date_key.type = AVKeyType::Date;
    static_cast<void>(attribute_view.add_key(date_key));

    static_cast<void>(attribute_view.add_row("row1"));
    static_cast<void>(attribute_view.add_row("row2"));

    // Only set date on row1
    AVValueDate date1;
    date1.content = 1736899200000;
    attribute_view.set_value("k_date", "row1", date1);

    AVTimelineView timeline;
    timeline.set_date_key("k_date");

    auto unscheduled = timeline.unscheduled_rows(attribute_view);
    REQUIRE(unscheduled.size() == 1);
    REQUIRE(unscheduled[0] == "row2");
}

TEST_CASE("AVTimelineView: move_row", "[av][timeline]")
{
    AttributeView attribute_view;
    attribute_view.id = "move_test";
    attribute_view.name = "Move Test";

    AVKey date_key;
    date_key.id = "k_date";
    date_key.name = "Date";
    date_key.type = AVKeyType::Date;
    static_cast<void>(attribute_view.add_key(date_key));

    static_cast<void>(attribute_view.add_row("row1"));

    AVTimelineView timeline;
    timeline.set_date_key("k_date");

    auto result = timeline.move_row(attribute_view, "row1", 1739145600000);
    REQUIRE(result.has_value());
}

TEST_CASE("AVTimelineView: granularity string conversion", "[av][timeline]")
{
    REQUIRE(granularity_to_string(AVTimelineGranularity::Week) == "week");
    REQUIRE(string_to_granularity("quarter") == AVTimelineGranularity::Quarter);
}

// ════════════════════════════════════════════════════════════
// AVLinkedDatabase Tests
// ════════════════════════════════════════════════════════════

TEST_CASE("AVLinkedDatabase: create_link", "[av][linked]")
{
    auto tmp_dir = std::filesystem::temp_directory_path() / "av_linked_test";
    std::filesystem::create_directories(tmp_dir);

    AttributeViewStore store(tmp_dir);
    auto source = make_sample_av();
    store.save(source);

    AVLinkedDatabase linked(store);
    auto config = linked.create_link(source.id, "My Link");

    REQUIRE(!config.link_id.empty());
    REQUIRE(config.source_av_id == "test_av");
    REQUIRE(config.view_name == "My Link");
    REQUIRE(linked.link_count() == 1);

    std::filesystem::remove_all(tmp_dir);
}

TEST_CASE("AVLinkedDatabase: resolve", "[av][linked]")
{
    auto tmp_dir = std::filesystem::temp_directory_path() / "av_linked_resolve_test";
    std::filesystem::create_directories(tmp_dir);

    AttributeViewStore store(tmp_dir);
    auto source = make_sample_av();
    store.save(source);

    AVLinkedDatabase linked(store);
    auto config = linked.create_link(source.id);

    auto snapshot = linked.resolve(config);
    REQUIRE(snapshot.has_value());
    REQUIRE(snapshot->source_av_name == "Test AV");
    REQUIRE(snapshot->total_rows == 3);
    REQUIRE(snapshot->block_ids.size() == 3);

    std::filesystem::remove_all(tmp_dir);
}

TEST_CASE("AVLinkedDatabase: hide/show columns", "[av][linked]")
{
    auto tmp_dir = std::filesystem::temp_directory_path() / "av_linked_cols_test";
    std::filesystem::create_directories(tmp_dir);

    AttributeViewStore store(tmp_dir);
    auto source = make_sample_av();
    store.save(source);

    AVLinkedDatabase linked(store);
    auto config = linked.create_link(source.id);

    linked.hide_columns(config, {"k_price"});
    auto snapshot = linked.resolve(config);
    REQUIRE(snapshot.has_value());
    // Should have 2 visible keys (Name, Done) instead of 3
    REQUIRE(snapshot->visible_keys.size() == 2);

    linked.show_columns(config, {"k_price"});
    snapshot = linked.resolve(config);
    REQUIRE(snapshot->visible_keys.size() == 3);

    std::filesystem::remove_all(tmp_dir);
}

TEST_CASE("AVLinkedDatabase: is_valid", "[av][linked]")
{
    auto tmp_dir = std::filesystem::temp_directory_path() / "av_linked_valid_test";
    std::filesystem::create_directories(tmp_dir);

    AttributeViewStore store(tmp_dir);
    auto source = make_sample_av();
    store.save(source);

    AVLinkedDatabase linked(store);
    auto config = linked.create_link(source.id);

    REQUIRE(linked.is_valid(config));

    AVLinkedConfig bad_config;
    bad_config.source_av_id = "nonexistent";
    REQUIRE_FALSE(linked.is_valid(bad_config));

    std::filesystem::remove_all(tmp_dir);
}
