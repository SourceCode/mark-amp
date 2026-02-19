// V11 Node Editor Tier 4 — Unit Tests
// Covers Phases 31-40: Runtime Infrastructure

#include "node_editor/AsyncEvaluator.h"
#include "node_editor/DebugOverlay.h"
#include "node_editor/DirtyPropagator.h"
#include "node_editor/InspectorModel.h"
#include "node_editor/NodeDiagnostics.h"
#include "node_editor/NodeGraph.h"
#include "node_editor/NodeLayout.h"
#include "node_editor/NodeShortcuts.h"
#include "node_editor/NodeTestHarness.h"
#include "node_editor/PreviewSystem.h"
#include "node_editor/SelectionModel.h"
#include "node_editor/ViewportCuller.h"
#include "node_editor/WorkspacePersistence.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::node_editor;

// ============================================================================
// Phase 31: InspectorModel
// ============================================================================

TEST_CASE("InspectorModel: empty when no selection", "[inspector]")
{
    InspectorModel inspector;
    NodeGraph graph;
    SelectionModel selection;

    inspector.refresh(graph, selection);

    REQUIRE(inspector.is_empty());
    REQUIRE(inspector.property_count() == 0);
    REQUIRE(inspector.selected_count() == 0);
    REQUIRE(inspector.header_text() == "No selection");
}

TEST_CASE("InspectorModel: single node selection shows properties", "[inspector]")
{
    InspectorModel inspector;
    NodeGraph graph;
    SelectionModel selection;

    auto nid = graph.add_node("test_node", Vec2{10.0F, 20.0F});
    graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "Threshold");
    selection.select(nid);

    inspector.refresh(graph, selection);

    REQUIRE_FALSE(inspector.is_empty());
    REQUIRE(inspector.selected_count() == 1);
    REQUIRE(inspector.header_text() == "Node Inspector");
    // At minimum: Label, Position X, Position Y, Threshold
    REQUIRE(inspector.property_count() >= 4);
}

TEST_CASE("InspectorModel: multi-selection shows mixed state for differing labels", "[inspector]")
{
    InspectorModel inspector;
    NodeGraph graph;
    SelectionModel selection;

    auto nid1 = graph.add_node("type_a", Vec2{0.0F, 0.0F});
    auto nid2 = graph.add_node("type_b", Vec2{100.0F, 0.0F});
    selection.select(nid1);
    selection.select(nid2);

    inspector.refresh(graph, selection);

    REQUIRE(inspector.selected_count() == 2);
    // Label property (index 0) should be mixed
    REQUIRE(inspector.is_mixed(0));
}

TEST_CASE("InspectorModel: staged edit lifecycle", "[inspector]")
{
    InspectorModel inspector;
    NodeGraph graph;
    SelectionModel selection;

    auto nid = graph.add_node("test_node");
    selection.select(nid);
    inspector.refresh(graph, selection);

    REQUIRE_FALSE(inspector.is_editing());
    REQUIRE_FALSE(inspector.has_staged_changes());

    inspector.begin_edit();
    REQUIRE(inspector.is_editing());

    inspector.set_property(0, std::string("NewLabel"));
    REQUIRE(inspector.has_staged_changes());

    inspector.cancel();
    REQUIRE_FALSE(inspector.is_editing());
    REQUIRE_FALSE(inspector.has_staged_changes());
}

TEST_CASE("InspectorModel: apply changes to graph", "[inspector]")
{
    InspectorModel inspector;
    NodeGraph graph;
    SelectionModel selection;

    auto nid = graph.add_node("test_node", Vec2{0.0F, 0.0F});
    selection.select(nid);
    inspector.refresh(graph, selection);

    inspector.begin_edit();
    // Set Position X (index 1) to 500.0F
    inspector.set_property(1, 500.0F);
    inspector.apply(graph);

    const auto* node = graph.find_node(nid);
    REQUIRE(node != nullptr);
    REQUIRE(node->position.x == 500.0F);
}

// ============================================================================
// Phase 32: PreviewSystem
// ============================================================================

TEST_CASE("PreviewSystem: request and update preview", "[preview]")
{
    PreviewSystem preview;
    NodeId nid{1};

    REQUIRE(preview.preview_count() == 0);

    preview.request_preview(nid);
    REQUIRE(preview.is_requested(nid));

    PreviewData data;
    data.node_id = nid;
    data.format = PreviewFormat::kText;
    data.text_value = "42.0";
    data.generation = 1;
    data.timestamp = std::chrono::steady_clock::now();

    preview.update_preview(nid, data);
    REQUIRE(preview.preview_count() == 1);
    REQUIRE_FALSE(preview.is_requested(nid)); // Cleared after update

    const auto* result = preview.preview_data(nid);
    REQUIRE(result != nullptr);
    REQUIRE(result->text_value == "42.0");
    REQUIRE(result->is_valid());
}

TEST_CASE("PreviewSystem: pin management", "[preview]")
{
    PreviewSystem preview;
    NodeId nid{1};

    REQUIRE_FALSE(preview.is_pinned(nid));

    preview.pin(nid);
    REQUIRE(preview.is_pinned(nid));

    preview.toggle_pin(nid);
    REQUIRE_FALSE(preview.is_pinned(nid));

    preview.toggle_pin(nid);
    REQUIRE(preview.is_pinned(nid));

    auto pinned = preview.pinned_nodes();
    REQUIRE(pinned.size() == 1);
}

TEST_CASE("PreviewSystem: inline value display", "[preview]")
{
    PreviewSystem preview;
    SocketId sid{10};

    REQUIRE_FALSE(preview.has_inline_value(sid));

    preview.set_inline_value(sid, "3.14");
    REQUIRE(preview.has_inline_value(sid));
    REQUIRE(preview.inline_value(sid) == "3.14");
}

TEST_CASE("PreviewSystem: clear all", "[preview]")
{
    PreviewSystem preview;
    NodeId nid{1};

    preview.request_preview(nid);
    preview.pin(nid);

    PreviewData data;
    data.node_id = nid;
    data.format = PreviewFormat::kRGBA8;
    data.timestamp = std::chrono::steady_clock::now();
    preview.update_preview(nid, data);

    preview.clear_all();
    REQUIRE(preview.preview_count() == 0);
    REQUIRE_FALSE(preview.is_pinned(nid));
    REQUIRE_FALSE(preview.is_requested(nid));
}

// ============================================================================
// Phase 33: DebugOverlay
// ============================================================================

TEST_CASE("DebugOverlay: breakpoint management", "[debug]")
{
    DebugOverlay debug;

    NodeId nid{5};
    REQUIRE_FALSE(debug.has_breakpoint(nid));
    REQUIRE(debug.breakpoint_count() == 0);

    debug.set_breakpoint(nid);
    REQUIRE(debug.has_breakpoint(nid));
    REQUIRE(debug.breakpoint_count() == 1);

    debug.toggle_breakpoint(nid);
    REQUIRE_FALSE(debug.has_breakpoint(nid));

    debug.toggle_breakpoint(nid);
    REQUIRE(debug.has_breakpoint(nid));

    debug.clear_breakpoints();
    REQUIRE(debug.breakpoint_count() == 0);
}

TEST_CASE("DebugOverlay: trace recording", "[debug]")
{
    DebugOverlay debug;

    TraceEntry entry;
    entry.node_id = NodeId{1};
    entry.execution_index = 0;
    entry.status = TraceStatus::kCompleted;
    entry.duration_ms = 1.5F;

    debug.record_trace(entry);
    REQUIRE(debug.trace_count() == 1);

    const auto* found = debug.trace_for(NodeId{1});
    REQUIRE(found != nullptr);
    REQUIRE(found->duration_ms == 1.5F);

    auto order = debug.execution_order();
    REQUIRE(order.size() == 1);
    REQUIRE(order[0] == NodeId{1});
}

TEST_CASE("DebugOverlay: session lifecycle", "[debug]")
{
    DebugOverlay debug;

    REQUIRE_FALSE(debug.is_active());
    REQUIRE(debug.phase() == DebugPhase::kIdle);

    debug.start_session();
    REQUIRE(debug.is_active());
    REQUIRE(debug.phase() == DebugPhase::kRunning);

    debug.pause();
    REQUIRE(debug.is_paused());

    debug.step_over();
    REQUIRE(debug.phase() == DebugPhase::kStepping);

    debug.continue_execution();
    REQUIRE(debug.phase() == DebugPhase::kRunning);

    debug.stop_session();
    REQUIRE_FALSE(debug.is_active());
}

TEST_CASE("DebugOverlay: overlay flags", "[debug]")
{
    DebugOverlay debug;

    REQUIRE_FALSE(debug.flags().show_timings);
    debug.set_show_timings(true);
    REQUIRE(debug.flags().show_timings);

    debug.set_show_values(true);
    REQUIRE(debug.flags().show_values);

    debug.set_show_exec_order(true);
    REQUIRE(debug.flags().show_exec_order);
}

// ============================================================================
// Phase 34: DirtyPropagator
// ============================================================================

TEST_CASE("DirtyPropagator: mark and clear", "[dirty]")
{
    DirtyPropagator dirty;
    NodeId nid{1};

    REQUIRE_FALSE(dirty.is_dirty(nid));
    REQUIRE(dirty.dirty_count() == 0);

    dirty.mark_dirty_local(nid);
    REQUIRE(dirty.is_dirty(nid));
    REQUIRE(dirty.dirty_count() == 1);

    dirty.clear_dirty(nid);
    REQUIRE_FALSE(dirty.is_dirty(nid));
}

TEST_CASE("DirtyPropagator: cache versioning", "[dirty]")
{
    DirtyPropagator dirty;
    NodeId nid{1};

    REQUIRE(dirty.cache_version(nid) == 0);
    REQUIRE(dirty.is_cache_valid(nid, 0));

    dirty.mark_dirty_local(nid);
    REQUIRE(dirty.cache_version(nid) == 1);
    REQUIRE_FALSE(dirty.is_cache_valid(nid, 0));
    REQUIRE(dirty.is_cache_valid(nid, 1));

    dirty.mark_dirty_local(nid);
    REQUIRE(dirty.cache_version(nid) == 2);
}

TEST_CASE("DirtyPropagator: downstream propagation through graph", "[dirty]")
{
    NodeGraph graph;
    auto nid1 = graph.add_node("source");
    auto nid2 = graph.add_node("middle");
    auto nid3 = graph.add_node("sink");

    auto out1 = graph.add_socket(nid1, SocketDirection::Output, SocketDataType::Float, "out");
    auto in2 = graph.add_socket(nid2, SocketDirection::Input, SocketDataType::Float, "in");
    auto out2 = graph.add_socket(nid2, SocketDirection::Output, SocketDataType::Float, "out");
    auto in3 = graph.add_socket(nid3, SocketDirection::Input, SocketDataType::Float, "in");

    graph.add_link(out1, in2);
    graph.add_link(out2, in3);

    DirtyPropagator dirty;
    dirty.mark_dirty(nid1, graph);

    REQUIRE(dirty.is_dirty(nid1));
    REQUIRE(dirty.is_dirty(nid2));
    REQUIRE(dirty.is_dirty(nid3));
}

TEST_CASE("DirtyPropagator: clear all", "[dirty]")
{
    DirtyPropagator dirty;
    dirty.mark_dirty_local(NodeId{1});
    dirty.mark_dirty_local(NodeId{2});
    REQUIRE(dirty.dirty_count() == 2);

    dirty.clear_all();
    REQUIRE(dirty.dirty_count() == 0);
}

// ============================================================================
// Phase 35: ViewportCuller
// ============================================================================

TEST_CASE("ViewportCuller: LOD for different zoom levels", "[culler]")
{
    REQUIRE(ViewportCuller::lod_for_zoom(1.0F) == LodLevel::kFull);
    REQUIRE(ViewportCuller::lod_for_zoom(0.5F) == LodLevel::kFull);
    REQUIRE(ViewportCuller::lod_for_zoom(0.3F) == LodLevel::kSimplified);
    REQUIRE(ViewportCuller::lod_for_zoom(0.1F) == LodLevel::kBlock);
    REQUIRE(ViewportCuller::lod_for_zoom(0.01F) == LodLevel::kDot);
}

TEST_CASE("ViewportCuller: cull nodes outside viewport", "[culler]")
{
    ViewportCuller culler(0.0F); // No margin for precise testing

    // Create two layout results
    NodeLayoutResult layout1;
    layout1.node_id = NodeId{1};
    layout1.total_bounds = Rect{0.0F, 0.0F, 100.0F, 80.0F};

    NodeLayoutResult layout2;
    layout2.node_id = NodeId{2};
    layout2.total_bounds = Rect{2000.0F, 2000.0F, 100.0F, 80.0F};

    std::vector<NodeLayoutResult> layouts = {layout1, layout2};

    Rect viewport{0.0F, 0.0F, 500.0F, 500.0F};
    auto result = culler.cull_nodes(viewport, layouts);

    REQUIRE(result.visible_nodes.size() == 1);
    REQUIRE(result.visible_nodes[0] == NodeId{1});
    REQUIRE(result.culled_nodes == 1);
    REQUIRE(result.total_nodes == 2);
}

TEST_CASE("ViewportCuller: margin configuration", "[culler]")
{
    ViewportCuller culler(50.0F);
    REQUIRE(culler.margin() == 50.0F);

    culler.set_margin(100.0F);
    REQUIRE(culler.margin() == 100.0F);
}

// ============================================================================
// Phase 36: AsyncEvaluator
// ============================================================================

TEST_CASE("AsyncEvaluator: submit and query job", "[async]")
{
    AsyncEvaluator evaluator;

    auto job_id = evaluator.submit_job("Test eval", {NodeId{1}, NodeId{2}});
    REQUIRE(job_id.is_valid());
    REQUIRE(evaluator.job_count() == 1);
    REQUIRE(evaluator.job_status(job_id) == JobStatus::kPending);

    const auto* info = evaluator.job_info(job_id);
    REQUIRE(info != nullptr);
    REQUIRE(info->description == "Test eval");
    REQUIRE(info->dirty_node_count == 2);
}

TEST_CASE("AsyncEvaluator: job lifecycle", "[async]")
{
    AsyncEvaluator evaluator;

    auto job_id = evaluator.submit_job("Lifecycle test", {});

    evaluator.start_job(job_id);
    REQUIRE(evaluator.job_status(job_id) == JobStatus::kRunning);

    evaluator.update_progress(job_id, 0.5F);
    REQUIRE(evaluator.job_progress(job_id) == 0.5F);

    evaluator.complete_job(job_id);
    REQUIRE(evaluator.job_status(job_id) == JobStatus::kCompleted);
    REQUIRE(evaluator.job_progress(job_id) == 1.0F);
}

TEST_CASE("AsyncEvaluator: cancel job", "[async]")
{
    AsyncEvaluator evaluator;

    auto job_id = evaluator.submit_job("Cancel test", {});
    evaluator.cancel_job(job_id);
    REQUIRE(evaluator.job_status(job_id) == JobStatus::kCancelled);
}

TEST_CASE("AsyncEvaluator: fail job with error", "[async]")
{
    AsyncEvaluator evaluator;

    auto job_id = evaluator.submit_job("Fail test", {});
    evaluator.start_job(job_id);
    evaluator.fail_job(job_id, "Division by zero");

    REQUIRE(evaluator.job_status(job_id) == JobStatus::kFailed);
    REQUIRE(evaluator.job_info(job_id)->error_message == "Division by zero");
}

TEST_CASE("AsyncEvaluator: active job tracking", "[async]")
{
    AsyncEvaluator evaluator;

    auto jid1 = evaluator.submit_job("Job1", {});
    auto jid2 = evaluator.submit_job("Job2", {});

    REQUIRE(evaluator.active_count() == 2);

    evaluator.complete_job(jid1);
    REQUIRE(evaluator.active_count() == 1);

    evaluator.clear_completed();
    REQUIRE(evaluator.job_count() == 1);
}

TEST_CASE("AsyncEvaluator: cancel all", "[async]")
{
    AsyncEvaluator evaluator;

    evaluator.submit_job("Job1", {});
    evaluator.submit_job("Job2", {});
    evaluator.submit_job("Job3", {});

    evaluator.cancel_all();
    REQUIRE(evaluator.active_count() == 0);
}

// ============================================================================
// Phase 37: NodeDiagnostics
// ============================================================================

TEST_CASE("NodeDiagnostics: add and query", "[diagnostics]")
{
    NodeDiagnostics diag;
    NodeId nid{1};

    REQUIRE(diag.total_count() == 0);

    auto err_id = diag.add_error(nid, "Type mismatch");
    REQUIRE(err_id > 0);
    REQUIRE(diag.error_count() == 1);
    REQUIRE(diag.total_count() == 1);
    REQUIRE(diag.has_errors(nid));
}

TEST_CASE("NodeDiagnostics: mixed severities", "[diagnostics]")
{
    NodeDiagnostics diag;
    NodeId nid{1};

    diag.add_error(nid, "Error msg");
    diag.add_warning(nid, "Warning msg");
    diag.add_info(nid, "Info msg");

    REQUIRE(diag.error_count() == 1);
    REQUIRE(diag.warning_count() == 1);
    REQUIRE(diag.total_count() == 3);
    REQUIRE(diag.has_errors(nid));
    REQUIRE(diag.has_warnings(nid));
}

TEST_CASE("NodeDiagnostics: clear by node", "[diagnostics]")
{
    NodeDiagnostics diag;
    NodeId nid1{1};
    NodeId nid2{2};

    diag.add_error(nid1, "Error1");
    diag.add_error(nid2, "Error2");
    REQUIRE(diag.total_count() == 2);

    diag.clear(nid1);
    REQUIRE(diag.total_count() == 1);
    REQUIRE_FALSE(diag.has_errors(nid1));
    REQUIRE(diag.has_errors(nid2));
}

TEST_CASE("NodeDiagnostics: find and remove by ID", "[diagnostics]")
{
    NodeDiagnostics diag;
    auto diag_id = diag.add_error(NodeId{1}, "Test error");

    const auto* found = diag.find_diagnostic(diag_id);
    REQUIRE(found != nullptr);
    REQUIRE(found->message == "Test error");

    diag.remove(diag_id);
    REQUIRE(diag.total_count() == 0);
}

TEST_CASE("NodeDiagnostics: recovery information", "[diagnostics]")
{
    Diagnostic recovery_diag;
    recovery_diag.severity = DiagnosticSeverity::kError;
    recovery_diag.message = "Missing input";
    recovery_diag.has_recovery = true;
    recovery_diag.recovery_description = "Connect float input";

    REQUIRE(NodeDiagnostics::has_recovery(recovery_diag));
    REQUIRE(NodeDiagnostics::recovery_description(recovery_diag) == "Connect float input");
}

// ============================================================================
// Phase 38: NodeShortcuts
// ============================================================================

TEST_CASE("NodeShortcuts: default bindings exist", "[shortcuts]")
{
    NodeShortcuts shortcuts;

    REQUIRE(shortcuts.binding_count() > 0);
    // Should have common shortcuts
    auto copy_cmd = shortcuts.find_command('C', KeyModifier::kCtrl);
    REQUIRE(copy_cmd.has_value());
    REQUIRE(*copy_cmd == "node.copy");
}

TEST_CASE("NodeShortcuts: find command by key", "[shortcuts]")
{
    NodeShortcuts shortcuts;

    auto paste_cmd = shortcuts.find_command('V', KeyModifier::kCtrl);
    REQUIRE(paste_cmd.has_value());
    REQUIRE(*paste_cmd == "node.paste");

    auto unknown = shortcuts.find_command('Q', KeyModifier::kNone);
    REQUIRE_FALSE(unknown.has_value());
}

TEST_CASE("NodeShortcuts: register custom shortcut", "[shortcuts]")
{
    NodeShortcuts shortcuts;

    shortcuts.register_shortcut(
        "custom.action", 'K', KeyModifier::kCtrl, "Ctrl+K", "Custom action");

    auto found = shortcuts.find_command('K', KeyModifier::kCtrl);
    REQUIRE(found.has_value());
    REQUIRE(*found == "custom.action");
}

TEST_CASE("NodeShortcuts: unregister and reset", "[shortcuts]")
{
    NodeShortcuts shortcuts;
    auto initial_count = shortcuts.binding_count();

    shortcuts.register_shortcut("temp.action", 'T', KeyModifier::kCtrl, "Ctrl+T", "Temp");
    REQUIRE(shortcuts.binding_count() == initial_count + 1);

    shortcuts.unregister("temp.action");
    REQUIRE(shortcuts.binding_count() == initial_count);

    shortcuts.reset_to_defaults();
    REQUIRE(shortcuts.binding_count() == initial_count);
}

TEST_CASE("NodeShortcuts: accessibility description", "[shortcuts]")
{
    NodeShortcuts shortcuts;

    auto desc = shortcuts.describe_action("node.copy");
    REQUIRE(desc == "Copy selected nodes");

    auto unknown = shortcuts.describe_action("nonexistent");
    REQUIRE(unknown.find("Unknown") != std::string::npos);
}

TEST_CASE("NodeShortcuts: conflict detection", "[shortcuts]")
{
    NodeShortcuts shortcuts;

    REQUIRE(shortcuts.is_bound('C', KeyModifier::kCtrl));
    REQUIRE_FALSE(shortcuts.is_bound('Q', KeyModifier::kNone));
}

// ============================================================================
// Phase 39: WorkspacePersistence
// ============================================================================

TEST_CASE("WorkspacePersistence: capture and store snapshot", "[workspace]")
{
    WorkspacePersistence persistence;

    auto snapshot = persistence.capture_snapshot("{\"nodes\":[]}", ViewportState{}, {});

    REQUIRE(snapshot.is_valid());
    REQUIRE(snapshot.version == 1);

    persistence.store_snapshot(snapshot);
    REQUIRE(persistence.snapshot_count() == 1);

    const auto* latest = persistence.latest_snapshot();
    REQUIRE(latest != nullptr);
    REQUIRE(latest->graph_data == "{\"nodes\":[]}");
}

TEST_CASE("WorkspacePersistence: autosave logic", "[workspace]")
{
    WorkspacePersistence persistence;

    // Not modified: should not autosave
    REQUIRE_FALSE(persistence.should_autosave(60000.0F));

    persistence.mark_modified();
    REQUIRE(persistence.is_modified());

    // Modified but not enough time: should not autosave (default 30s)
    REQUIRE_FALSE(persistence.should_autosave(10000.0F));

    // Modified and enough time: should autosave
    REQUIRE(persistence.should_autosave(40000.0F));
}

TEST_CASE("WorkspacePersistence: crash recovery", "[workspace]")
{
    WorkspacePersistence persistence;

    REQUIRE_FALSE(persistence.has_recovery_data());

    auto snapshot = persistence.capture_snapshot("{\"recovery\":true}", ViewportState{}, {});
    persistence.store_snapshot(snapshot);
    persistence.save_as_recovery();

    REQUIRE(persistence.has_recovery_data());

    const auto* recovery = persistence.load_recovery();
    REQUIRE(recovery != nullptr);
    REQUIRE(recovery->graph_data == "{\"recovery\":true}");

    persistence.discard_recovery();
    REQUIRE_FALSE(persistence.has_recovery_data());
}

TEST_CASE("WorkspacePersistence: snapshot trimming", "[workspace]")
{
    WorkspacePersistence persistence;
    persistence.config_mut().max_snapshots = 3;

    for (int idx = 0; idx < 5; ++idx)
    {
        auto snap =
            persistence.capture_snapshot("data_" + std::to_string(idx), ViewportState{}, {});
        persistence.store_snapshot(snap);
    }

    REQUIRE(persistence.snapshot_count() == 3);
    REQUIRE(persistence.latest_snapshot()->graph_data == "data_4");
}

TEST_CASE("WorkspacePersistence: configuration", "[workspace]")
{
    WorkspacePersistence persistence;

    persistence.set_autosave_enabled(false);
    REQUIRE_FALSE(persistence.config().enabled);

    persistence.set_autosave_interval(5000);
    REQUIRE(persistence.config().interval_ms == 5000);
}

// ============================================================================
// Phase 40: NodeTestHarness
// ============================================================================

TEST_CASE("NodeTestHarness: create chain graph", "[harness]")
{
    auto graph = NodeTestHarness::create_chain_graph(5);
    REQUIRE(graph.node_count() == 5);
    REQUIRE(NodeTestHarness::verify_integrity(graph));
    REQUIRE(NodeTestHarness::link_count(graph) == 4); // 5 nodes in chain = 4 links
}

TEST_CASE("NodeTestHarness: create diamond graph", "[harness]")
{
    auto graph = NodeTestHarness::create_diamond_graph(3, 4);
    REQUIRE(graph.node_count() > 0);
    REQUIRE(NodeTestHarness::verify_integrity(graph));
}

TEST_CASE("NodeTestHarness: create random graph", "[harness]")
{
    auto graph = NodeTestHarness::create_random_graph(20, 0.3F);
    REQUIRE(graph.node_count() == 20);
    REQUIRE(NodeTestHarness::verify_integrity(graph));
}

TEST_CASE("NodeTestHarness: stress graph", "[harness]")
{
    auto graph = NodeTestHarness::create_stress_graph(100); // Smaller for fast tests
    REQUIRE(graph.node_count() == 100);
    REQUIRE(NodeTestHarness::verify_integrity(graph));
}

TEST_CASE("NodeTestHarness: benchmark utility", "[harness]")
{
    int counter = 0;
    auto result = NodeTestHarness::benchmark("counter_test", 100, [&]() { ++counter; });

    REQUIRE(counter == 100);
    REQUIRE(result.passed);
    REQUIRE(result.iterations == 100);
    REQUIRE(result.duration_ms >= 0.0F);
    REQUIRE(result.per_op_ms >= 0.0F);
}

TEST_CASE("NodeTestHarness: benchmark with threshold", "[harness]")
{
    auto result = NodeTestHarness::benchmark(
        "fast_op",
        10,
        []() { /* no-op */ },
        10000.0F); // 10 second threshold
    REQUIRE(result.passed);
}

TEST_CASE("NodeTestHarness: measure single operation", "[harness]")
{
    auto result = NodeTestHarness::measure("single",
                                           []()
                                           {
                                               int sum = 0;
                                               for (int idx = 0; idx < 100; ++idx)
                                               {
                                                   sum += idx;
                                               }
                                               (void)sum; // prevent optimization
                                           });

    REQUIRE(result.iterations == 1);
    REQUIRE(result.duration_ms >= 0.0F);
}

TEST_CASE("NodeTestHarness: empty graph integrity", "[harness]")
{
    NodeGraph graph;
    REQUIRE(NodeTestHarness::verify_integrity(graph));
    REQUIRE(NodeTestHarness::link_count(graph) == 0);
}
