/// @file test_v21_context_menus.cpp
/// @brief V21 Phase 04 — Tests for Context Menus, Inline Actions & Target-Aware Enablement.
///
/// Tests cover:
///   - ContextMenuActionBinder: registration, target-aware resolution,
///     dispatch, no-op blocking, keyboard accessibility, diagnostics
///   - InlineActionRouter: registration, surface/host filtering, dispatch,
///     enablement sync, diagnostics
///   - Integration: end-to-end context menu + inline action + manifest lifecycle

#include "core/ContextMenuActionBinder.h"
#include "core/ControlActionManifest.h"
#include "core/InlineActionRouter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Helper: create a context-menu-oriented manifest
// ============================================================================

static auto make_context_manifest() -> ControlActionManifest
{
    ControlActionManifest manifest;

    ActionEntry copy;
    copy.action_id = "edit.copy";
    copy.label = "Copy";
    copy.shortcut_hint = "⌘C";
    copy.surfaces = {ControlSurface::kContextMenu};
    copy.handler = []() -> bool { return true; };
    copy.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(copy));

    ActionEntry paste;
    paste.action_id = "edit.paste";
    paste.label = "Paste";
    paste.shortcut_hint = "⌘V";
    paste.surfaces = {ControlSurface::kContextMenu};
    paste.handler = []() -> bool { return true; };
    paste.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(paste));

    ActionEntry rename;
    rename.action_id = "file.rename";
    rename.label = "Rename";
    rename.surfaces = {ControlSurface::kContextMenu};
    rename.handler = []() -> bool { return true; };
    rename.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(rename));

    ActionEntry del;
    del.action_id = "file.delete";
    del.label = "Delete";
    del.surfaces = {ControlSurface::kContextMenu};
    del.handler = []() -> bool { return true; };
    del.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(del));

    ActionEntry tab_close;
    tab_close.action_id = "tab.close";
    tab_close.label = "Close Tab";
    tab_close.surfaces = {ControlSurface::kContextMenu};
    tab_close.handler = []() -> bool { return true; };
    tab_close.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(tab_close));

    // A dead action (no handler)
    ActionEntry stub_action;
    stub_action.action_id = "scm.commit";
    stub_action.label = "Commit";
    stub_action.surfaces = {ControlSurface::kContextMenu};
    stub_action.validation_status = ActionValidationStatus::kStub;
    manifest.register_action(std::move(stub_action));

    // Inline actions
    ActionEntry collapse;
    collapse.action_id = "panel.collapse";
    collapse.label = "Collapse";
    collapse.surfaces = {ControlSurface::kToolbar};
    collapse.handler = []() -> bool { return true; };
    collapse.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(collapse));

    ActionEntry refresh;
    refresh.action_id = "explorer.refresh";
    refresh.label = "Refresh";
    refresh.surfaces = {ControlSurface::kToolbar};
    refresh.handler = []() -> bool { return true; };
    refresh.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(refresh));

    return manifest;
}

// ============================================================================
// TargetType — enum tests
// ============================================================================

TEST_CASE("TargetType — label conversion", "[v21][p04][context]")
{
    CHECK(std::string(target_type_label(TargetType::kFile)) == "File");
    CHECK(std::string(target_type_label(TargetType::kFolder)) == "Folder");
    CHECK(std::string(target_type_label(TargetType::kTab)) == "Tab");
    CHECK(std::string(target_type_label(TargetType::kEmptyArea)) == "EmptyArea");
    CHECK(std::string(target_type_label(TargetType::kEditorSelection)) == "EditorSelection");
}

// ============================================================================
// ContextMenuActionBinder — Registration
// ============================================================================

TEST_CASE("ContextMenuActionBinder — register and query contexts", "[v21][p04][context]")
{
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> file_tree_items;
    ContextMenuBinding copy_bind;
    copy_bind.action_id = "edit.copy";
    copy_bind.label = "Copy";
    copy_bind.is_bound = true;
    file_tree_items.push_back(std::move(copy_bind));

    ContextMenuBinding rename_bind;
    rename_bind.action_id = "file.rename";
    rename_bind.label = "Rename";
    rename_bind.is_bound = true;
    file_tree_items.push_back(std::move(rename_bind));

    binder.register_context("file_tree", std::move(file_tree_items));

    CHECK(binder.has_context("file_tree"));
    CHECK(!binder.has_context("editor"));
    CHECK(binder.context_count() == 1);
    CHECK(binder.bindings_for_context("file_tree").size() == 2);

    auto types = binder.registered_contexts();
    REQUIRE(types.size() == 1);
    CHECK(types[0] == "file_tree");
}

// ============================================================================
// ContextMenuActionBinder — Target-Aware Resolution
// ============================================================================

TEST_CASE("ContextMenuActionBinder — resolve with file target", "[v21][p04][context]")
{
    auto manifest = make_context_manifest();
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> items;
    items.push_back({.action_id = "edit.copy", .label = "Copy", .is_bound = true});
    items.push_back({.action_id = "file.rename", .label = "Rename", .is_bound = true});
    items.push_back({.action_id = "file.delete", .label = "Delete", .is_bound = true, .is_destructive = true});
    items.push_back({.action_id = "folder.newFile", .label = "New File", .is_bound = true});
    binder.register_context("file_tree", std::move(items));

    TargetContext file_target;
    file_target.type = TargetType::kFile;
    file_target.target_id = "/path/to/file.md";
    file_target.surface = "file_tree";

    auto resolved = binder.resolve("file_tree", file_target, manifest);
    REQUIRE(resolved.size() == 4);

    // folder.* items should be hidden when target is a file
    bool folder_item_visible = false;
    for (const auto& item : resolved)
    {
        if (item.action_id == "folder.newFile")
        {
            folder_item_visible = item.is_visible;
        }
    }
    CHECK(!folder_item_visible);
}

TEST_CASE("ContextMenuActionBinder — resolve with read-only target", "[v21][p04][context]")
{
    auto manifest = make_context_manifest();
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> items;
    items.push_back({.action_id = "file.delete", .label = "Delete", .is_bound = true, .is_destructive = true});
    binder.register_context("file_tree", std::move(items));

    TargetContext readonly_target;
    readonly_target.type = TargetType::kFile;
    readonly_target.is_readonly = true;

    auto resolved = binder.resolve("file_tree", readonly_target, manifest);
    REQUIRE(resolved.size() == 1);
    CHECK(!resolved[0].is_enabled); // Destructive on readonly = disabled
}

TEST_CASE("ContextMenuActionBinder — resolve with dirty tab target", "[v21][p04][context]")
{
    auto manifest = make_context_manifest();
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> items;
    items.push_back({.action_id = "tab.close", .label = "Close Tab", .is_bound = true});
    binder.register_context("tab_bar", std::move(items));

    TargetContext dirty_tab;
    dirty_tab.type = TargetType::kTab;
    dirty_tab.is_dirty = true;

    auto resolved = binder.resolve("tab_bar", dirty_tab, manifest);
    REQUIRE(resolved.size() == 1);
    CHECK(resolved[0].is_destructive); // Dirty tab close = destructive
}

// ============================================================================
// ContextMenuActionBinder — Dispatch
// ============================================================================

TEST_CASE("ContextMenuActionBinder — dispatch", "[v21][p04][context]")
{
    auto manifest = make_context_manifest();
    ContextMenuActionBinder binder;

    TargetContext target;
    target.type = TargetType::kFile;

    CHECK(binder.dispatch("edit.copy", target, manifest));
    CHECK(!binder.dispatch("nonexistent", target, manifest));
}

// ============================================================================
// ContextMenuActionBinder — No-Op Blocking
// ============================================================================

TEST_CASE("ContextMenuActionBinder — no-op blocking", "[v21][p04][blocking]")
{
    ContextMenuActionBinder binder;

    binder.block_noop("scm.commit", "Source control not implemented");

    CHECK(binder.is_blocked("scm.commit"));
    CHECK(!binder.is_blocked("edit.copy"));
    CHECK(binder.block_reason("scm.commit") == "Source control not implemented");
    CHECK(binder.block_reason("edit.copy").empty());

    auto blocked = binder.blocked_actions();
    REQUIRE(blocked.size() == 1);
    CHECK(blocked[0] == "scm.commit");

    // Blocked actions should not dispatch
    auto manifest = make_context_manifest();
    TargetContext target;
    CHECK(!binder.dispatch("scm.commit", target, manifest));

    // Unblock
    binder.unblock("scm.commit");
    CHECK(!binder.is_blocked("scm.commit"));
}

TEST_CASE("ContextMenuActionBinder — blocked actions hidden in resolution", "[v21][p04][blocking]")
{
    auto manifest = make_context_manifest();
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> items;
    items.push_back({.action_id = "edit.copy", .label = "Copy", .is_bound = true});
    items.push_back({.action_id = "scm.commit", .label = "Commit", .is_bound = true});
    binder.register_context("editor", std::move(items));

    binder.block_noop("scm.commit", "Not implemented");

    TargetContext target;
    target.type = TargetType::kEditorSelection;

    auto resolved = binder.resolve("editor", target, manifest);
    bool commit_visible = false;
    for (const auto& item : resolved)
    {
        if (item.action_id == "scm.commit")
        {
            commit_visible = item.is_visible;
        }
    }
    CHECK(!commit_visible);
}

// ============================================================================
// ContextMenuActionBinder — Keyboard Accessibility
// ============================================================================

TEST_CASE("ContextMenuActionBinder — keyboard accessibility", "[v21][p04][a11y]")
{
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> items;
    items.push_back({.action_id = "edit.copy", .label = "Copy", .is_bound = true, .is_enabled = true});
    items.push_back({.action_id = "edit.paste", .label = "Paste", .is_bound = true, .is_enabled = true});
    binder.register_context("editor", std::move(items));

    // Mark copy as keyboard-accessible
    binder.set_keyboard_accessible("edit.copy", true);

    CHECK(binder.is_keyboard_accessible("edit.copy"));
    CHECK(!binder.is_keyboard_accessible("edit.paste"));

    // paste should be in the gaps
    auto gaps = binder.keyboard_gaps();
    CHECK(!gaps.empty());
    bool found_paste = false;
    for (const auto& id : gaps)
    {
        if (id == "edit.paste") found_paste = true;
    }
    CHECK(found_paste);
}

// ============================================================================
// ContextMenuActionBinder — Diagnostics
// ============================================================================

TEST_CASE("ContextMenuActionBinder — diagnostics", "[v21][p04][context]")
{
    auto manifest = make_context_manifest();
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> items;
    items.push_back({.action_id = "edit.copy", .label = "Copy", .is_bound = true});
    items.push_back({.action_id = "scm.commit", .label = "Commit", .is_bound = true}); // stub
    items.push_back({.action_id = "orphan.action", .label = "Orphan", .is_bound = true}); // not in manifest
    binder.register_context("editor", std::move(items));

    auto diagnostics = binder.diagnose(manifest);

    bool found_noop = false;
    bool found_missing = false;
    bool found_no_kb = false;
    for (const auto& diag : diagnostics)
    {
        if (diag.action_id == "scm.commit" && diag.is_noop) found_noop = true;
        if (diag.action_id == "orphan.action" && diag.is_missing_target) found_missing = true;
        if (diag.is_no_keyboard) found_no_kb = true;
    }
    CHECK(found_noop);
    CHECK(found_missing);
    CHECK(found_no_kb); // copy has no keyboard_access entry
}

TEST_CASE("ContextMenuActionBinder — live vs dead counts", "[v21][p04][context]")
{
    ContextMenuActionBinder binder;

    std::vector<ContextMenuBinding> items;
    items.push_back({.action_id = "live", .is_bound = true, .is_enabled = true});
    items.push_back({.action_id = "dead", .is_bound = false});
    binder.register_context("test", std::move(items));

    CHECK(binder.total_live_bindings() == 1);
    CHECK(binder.total_dead_bindings() == 1);
}

// ============================================================================
// InlineActionRouter — Registration & Lookup
// ============================================================================

TEST_CASE("InlineActionRouter — register and lookup", "[v21][p04][inline]")
{
    InlineActionRouter router;

    InlineActionBinding binding;
    binding.action_id = "panel.collapse";
    binding.label = "Collapse";
    binding.surface = "panel_header";
    binding.host_id = "explorer";
    binding.is_bound = true;
    router.register_action(std::move(binding));

    CHECK(router.action_count() == 1);

    auto* found = router.get_action("panel.collapse");
    REQUIRE(found != nullptr);
    CHECK(found->surface == "panel_header");
    CHECK(found->host_id == "explorer");
}

TEST_CASE("InlineActionRouter — surface and host filtering", "[v21][p04][inline]")
{
    InlineActionRouter router;

    InlineActionBinding b1;
    b1.action_id = "panel.collapse";
    b1.surface = "panel_header";
    b1.host_id = "explorer";
    b1.is_bound = true;
    router.register_action(std::move(b1));

    InlineActionBinding b2;
    b2.action_id = "explorer.refresh";
    b2.surface = "explorer_toolbar";
    b2.host_id = "explorer";
    b2.is_bound = true;
    router.register_action(std::move(b2));

    InlineActionBinding b3;
    b3.action_id = "ext.settings";
    b3.surface = "extension_card";
    b3.host_id = "ext_python";
    b3.is_bound = true;
    router.register_action(std::move(b3));

    CHECK(router.actions_for_surface("panel_header").size() == 1);
    CHECK(router.actions_for_surface("explorer_toolbar").size() == 1);
    CHECK(router.actions_for_surface("extension_card").size() == 1);
    CHECK(router.actions_for_host("explorer").size() == 2);
    CHECK(router.actions_for_host("ext_python").size() == 1);
}

// ============================================================================
// InlineActionRouter — Dispatch & Enablement
// ============================================================================

TEST_CASE("InlineActionRouter — dispatch", "[v21][p04][inline]")
{
    auto manifest = make_context_manifest();
    InlineActionRouter router;

    InlineActionBinding binding;
    binding.action_id = "panel.collapse";
    binding.is_bound = true;
    router.register_action(std::move(binding));

    CHECK(router.dispatch("panel.collapse", manifest));
    CHECK(!router.dispatch("nonexistent", manifest));
}

TEST_CASE("InlineActionRouter — enablement refresh", "[v21][p04][inline]")
{
    auto manifest = make_context_manifest();
    InlineActionRouter router;

    InlineActionBinding binding;
    binding.action_id = "panel.collapse";
    binding.is_bound = true;
    binding.is_enabled = true;
    router.register_action(std::move(binding));

    ContextKeyService ctx;
    router.refresh_enablement(manifest, ctx);

    auto* found = router.get_action("panel.collapse");
    REQUIRE(found != nullptr);
    CHECK(found->is_bound);
}

// ============================================================================
// InlineActionRouter — Diagnostics
// ============================================================================

TEST_CASE("InlineActionRouter — diagnostics", "[v21][p04][inline]")
{
    auto manifest = make_context_manifest();
    InlineActionRouter router;

    InlineActionBinding live;
    live.action_id = "panel.collapse";
    live.surface = "panel_header";
    live.is_bound = true;
    router.register_action(std::move(live));

    InlineActionBinding orphan;
    orphan.action_id = "orphan.inline";
    orphan.surface = "panel_header";
    orphan.is_bound = true;
    router.register_action(std::move(orphan));

    auto diagnostics = router.diagnose(manifest);
    REQUIRE(diagnostics.size() == 1);
    CHECK(diagnostics[0].action_id == "orphan.inline");
    CHECK(diagnostics[0].is_orphaned);
}

TEST_CASE("InlineActionRouter — live vs dead counts", "[v21][p04][inline]")
{
    InlineActionRouter router;

    InlineActionBinding live;
    live.action_id = "live";
    live.is_bound = true;
    live.is_enabled = true;
    router.register_action(std::move(live));

    InlineActionBinding dead;
    dead.action_id = "dead";
    dead.is_bound = false;
    router.register_action(std::move(dead));

    CHECK(router.live_count() == 1);
    CHECK(router.dead_count() == 1);
}

// ============================================================================
// Integration — full context menu + inline action lifecycle
// ============================================================================

TEST_CASE("Integration — context menu + inline action lifecycle", "[v21][p04][integration]")
{
    auto manifest = make_context_manifest();

    // 1. Register context menus
    ContextMenuActionBinder ctx_binder;
    std::vector<ContextMenuBinding> file_tree_items;
    file_tree_items.push_back({.action_id = "edit.copy", .label = "Copy", .is_bound = true});
    file_tree_items.push_back({.action_id = "file.rename", .label = "Rename", .is_bound = true});
    file_tree_items.push_back({.action_id = "file.delete", .label = "Delete", .is_bound = true, .is_destructive = true});
    ctx_binder.register_context("file_tree", std::move(file_tree_items));

    // 2. Block a no-op action
    ctx_binder.block_noop("scm.commit", "Not implemented");

    // 3. Resolve for a file target
    TargetContext file_target;
    file_target.type = TargetType::kFile;
    file_target.target_id = "/path/to/readme.md";

    auto resolved = ctx_binder.resolve("file_tree", file_target, manifest);
    CHECK(!resolved.empty());

    // 4. Dispatch a context action
    CHECK(ctx_binder.dispatch("edit.copy", file_target, manifest));
    CHECK(!ctx_binder.dispatch("scm.commit", file_target, manifest)); // Blocked

    // 5. Register inline actions
    InlineActionRouter inline_router;
    InlineActionBinding collapse;
    collapse.action_id = "panel.collapse";
    collapse.surface = "panel_header";
    collapse.host_id = "explorer";
    collapse.is_bound = true;
    inline_router.register_action(std::move(collapse));

    // 6. Dispatch inline action
    CHECK(inline_router.dispatch("panel.collapse", manifest));

    // 7. Refresh enablement
    ContextKeyService ctx;
    inline_router.refresh_enablement(manifest, ctx);

    // 8. Run diagnostics
    auto ctx_diags = ctx_binder.diagnose(manifest);
    auto inline_diags = inline_router.diagnose(manifest);

    // Context diagnostics should flag keyboard accessibility gaps
    bool found_kb_gap = false;
    for (const auto& d : ctx_diags)
    {
        if (d.is_no_keyboard) found_kb_gap = true;
    }
    CHECK(found_kb_gap);

    // Inline diagnostics should be clean (all actions exist in manifest)
    CHECK(inline_diags.empty());
}
