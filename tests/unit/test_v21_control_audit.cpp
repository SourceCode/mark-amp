/// @file test_v21_control_audit.cpp
/// @brief V21 Phase 01 — Comprehensive tests for Control Audit Spine & Canonical Action Model.
///
/// Tests cover:
///   - ControlActionManifest: registration, lookup, surface/category filtering,
///     context-aware active actions, execution dispatch, validation audit queries
///   - ControlExecutionTracer: activation recording, dead-affordance detection,
///     aggregate stats, JSON export
///   - ControlAuditReport: report generation, summary metrics, exit criteria,
///     JSON/Markdown export

#include "core/ControlActionManifest.h"
#include "core/ControlAuditReport.h"
#include "core/ControlExecutionTracer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// ControlSurface enum tests
// ============================================================================

TEST_CASE("ControlSurface — label conversion", "[v21][p01][surface]")
{
    CHECK(std::string(control_surface_label(ControlSurface::kMenu)) == "Menu");
    CHECK(std::string(control_surface_label(ControlSurface::kToolbar)) == "Toolbar");
    CHECK(std::string(control_surface_label(ControlSurface::kStatusBar)) == "StatusBar");
    CHECK(std::string(control_surface_label(ControlSurface::kContextMenu)) == "ContextMenu");
    CHECK(std::string(control_surface_label(ControlSurface::kPanelHeader)) == "PanelHeader");
    CHECK(std::string(control_surface_label(ControlSurface::kInlineAction)) == "InlineAction");
    CHECK(std::string(control_surface_label(ControlSurface::kCommandPalette)) == "CommandPalette");
    CHECK(std::string(control_surface_label(ControlSurface::kSettings)) == "Settings");
    CHECK(std::string(control_surface_label(ControlSurface::kStartup)) == "Startup");
    CHECK(std::string(control_surface_label(ControlSurface::kOther)) == "Other");
}

// ============================================================================
// ActionValidationStatus enum tests
// ============================================================================

TEST_CASE("ActionValidationStatus — label conversion", "[v21][p01][validation]")
{
    CHECK(std::string(validation_status_label(ActionValidationStatus::kLive)) == "Live");
    CHECK(std::string(validation_status_label(ActionValidationStatus::kPartial)) == "Partial");
    CHECK(std::string(validation_status_label(ActionValidationStatus::kStub)) == "Stub");
    CHECK(std::string(validation_status_label(ActionValidationStatus::kDead)) == "Dead");
    CHECK(std::string(validation_status_label(ActionValidationStatus::kDuplicate)) == "Duplicate");
    CHECK(std::string(validation_status_label(ActionValidationStatus::kDeprecated)) == "Deprecated");
    CHECK(std::string(validation_status_label(ActionValidationStatus::kGated)) == "Gated");
}

// ============================================================================
// ActionEntry struct tests
// ============================================================================

TEST_CASE("ActionEntry — handler and wiring checks", "[v21][p01][entry]")
{
    ActionEntry entry;
    entry.action_id = "file.save";
    entry.label = "Save";

    SECTION("No handler means not wired")
    {
        CHECK(!entry.has_handler());
        CHECK(!entry.is_fully_wired());
    }

    SECTION("With handler and live status means fully wired")
    {
        entry.handler = []() -> bool { return true; };
        entry.validation_status = ActionValidationStatus::kLive;
        CHECK(entry.has_handler());
        CHECK(entry.is_fully_wired());
    }

    SECTION("With handler but stub status is not fully wired")
    {
        entry.handler = []() -> bool { return true; };
        entry.validation_status = ActionValidationStatus::kStub;
        CHECK(entry.has_handler());
        CHECK(!entry.is_fully_wired());
    }
}

TEST_CASE("ActionEntry — enablement and visibility predicates", "[v21][p01][entry]")
{
    ContextKeyService context;
    context.set_context("editorFocus", true);

    ActionEntry entry;
    entry.action_id = "edit.find";

    SECTION("No predicates means always enabled and visible")
    {
        CHECK(entry.is_enabled(context));
        CHECK(entry.is_visible(context));
    }

    SECTION("Enablement predicate controls enabled state")
    {
        entry.enablement = [](const ContextKeyService& ctx) -> bool
        { return ctx.get_bool("editorFocus"); };

        CHECK(entry.is_enabled(context));

        ContextKeyService no_focus;
        CHECK(!entry.is_enabled(no_focus));
    }

    SECTION("Visibility predicate controls visible state")
    {
        entry.visibility = [](const ContextKeyService& ctx) -> bool
        { return ctx.get_bool("editorFocus"); };

        CHECK(entry.is_visible(context));
    }
}

TEST_CASE("ActionEntry — surface presence check", "[v21][p01][entry]")
{
    ActionEntry entry;
    entry.action_id = "file.save";
    entry.surfaces = {ControlSurface::kMenu, ControlSurface::kToolbar};

    CHECK(entry.appears_on(ControlSurface::kMenu));
    CHECK(entry.appears_on(ControlSurface::kToolbar));
    CHECK(!entry.appears_on(ControlSurface::kContextMenu));
    CHECK(!entry.appears_on(ControlSurface::kStatusBar));
}

// ============================================================================
// ControlActionManifest tests
// ============================================================================

TEST_CASE("ControlActionManifest — registration and lookup", "[v21][p01][manifest]")
{
    ControlActionManifest manifest;

    SECTION("Register and find by ID")
    {
        ActionEntry entry;
        entry.action_id = "file.save";
        entry.label = "Save";
        entry.category = "File";
        entry.handler = []() -> bool { return true; };
        manifest.register_action(std::move(entry));

        auto* found = manifest.get_action("file.save");
        REQUIRE(found != nullptr);
        CHECK(found->label == "Save");
        CHECK(found->category == "File");
    }

    SECTION("Returns nullptr for unknown ID")
    {
        CHECK(manifest.get_action("nonexistent") == nullptr);
    }

    SECTION("Count tracks registrations")
    {
        CHECK(manifest.action_count() == 0);

        ActionEntry e1;
        e1.action_id = "cmd1";
        e1.label = "Cmd 1";
        manifest.register_action(std::move(e1));

        ActionEntry e2;
        e2.action_id = "cmd2";
        e2.label = "Cmd 2";
        manifest.register_action(std::move(e2));

        CHECK(manifest.action_count() == 2);
    }

    SECTION("Duplicate registration overwrites")
    {
        ActionEntry e1;
        e1.action_id = "dup.cmd";
        e1.label = "Original";
        manifest.register_action(std::move(e1));

        ActionEntry e2;
        e2.action_id = "dup.cmd";
        e2.label = "Updated";
        manifest.register_action(std::move(e2));

        auto* found = manifest.get_action("dup.cmd");
        REQUIRE(found != nullptr);
        CHECK(found->label == "Updated");
        CHECK(manifest.action_count() == 1);
    }

    SECTION("Unregister removes action")
    {
        ActionEntry entry;
        entry.action_id = "test.remove";
        entry.label = "Remove Me";
        manifest.register_action(std::move(entry));

        REQUIRE(manifest.has_action("test.remove"));
        CHECK(manifest.unregister_action("test.remove"));
        CHECK(!manifest.has_action("test.remove"));
        CHECK(!manifest.unregister_action("test.remove")); // Already removed
    }
}

TEST_CASE("ControlActionManifest — batch registration", "[v21][p01][manifest]")
{
    ControlActionManifest manifest;

    std::vector<ActionEntry> entries;

    ActionEntry e1;
    e1.action_id = "cmd.a";
    e1.label = "A";
    entries.push_back(std::move(e1));

    ActionEntry e2;
    e2.action_id = "cmd.b";
    e2.label = "B";
    entries.push_back(std::move(e2));

    manifest.register_actions(std::move(entries));
    CHECK(manifest.action_count() == 2);
    CHECK(manifest.has_action("cmd.a"));
    CHECK(manifest.has_action("cmd.b"));
}

TEST_CASE("ControlActionManifest — surface filtering", "[v21][p01][manifest]")
{
    ControlActionManifest manifest;

    ActionEntry e1;
    e1.action_id = "file.save";
    e1.label = "Save";
    e1.surfaces = {ControlSurface::kMenu, ControlSurface::kToolbar};
    manifest.register_action(std::move(e1));

    ActionEntry e2;
    e2.action_id = "view.toggleSidebar";
    e2.label = "Toggle Sidebar";
    e2.surfaces = {ControlSurface::kMenu, ControlSurface::kCommandPalette};
    manifest.register_action(std::move(e2));

    ActionEntry e3;
    e3.action_id = "status.encoding";
    e3.label = "Encoding";
    e3.surfaces = {ControlSurface::kStatusBar};
    manifest.register_action(std::move(e3));

    auto menu_actions = manifest.actions_for_surface(ControlSurface::kMenu);
    CHECK(menu_actions.size() == 2);

    auto toolbar_actions = manifest.actions_for_surface(ControlSurface::kToolbar);
    CHECK(toolbar_actions.size() == 1);

    auto status_actions = manifest.actions_for_surface(ControlSurface::kStatusBar);
    CHECK(status_actions.size() == 1);

    auto ctx_actions = manifest.actions_for_surface(ControlSurface::kContextMenu);
    CHECK(ctx_actions.empty());
}

TEST_CASE("ControlActionManifest — category filtering", "[v21][p01][manifest]")
{
    ControlActionManifest manifest;

    ActionEntry e1;
    e1.action_id = "file.save";
    e1.category = "File";
    manifest.register_action(std::move(e1));

    ActionEntry e2;
    e2.action_id = "file.open";
    e2.category = "File";
    manifest.register_action(std::move(e2));

    ActionEntry e3;
    e3.action_id = "edit.undo";
    e3.category = "Edit";
    manifest.register_action(std::move(e3));

    auto file_actions = manifest.actions_for_category("File");
    CHECK(file_actions.size() == 2);

    auto edit_actions = manifest.actions_for_category("Edit");
    CHECK(edit_actions.size() == 1);

    auto categories = manifest.get_categories();
    CHECK(categories.size() == 2);
}

TEST_CASE("ControlActionManifest — context-aware active actions", "[v21][p01][manifest]")
{
    ControlActionManifest manifest;
    ContextKeyService context;
    context.set_context("editorFocus", true);

    ActionEntry always;
    always.action_id = "file.save";
    always.label = "Save";
    always.handler = []() -> bool { return true; };
    manifest.register_action(std::move(always));

    ActionEntry editor_only;
    editor_only.action_id = "edit.find";
    editor_only.label = "Find";
    editor_only.enablement = [](const ContextKeyService& ctx) -> bool
    { return ctx.get_bool("editorFocus"); };
    editor_only.handler = []() -> bool { return true; };
    manifest.register_action(std::move(editor_only));

    ActionEntry hidden;
    hidden.action_id = "canvas.draw";
    hidden.label = "Draw";
    hidden.visibility = [](const ContextKeyService& ctx) -> bool
    { return ctx.get_bool("canvasFocus"); };
    hidden.handler = []() -> bool { return true; };
    manifest.register_action(std::move(hidden));

    auto active = manifest.active_actions(context);
    CHECK(active.size() == 2); // file.save + edit.find (canvas.draw hidden)
}

TEST_CASE("ControlActionManifest — execution dispatch", "[v21][p01][manifest]")
{
    ControlActionManifest manifest;

    bool executed = false;
    ActionEntry entry;
    entry.action_id = "test.execute";
    entry.handler = [&executed]() -> bool
    {
        executed = true;
        return true;
    };
    manifest.register_action(std::move(entry));

    CHECK(manifest.execute_action("test.execute"));
    CHECK(executed);

    // Non-existent action
    CHECK(!manifest.execute_action("test.nonexistent"));
}

TEST_CASE("ControlActionManifest — validation summary", "[v21][p01][manifest]")
{
    ControlActionManifest manifest;

    ActionEntry live;
    live.action_id = "live.cmd";
    live.validation_status = ActionValidationStatus::kLive;
    live.handler = []() -> bool { return true; };
    manifest.register_action(std::move(live));

    ActionEntry stub;
    stub.action_id = "stub.cmd";
    stub.validation_status = ActionValidationStatus::kStub;
    manifest.register_action(std::move(stub));

    ActionEntry dead;
    dead.action_id = "dead.cmd";
    dead.validation_status = ActionValidationStatus::kDead;
    manifest.register_action(std::move(dead));

    ActionEntry gated;
    gated.action_id = "gated.cmd";
    gated.validation_status = ActionValidationStatus::kGated;
    manifest.register_action(std::move(gated));

    auto summary = manifest.validation_summary();
    CHECK(summary.live == 1);
    CHECK(summary.stub == 1);
    CHECK(summary.dead == 1);
    CHECK(summary.gated == 1);

    auto unresolved = manifest.unresolved_actions();
    CHECK(unresolved.size() == 3); // stub, dead, gated — no handler

    auto incomplete = manifest.incomplete_actions();
    CHECK(incomplete.size() == 1); // stub only

    auto with_dead = manifest.actions_with_status(ActionValidationStatus::kDead);
    CHECK(with_dead.size() == 1);
}

// ============================================================================
// ControlExecutionTracer tests
// ============================================================================

TEST_CASE("ControlExecutionTracer — record and query activations", "[v21][p01][tracer]")
{
    ControlExecutionTracer tracer;

    SECTION("Record and count")
    {
        tracer.record_activation("file.save", ControlSurface::kMenu, true, 100);
        tracer.record_activation("file.save", ControlSurface::kToolbar, true, 50);
        tracer.record_activation("edit.undo", ControlSurface::kMenu, false, 10);

        CHECK(tracer.activation_count() == 3);
        CHECK(tracer.unique_action_count() == 2);
        CHECK(tracer.failure_count() == 1);
    }

    SECTION("Query by action ID")
    {
        tracer.record_activation("file.save", ControlSurface::kMenu, true);
        tracer.record_activation("file.save", ControlSurface::kToolbar, true);
        tracer.record_activation("edit.undo", ControlSurface::kMenu, true);

        auto saves = tracer.activations_for("file.save");
        CHECK(saves.size() == 2);

        auto undos = tracer.activations_for("edit.undo");
        CHECK(undos.size() == 1);
    }

    SECTION("Query by surface")
    {
        tracer.record_activation("a", ControlSurface::kMenu, true);
        tracer.record_activation("b", ControlSurface::kToolbar, true);
        tracer.record_activation("c", ControlSurface::kMenu, true);

        auto menu = tracer.activations_from_surface(ControlSurface::kMenu);
        CHECK(menu.size() == 2);

        auto toolbar = tracer.activations_from_surface(ControlSurface::kToolbar);
        CHECK(toolbar.size() == 1);
    }
}

TEST_CASE("ControlExecutionTracer — dead affordance detection", "[v21][p01][tracer]")
{
    ControlExecutionTracer tracer;

    SECTION("Never-activated actions detected")
    {
        tracer.mark_rendered("file.save", ControlSurface::kMenu);
        tracer.mark_rendered("file.open", ControlSurface::kMenu);
        tracer.record_activation("file.save", ControlSurface::kMenu, true);

        auto never = tracer.never_activated_actions();
        REQUIRE(never.size() == 1);
        CHECK(never[0] == "file.open");
    }

    SECTION("Always-failing actions detected")
    {
        tracer.record_activation("broken.cmd", ControlSurface::kMenu, false);
        tracer.record_activation("broken.cmd", ControlSurface::kToolbar, false);
        tracer.record_activation("ok.cmd", ControlSurface::kMenu, true);

        auto failing = tracer.always_failing_actions();
        REQUIRE(failing.size() == 1);
        CHECK(failing[0] == "broken.cmd");
    }

    SECTION("Manifest cross-reference detects dead actions")
    {
        ControlActionManifest manifest;

        ActionEntry live;
        live.action_id = "live.cmd";
        live.validation_status = ActionValidationStatus::kLive;
        live.handler = []() -> bool { return true; };
        manifest.register_action(std::move(live));

        ActionEntry dead;
        dead.action_id = "dead.cmd";
        dead.validation_status = ActionValidationStatus::kDead;
        manifest.register_action(std::move(dead));

        ActionEntry no_handler;
        no_handler.action_id = "nohandler.cmd";
        no_handler.validation_status = ActionValidationStatus::kLive;
        // No handler set
        manifest.register_action(std::move(no_handler));

        auto detected = tracer.detect_dead_affordances(manifest);
        CHECK(detected.size() == 2); // dead.cmd + nohandler.cmd
    }
}

TEST_CASE("ControlExecutionTracer — aggregate stats", "[v21][p01][tracer]")
{
    ControlExecutionTracer tracer;

    tracer.record_activation("file.save", ControlSurface::kMenu, true, 100);
    tracer.record_activation("file.save", ControlSurface::kToolbar, true, 200);
    tracer.record_activation("file.save", ControlSurface::kMenu, false, 50);

    auto stats = tracer.stats_for("file.save");
    CHECK(stats.action_id == "file.save");
    CHECK(stats.total_activations == 3);
    CHECK(stats.success_count == 2);
    CHECK(stats.failure_count == 1);
    CHECK(stats.average_duration_us == 116); // (100+200+50)/3

    auto all_stats = tracer.all_stats();
    CHECK(all_stats.size() == 1);
}

TEST_CASE("ControlExecutionTracer — JSON export", "[v21][p01][tracer]")
{
    ControlExecutionTracer tracer;

    tracer.record_activation("file.save", ControlSurface::kMenu, true, 100);

    auto json = tracer.export_json();
    CHECK(json.find("file.save") != std::string::npos);
    CHECK(json.find("Menu") != std::string::npos);
    CHECK(json.find("true") != std::string::npos);
}

TEST_CASE("ControlExecutionTracer — clear", "[v21][p01][tracer]")
{
    ControlExecutionTracer tracer;

    tracer.record_activation("a", ControlSurface::kMenu, true);
    tracer.mark_rendered("b", ControlSurface::kToolbar);
    CHECK(tracer.activation_count() == 1);

    tracer.clear();
    CHECK(tracer.activation_count() == 0);
    CHECK(tracer.never_activated_actions().empty());
}

// ============================================================================
// ControlAuditReport tests
// ============================================================================

TEST_CASE("ControlAuditReport — generate entries from manifest", "[v21][p01][report]")
{
    ControlActionManifest manifest;

    ActionEntry e1;
    e1.action_id = "file.save";
    e1.label = "Save";
    e1.category = "File";
    e1.handler = []() -> bool { return true; };
    e1.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(e1));

    ActionEntry e2;
    e2.action_id = "stub.cmd";
    e2.label = "Stub";
    e2.category = "Test";
    e2.validation_status = ActionValidationStatus::kStub;
    manifest.register_action(std::move(e2));

    ControlAuditReport report;
    auto entries = report.generate(manifest);

    REQUIRE(entries.size() == 2);
    CHECK(entries[0].action_id == "file.save");
    CHECK(entries[0].has_handler);
    CHECK(entries[0].validation_status == ActionValidationStatus::kLive);
    CHECK(entries[1].action_id == "stub.cmd");
    CHECK(!entries[1].has_handler);
    CHECK(entries[1].validation_status == ActionValidationStatus::kStub);
}

TEST_CASE("ControlAuditReport — generate with tracer includes stats", "[v21][p01][report]")
{
    ControlActionManifest manifest;

    ActionEntry entry;
    entry.action_id = "file.save";
    entry.label = "Save";
    entry.handler = []() -> bool { return true; };
    entry.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(entry));

    ControlExecutionTracer tracer;
    tracer.record_activation("file.save", ControlSurface::kMenu, true, 100);
    tracer.record_activation("file.save", ControlSurface::kToolbar, false, 50);

    ControlAuditReport report;
    auto entries = report.generate(manifest, &tracer);

    REQUIRE(entries.size() == 1);
    CHECK(entries[0].activation_count == 2);
    CHECK(entries[0].success_count == 1);
    CHECK(entries[0].failure_count == 1);
}

TEST_CASE("ControlAuditReport — summary metrics", "[v21][p01][report]")
{
    ControlActionManifest manifest;

    ActionEntry live;
    live.action_id = "live";
    live.validation_status = ActionValidationStatus::kLive;
    live.handler = []() -> bool { return true; };
    manifest.register_action(std::move(live));

    ActionEntry stub;
    stub.action_id = "stub";
    stub.validation_status = ActionValidationStatus::kStub;
    manifest.register_action(std::move(stub));

    ActionEntry dead;
    dead.action_id = "dead";
    dead.validation_status = ActionValidationStatus::kDead;
    manifest.register_action(std::move(dead));

    ActionEntry gated;
    gated.action_id = "gated";
    gated.validation_status = ActionValidationStatus::kGated;
    manifest.register_action(std::move(gated));

    ControlAuditReport report;
    auto summary = report.summarize(manifest);

    CHECK(summary.total_actions == 4);
    CHECK(summary.live_actions == 1);
    CHECK(summary.stub_actions == 1);
    CHECK(summary.dead_actions == 1);
    CHECK(summary.gated_actions == 1);
    CHECK(summary.health_pct() == 25); // 1/4 * 100
    CHECK(!summary.passes_exit_criteria()); // Has dead and stub
}

TEST_CASE("ControlAuditReport — exit criteria passes when all live or gated", "[v21][p01][report]")
{
    ControlActionManifest manifest;

    ActionEntry live;
    live.action_id = "live";
    live.validation_status = ActionValidationStatus::kLive;
    live.handler = []() -> bool { return true; };
    manifest.register_action(std::move(live));

    ActionEntry gated;
    gated.action_id = "gated";
    gated.validation_status = ActionValidationStatus::kGated;
    manifest.register_action(std::move(gated));

    ControlAuditReport report;
    auto summary = report.summarize(manifest);

    CHECK(summary.passes_exit_criteria());
}

TEST_CASE("ControlAuditReport — JSON export", "[v21][p01][report]")
{
    ControlActionManifest manifest;

    ActionEntry entry;
    entry.action_id = "test.json";
    entry.label = "Test";
    entry.category = "Test";
    entry.validation_status = ActionValidationStatus::kLive;
    entry.handler = []() -> bool { return true; };
    manifest.register_action(std::move(entry));

    ControlAuditReport report;
    auto entries = report.generate(manifest);
    auto json = report.export_json(entries);

    CHECK(json.find("test.json") != std::string::npos);
    CHECK(json.find("Live") != std::string::npos);
    CHECK(json.find("\"has_handler\": true") != std::string::npos);
}

TEST_CASE("ControlAuditReport — Markdown export", "[v21][p01][report]")
{
    ControlActionManifest manifest;

    ActionEntry entry;
    entry.action_id = "test.md";
    entry.label = "Markdown Test";
    entry.category = "Test";
    entry.validation_status = ActionValidationStatus::kLive;
    entry.handler = []() -> bool { return true; };
    manifest.register_action(std::move(entry));

    ControlAuditReport report;
    auto entries = report.generate(manifest);
    auto summary = report.summarize(entries);
    auto markdown = report.export_markdown(entries, summary);

    CHECK(markdown.find("V21 Control Audit Report") != std::string::npos);
    CHECK(markdown.find("test.md") != std::string::npos);
    CHECK(markdown.find("PASS") != std::string::npos);
    CHECK(markdown.find("100%") != std::string::npos);
}

// ============================================================================
// Integration — end-to-end manifest + tracer + report lifecycle
// ============================================================================

TEST_CASE("Integration — full V21 control audit lifecycle", "[v21][p01][integration]")
{
    // 1. Register actions in manifest
    ControlActionManifest manifest;

    ActionEntry save;
    save.action_id = "file.save";
    save.label = "Save";
    save.category = "File";
    save.surfaces = {ControlSurface::kMenu, ControlSurface::kToolbar};
    save.handler = []() -> bool { return true; };
    save.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(save));

    ActionEntry find_cmd;
    find_cmd.action_id = "edit.find";
    find_cmd.label = "Find";
    find_cmd.category = "Edit";
    find_cmd.surfaces = {ControlSurface::kMenu, ControlSurface::kCommandPalette};
    find_cmd.handler = []() -> bool { return true; };
    find_cmd.enablement = [](const ContextKeyService& ctx) -> bool
    { return ctx.get_bool("editorFocus"); };
    find_cmd.validation_status = ActionValidationStatus::kLive;
    manifest.register_action(std::move(find_cmd));

    ActionEntry dead_item;
    dead_item.action_id = "git.sync";
    dead_item.label = "Sync";
    dead_item.category = "Git";
    dead_item.surfaces = {ControlSurface::kMenu};
    dead_item.validation_status = ActionValidationStatus::kDead;
    manifest.register_action(std::move(dead_item));

    // 2. Trace activations
    ControlExecutionTracer tracer;
    tracer.mark_rendered("file.save", ControlSurface::kMenu);
    tracer.mark_rendered("edit.find", ControlSurface::kMenu);
    tracer.mark_rendered("git.sync", ControlSurface::kMenu);

    tracer.record_activation("file.save", ControlSurface::kMenu, true, 50);
    tracer.record_activation("file.save", ControlSurface::kToolbar, true, 30);
    tracer.record_activation("edit.find", ControlSurface::kMenu, true, 10);

    // 3. Detect dead affordances
    auto dead = tracer.detect_dead_affordances(manifest);
    REQUIRE(dead.size() == 1);
    CHECK(dead[0]->action_id == "git.sync");

    // 4. Generate audit report
    ControlAuditReport report;
    auto entries = report.generate(manifest, &tracer);
    CHECK(entries.size() == 3);

    auto summary = report.summarize(entries);
    CHECK(summary.total_actions == 3);
    CHECK(summary.live_actions == 2);
    CHECK(summary.dead_actions == 1);
    CHECK(!summary.passes_exit_criteria()); // Has dead action

    // 5. Export
    auto json = report.export_json(entries);
    CHECK(!json.empty());
    CHECK(json.find("git.sync") != std::string::npos);

    auto markdown = report.export_markdown(entries, summary);
    CHECK(!markdown.empty());
    CHECK(markdown.find("FAIL") != std::string::npos); // Exit criteria fails
}
