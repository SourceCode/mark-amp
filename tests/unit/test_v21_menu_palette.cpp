/// @file test_v21_menu_palette.cpp
/// @brief V21 Phase 02 — Tests for Menu / Palette / Shortcut Parity.
///
/// Tests cover:
///   - MenuCommandBinder: binding registration, dispatch, enablement sync,
///     accelerator generation, manifest sync, diagnostics, deep-link routing
///   - PaletteManifestSync: manifest-to-palette sync, context-aware filtering,
///     shortcut consistency, menu-palette parity
///   - Integration: end-to-end menu+palette+manifest lifecycle

#include "core/ControlActionManifest.h"
#include "core/MenuCommandBinder.h"
#include "core/PaletteManifestSync.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// Helper: create a populated manifest for testing
// ============================================================================

static auto make_test_manifest() -> ControlActionManifest
{
    ControlActionManifest manifest;

    ActionEntry save;
    save.action_id = "file.save";
    save.label = "Save";
    save.category = "File";
    save.shortcut_hint = "⌘S";
    save.description = "Save the current file";
    save.surfaces = {ControlSurface::kMenu, ControlSurface::kToolbar,
                     ControlSurface::kCommandPalette};
    save.handler = []() -> bool { return true; };
    save.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(save));

    ActionEntry open;
    open.action_id = "file.open";
    open.label = "Open File...";
    open.category = "File";
    open.shortcut_hint = "⌘O";
    open.description = "Open a file";
    open.surfaces = {ControlSurface::kMenu, ControlSurface::kCommandPalette};
    open.handler = []() -> bool { return true; };
    open.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(open));

    ActionEntry find_cmd;
    find_cmd.action_id = "edit.find";
    find_cmd.label = "Find";
    find_cmd.category = "Edit";
    find_cmd.shortcut_hint = "⌘F";
    find_cmd.description = "Find text";
    find_cmd.surfaces = {ControlSurface::kMenu, ControlSurface::kCommandPalette};
    find_cmd.handler = []() -> bool { return true; };
    find_cmd.enablement = [](const ContextKeyService& ctx) -> bool
    { return ctx.get_bool("editorFocus"); };
    find_cmd.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(find_cmd));

    ActionEntry toggle_sb;
    toggle_sb.action_id = "view.toggleSidebar";
    toggle_sb.label = "Toggle Sidebar";
    toggle_sb.category = "View";
    toggle_sb.shortcut_hint = "⌘B";
    toggle_sb.surfaces = {ControlSurface::kMenu, ControlSurface::kCommandPalette};
    toggle_sb.handler = []() -> bool { return true; };
    toggle_sb.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(toggle_sb));

    ActionEntry prefs;
    prefs.action_id = "app.preferences";
    prefs.label = "Preferences...";
    prefs.category = "App";
    prefs.shortcut_hint = "⌘,";
    prefs.description = "Open application preferences";
    prefs.surfaces = {ControlSurface::kMenu, ControlSurface::kCommandPalette};
    prefs.handler = []() -> bool { return true; };
    prefs.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(prefs));

    // A dead action with no handler
    ActionEntry stub_sync;
    stub_sync.action_id = "git.sync";
    stub_sync.label = "Sync";
    stub_sync.category = "Git";
    stub_sync.surfaces = {ControlSurface::kMenu};
    stub_sync.validation_status = ActionValidationStatus::kStub;
    manifest.register_action(std::move(stub_sync));

    // A palette-only action (no menu)
    ActionEntry palette_only;
    palette_only.action_id = "dev.reloadWindow";
    palette_only.label = "Developer: Reload Window";
    palette_only.category = "Developer";
    palette_only.surfaces = {ControlSurface::kCommandPalette};
    palette_only.handler = []() -> bool { return true; };
    palette_only.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(palette_only));

    return manifest;
}

// ============================================================================
// MenuCommandBinder — Registration & Lookup
// ============================================================================

TEST_CASE("MenuCommandBinder — bind and lookup", "[v21][p02][menu]")
{
    MenuCommandBinder binder;

    MenuItemBinding binding;
    binding.action_id = "file.save";
    binding.label = "Save";
    binding.menu_path = "File/Save";
    binding.menu_item_id = 1001;
    binding.shortcut_display = "⌘S";
    binding.is_bound = true;
    binder.bind(std::move(binding));

    SECTION("Find by action ID")
    {
        auto* found = binder.get_binding("file.save");
        REQUIRE(found != nullptr);
        CHECK(found->label == "Save");
        CHECK(found->menu_path == "File/Save");
        CHECK(found->menu_item_id == 1001);
        CHECK(found->shortcut_display == "⌘S");
    }

    SECTION("Find by menu item ID")
    {
        auto* found = binder.get_binding_by_menu_id(1001);
        REQUIRE(found != nullptr);
        CHECK(found->action_id == "file.save");
    }

    SECTION("Returns nullptr for unknown")
    {
        CHECK(binder.get_binding("nonexistent") == nullptr);
        CHECK(binder.get_binding_by_menu_id(9999) == nullptr);
    }

    SECTION("Binding count")
    {
        CHECK(binder.binding_count() == 1);
    }

    SECTION("Unbind removes binding")
    {
        CHECK(binder.unbind("file.save"));
        CHECK(binder.binding_count() == 0);
        CHECK(binder.get_binding("file.save") == nullptr);
        CHECK(!binder.unbind("file.save")); // Already removed
    }
}

TEST_CASE("MenuCommandBinder — batch bind", "[v21][p02][menu]")
{
    MenuCommandBinder binder;

    std::vector<MenuItemBinding> bindings;

    MenuItemBinding b1;
    b1.action_id = "file.save";
    b1.label = "Save";
    b1.menu_item_id = 1001;
    b1.is_bound = true;
    bindings.push_back(std::move(b1));

    MenuItemBinding b2;
    b2.action_id = "file.open";
    b2.label = "Open";
    b2.menu_item_id = 1002;
    b2.is_bound = true;
    bindings.push_back(std::move(b2));

    binder.bind_all(std::move(bindings));
    CHECK(binder.binding_count() == 2);
}

TEST_CASE("MenuCommandBinder — menu path filtering", "[v21][p02][menu]")
{
    MenuCommandBinder binder;

    MenuItemBinding b1;
    b1.action_id = "file.save";
    b1.menu_path = "File/Save";
    b1.is_bound = true;
    binder.bind(std::move(b1));

    MenuItemBinding b2;
    b2.action_id = "file.open";
    b2.menu_path = "File/Open";
    b2.is_bound = true;
    binder.bind(std::move(b2));

    MenuItemBinding b3;
    b3.action_id = "edit.undo";
    b3.menu_path = "Edit/Undo";
    b3.is_bound = true;
    binder.bind(std::move(b3));

    auto file_items = binder.bindings_for_menu("File");
    CHECK(file_items.size() == 2);

    auto edit_items = binder.bindings_for_menu("Edit");
    CHECK(edit_items.size() == 1);

    auto view_items = binder.bindings_for_menu("View");
    CHECK(view_items.empty());
}

// ============================================================================
// MenuCommandBinder — Dispatch
// ============================================================================

TEST_CASE("MenuCommandBinder — dispatch menu event", "[v21][p02][menu]")
{
    auto manifest = make_test_manifest();
    MenuCommandBinder binder;

    MenuItemBinding binding;
    binding.action_id = "file.save";
    binding.menu_item_id = 1001;
    binding.is_bound = true;
    binder.bind(std::move(binding));

    CHECK(binder.dispatch_menu_event(1001, manifest));
    CHECK(!binder.dispatch_menu_event(9999, manifest)); // Unknown menu ID
}

// ============================================================================
// MenuCommandBinder — Enablement Sync
// ============================================================================

TEST_CASE("MenuCommandBinder — enablement refresh from context", "[v21][p02][menu]")
{
    auto manifest = make_test_manifest();
    MenuCommandBinder binder;

    MenuItemBinding b1;
    b1.action_id = "file.save";
    b1.is_bound = true;
    binder.bind(std::move(b1));

    MenuItemBinding b2;
    b2.action_id = "edit.find";
    b2.is_bound = true;
    binder.bind(std::move(b2));

    // Without editorFocus, edit.find should be disabled
    ContextKeyService no_focus;
    binder.refresh_enablement(manifest, no_focus);

    auto disabled = binder.disabled_bindings();
    REQUIRE(disabled.size() == 1);
    CHECK(disabled[0]->action_id == "edit.find");

    // With editorFocus, all should be enabled
    ContextKeyService with_focus;
    with_focus.set_context("editorFocus", true);
    binder.refresh_enablement(manifest, with_focus);

    CHECK(binder.disabled_bindings().empty());
}

// ============================================================================
// MenuCommandBinder — Accelerator Generation
// ============================================================================

TEST_CASE("MenuCommandBinder — accelerator generation", "[v21][p02][menu]")
{
    MenuCommandBinder binder;

    MenuItemBinding b1;
    b1.action_id = "file.save";
    b1.shortcut_display = "⌘S";
    b1.is_bound = true;
    binder.bind(std::move(b1));

    MenuItemBinding b2;
    b2.action_id = "file.open";
    b2.shortcut_display = "⌘O";
    b2.is_bound = true;
    binder.bind(std::move(b2));

    MenuItemBinding b3;
    b3.action_id = "no.shortcut";
    // No shortcut display
    b3.is_bound = true;
    binder.bind(std::move(b3));

    auto accels = binder.generate_accelerators();
    CHECK(accels.size() == 2); // Only items with shortcuts
    CHECK(accels[0].action_id == "file.save");
    CHECK(accels[0].display == "⌘S");
}

// ============================================================================
// MenuCommandBinder — Manifest Sync
// ============================================================================

TEST_CASE("MenuCommandBinder — sync_from_manifest creates bindings", "[v21][p02][menu]")
{
    auto manifest = make_test_manifest();
    MenuCommandBinder binder;

    int synced = binder.sync_from_manifest(manifest);
    CHECK(synced > 0);

    // All menu-surface actions should have bindings
    auto menu_actions = manifest.actions_for_surface(ControlSurface::kMenu);
    CHECK(binder.binding_count() == menu_actions.size());

    // Verify specific binding
    auto* save_binding = binder.get_binding("file.save");
    REQUIRE(save_binding != nullptr);
    CHECK(save_binding->label == "Save");
    CHECK(save_binding->shortcut_display == "⌘S");
    CHECK(save_binding->is_bound);
}

// ============================================================================
// MenuCommandBinder — Diagnostics
// ============================================================================

TEST_CASE("MenuCommandBinder — diagnose detects issues", "[v21][p02][menu]")
{
    auto manifest = make_test_manifest();
    MenuCommandBinder binder;

    // Sync from manifest including the stub git.sync action
    binder.sync_from_manifest(manifest);

    // Add an orphaned binding not in manifest
    MenuItemBinding orphan;
    orphan.action_id = "orphan.action";
    orphan.menu_path = "Other/Orphan";
    orphan.is_bound = true;
    binder.bind(std::move(orphan));

    auto diagnostics = binder.diagnose(manifest);
    REQUIRE(diagnostics.size() >= 2); // At least stub (git.sync) + orphan

    bool found_stub = false;
    bool found_orphan = false;
    for (const auto& diag : diagnostics)
    {
        if (diag.action_id == "git.sync")
        {
            found_stub = true;
            CHECK(diag.is_dead); // No handler → dead, even though status is kStub
        }
        if (diag.action_id == "orphan.action")
        {
            found_orphan = true;
            CHECK(diag.is_orphaned);
        }
    }
    CHECK(found_stub);
    CHECK(found_orphan);
}

TEST_CASE("MenuCommandBinder — live vs dead count", "[v21][p02][menu]")
{
    MenuCommandBinder binder;

    MenuItemBinding live;
    live.action_id = "live.cmd";
    live.is_bound = true;
    live.is_visible = true;
    binder.bind(std::move(live));

    MenuItemBinding dead;
    dead.action_id = "dead.cmd";
    dead.is_bound = false;
    dead.is_visible = true;
    binder.bind(std::move(dead));

    CHECK(binder.live_binding_count() == 1);
    CHECK(binder.dead_binding_count() == 1);
}

// ============================================================================
// MenuCommandBinder — Deep-Link Routing
// ============================================================================

TEST_CASE("MenuCommandBinder — settings deep-link routing", "[v21][p02][deeplink]")
{
    MenuCommandBinder binder;

    DeepLinkTarget target;
    target.setting_id = "editor.fontSize";
    target.query = "font size";
    target.scope = "user";
    target.category = "Editor";
    binder.register_deep_link("app.preferences", std::move(target));

    SECTION("Resolve deep-link")
    {
        auto* link = binder.get_deep_link("app.preferences");
        REQUIRE(link != nullptr);
        CHECK(link->setting_id == "editor.fontSize");
        CHECK(link->query == "font size");
        CHECK(link->scope == "user");
        CHECK(link->category == "Editor");
    }

    SECTION("Check is_deep_link")
    {
        CHECK(binder.is_deep_link("app.preferences"));
        CHECK(!binder.is_deep_link("file.save"));
    }

    SECTION("Returns nullptr for non-deep-link")
    {
        CHECK(binder.get_deep_link("file.save") == nullptr);
    }
}

// ============================================================================
// PaletteManifestSync — Basic Sync
// ============================================================================

TEST_CASE("PaletteManifestSync — sync from manifest (palette surface only)", "[v21][p02][palette]")
{
    auto manifest = make_test_manifest();
    PaletteManifestSync sync;

    auto entries = sync.sync_from_manifest(manifest, false);

    // Should contain only palette-surface actions
    auto palette_actions = manifest.actions_for_surface(ControlSurface::kCommandPalette);
    CHECK(entries.size() == palette_actions.size());

    // Verify a specific entry
    bool found_save = false;
    for (const auto& entry : entries)
    {
        if (entry.action_id == "file.save")
        {
            found_save = true;
            CHECK(entry.label == "Save");
            CHECK(entry.category == "File");
            CHECK(entry.shortcut == "⌘S");
            CHECK(entry.has_handler);
        }
    }
    CHECK(found_save);
}

TEST_CASE("PaletteManifestSync — sync all actions", "[v21][p02][palette]")
{
    auto manifest = make_test_manifest();
    PaletteManifestSync sync;

    auto entries = sync.sync_from_manifest(manifest, true);
    CHECK(entries.size() == manifest.action_count());
}

// ============================================================================
// PaletteManifestSync — Context-Aware Sync
// ============================================================================

TEST_CASE("PaletteManifestSync — context-aware filtering", "[v21][p02][palette]")
{
    auto manifest = make_test_manifest();
    PaletteManifestSync sync;

    // Add a hidden action
    ActionEntry hidden;
    hidden.action_id = "canvas.draw";
    hidden.label = "Draw";
    hidden.surfaces = {ControlSurface::kCommandPalette};
    hidden.handler = []() -> bool { return true; };
    hidden.visibility = [](const ContextKeyService& ctx) -> bool
    { return ctx.get_bool("canvasFocus"); };
    manifest.register_action(std::move(hidden));

    ContextKeyService editor_context;
    editor_context.set_context("editorFocus", true);

    auto entries = sync.sync_context_aware(manifest, editor_context);

    // canvas.draw should be filtered out (not visible without canvasFocus)
    bool found_draw = false;
    for (const auto& entry : entries)
    {
        if (entry.action_id == "canvas.draw")
        {
            found_draw = true;
        }
    }
    CHECK(!found_draw);

    // edit.find should be present and enabled (editorFocus is set)
    bool found_find = false;
    for (const auto& entry : entries)
    {
        if (entry.action_id == "edit.find")
        {
            found_find = true;
            CHECK(entry.is_enabled);
        }
    }
    CHECK(found_find);
}

// ============================================================================
// PaletteManifestSync — Shortcut Consistency
// ============================================================================

TEST_CASE("PaletteManifestSync — shortcut consistency check", "[v21][p02][palette]")
{
    auto manifest = make_test_manifest();
    PaletteManifestSync sync;

    // Sync and then verify no discrepancies when shortcuts match
    auto entries = sync.sync_from_manifest(manifest, false);
    auto discrepancies = sync.check_shortcut_consistency(manifest, entries);
    CHECK(discrepancies.empty()); // All shortcuts should match

    // Introduce a discrepancy
    entries[0].shortcut = "Ctrl+S"; // Doesn't match ⌘S
    discrepancies = sync.check_shortcut_consistency(manifest, entries);
    CHECK(discrepancies.size() == 1);
    CHECK(discrepancies[0].manifest_shortcut == "⌘S");
    CHECK(discrepancies[0].palette_shortcut == "Ctrl+S");
}

// ============================================================================
// PaletteManifestSync — Menu-Palette Parity
// ============================================================================

TEST_CASE("PaletteManifestSync — menu-palette parity report", "[v21][p02][parity]")
{
    auto manifest = make_test_manifest();
    PaletteManifestSync sync;

    auto report = sync.check_menu_palette_parity(manifest);

    CHECK(report.total_menu > 0);
    CHECK(report.total_palette > 0);

    // git.sync is menu-only (no palette surface)
    bool found_git_sync = false;
    for (const auto& id : report.menu_only)
    {
        if (id == "git.sync")
        {
            found_git_sync = true;
        }
    }
    CHECK(found_git_sync);

    // dev.reloadWindow is palette-only (no menu surface)
    bool found_reload = false;
    for (const auto& id : report.palette_only)
    {
        if (id == "dev.reloadWindow")
        {
            found_reload = true;
        }
    }
    CHECK(found_reload);

    // file.save, file.open, edit.find, view.toggleSidebar, app.preferences are in both
    CHECK(report.both.size() >= 5);

    // Parity percentage should be meaningful
    CHECK(report.parity_pct > 0);
    CHECK(report.parity_pct <= 100);
}

TEST_CASE("PaletteManifestSync — query helpers", "[v21][p02][parity]")
{
    auto manifest = make_test_manifest();
    PaletteManifestSync sync;

    CHECK(sync.palette_eligible_count(manifest) > 0);
    CHECK(sync.menu_eligible_count(manifest) > 0);

    auto menu_no_palette = sync.menu_without_palette(manifest);
    CHECK(!menu_no_palette.empty()); // git.sync has no palette surface

    auto palette_no_menu = sync.palette_without_menu(manifest);
    CHECK(!palette_no_menu.empty()); // dev.reloadWindow has no menu surface
}

// ============================================================================
// Integration — End-to-end menu + palette + manifest lifecycle
// ============================================================================

TEST_CASE("Integration — full menu/palette/shortcut lifecycle", "[v21][p02][integration]")
{
    // 1. Create manifest with menu + palette actions
    auto manifest = make_test_manifest();

    // 2. Sync menu binder from manifest
    MenuCommandBinder binder;
    int menu_synced = binder.sync_from_manifest(manifest);
    CHECK(menu_synced > 0);

    // 3. Sync palette from manifest
    PaletteManifestSync palette_sync;
    auto palette_entries = palette_sync.sync_from_manifest(manifest, false);
    CHECK(!palette_entries.empty());

    // 4. Verify shortcut consistency
    auto discrepancies = palette_sync.check_shortcut_consistency(manifest, palette_entries);
    CHECK(discrepancies.empty()); // All consistent

    // 5. Check menu-palette parity
    auto parity = palette_sync.check_menu_palette_parity(manifest);
    CHECK(parity.parity_pct > 50);

    // 6. Dispatch a menu event
    auto* save_binding = binder.get_binding("file.save");
    REQUIRE(save_binding != nullptr);
    CHECK(binder.dispatch_menu_event(save_binding->menu_item_id, manifest));

    // 7. Refresh enablement with context
    ContextKeyService context;
    context.set_context("editorFocus", true);
    binder.refresh_enablement(manifest, context);

    auto disabled = binder.disabled_bindings();
    CHECK(disabled.empty()); // All enabled with editor focus

    // 8. Register a deep-link for preferences
    DeepLinkTarget dl;
    dl.setting_id = "editor.fontSize";
    dl.category = "Editor";
    binder.register_deep_link("app.preferences", std::move(dl));
    CHECK(binder.is_deep_link("app.preferences"));

    // 9. Run diagnostics
    auto diagnostics = binder.diagnose(manifest);
    // At minimum, git.sync (stub) should be flagged
    bool found_stub = false;
    for (const auto& diag : diagnostics)
    {
        if (diag.action_id == "git.sync")
        {
            found_stub = true;
            CHECK(diag.is_dead); // No handler → dead, even though status is kStub
        }
    }
    CHECK(found_stub);

    // 10. Verify accelerators generated
    auto accels = binder.generate_accelerators();
    CHECK(!accels.empty());
}
