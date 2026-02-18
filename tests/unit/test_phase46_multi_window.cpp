/// @file test_phase46_multi_window.cpp
/// @brief Phase 46 tests — Multi-Window & Workspace Management.

#include "core/EditorGroupManager.h"
#include "core/Events.h"
#include "core/LayoutCommandProvider.h"
#include "core/WindowCommandProvider.h"
#include "core/WindowManager.h"
#include "core/WorkspaceLayoutEngine.h"
#include "core/WorkspaceSessionRestore.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// WindowManager
// ============================================================================

TEST_CASE("WindowManager create window", "[phase46][window]")
{
    WindowManager mgr;
    auto id = mgr.create_window("Test Window");
    REQUIRE_FALSE(id.empty());
    REQUIRE(mgr.window_count() == 1);
}

TEST_CASE("WindowManager first window auto-focused", "[phase46][window]")
{
    WindowManager mgr;
    auto id = mgr.create_window();
    const auto* win = mgr.get_window(id);
    REQUIRE(win != nullptr);
    REQUIRE(win->is_focused);
}

TEST_CASE("WindowManager close window", "[phase46][window]")
{
    WindowManager mgr;
    auto id = mgr.create_window();
    REQUIRE(mgr.close_window(id));
    REQUIRE(mgr.window_count() == 0);
}

TEST_CASE("WindowManager close nonexistent returns false", "[phase46][window]")
{
    WindowManager mgr;
    REQUIRE_FALSE(mgr.close_window("nonexistent"));
}

TEST_CASE("WindowManager focus window", "[phase46][window]")
{
    WindowManager mgr;
    auto id1 = mgr.create_window("Win1");
    auto id2 = mgr.create_window("Win2");
    mgr.focus_window(id2);
    const auto* focused = mgr.focused_window();
    REQUIRE(focused != nullptr);
    REQUIRE(focused->window_id == id2);
    REQUIRE_FALSE(mgr.get_window(id1)->is_focused);
}

TEST_CASE("WindowManager set window state", "[phase46][window]")
{
    WindowManager mgr;
    auto id = mgr.create_window();
    REQUIRE(mgr.set_window_state(id, WindowState::kMaximized));
    REQUIRE(mgr.get_window(id)->state == WindowState::kMaximized);
}

TEST_CASE("WindowManager set window bounds", "[phase46][window]")
{
    WindowManager mgr;
    auto id = mgr.create_window();
    WindowBounds bounds{100, 200, 1920, 1080};
    REQUIRE(mgr.set_window_bounds(id, bounds));
    const auto* win = mgr.get_window(id);
    REQUIRE(win->bounds.width == 1920);
    REQUIRE(win->bounds.height == 1080);
}

TEST_CASE("WindowManager all windows", "[phase46][window]")
{
    WindowManager mgr;
    mgr.create_window("A");
    mgr.create_window("B");
    mgr.create_window("C");
    auto all = mgr.all_windows();
    REQUIRE(all.size() == 3);
}

TEST_CASE("WindowManager clear all", "[phase46][window]")
{
    WindowManager mgr;
    mgr.create_window();
    mgr.create_window();
    mgr.clear_all();
    REQUIRE(mgr.window_count() == 0);
}

// ============================================================================
// EditorGroupManager
// ============================================================================

TEST_CASE("EditorGroupManager create group", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto id = mgr.create_group();
    REQUIRE_FALSE(id.empty());
    REQUIRE(mgr.group_count() == 1);
}

TEST_CASE("EditorGroupManager first group is active", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto id = mgr.create_group();
    const auto* grp = mgr.active_group();
    REQUIRE(grp != nullptr);
    REQUIRE(grp->group_id == id);
}

TEST_CASE("EditorGroupManager open tab", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto gid = mgr.create_group();
    auto tid = mgr.open_tab(gid, "/path/file.md", "file.md");
    REQUIRE_FALSE(tid.empty());
    REQUIRE(mgr.total_tab_count() == 1);
}

TEST_CASE("EditorGroupManager open duplicate tab returns existing", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto gid = mgr.create_group();
    auto tid1 = mgr.open_tab(gid, "/path/file.md");
    auto tid2 = mgr.open_tab(gid, "/path/file.md");
    REQUIRE(tid1 == tid2);
    REQUIRE(mgr.total_tab_count() == 1);
}

TEST_CASE("EditorGroupManager close tab", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto gid = mgr.create_group();
    auto tid = mgr.open_tab(gid, "/file.md");
    REQUIRE(mgr.close_tab(gid, tid));
    REQUIRE(mgr.total_tab_count() == 0);
}

TEST_CASE("EditorGroupManager pin and unpin tab", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto gid = mgr.create_group();
    auto tid = mgr.open_tab(gid, "/file.md");
    REQUIRE(mgr.pin_tab(gid, tid));
    const auto* grp = mgr.get_group(gid);
    REQUIRE(grp->tabs[0].is_pinned);
    REQUIRE(mgr.unpin_tab(gid, tid));
    REQUIRE_FALSE(mgr.get_group(gid)->tabs[0].is_pinned);
}

TEST_CASE("EditorGroupManager split group", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto gid = mgr.create_group();
    auto new_gid = mgr.split_group(gid, SplitDirection::kVertical);
    REQUIRE_FALSE(new_gid.empty());
    REQUIRE(mgr.group_count() == 2);
}

TEST_CASE("EditorGroupManager move tab between groups", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto g1 = mgr.create_group();
    auto g2 = mgr.create_group();
    auto tid = mgr.open_tab(g1, "/file.md");
    REQUIRE(mgr.move_tab(g1, tid, g2));
    REQUIRE(mgr.get_group(g1)->tabs.empty());
    REQUIRE(mgr.get_group(g2)->tabs.size() == 1);
}

TEST_CASE("EditorGroupManager close group", "[phase46][group]")
{
    EditorGroupManager mgr;
    auto g1 = mgr.create_group();
    auto g2 = mgr.create_group();
    REQUIRE(mgr.close_group(g1));
    REQUIRE(mgr.group_count() == 1);
    REQUIRE(mgr.active_group()->group_id == g2);
}

TEST_CASE("EditorGroupManager set active group", "[phase46][group]")
{
    EditorGroupManager mgr;
    mgr.create_group();
    auto g2 = mgr.create_group();
    REQUIRE(mgr.set_active_group(g2));
    REQUIRE(mgr.active_group()->group_id == g2);
}

// ============================================================================
// WorkspaceLayoutEngine
// ============================================================================

TEST_CASE("WorkspaceLayoutEngine initial state", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    REQUIRE_FALSE(engine.has_root());
    REQUIRE(engine.preset_count() == 0);
}

TEST_CASE("WorkspaceLayoutEngine set and get root", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    LayoutNode root;
    root.node_id = "root";
    root.type = LayoutNodeType::kGroup;
    engine.set_root(root);
    REQUIRE(engine.has_root());
    REQUIRE(engine.get_root().node_id == "root");
}

TEST_CASE("WorkspaceLayoutEngine load defaults", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    engine.load_defaults();
    REQUIRE(engine.preset_count() == 3);
}

TEST_CASE("WorkspaceLayoutEngine save and restore", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    LayoutNode root;
    root.node_id = "custom_root";
    root.type = LayoutNodeType::kGroup;
    engine.set_root(root);
    auto preset_id = engine.save_layout("My Layout", "A saved layout");
    REQUIRE_FALSE(preset_id.empty());

    // Modify root
    LayoutNode new_root;
    new_root.node_id = "changed";
    engine.set_root(new_root);
    REQUIRE(engine.get_root().node_id == "changed");

    // Restore
    REQUIRE(engine.restore_layout(preset_id));
    REQUIRE(engine.get_root().node_id == "custom_root");
}

TEST_CASE("WorkspaceLayoutEngine get preset", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    engine.load_defaults();
    const auto* preset = engine.get_preset("builtin_single");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->name == "Single Column");
}

TEST_CASE("WorkspaceLayoutEngine two column preset has children", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    engine.load_defaults();
    const auto* preset = engine.get_preset("builtin_two_col");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->root_node.type == LayoutNodeType::kSplit);
    REQUIRE(preset->root_node.children.size() == 2);
}

TEST_CASE("WorkspaceLayoutEngine three panel preset", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    engine.load_defaults();
    const auto* preset = engine.get_preset("builtin_three_panel");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->root_node.children.size() == 2);
    REQUIRE(preset->root_node.children[1].type == LayoutNodeType::kSplit);
    REQUIRE(preset->root_node.children[1].children.size() == 2);
}

TEST_CASE("WorkspaceLayoutEngine clear presets", "[phase46][layout]")
{
    WorkspaceLayoutEngine engine;
    engine.load_defaults();
    engine.clear_presets();
    REQUIRE(engine.preset_count() == 0);
}

// ============================================================================
// WorkspaceSessionRestore
// ============================================================================

TEST_CASE("WorkspaceSessionRestore save snapshot", "[phase46][session]")
{
    WorkspaceSessionRestore restore;
    auto id = restore.save_snapshot("MyWorkspace");
    REQUIRE_FALSE(id.empty());
    REQUIRE(restore.snapshot_count() == 1);
}

TEST_CASE("WorkspaceSessionRestore latest snapshot", "[phase46][session]")
{
    WorkspaceSessionRestore restore;
    restore.save_snapshot("WS");
    auto id2 = restore.save_snapshot("WS");
    const auto* latest = restore.latest_snapshot("WS");
    REQUIRE(latest != nullptr);
    REQUIRE(latest->snapshot_id == id2);
}

TEST_CASE("WorkspaceSessionRestore restore snapshot", "[phase46][session]")
{
    WorkspaceSessionRestore restore;
    auto id = restore.save_snapshot("WS");
    REQUIRE(restore.restore_snapshot(id));
    REQUIRE_FALSE(restore.restore_snapshot("nonexistent"));
}

TEST_CASE("WorkspaceSessionRestore delete snapshot", "[phase46][session]")
{
    WorkspaceSessionRestore restore;
    auto id = restore.save_snapshot("WS");
    REQUIRE(restore.delete_snapshot(id));
    REQUIRE(restore.snapshot_count() == 0);
}

TEST_CASE("WorkspaceSessionRestore policy", "[phase46][session]")
{
    WorkspaceSessionRestore restore;
    RestorePolicy policy;
    policy.auto_restore = false;
    policy.max_snapshots = 5;
    policy.restore_window_positions = false;
    restore.set_policy(policy);
    auto got = restore.get_policy();
    REQUIRE_FALSE(got.auto_restore);
    REQUIRE(got.max_snapshots == 5);
}

TEST_CASE("WorkspaceSessionRestore max snapshots policy", "[phase46][session]")
{
    WorkspaceSessionRestore restore;
    RestorePolicy policy;
    policy.max_snapshots = 3;
    restore.set_policy(policy);

    restore.save_snapshot("WS");
    restore.save_snapshot("WS");
    restore.save_snapshot("WS");
    restore.save_snapshot("WS"); // 4th — should evict oldest
    REQUIRE(restore.snapshot_count() == 3);
}

TEST_CASE("WorkspaceSessionRestore add file to snapshot", "[phase46][session]")
{
    WorkspaceSessionRestore restore;
    auto id = restore.save_snapshot("WS");
    REQUIRE(restore.add_file_to_snapshot(id, "/path/to/file.md"));
    const auto* snap = restore.latest_snapshot("WS");
    REQUIRE(snap->open_files.size() == 1);
}

// ============================================================================
// WindowCommandProvider
// ============================================================================

TEST_CASE("WindowCommandProvider provides 8 commands", "[phase46][commands]")
{
    REQUIRE(WindowCommandProvider::command_count() == 8);
    REQUIRE(WindowCommandProvider::command_ids().size() == 8);
}

TEST_CASE("WindowCommandProvider register and lookup", "[phase46][commands]")
{
    WindowCommandProvider provider;
    auto cmd = provider.get_command("window.new");
    REQUIRE(cmd.id == "window.new");
    REQUIRE(cmd.category == "Window");
}

// ============================================================================
// LayoutCommandProvider
// ============================================================================

TEST_CASE("LayoutCommandProvider provides 8 commands", "[phase46][commands]")
{
    REQUIRE(LayoutCommandProvider::command_count() == 8);
    REQUIRE(LayoutCommandProvider::command_ids().size() == 8);
}

TEST_CASE("LayoutCommandProvider register and lookup", "[phase46][commands]")
{
    LayoutCommandProvider provider;
    auto cmd = provider.get_command("layout.save");
    REQUIRE(cmd.id == "layout.save");
    REQUIRE(cmd.category == "Layout");
}

// ============================================================================
// Phase 46 Events
// ============================================================================

TEST_CASE("WindowCreatedEvent fields", "[phase46][events]")
{
    events::WindowCreatedEvent evt;
    evt.window_id = "win_1";
    evt.title = "Test";
    REQUIRE(evt.window_id == "win_1");
    REQUIRE(evt.title == "Test");
}

TEST_CASE("WindowClosedEvent fields", "[phase46][events]")
{
    events::WindowClosedEvent evt;
    evt.window_id = "win_2";
    REQUIRE(evt.window_id == "win_2");
}

TEST_CASE("EditorGroupChangedEvent fields", "[phase46][events]")
{
    events::EditorGroupChangedEvent evt;
    evt.group_id = "grp_1";
    evt.action = "split";
    REQUIRE(evt.action == "split");
}

TEST_CASE("TabOpenedEvent fields", "[phase46][events]")
{
    events::TabOpenedEvent evt;
    evt.group_id = "grp_1";
    evt.tab_id = "tab_1";
    evt.file_path = "/path.md";
    REQUIRE(evt.file_path == "/path.md");
}

TEST_CASE("LayoutRestoredEvent fields", "[phase46][events]")
{
    events::LayoutRestoredEvent evt;
    evt.preset_id = "preset_1";
    evt.preset_name = "Two Column";
    REQUIRE(evt.preset_name == "Two Column");
}

TEST_CASE("SessionRestoredEvent fields", "[phase46][events]")
{
    events::SessionRestoredEvent evt;
    evt.snapshot_id = "snap_1";
    evt.workspace_name = "WS";
    evt.files_restored = 5;
    REQUIRE(evt.files_restored == 5);
}
