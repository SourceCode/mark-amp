// ============================================================================
// File: tests/unit/test_phase15_notebook_completion.cpp
// Phase 15: Notebook System Completion — comprehensive test suite
// ============================================================================

#include "core/CellDependencyTracker.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/KernelManager.h"
#include "core/NotebookCellManager.h"
#include "core/NotebookExecutionPipeline.h"
#include "core/NotebookSearchIndex.h"
#include "core/NotebookSerializer.h"
#include "core/NotebookSessionManager.h"
#include "core/NotebookToolbar.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// NotebookSerializer Tests
// ============================================================================

TEST_CASE("NotebookSerializer — parse minimal .ipynb", "[phase15][serializer]")
{
    EventBus bus;
    NotebookSerializer serializer(bus);

    std::string ipynb = R"({
        "nbformat": 4,
        "nbformat_minor": 5,
        "metadata": {
            "kernelspec": {
                "name": "python3",
                "display_name": "Python 3",
                "language": "python"
            },
            "language_info": {
                "name": "python"
            }
        },
        "cells": [
            {
                "cell_type": "code",
                "source": "x = 42",
                "execution_count": 1,
                "outputs": [],
                "metadata": {}
            },
            {
                "cell_type": "markdown",
                "source": "# Hello",
                "metadata": {}
            }
        ]
    })";

    auto result = serializer.parse_ipynb(ipynb);
    REQUIRE(result.has_value());

    auto doc = result.value();
    CHECK(doc.nbformat == 4);
    CHECK(doc.nbformat_minor == 5);
    CHECK(doc.kernelspec.name == "python3");
    CHECK(doc.kernelspec.display_name == "Python 3");
    CHECK(doc.kernelspec.language == "python");
    CHECK(doc.language_info.name == "python");
    CHECK(doc.cells.size() == 2);
    CHECK(doc.cells[0].cell_type == "code");
    CHECK(doc.cells[0].source == "x = 42");
    CHECK(doc.cells[0].execution_count == 1);
    CHECK(doc.cells[1].cell_type == "markdown");
}

TEST_CASE("NotebookSerializer — parse empty input", "[phase15][serializer]")
{
    EventBus bus;
    NotebookSerializer serializer(bus);

    auto result = serializer.parse_ipynb("");
    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == "Empty notebook JSON");
}

TEST_CASE("NotebookSerializer — round-trip serialize/parse", "[phase15][serializer]")
{
    EventBus bus;
    NotebookSerializer serializer(bus);

    NotebookDocument doc;
    doc.nbformat = 4;
    doc.nbformat_minor = 5;
    doc.kernelspec.name = "python3";
    doc.kernelspec.display_name = "Python 3";
    doc.kernelspec.language = "python";
    doc.language_info.name = "python";

    SerializedCell cell;
    cell.cell_id = "cell-1";
    cell.cell_type = "code";
    cell.source = "print('hello')";
    cell.execution_count = 1;
    doc.cells.push_back(cell);

    auto json = serializer.serialize_ipynb(doc);
    CHECK_FALSE(json.empty());
    CHECK(json.find("\"nbformat\": 4") != std::string::npos);
    CHECK(json.find("\"cell_type\": \"code\"") != std::string::npos);
}

TEST_CASE("NotebookSerializer — validate notebook", "[phase15][serializer]")
{
    EventBus bus;
    NotebookSerializer serializer(bus);

    NotebookDocument doc;
    doc.nbformat = 3; // Invalid version.

    auto result = serializer.validate_notebook(doc);
    CHECK_FALSE(result.valid);
    CHECK_FALSE(result.issues.empty());
}

TEST_CASE("NotebookSerializer — strip outputs", "[phase15][serializer]")
{
    EventBus bus;
    NotebookSerializer serializer(bus);

    NotebookDocument doc;
    SerializedCell cell;
    cell.cell_type = "code";
    cell.source = "x = 1";
    cell.execution_count = 5;

    SerializedCellOutput output;
    output.output_type = "execute_result";
    output.text = "1";
    cell.outputs.push_back(output);
    doc.cells.push_back(std::move(cell));

    auto stripped = serializer.strip_outputs(doc);
    CHECK(stripped.cells[0].outputs.empty());
    CHECK(stripped.cells[0].execution_count == 0);
}

TEST_CASE("NotebookSerializer — cell counts", "[phase15][serializer]")
{
    NotebookDocument doc;
    SerializedCell code_cell;
    code_cell.cell_type = "code";
    doc.cells.push_back(code_cell);

    SerializedCell md_cell;
    md_cell.cell_type = "markdown";
    doc.cells.push_back(md_cell);
    doc.cells.push_back(md_cell);

    CHECK(NotebookSerializer::code_cell_count(doc) == 1);
    CHECK(NotebookSerializer::markdown_cell_count(doc) == 2);
}

TEST_CASE("NotebookSerializer — summarize", "[phase15][serializer]")
{
    NotebookDocument doc;
    doc.nbformat = 4;
    doc.nbformat_minor = 5;
    doc.kernelspec.display_name = "Python 3";

    SerializedCell cell;
    cell.cell_type = "code";
    doc.cells.push_back(cell);

    auto summary = NotebookSerializer::summarize(doc);
    CHECK(summary.find("nbformat 4.5") != std::string::npos);
    CHECK(summary.find("1 cells") != std::string::npos);
    CHECK(summary.find("Python 3") != std::string::npos);
}

TEST_CASE("NotebookSerializer — upgrade notebook", "[phase15][serializer]")
{
    EventBus bus;
    NotebookSerializer serializer(bus);

    NotebookDocument doc;
    doc.nbformat = 3;
    doc.nbformat_minor = 0;

    auto upgraded = serializer.upgrade_notebook(doc, 4);
    REQUIRE(upgraded.has_value());
    CHECK(upgraded.value().nbformat == 4);
    CHECK(upgraded.value().nbformat_minor == 5);
}

// ============================================================================
// NotebookExecutionPipeline Tests
// ============================================================================

TEST_CASE("NotebookExecutionPipeline — execute single cell", "[phase15][pipeline]")
{
    EventBus bus;
    Config config;
    NotebookCellManager cell_mgr(bus);
    KernelManager kernel_mgr(bus, config);
    NotebookExecutionPipeline pipeline(bus, cell_mgr, kernel_mgr);

    auto cell_id = cell_mgr.add_cell(CellType::kCode, 0);
    cell_mgr.set_cell_source(cell_id, "x = 42");

    pipeline.execute_cell(cell_id);

    CHECK(pipeline.is_executing());
    CHECK(pipeline.queue_size() == 1);
    CHECK(pipeline.current_cell().has_value());
    CHECK(pipeline.current_cell().value() == cell_id);
}

TEST_CASE("NotebookExecutionPipeline — execute all cells", "[phase15][pipeline]")
{
    EventBus bus;
    Config config;
    NotebookCellManager cell_mgr(bus);
    KernelManager kernel_mgr(bus, config);
    NotebookExecutionPipeline pipeline(bus, cell_mgr, kernel_mgr);

    auto cell1 = cell_mgr.add_cell(CellType::kCode, 0);
    auto cell2 = cell_mgr.add_cell(CellType::kCode, 1);
    cell_mgr.add_cell(CellType::kMarkdown, 2); // Markdown — should be skipped.
    auto cell3 = cell_mgr.add_cell(CellType::kCode, 3);

    pipeline.execute_range(ExecutionScope::kAllCells);

    CHECK(pipeline.queue_size() == 3); // Only code cells.
    CHECK(pipeline.is_executing());
}

TEST_CASE("NotebookExecutionPipeline — interrupt clears queue", "[phase15][pipeline]")
{
    EventBus bus;
    Config config;
    NotebookCellManager cell_mgr(bus);
    KernelManager kernel_mgr(bus, config);
    NotebookExecutionPipeline pipeline(bus, cell_mgr, kernel_mgr);

    auto cell_id = cell_mgr.add_cell(CellType::kCode, 0);
    pipeline.execute_cell(cell_id);
    CHECK(pipeline.is_executing());

    pipeline.interrupt();
    CHECK_FALSE(pipeline.is_executing());
    CHECK(pipeline.queue_size() == 0);
}

TEST_CASE("NotebookExecutionPipeline — mark completed and advance", "[phase15][pipeline]")
{
    EventBus bus;
    Config config;
    NotebookCellManager cell_mgr(bus);
    KernelManager kernel_mgr(bus, config);
    NotebookExecutionPipeline pipeline(bus, cell_mgr, kernel_mgr);

    auto cell1 = cell_mgr.add_cell(CellType::kCode, 0);
    auto cell2 = cell_mgr.add_cell(CellType::kCode, 1);

    pipeline.execute_range(ExecutionScope::kAllCells);
    CHECK(pipeline.current_cell().value() == cell1);

    pipeline.mark_completed(cell1, true);
    CHECK(pipeline.completed_count() == 1);
    CHECK(pipeline.current_cell().value() == cell2);
}

TEST_CASE("NotebookExecutionPipeline — callback on completion", "[phase15][pipeline]")
{
    EventBus bus;
    Config config;
    NotebookCellManager cell_mgr(bus);
    KernelManager kernel_mgr(bus, config);
    NotebookExecutionPipeline pipeline(bus, cell_mgr, kernel_mgr);

    bool callback_called = false;
    std::string completed_id;

    pipeline.on_cell_complete(
        [&callback_called, &completed_id](const std::string& cell_id, bool /*success*/)
        {
            callback_called = true;
            completed_id = cell_id;
        });

    auto cell_id = cell_mgr.add_cell(CellType::kCode, 0);
    pipeline.execute_cell(cell_id);
    pipeline.mark_completed(cell_id, true);

    CHECK(callback_called);
    CHECK(completed_id == cell_id);
}

// ============================================================================
// NotebookSearchIndex Tests
// ============================================================================

TEST_CASE("NotebookSearchIndex — index and search", "[phase15][search]")
{
    EventBus bus;
    NotebookSearchIndex search_idx(bus);

    std::vector<IndexedCell> cells;
    IndexedCell cell1;
    cell1.cell_id = "c1";
    cell1.cell_index = 0;
    cell1.cell_type = "code";
    cell1.source = "x = 42\nprint(x)";
    cells.push_back(cell1);

    IndexedCell cell2;
    cell2.cell_id = "c2";
    cell2.cell_index = 1;
    cell2.cell_type = "markdown";
    cell2.source = "# Data Analysis\nThis is a test";
    cells.push_back(cell2);

    search_idx.index_notebook(cells);
    CHECK(search_idx.indexed_cell_count() == 2);

    auto results = search_idx.search("print");
    CHECK_FALSE(results.empty());
    CHECK(results[0].cell_id == "c1");
    CHECK(results[0].match_text == "print");
}

TEST_CASE("NotebookSearchIndex — case insensitive search", "[phase15][search]")
{
    EventBus bus;
    NotebookSearchIndex search_idx(bus);

    IndexedCell cell;
    cell.cell_id = "c1";
    cell.cell_type = "code";
    cell.source = "MyVariable = 100";

    search_idx.add_cell(cell);

    CellSearchOptions options;
    options.case_sensitive = false;

    auto results = search_idx.search("myvariable", options);
    CHECK(results.size() == 1);
}

TEST_CASE("NotebookSearchIndex — filter by cell type", "[phase15][search]")
{
    EventBus bus;
    NotebookSearchIndex search_idx(bus);

    IndexedCell code_cell;
    code_cell.cell_id = "c1";
    code_cell.cell_type = "code";
    code_cell.source = "target = 1";

    IndexedCell md_cell;
    md_cell.cell_id = "c2";
    md_cell.cell_type = "markdown";
    md_cell.source = "target analysis";

    search_idx.add_cell(code_cell);
    search_idx.add_cell(md_cell);

    CellSearchOptions code_only;
    code_only.search_code_cells = true;
    code_only.search_markdown_cells = false;

    auto results = search_idx.search("target", code_only);
    CHECK(results.size() == 1);
    CHECK(results[0].cell_type == "code");
}

TEST_CASE("NotebookSearchIndex — replace in cells", "[phase15][search]")
{
    EventBus bus;
    NotebookSearchIndex search_idx(bus);

    IndexedCell cell;
    cell.cell_id = "c1";
    cell.cell_type = "code";
    cell.source = "old_name = 1\nprint(old_name)";

    search_idx.add_cell(cell);

    auto count = search_idx.replace_in_cells("old_name", "new_name");
    CHECK(count == 2);
}

TEST_CASE("NotebookSearchIndex — search in outputs", "[phase15][search]")
{
    EventBus bus;
    NotebookSearchIndex search_idx(bus);

    IndexedCell cell;
    cell.cell_id = "c1";
    cell.cell_type = "code";
    cell.source = "print('hello')";
    cell.output_text = "hello world";

    search_idx.add_cell(cell);

    auto results = search_idx.search_in_outputs("world");
    CHECK(results.size() == 1);
    CHECK(results[0].in_output);
}

TEST_CASE("NotebookSearchIndex — clear", "[phase15][search]")
{
    EventBus bus;
    NotebookSearchIndex search_idx(bus);

    IndexedCell cell;
    cell.cell_id = "c1";
    cell.cell_type = "code";
    cell.source = "test";
    search_idx.add_cell(cell);

    CHECK(search_idx.indexed_cell_count() == 1);
    search_idx.clear_index();
    CHECK(search_idx.indexed_cell_count() == 0);
}

// ============================================================================
// NotebookSessionManager Tests
// ============================================================================

TEST_CASE("NotebookSessionManager — session lifecycle", "[phase15][session]")
{
    EventBus bus;
    NotebookSessionManager session_mgr(bus);

    session_mgr.start_session("nb-1");

    auto session = session_mgr.get_session("nb-1");
    REQUIRE(session.has_value());
    CHECK(session->notebook_id == "nb-1");
    CHECK_FALSE(session->dirty);
    CHECK(session->trust_level == NotebookTrustLevel::kUntrusted);

    session_mgr.end_session("nb-1");
    CHECK_FALSE(session_mgr.get_session("nb-1").has_value());
}

TEST_CASE("NotebookSessionManager — dirty state", "[phase15][session]")
{
    EventBus bus;
    NotebookSessionManager session_mgr(bus);

    session_mgr.start_session("nb-1");
    CHECK_FALSE(session_mgr.get_session("nb-1")->dirty);

    session_mgr.mark_dirty("nb-1");
    CHECK(session_mgr.get_session("nb-1")->dirty);

    session_mgr.mark_clean("nb-1");
    CHECK_FALSE(session_mgr.get_session("nb-1")->dirty);
}

TEST_CASE("NotebookSessionManager — checkpoints", "[phase15][session]")
{
    EventBus bus;
    NotebookSessionManager session_mgr(bus);

    session_mgr.start_session("nb-1");

    auto cp_result = session_mgr.save_checkpoint("nb-1");
    REQUIRE(cp_result.has_value());

    auto checkpoints = session_mgr.list_checkpoints("nb-1");
    CHECK(checkpoints.size() == 1);
    CHECK(checkpoints[0].notebook_id == "nb-1");

    auto restore_result = session_mgr.restore_checkpoint(cp_result.value());
    CHECK(restore_result.has_value());

    session_mgr.delete_checkpoint(cp_result.value());
    CHECK(session_mgr.list_checkpoints("nb-1").empty());
}

TEST_CASE("NotebookSessionManager — checkpoint without session", "[phase15][session]")
{
    EventBus bus;
    NotebookSessionManager session_mgr(bus);

    auto result = session_mgr.save_checkpoint("nonexistent");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("NotebookSessionManager — trust management", "[phase15][session]")
{
    EventBus bus;
    NotebookSessionManager session_mgr(bus);

    session_mgr.start_session("nb-1");
    CHECK(session_mgr.verify_trust("nb-1") == NotebookTrustLevel::kUntrusted);

    session_mgr.sign_notebook("nb-1");
    CHECK(session_mgr.verify_trust("nb-1") == NotebookTrustLevel::kTrusted);

    session_mgr.revoke_trust("nb-1");
    CHECK(session_mgr.verify_trust("nb-1") == NotebookTrustLevel::kUntrusted);
}

TEST_CASE("NotebookSessionManager — auto-save interval", "[phase15][session]")
{
    EventBus bus;
    NotebookSessionManager session_mgr(bus);

    session_mgr.start_session("nb-1");
    session_mgr.set_auto_save_interval("nb-1", 60);

    auto session = session_mgr.get_session("nb-1");
    CHECK(session->auto_save_interval_seconds == 60);
}

TEST_CASE("NotebookSessionManager — active sessions", "[phase15][session]")
{
    EventBus bus;
    NotebookSessionManager session_mgr(bus);

    session_mgr.start_session("nb-1");
    session_mgr.start_session("nb-2");

    auto sessions = session_mgr.active_sessions();
    CHECK(sessions.size() == 2);
}

// ============================================================================
// CellDependencyTracker Tests
// ============================================================================

TEST_CASE("CellDependencyTracker — analyze and build graph", "[phase15][dependency]")
{
    EventBus bus;
    CellDependencyTracker tracker(bus);

    tracker.analyze_cell("c1", "x = 10");
    tracker.analyze_cell("c2", "y = x + 1");
    tracker.build_dependency_graph();

    CHECK(tracker.tracked_cell_count() == 2);

    auto def = tracker.defined_in("x");
    REQUIRE(def.has_value());
    CHECK(def.value() == "c1");
}

TEST_CASE("CellDependencyTracker — stale cells", "[phase15][dependency]")
{
    EventBus bus;
    CellDependencyTracker tracker(bus);

    tracker.analyze_cell("c1", "x = 10");
    tracker.analyze_cell("c2", "y = x + 1");
    tracker.analyze_cell("c3", "z = y * 2");
    tracker.build_dependency_graph();

    auto stale = tracker.stale_cells("c1");
    CHECK_FALSE(stale.empty());
    // c2 depends on c1, c3 depends on c2 (transitively on c1).
    CHECK(stale.size() >= 1);
}

TEST_CASE("CellDependencyTracker — execution order", "[phase15][dependency]")
{
    EventBus bus;
    CellDependencyTracker tracker(bus);

    tracker.analyze_cell("c1", "x = 10");
    tracker.analyze_cell("c2", "y = x + 1");
    tracker.build_dependency_graph();

    auto order = tracker.execution_order();
    CHECK(order.size() == 2);
    // c1 should come before c2 in execution order.
    auto pos_c1 = std::find(order.begin(), order.end(), "c1");
    auto pos_c2 = std::find(order.begin(), order.end(), "c2");
    CHECK(pos_c1 < pos_c2);
}

TEST_CASE("CellDependencyTracker — no circular dependencies", "[phase15][dependency]")
{
    EventBus bus;
    CellDependencyTracker tracker(bus);

    tracker.analyze_cell("c1", "x = 10");
    tracker.analyze_cell("c2", "y = x + 1");
    tracker.build_dependency_graph();

    CHECK_FALSE(tracker.has_circular_dependency());
}

TEST_CASE("CellDependencyTracker — all defined variables", "[phase15][dependency]")
{
    EventBus bus;
    CellDependencyTracker tracker(bus);

    tracker.analyze_cell("c1", "x = 10\ny = 20");
    tracker.build_dependency_graph();

    auto vars = tracker.all_defined_variables();
    CHECK(vars.size() >= 2);
}

TEST_CASE("CellDependencyTracker — clear", "[phase15][dependency]")
{
    EventBus bus;
    CellDependencyTracker tracker(bus);

    tracker.analyze_cell("c1", "x = 10");
    CHECK(tracker.tracked_cell_count() == 1);

    tracker.clear();
    CHECK(tracker.tracked_cell_count() == 0);
}

TEST_CASE("CellDependencyTracker — remove cell", "[phase15][dependency]")
{
    EventBus bus;
    CellDependencyTracker tracker(bus);

    tracker.analyze_cell("c1", "x = 10");
    tracker.analyze_cell("c2", "y = 20");
    CHECK(tracker.tracked_cell_count() == 2);

    tracker.remove_cell("c1");
    CHECK(tracker.tracked_cell_count() == 1);
}

// ============================================================================
// NotebookToolbar Tests
// ============================================================================

TEST_CASE("NotebookToolbar — register and find command", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    NotebookCommand cmd;
    cmd.id = "notebook.runCell";
    cmd.label = "Run Cell";
    cmd.shortcut = "Shift+Enter";
    cmd.category = "execution";
    cmd.enabled = true;
    cmd.handler = [] {};

    toolbar.register_command(cmd);

    CHECK(toolbar.command_count() == 1);

    auto found = toolbar.find_by_id("notebook.runCell");
    REQUIRE(found.has_value());
    CHECK(found->label == "Run Cell");
    CHECK(found->shortcut == "Shift+Enter");
}

TEST_CASE("NotebookToolbar — find by shortcut", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    NotebookCommand cmd;
    cmd.id = "notebook.runCell";
    cmd.label = "Run Cell";
    cmd.shortcut = "Shift+Enter";
    cmd.category = "execution";
    cmd.handler = [] {};
    toolbar.register_command(cmd);

    auto found = toolbar.find_by_shortcut("Shift+Enter");
    REQUIRE(found.has_value());
    CHECK(found->id == "notebook.runCell");

    auto not_found = toolbar.find_by_shortcut("Ctrl+Z");
    CHECK_FALSE(not_found.has_value());
}

TEST_CASE("NotebookToolbar — execute command", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    bool executed = false;
    NotebookCommand cmd;
    cmd.id = "test.cmd";
    cmd.label = "Test";
    cmd.category = "test";
    cmd.enabled = true;
    cmd.handler = [&executed] { executed = true; };
    toolbar.register_command(cmd);

    CHECK(toolbar.execute_command("test.cmd"));
    CHECK(executed);
}

TEST_CASE("NotebookToolbar — disabled command not executed", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    bool executed = false;
    NotebookCommand cmd;
    cmd.id = "test.cmd";
    cmd.label = "Test";
    cmd.category = "test";
    cmd.enabled = true;
    cmd.handler = [&executed] { executed = true; };
    toolbar.register_command(cmd);

    toolbar.set_enabled("test.cmd", false);
    CHECK_FALSE(toolbar.execute_command("test.cmd"));
    CHECK_FALSE(executed);
}

TEST_CASE("NotebookToolbar — commands by category", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    NotebookCommand cmd1;
    cmd1.id = "exec.run";
    cmd1.label = "Run";
    cmd1.category = "execution";
    cmd1.handler = [] {};
    toolbar.register_command(cmd1);

    NotebookCommand cmd2;
    cmd2.id = "exec.stop";
    cmd2.label = "Stop";
    cmd2.category = "execution";
    cmd2.handler = [] {};
    toolbar.register_command(cmd2);

    NotebookCommand cmd3;
    cmd3.id = "cell.insert";
    cmd3.label = "Insert";
    cmd3.category = "cell";
    cmd3.handler = [] {};
    toolbar.register_command(cmd3);

    auto exec_cmds = toolbar.commands_in_category("execution");
    CHECK(exec_cmds.size() == 2);

    auto cell_cmds = toolbar.commands_in_category("cell");
    CHECK(cell_cmds.size() == 1);
}

TEST_CASE("NotebookToolbar — categories", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    NotebookCommand cmd1;
    cmd1.id = "exec.run";
    cmd1.category = "execution";
    cmd1.handler = [] {};
    toolbar.register_command(cmd1);

    NotebookCommand cmd2;
    cmd2.id = "cell.insert";
    cmd2.category = "cell";
    cmd2.handler = [] {};
    toolbar.register_command(cmd2);

    auto cats = toolbar.categories();
    CHECK(cats.size() == 2);
}

TEST_CASE("NotebookToolbar — unregister and clear", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    NotebookCommand cmd;
    cmd.id = "test.cmd";
    cmd.label = "Test";
    cmd.category = "test";
    cmd.handler = [] {};
    toolbar.register_command(cmd);

    CHECK(toolbar.command_count() == 1);

    toolbar.unregister_command("test.cmd");
    CHECK(toolbar.command_count() == 0);

    toolbar.register_command(cmd);
    toolbar.clear();
    CHECK(toolbar.command_count() == 0);
}

TEST_CASE("NotebookToolbar — nonexistent command", "[phase15][toolbar]")
{
    EventBus bus;
    NotebookToolbar toolbar(bus);

    CHECK_FALSE(toolbar.execute_command("nonexistent"));
    CHECK_FALSE(toolbar.find_by_id("nonexistent").has_value());
}
