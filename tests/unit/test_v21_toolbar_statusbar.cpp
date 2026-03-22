/// @file test_v21_toolbar_statusbar.cpp
/// @brief V21 Phase 03 — Tests for Toolbar, Status Bar & Global Button Wiring.
///
/// Tests cover:
///   - ToolbarCommandBinder: binding, dispatch, enablement sync, toggle state,
///     manifest sync, CTA wiring, diagnostics
///   - StatusBarActionRouter: registration, classification, dispatch,
///     enablement sync, manifest sync, diagnostics
///   - Integration: end-to-end toolbar + status bar + manifest lifecycle

#include "core/ControlActionManifest.h"
#include "core/StatusBarActionRouter.h"
#include "core/ToolbarCommandBinder.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Helper: create a manifest with toolbar and status bar actions
// ============================================================================

static auto make_toolbar_manifest() -> ControlActionManifest
{
    ControlActionManifest manifest;

    ActionEntry save;
    save.action_id = "file.save";
    save.label = "Save";
    save.tooltip = "Save the current file";
    save.icon = "save";
    save.shortcut_hint = "⌘S";
    save.category = "File";
    save.surfaces = {ControlSurface::kToolbar, ControlSurface::kMenu};
    save.handler = []() -> bool { return true; };
    save.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(save));

    ActionEntry toggle_sb;
    toggle_sb.action_id = "view.toggleSidebar";
    toggle_sb.label = "Toggle Sidebar";
    toggle_sb.tooltip = "Show/hide the sidebar";
    toggle_sb.icon = "sidebar";
    toggle_sb.shortcut_hint = "⌘B";
    toggle_sb.category = "View";
    toggle_sb.surfaces = {ControlSurface::kToolbar};
    toggle_sb.handler = []() -> bool { return true; };
    toggle_sb.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(toggle_sb));

    ActionEntry canvas_tool;
    canvas_tool.action_id = "canvas.selectTool";
    canvas_tool.label = "Select";
    canvas_tool.tooltip = "Select tool";
    canvas_tool.icon = "cursor";
    canvas_tool.category = "Canvas";
    canvas_tool.surfaces = {ControlSurface::kToolbar};
    canvas_tool.handler = []() -> bool { return true; };
    canvas_tool.visibility = [](const ContextKeyService& ctx) -> bool
    { return ctx.get_bool("canvasMode"); };
    canvas_tool.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(canvas_tool));

    ActionEntry dead_btn;
    dead_btn.action_id = "toolbar.deadButton";
    dead_btn.label = "Dead Button";
    dead_btn.surfaces = {ControlSurface::kToolbar};
    dead_btn.validation_status = ActionValidationStatus::kDead;
    manifest.register_action(std::move(dead_btn));

    // Status bar actions
    ActionEntry encoding;
    encoding.action_id = "status.encoding";
    encoding.label = "UTF-8";
    encoding.tooltip = "Change encoding";
    encoding.surfaces = {ControlSurface::kStatusBar};
    encoding.handler = []() -> bool { return true; };
    encoding.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(encoding));

    ActionEntry line_info;
    line_info.action_id = "status.lineInfo";
    line_info.label = "Ln 1, Col 1";
    line_info.tooltip = "Go to line";
    line_info.surfaces = {ControlSurface::kStatusBar};
    line_info.handler = []() -> bool { return true; };
    line_info.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(line_info));

    return manifest;
}

// ============================================================================
// StatusItemKind enum tests
// ============================================================================

TEST_CASE("StatusItemKind — label conversion", "[v21][p03][statusbar]")
{
    CHECK(std::string(status_item_kind_label(StatusItemKind::kActionable)) == "Actionable");
    CHECK(std::string(status_item_kind_label(StatusItemKind::kInformational)) == "Informational");
    CHECK(std::string(status_item_kind_label(StatusItemKind::kToggle)) == "Toggle");
}

// ============================================================================
// ToolbarCommandBinder — Registration & Lookup
// ============================================================================

TEST_CASE("ToolbarCommandBinder — bind and lookup", "[v21][p03][toolbar]")
{
    ToolbarCommandBinder binder;

    ToolbarButtonBinding binding;
    binding.action_id = "file.save";
    binding.label = "Save";
    binding.icon = "save";
    binding.is_bound = true;
    binder.bind(std::move(binding));

    SECTION("Find by action ID")
    {
        auto* found = binder.get_binding("file.save");
        REQUIRE(found != nullptr);
        CHECK(found->label == "Save");
        CHECK(found->icon == "save");
    }

    SECTION("Returns nullptr for unknown")
    {
        CHECK(binder.get_binding("nonexistent") == nullptr);
    }

    SECTION("Count and unbind")
    {
        CHECK(binder.binding_count() == 1);
        CHECK(binder.unbind("file.save"));
        CHECK(binder.binding_count() == 0);
    }
}

TEST_CASE("ToolbarCommandBinder — mode-based filtering", "[v21][p03][toolbar]")
{
    ToolbarCommandBinder binder;

    ToolbarButtonBinding global;
    global.action_id = "file.save";
    global.context_mode = ""; // global
    global.is_bound = true;
    binder.bind(std::move(global));

    ToolbarButtonBinding editor;
    editor.action_id = "editor.format";
    editor.context_mode = "editor";
    editor.is_bound = true;
    binder.bind(std::move(editor));

    ToolbarButtonBinding canvas;
    canvas.action_id = "canvas.select";
    canvas.context_mode = "canvas";
    canvas.is_bound = true;
    binder.bind(std::move(canvas));

    CHECK(binder.global_bindings().size() == 1);
    CHECK(binder.bindings_for_mode("editor").size() == 1);
    CHECK(binder.bindings_for_mode("canvas").size() == 1);
    CHECK(binder.bindings_for_mode("notebook").empty());
}

// ============================================================================
// ToolbarCommandBinder — Dispatch
// ============================================================================

TEST_CASE("ToolbarCommandBinder — dispatch click", "[v21][p03][toolbar]")
{
    auto manifest = make_toolbar_manifest();
    ToolbarCommandBinder binder;

    ToolbarButtonBinding binding;
    binding.action_id = "file.save";
    binding.is_bound = true;
    binder.bind(std::move(binding));

    CHECK(binder.dispatch_click("file.save", manifest));
    CHECK(!binder.dispatch_click("nonexistent", manifest));
}

// ============================================================================
// ToolbarCommandBinder — Toggle State
// ============================================================================

TEST_CASE("ToolbarCommandBinder — toggle state management", "[v21][p03][toggle]")
{
    ToolbarCommandBinder binder;

    ToolbarButtonBinding binding;
    binding.action_id = "view.toggleSidebar";
    binding.is_bound = true;
    binder.bind(std::move(binding));

    ToggleStateEntry toggle;
    toggle.action_id = "view.toggleSidebar";
    toggle.state_key = "sidebar.visible";
    toggle.current_state = false;
    binder.register_toggle(std::move(toggle));

    SECTION("Initial state is off")
    {
        CHECK(!binder.get_toggle_state("view.toggleSidebar"));
        auto* btn = binder.get_binding("view.toggleSidebar");
        REQUIRE(btn != nullptr);
        CHECK(btn->is_toggle);
        CHECK(!btn->is_toggled);
    }

    SECTION("Set toggle state")
    {
        binder.set_toggle_state("view.toggleSidebar", true);
        CHECK(binder.get_toggle_state("view.toggleSidebar"));

        auto* btn = binder.get_binding("view.toggleSidebar");
        CHECK(btn->is_toggled);
    }

    SECTION("Toggle flips on dispatch")
    {
        auto manifest = make_toolbar_manifest();
        binder.dispatch_click("view.toggleSidebar", manifest);
        CHECK(binder.get_toggle_state("view.toggleSidebar")); // Was false → true

        binder.dispatch_click("view.toggleSidebar", manifest);
        CHECK(!binder.get_toggle_state("view.toggleSidebar")); // Was true → false
    }

    SECTION("Query toggle entries")
    {
        auto entries = binder.toggle_entries();
        CHECK(entries.size() == 1);
        CHECK(entries[0]->state_key == "sidebar.visible");
    }
}

// ============================================================================
// ToolbarCommandBinder — Enablement Sync
// ============================================================================

TEST_CASE("ToolbarCommandBinder — enablement refresh", "[v21][p03][toolbar]")
{
    auto manifest = make_toolbar_manifest();
    ToolbarCommandBinder binder;

    ToolbarButtonBinding b1;
    b1.action_id = "file.save";
    b1.is_bound = true;
    binder.bind(std::move(b1));

    ToolbarButtonBinding b2;
    b2.action_id = "canvas.selectTool";
    b2.is_bound = true;
    binder.bind(std::move(b2));

    // Without canvas mode, canvas button should be hidden
    ContextKeyService no_canvas;
    binder.refresh_enablement(manifest, no_canvas);

    auto* canvas_btn = binder.get_binding("canvas.selectTool");
    REQUIRE(canvas_btn != nullptr);
    CHECK(!canvas_btn->is_visible);

    // With canvas mode
    ContextKeyService with_canvas;
    with_canvas.set_context("canvasMode", true);
    binder.refresh_enablement(manifest, with_canvas);

    canvas_btn = binder.get_binding("canvas.selectTool");
    CHECK(canvas_btn->is_visible);
}

// ============================================================================
// ToolbarCommandBinder — Manifest Sync
// ============================================================================

TEST_CASE("ToolbarCommandBinder — sync from manifest", "[v21][p03][toolbar]")
{
    auto manifest = make_toolbar_manifest();
    ToolbarCommandBinder binder;

    int synced = binder.sync_from_manifest(manifest);
    CHECK(synced > 0);

    auto toolbar_actions = manifest.actions_for_surface(ControlSurface::kToolbar);
    CHECK(binder.binding_count() == toolbar_actions.size());

    auto* save = binder.get_binding("file.save");
    REQUIRE(save != nullptr);
    CHECK(save->label == "Save");
    CHECK(save->icon == "save");
    CHECK(save->shortcut_hint == "⌘S");
}

// ============================================================================
// ToolbarCommandBinder — CTA Wiring
// ============================================================================

TEST_CASE("ToolbarCommandBinder — CTA button wiring", "[v21][p03][cta]")
{
    ToolbarCommandBinder binder;

    CTABinding cta1;
    cta1.action_id = "file.open";
    cta1.label = "Open Folder";
    cta1.surface = "startup";
    cta1.is_primary = true;
    cta1.is_bound = true;
    binder.register_cta(std::move(cta1));

    CTABinding cta2;
    cta2.action_id = "file.new";
    cta2.label = "New File";
    cta2.surface = "startup";
    cta2.is_bound = true;
    binder.register_cta(std::move(cta2));

    CTABinding cta3;
    cta3.action_id = "help.walkthrough";
    cta3.label = "Start Walkthrough";
    cta3.surface = "welcome";
    cta3.is_bound = false;
    binder.register_cta(std::move(cta3));

    SECTION("Query all CTAs")
    {
        auto all = binder.all_ctas();
        CHECK(all.size() == 3);
    }

    SECTION("Filter by surface")
    {
        auto startup = binder.ctas_for_surface("startup");
        CHECK(startup.size() == 2);

        auto welcome = binder.ctas_for_surface("welcome");
        CHECK(welcome.size() == 1);

        auto empty = binder.ctas_for_surface("walkthrough");
        CHECK(empty.empty());
    }

    SECTION("Live vs dead CTA count")
    {
        CHECK(binder.live_cta_count() == 2);
        CHECK(binder.dead_cta_count() == 1);
    }

    SECTION("CTA dispatch")
    {
        auto manifest = make_toolbar_manifest();

        // file.open is in manifest
        ActionEntry open;
        open.action_id = "file.open";
        open.handler = []() -> bool { return true; };
        manifest.register_action(std::move(open));

        CHECK(binder.dispatch_cta("file.open", manifest));
        CHECK(!binder.dispatch_cta("nonexistent.cta", manifest));
    }
}

// ============================================================================
// ToolbarCommandBinder — Diagnostics
// ============================================================================

TEST_CASE("ToolbarCommandBinder — diagnostics", "[v21][p03][toolbar]")
{
    auto manifest = make_toolbar_manifest();
    ToolbarCommandBinder binder;

    binder.sync_from_manifest(manifest);

    // Add an orphaned binding (not in manifest)
    ToolbarButtonBinding orphan;
    orphan.action_id = "orphan.btn";
    orphan.is_bound = true;
    binder.bind(std::move(orphan));

    auto diagnostics = binder.diagnose(manifest);
    // Should detect: toolbar.deadButton (no handler) + orphan.btn (no manifest)
    CHECK(diagnostics.size() >= 2);

    bool found_dead = false;
    bool found_orphan = false;
    for (const auto& diag : diagnostics)
    {
        if (diag.action_id == "toolbar.deadButton") { found_dead = true; CHECK(diag.is_dead); }
        if (diag.action_id == "orphan.btn") { found_orphan = true; CHECK(diag.is_wrong_target); }
    }
    CHECK(found_dead);
    CHECK(found_orphan);
}

TEST_CASE("ToolbarCommandBinder — live vs dead count", "[v21][p03][toolbar]")
{
    ToolbarCommandBinder binder;

    ToolbarButtonBinding live;
    live.action_id = "live";
    live.is_bound = true;
    live.is_visible = true;
    live.is_enabled = true;
    binder.bind(std::move(live));

    ToolbarButtonBinding dead;
    dead.action_id = "dead";
    dead.is_bound = false;
    binder.bind(std::move(dead));

    CHECK(binder.live_binding_count() == 1);
    CHECK(binder.dead_binding_count() == 1);
}

// ============================================================================
// StatusBarActionRouter — Registration & Classification
// ============================================================================

TEST_CASE("StatusBarActionRouter — register and classify items", "[v21][p03][statusbar]")
{
    StatusBarActionRouter router;

    StatusBarBinding actionable;
    actionable.item_id = "encoding";
    actionable.action_id = "status.encoding";
    actionable.label = "UTF-8";
    actionable.kind = StatusItemKind::kActionable;
    actionable.is_bound = true;
    router.register_item(std::move(actionable));

    StatusBarBinding informational;
    informational.item_id = "branch";
    informational.label = "main";
    informational.kind = StatusItemKind::kInformational;
    router.register_item(std::move(informational));

    StatusBarBinding toggle;
    toggle.item_id = "wordWrap";
    toggle.action_id = "editor.toggleWordWrap";
    toggle.label = "Word Wrap";
    toggle.kind = StatusItemKind::kToggle;
    toggle.is_bound = true;
    router.register_item(std::move(toggle));

    CHECK(router.item_count() == 3);
    CHECK(router.actionable_items().size() == 2); // actionable + toggle
    CHECK(router.informational_items().size() == 1);

    // Verify clickability
    auto* enc = router.get_item("encoding");
    REQUIRE(enc != nullptr);
    CHECK(enc->is_clickable());

    auto* branch = router.get_item("branch");
    REQUIRE(branch != nullptr);
    CHECK(!branch->is_clickable()); // Informational

    // Remove
    CHECK(router.remove_item("branch"));
    CHECK(router.item_count() == 2);
}

// ============================================================================
// StatusBarActionRouter — Dispatch
// ============================================================================

TEST_CASE("StatusBarActionRouter — dispatch click", "[v21][p03][statusbar]")
{
    auto manifest = make_toolbar_manifest();
    StatusBarActionRouter router;

    StatusBarBinding actionable;
    actionable.item_id = "encoding";
    actionable.action_id = "status.encoding";
    actionable.kind = StatusItemKind::kActionable;
    actionable.is_bound = true;
    router.register_item(std::move(actionable));

    StatusBarBinding info;
    info.item_id = "branch";
    info.kind = StatusItemKind::kInformational;
    router.register_item(std::move(info));

    CHECK(router.dispatch_click("encoding", manifest));
    CHECK(!router.dispatch_click("branch", manifest));    // Informational
    CHECK(!router.dispatch_click("unknown", manifest));   // Not registered
}

// ============================================================================
// StatusBarActionRouter — Manifest Sync
// ============================================================================

TEST_CASE("StatusBarActionRouter — sync from manifest", "[v21][p03][statusbar]")
{
    auto manifest = make_toolbar_manifest();
    StatusBarActionRouter router;

    int synced = router.sync_from_manifest(manifest);
    CHECK(synced > 0);

    auto status_actions = manifest.actions_for_surface(ControlSurface::kStatusBar);
    CHECK(router.item_count() == status_actions.size());

    auto* enc = router.get_item("status.encoding");
    REQUIRE(enc != nullptr);
    CHECK(enc->label == "UTF-8");
    CHECK(enc->kind == StatusItemKind::kActionable);
    CHECK(enc->is_bound);
}

// ============================================================================
// StatusBarActionRouter — Diagnostics
// ============================================================================

TEST_CASE("StatusBarActionRouter — diagnostics", "[v21][p03][statusbar]")
{
    auto manifest = make_toolbar_manifest();
    StatusBarActionRouter router;

    // Add an item that targets a non-existent action
    StatusBarBinding misleading;
    misleading.item_id = "misleading";
    misleading.action_id = "nonexistent.action";
    misleading.kind = StatusItemKind::kActionable;
    misleading.is_bound = true;
    router.register_item(std::move(misleading));

    auto diagnostics = router.diagnose(manifest);
    REQUIRE(diagnostics.size() == 1);
    CHECK(diagnostics[0].is_misleading);
}

TEST_CASE("StatusBarActionRouter — live vs dead counts", "[v21][p03][statusbar]")
{
    StatusBarActionRouter router;

    StatusBarBinding live;
    live.item_id = "live";
    live.action_id = "status.encoding";
    live.kind = StatusItemKind::kActionable;
    live.is_bound = true;
    router.register_item(std::move(live));

    StatusBarBinding dead;
    dead.item_id = "dead";
    dead.action_id = "status.dead";
    dead.kind = StatusItemKind::kActionable;
    dead.is_bound = false;
    router.register_item(std::move(dead));

    StatusBarBinding info;
    info.item_id = "info";
    info.kind = StatusItemKind::kInformational;
    router.register_item(std::move(info));

    CHECK(router.live_action_count() == 1);
    CHECK(router.dead_action_count() == 1);
}

// ============================================================================
// Integration — end-to-end toolbar + status bar lifecycle
// ============================================================================

TEST_CASE("Integration — full toolbar + status bar lifecycle", "[v21][p03][integration]")
{
    // 1. Create manifest
    auto manifest = make_toolbar_manifest();

    // 2. Sync toolbar from manifest
    ToolbarCommandBinder toolbar;
    int tb_synced = toolbar.sync_from_manifest(manifest);
    CHECK(tb_synced > 0);

    // 3. Register toggle for sidebar
    ToggleStateEntry toggle;
    toggle.action_id = "view.toggleSidebar";
    toggle.state_key = "sidebar.visible";
    toggle.current_state = true;
    toolbar.register_toggle(std::move(toggle));

    // 4. Register CTAs
    CTABinding cta;
    cta.action_id = "file.save";
    cta.label = "Save File";
    cta.surface = "startup";
    cta.is_primary = true;
    cta.is_bound = true;
    toolbar.register_cta(std::move(cta));

    // 5. Sync status bar from manifest
    StatusBarActionRouter status_bar;
    int sb_synced = status_bar.sync_from_manifest(manifest);
    CHECK(sb_synced > 0);

    // 6. Refresh enablement with editor context
    ContextKeyService editor_ctx;
    editor_ctx.set_context("editorFocus", true);
    toolbar.refresh_enablement(manifest, editor_ctx);
    status_bar.refresh_enablement(manifest, editor_ctx);

    // 7. Dispatch toolbar button
    CHECK(toolbar.dispatch_click("file.save", manifest));

    // 8. Toggle sidebar
    CHECK(toolbar.get_toggle_state("view.toggleSidebar")); // true
    toolbar.dispatch_click("view.toggleSidebar", manifest);
    CHECK(!toolbar.get_toggle_state("view.toggleSidebar")); // Now false

    // 9. Dispatch status bar click
    CHECK(status_bar.dispatch_click("status.encoding", manifest));

    // 10. Dispatch CTA
    CHECK(toolbar.dispatch_cta("file.save", manifest));
    CHECK(toolbar.live_cta_count() == 1);

    // 11. Run diagnostics
    auto tb_diags = toolbar.diagnose(manifest);
    bool has_dead_toolbar = false;
    for (const auto& d : tb_diags)
    {
        if (d.action_id == "toolbar.deadButton") has_dead_toolbar = true;
    }
    CHECK(has_dead_toolbar);

    auto sb_diags = status_bar.diagnose(manifest);
    CHECK(sb_diags.empty()); // All status bar items have handlers
}
