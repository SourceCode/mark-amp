// V11 Node Editor Tier 3: UI Canvas & Interaction Tests
// Tests for Phases 21-30 components.

#include "node_editor/CanvasViewport.h"
#include "node_editor/DragState.h"
#include "node_editor/HitTester.h"
#include "node_editor/LinkDragState.h"
#include "node_editor/LinkRouter.h"
#include "node_editor/NodeAnnotation.h"
#include "node_editor/NodeClipboard.h"
#include "node_editor/NodeEditorTypes.h"
#include "node_editor/NodeEditorWorkbench.h"
#include "node_editor/NodeGraph.h"
#include "node_editor/NodeGroup.h"
#include "node_editor/NodeLayout.h"
#include "node_editor/NodeThemeTokens.h"
#include "node_editor/SelectionModel.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::node_editor;

// ============================================================================
// Rect
// ============================================================================

TEST_CASE("Rect default is zero", "[node_editor][tier3][types]")
{
    Rect rect;
    REQUIRE(rect.x == 0.0F);
    REQUIRE(rect.width == 0.0F);
}

TEST_CASE("Rect contains point", "[node_editor][tier3][types]")
{
    Rect rect{10.0F, 20.0F, 100.0F, 50.0F};
    REQUIRE(rect.contains({50.0F, 40.0F}));
    REQUIRE_FALSE(rect.contains({5.0F, 40.0F}));
}

TEST_CASE("Rect intersects", "[node_editor][tier3][types]")
{
    Rect a{0.0F, 0.0F, 100.0F, 100.0F};
    Rect b{50.0F, 50.0F, 100.0F, 100.0F};
    REQUIRE(a.intersects(b));
    Rect c{200.0F, 200.0F, 10.0F, 10.0F};
    REQUIRE_FALSE(a.intersects(c));
}

TEST_CASE("Rect union_with", "[node_editor][tier3][types]")
{
    Rect a{10.0F, 10.0F, 50.0F, 50.0F};
    Rect b{80.0F, 80.0F, 30.0F, 30.0F};
    Rect u = a.union_with(b);
    REQUIRE(u.x == 10.0F);
    REQUIRE(u.y == 10.0F);
    REQUIRE(u.right() == 110.0F);
    REQUIRE(u.bottom() == 110.0F);
}

TEST_CASE("FrameId validity", "[node_editor][tier3][types]")
{
    FrameId empty;
    REQUIRE_FALSE(empty.is_valid());
    FrameId valid{42};
    REQUIRE(valid.is_valid());
}

// ============================================================================
// CanvasViewport
// ============================================================================

TEST_CASE("CanvasViewport default state", "[node_editor][tier3][viewport]")
{
    CanvasViewport vp;
    REQUIRE(vp.zoom() == 1.0F);
    REQUIRE(vp.pan().x == 0.0F);
    REQUIRE(vp.pan().y == 0.0F);
}

TEST_CASE("CanvasViewport pan_by", "[node_editor][tier3][viewport]")
{
    CanvasViewport vp;
    vp.pan_by({100.0F, 200.0F});
    REQUIRE(vp.pan().x == 100.0F);
    REQUIRE(vp.pan().y == 200.0F);
}

TEST_CASE("CanvasViewport set_zoom clamps", "[node_editor][tier3][viewport]")
{
    ViewportConfig config;
    config.min_zoom = 0.5F;
    config.max_zoom = 4.0F;
    CanvasViewport vp(config);
    vp.set_zoom(10.0F);
    REQUIRE(vp.zoom() == 4.0F);
    vp.set_zoom(0.01F);
    REQUIRE(vp.zoom() == 0.5F);
}

TEST_CASE("CanvasViewport screen_to_world roundtrip", "[node_editor][tier3][viewport]")
{
    CanvasViewport vp;
    vp.set_pan({50.0F, 30.0F});
    vp.set_zoom(2.0F);

    Vec2 screen{100.0F, 80.0F};
    Vec2 world = vp.screen_to_world(screen);
    Vec2 back = vp.world_to_screen(world);
    REQUIRE_THAT(back.x, Catch::Matchers::WithinAbs(screen.x, 0.01));
    REQUIRE_THAT(back.y, Catch::Matchers::WithinAbs(screen.y, 0.01));
}

TEST_CASE("CanvasViewport snap_to_grid", "[node_editor][tier3][viewport]")
{
    CanvasViewport vp;
    Vec2 snapped = vp.snap_to_grid({27.0F, 53.0F});
    // Default grid: major=100, minor_divisions=4 -> minor_spacing=25
    REQUIRE(snapped.x == 25.0F);
    REQUIRE(snapped.y == 50.0F);
}

TEST_CASE("CanvasViewport zoom_to_fit", "[node_editor][tier3][viewport]")
{
    CanvasViewport vp;
    vp.set_viewport_size(800.0F, 600.0F);
    vp.zoom_to_fit({100.0F, 100.0F, 400.0F, 300.0F});
    REQUIRE(vp.zoom() > 0.0F);
}

TEST_CASE("CanvasViewport reset", "[node_editor][tier3][viewport]")
{
    CanvasViewport vp;
    vp.set_pan({100.0F, 200.0F});
    vp.set_zoom(3.0F);
    vp.reset();
    REQUIRE(vp.zoom() == 1.0F);
    REQUIRE(vp.pan().x == 0.0F);
}

// ============================================================================
// NodeThemeTokens
// ============================================================================

TEST_CASE("NodeThemeToken name lookup", "[node_editor][tier3][theme]")
{
    REQUIRE(node_theme_token_name(NodeThemeToken::CanvasBackground) == "CanvasBackground");
    REQUIRE(node_theme_token_name(NodeThemeToken::SocketFloat) == "SocketFloat");
    REQUIRE(node_theme_token_name(NodeThemeToken::MinimapNode) == "MinimapNode");
}

TEST_CASE("NodeThemeToken count", "[node_editor][tier3][theme]")
{
    REQUIRE(kNodeThemeTokenCount > 0);
}

// ============================================================================
// NodeLayout
// ============================================================================

TEST_CASE("NodeLayout computes bounds", "[node_editor][tier3][layout]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test.Node", {100.0F, 200.0F});
    graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "In");
    graph.add_socket(nid, SocketDirection::Output, SocketDataType::Float, "Out");

    NodeLayout layout;
    auto result = layout.compute_node_layout(graph, nid);
    REQUIRE(result.node_id == nid);
    REQUIRE(result.total_bounds.x == 100.0F);
    REQUIRE(result.total_bounds.y == 200.0F);
    REQUIRE(result.total_bounds.width > 0.0F);
    REQUIRE(result.total_bounds.height > 0.0F);
    REQUIRE(result.sockets.size() == 2);
}

TEST_CASE("NodeLayout collapsed node", "[node_editor][tier3][layout]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test.Node", {0.0F, 0.0F});
    graph.add_socket(nid, SocketDirection::Input, SocketDataType::Float, "In");
    auto* node = graph.find_node_mut(nid);
    REQUIRE(node != nullptr);
    node->flags = NodeFlags::Collapsed;

    NodeLayout layout;
    auto result = layout.compute_node_layout(graph, nid);
    REQUIRE(result.collapsed);
    REQUIRE(result.sockets.empty());
}

TEST_CASE("NodeLayout compute_graph_bounds multi-node", "[node_editor][tier3][layout]")
{
    NodeGraph graph;
    graph.add_node("a", {0.0F, 0.0F});
    graph.add_node("b", {500.0F, 300.0F});

    NodeLayout layout;
    Rect bounds = layout.compute_graph_bounds(graph);
    REQUIRE(bounds.x == 0.0F);
    REQUIRE(bounds.y == 0.0F);
    REQUIRE(bounds.right() >= 500.0F);
    REQUIRE(bounds.bottom() >= 300.0F);
}

// ============================================================================
// NodeEditorWorkbench
// ============================================================================

TEST_CASE("NodeEditorWorkbench lifecycle", "[node_editor][tier3][workbench]")
{
    NodeEditorWorkbench wb;
    REQUIRE_FALSE(wb.has_graph());
    REQUIRE(wb.mode_text() == "Idle");
    REQUIRE(wb.status_text() == "No graph open");

    auto graph = std::make_unique<NodeGraph>();
    graph->add_node("test.Node");
    wb.open_graph(std::move(graph));
    REQUIRE(wb.has_graph());
    REQUIRE(wb.status_text().find("1 nodes") != std::string::npos);

    wb.close_graph();
    REQUIRE_FALSE(wb.has_graph());
}

TEST_CASE("NodeEditorWorkbench panel state", "[node_editor][tier3][workbench]")
{
    NodeEditorWorkbench wb;
    REQUIRE(wb.active_panel() == WorkbenchPanel::Canvas);
    wb.set_active_panel(WorkbenchPanel::Inspector);
    REQUIRE(wb.active_panel() == WorkbenchPanel::Inspector);
}

TEST_CASE("NodeEditorWorkbench commands", "[node_editor][tier3][workbench]")
{
    NodeEditorWorkbench wb;
    auto graph = std::make_unique<NodeGraph>();
    wb.open_graph(std::move(graph));

    auto commands = wb.available_commands();
    REQUIRE(commands.size() >= 2);
    REQUIRE(wb.execute_command("node_editor.zoom_reset"));
    REQUIRE(wb.execute_command("node_editor.close_graph"));
    REQUIRE_FALSE(wb.has_graph());
}

// ============================================================================
// HitTester
// ============================================================================

TEST_CASE("HitTester detects node header", "[node_editor][tier3][hittest]")
{
    NodeGraph graph;
    auto nid = graph.add_node("test.Node", {100.0F, 100.0F});

    NodeLayout layout;
    auto layouts = layout.compute_all_layouts(graph);

    HitTester tester;
    auto result = tester.test({140.0F, 110.0F}, graph, layouts);
    REQUIRE(result.hit());
    REQUIRE(result.target == HitTarget::NodeHeader);
    REQUIRE(result.node_id == nid);
}

TEST_CASE("HitTester misses empty area", "[node_editor][tier3][hittest]")
{
    NodeGraph graph;
    graph.add_node("test.Node", {100.0F, 100.0F});

    NodeLayout layout;
    auto layouts = layout.compute_all_layouts(graph);

    HitTester tester;
    auto result = tester.test({0.0F, 0.0F}, graph, layouts);
    REQUIRE_FALSE(result.hit());
}

TEST_CASE("HitTester nodes_in_rect", "[node_editor][tier3][hittest]")
{
    NodeGraph graph;
    auto n1 = graph.add_node("a", {10.0F, 10.0F});
    auto n2 = graph.add_node("b", {500.0F, 500.0F});

    NodeLayout layout;
    auto layouts = layout.compute_all_layouts(graph);

    HitTester tester;
    auto hits = tester.nodes_in_rect({0.0F, 0.0F, 200.0F, 200.0F}, layouts);
    REQUIRE(hits.size() == 1);
    REQUIRE(hits[0] == n1);

    auto all_hits = tester.nodes_in_rect({0.0F, 0.0F, 1000.0F, 1000.0F}, layouts);
    REQUIRE(all_hits.size() == 2);
}

// ============================================================================
// LinkRouter
// ============================================================================

TEST_CASE("LinkRouter Bezier path", "[node_editor][tier3][link]")
{
    LinkRouter router;
    auto path = router.compute_path({0.0F, 0.0F}, {200.0F, 100.0F});
    REQUIRE(path.mode == RoutingMode::Bezier);
    REQUIRE(path.control_points.size() == 2);
}

TEST_CASE("LinkRouter straight path", "[node_editor][tier3][link]")
{
    LinkRouter router(RoutingMode::Straight);
    auto path = router.compute_path({0.0F, 0.0F}, {200.0F, 100.0F});
    REQUIRE(path.mode == RoutingMode::Straight);
    REQUIRE(path.control_points.empty());
}

TEST_CASE("LinkRouter orthogonal path", "[node_editor][tier3][link]")
{
    LinkRouter router;
    auto path = router.compute_path({0.0F, 0.0F}, {200.0F, 100.0F}, RoutingMode::Orthogonal);
    REQUIRE(path.mode == RoutingMode::Orthogonal);
    REQUIRE(path.control_points.size() == 2);
}

TEST_CASE("LinkRouter reroute insertion", "[node_editor][tier3][link]")
{
    LinkRouter router;
    auto path = router.compute_path({0.0F, 0.0F}, {200.0F, 100.0F});
    auto rerouted = LinkRouter::insert_reroute(path, {100.0F, 50.0F});
    REQUIRE(rerouted.control_points.size() == 1);
}

// ============================================================================
// LinkDragState
// ============================================================================

TEST_CASE("LinkDragState lifecycle", "[node_editor][tier3][linkdrag]")
{
    LinkDragState state;
    REQUIRE(state.phase == LinkDragPhase::Idle);
    REQUIRE_FALSE(state.is_active());

    state.begin(SocketId{1}, SocketDirection::Output, {50.0F, 60.0F});
    REQUIRE(state.is_active());
    REQUIRE(state.source_socket == SocketId{1});

    state.update({100.0F, 120.0F});
    REQUIRE(state.current_position.x == 100.0F);

    state.update({150.0F, 130.0F}, SocketId{2}, true);
    REQUIRE(state.phase == LinkDragPhase::OverTarget);
    REQUIRE(state.compatible);

    state.complete();
    REQUIRE(state.phase == LinkDragPhase::Completed);

    state.reset();
    REQUIRE(state.phase == LinkDragPhase::Idle);
}

TEST_CASE("LinkDragState cancel", "[node_editor][tier3][linkdrag]")
{
    LinkDragState state;
    state.begin(SocketId{5}, SocketDirection::Input, {0.0F, 0.0F});
    state.cancel();
    REQUIRE(state.phase == LinkDragPhase::Cancelled);
    REQUIRE_FALSE(state.is_active());
}

// ============================================================================
// SelectionModel
// ============================================================================

TEST_CASE("SelectionModel select/deselect", "[node_editor][tier3][selection]")
{
    SelectionModel sel;
    REQUIRE(sel.empty());

    NodeId n1{1};
    NodeId n2{2};
    sel.select(n1);
    REQUIRE(sel.is_selected(n1));
    REQUIRE(sel.count() == 1);

    sel.select(n2);
    REQUIRE(sel.count() == 2);

    sel.deselect(n1);
    REQUIRE_FALSE(sel.is_selected(n1));
    REQUIRE(sel.count() == 1);
}

TEST_CASE("SelectionModel toggle", "[node_editor][tier3][selection]")
{
    SelectionModel sel;
    NodeId n1{1};
    sel.toggle(n1);
    REQUIRE(sel.is_selected(n1));
    sel.toggle(n1);
    REQUIRE_FALSE(sel.is_selected(n1));
}

TEST_CASE("SelectionModel select_only", "[node_editor][tier3][selection]")
{
    SelectionModel sel;
    sel.select(NodeId{1});
    sel.select(NodeId{2});
    sel.select_only(NodeId{3});
    REQUIRE(sel.count() == 1);
    REQUIRE(sel.is_selected(NodeId{3}));
}

TEST_CASE("SelectionModel focus navigation", "[node_editor][tier3][selection]")
{
    SelectionModel sel;
    std::vector<NodeId> ids = {NodeId{1}, NodeId{2}, NodeId{3}};

    sel.focus_node(ids[0]);
    REQUIRE(sel.focused_node() == ids[0]);
    REQUIRE(sel.has_focus());

    sel.focus_next(ids);
    REQUIRE(sel.focused_node() == ids[1]);

    sel.focus_prev(ids);
    REQUIRE(sel.focused_node() == ids[0]);

    sel.clear_focus();
    REQUIRE_FALSE(sel.has_focus());
}

TEST_CASE("SelectionModel select_in_rect", "[node_editor][tier3][selection]")
{
    SelectionModel sel;
    std::vector<NodeId> ids = {NodeId{1}, NodeId{2}};
    std::vector<Rect> bounds = {{10.0F, 10.0F, 50.0F, 50.0F}, {200.0F, 200.0F, 50.0F, 50.0F}};

    sel.select_in_rect({0.0F, 0.0F, 100.0F, 100.0F}, ids, bounds);
    REQUIRE(sel.is_selected(NodeId{1}));
    REQUIRE_FALSE(sel.is_selected(NodeId{2}));
}

// ============================================================================
// DragState
// ============================================================================

TEST_CASE("DragState lifecycle", "[node_editor][tier3][drag]")
{
    DragState ds;
    REQUIRE_FALSE(ds.is_active());

    ds.begin(DragKind::MoveNodes, {10.0F, 20.0F});
    REQUIRE(ds.is_active());
    REQUIRE(ds.kind == DragKind::MoveNodes);

    ds.update({30.0F, 50.0F});
    auto d = ds.delta();
    REQUIRE(d.x == 20.0F);
    REQUIRE(d.y == 30.0F);

    ds.commit();
    REQUIRE(ds.phase == DragPhase::Committed);
}

TEST_CASE("DragState box select rect", "[node_editor][tier3][drag]")
{
    DragState ds;
    ds.begin(DragKind::BoxSelect, {100.0F, 100.0F});
    ds.update({200.0F, 250.0F});
    auto rect = ds.drag_rect();
    REQUIRE(rect.x == 100.0F);
    REQUIRE(rect.y == 100.0F);
    REQUIRE(rect.width == 100.0F);
    REQUIRE(rect.height == 150.0F);
}

TEST_CASE("DragState lasso points", "[node_editor][tier3][drag]")
{
    DragState ds;
    ds.begin(DragKind::LassoSelect, {10.0F, 10.0F});
    REQUIRE(ds.lasso_points.size() == 1);
    ds.update({20.0F, 20.0F});
    ds.update({30.0F, 15.0F});
    REQUIRE(ds.lasso_points.size() == 3);
}

// ============================================================================
// NodeClipboard
// ============================================================================

TEST_CASE("NodeClipboard copy", "[node_editor][tier3][clipboard]")
{
    NodeGraph graph;
    auto n1 = graph.add_node("a", {0.0F, 0.0F});
    auto n2 = graph.add_node("b", {100.0F, 0.0F});

    NodeClipboard clipboard;
    auto data = clipboard.copy(graph, {n1, n2});
    REQUIRE(data.valid);
    REQUIRE(data.node_count == 2);
    REQUIRE(NodeClipboard::is_valid(data));
}

TEST_CASE("NodeClipboard cut removes nodes", "[node_editor][tier3][clipboard]")
{
    NodeGraph graph;
    auto n1 = graph.add_node("a");
    graph.add_node("b");
    REQUIRE(graph.node_count() == 2);

    NodeClipboard clipboard;
    clipboard.cut(graph, {n1});
    REQUIRE(graph.node_count() == 1);
}

TEST_CASE("NodeClipboard duplicate", "[node_editor][tier3][clipboard]")
{
    NodeGraph graph;
    auto n1 = graph.add_node("test.Node", {50.0F, 50.0F});
    REQUIRE(graph.node_count() == 1);

    NodeClipboard clipboard;
    auto new_ids = clipboard.duplicate(graph, {n1}, {20.0F, 20.0F});
    REQUIRE(new_ids.size() == 1);
    REQUIRE(graph.node_count() == 2);

    auto* dup = graph.find_node(new_ids[0]);
    REQUIRE(dup != nullptr);
    REQUIRE(dup->position.x == 70.0F);
}

// ============================================================================
// NodeAnnotation
// ============================================================================

TEST_CASE("NodeAnnotation add/find/remove", "[node_editor][tier3][annotation]")
{
    NodeAnnotation annotations;
    auto fid = annotations.add_frame({0.0F, 0.0F, 300.0F, 200.0F}, "My Frame");
    REQUIRE(fid.is_valid());
    REQUIRE(annotations.count() == 1);

    const auto* frame = annotations.find(fid);
    REQUIRE(frame != nullptr);
    REQUIRE(frame->label == "My Frame");
    REQUIRE(frame->type == AnnotationType::Frame);

    REQUIRE(annotations.remove(fid));
    REQUIRE(annotations.count() == 0);
}

TEST_CASE("NodeAnnotation comment and text label", "[node_editor][tier3][annotation]")
{
    NodeAnnotation annotations;
    auto cid = annotations.add_comment({50.0F, 50.0F}, "Note here");
    auto tid = annotations.add_text_label({100.0F, 100.0F}, "Label");
    REQUIRE(annotations.count() == 2);
    REQUIRE(annotations.find(cid)->type == AnnotationType::Comment);
    REQUIRE(annotations.find(tid)->type == AnnotationType::TextLabel);
}

TEST_CASE("NodeAnnotation containment", "[node_editor][tier3][annotation]")
{
    NodeAnnotation annotations;
    auto fid = annotations.add_frame({0.0F, 0.0F, 500.0F, 500.0F}, "Big Frame");
    annotations.update_containment(fid, {NodeId{1}, NodeId{2}});

    const auto* frame = annotations.find(fid);
    REQUIRE(frame->contained_nodes.size() == 2);

    auto containing = annotations.containing_frame({250.0F, 250.0F});
    REQUIRE(containing == fid);
}

TEST_CASE("NodeAnnotation spatial query", "[node_editor][tier3][annotation]")
{
    NodeAnnotation annotations;
    annotations.add_frame({0.0F, 0.0F, 100.0F, 100.0F}, "A");
    annotations.add_frame({200.0F, 200.0F, 100.0F, 100.0F}, "B");

    auto hits = annotations.frames_at({50.0F, 50.0F});
    REQUIRE(hits.size() == 1);
}

// ============================================================================
// NodeGroup
// ============================================================================

TEST_CASE("NodeGroupManager create/remove", "[node_editor][tier3][group]")
{
    NodeGroupManager mgr;
    auto gid = mgr.create_group("TestGroup", NodeId{10});
    REQUIRE(mgr.is_group(NodeId{10}));
    REQUIRE(mgr.group_count() == 1);

    const auto* grp = mgr.find_group(gid);
    REQUIRE(grp != nullptr);
    REQUIRE(grp->name == "TestGroup");
    REQUIRE(grp->inner_graph != nullptr);

    REQUIRE(mgr.remove_group(gid));
    REQUIRE(mgr.group_count() == 0);
}

TEST_CASE("NodeGroupManager I/O interface", "[node_editor][tier3][group]")
{
    NodeGroupManager mgr;
    auto gid = mgr.create_group("IOGroup", NodeId{20});
    mgr.add_group_input(gid, "A", SocketDataType::Float);
    mgr.add_group_output(gid, "Result", SocketDataType::Float);

    const auto* grp = mgr.find_group(gid);
    REQUIRE(grp->input_sockets.size() == 1);
    REQUIRE(grp->output_sockets.size() == 1);
    REQUIRE(grp->input_sockets[0].name == "A");
}

TEST_CASE("NodeGroupManager navigation", "[node_editor][tier3][group]")
{
    NodeGroupManager mgr;
    mgr.create_group("Outer", NodeId{1});
    mgr.create_group("Inner", NodeId{2});

    REQUIRE(mgr.navigation_depth() == 0);

    mgr.push_into_group(NodeId{1});
    REQUIRE(mgr.navigation_depth() == 1);
    REQUIRE(mgr.current_group() == NodeId{1});

    mgr.push_into_group(NodeId{2});
    REQUIRE(mgr.navigation_depth() == 2);

    auto breadcrumb = mgr.breadcrumb_path();
    REQUIRE(breadcrumb.size() == 2);

    mgr.pop_to_parent();
    REQUIRE(mgr.navigation_depth() == 1);
    REQUIRE(mgr.current_group() == NodeId{1});
}
