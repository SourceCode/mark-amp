/// @file test_phase12_surface_theme.cpp
/// @brief Quality-bar tests for V8 Phase 12 (Phases 35–45).
/// Covers SurfaceLinkRouter, SurfaceTransitionCoordinator,
/// VsCodeThemeAdapter, ThemeScopeMapper, and related extensions.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/SurfaceLink.h"
#include "core/Theme.h"
#include "core/ThemeScopeMapper.h"
#include "core/VaultService.h"
#include "core/VsCodeThemeAdapter.h"
#include "ui/NavigationService.h"
#include "ui/SurfaceTransitionCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// SurfaceLinkRouter tests (Phase 35)
// ============================================================================

TEST_CASE("SurfaceLinkRouter resolves valid same-surface links", "[phase12][router]")
{
    EventBus bus;
    SurfaceLinkRouter router(bus);

    SurfaceLink link;
    link.from.surface_kind = SurfaceKind::kEditor;
    link.from.file_path = "notes/main.md";
    link.to.surface_kind = SurfaceKind::kEditor;
    link.to.file_path = "notes/other.md";

    auto result = router.resolve(link);
    REQUIRE(result.status == LinkResolveStatus::kResolved);
}

TEST_CASE("SurfaceLinkRouter resolves valid cross-surface links", "[phase12][router]")
{
    EventBus bus;
    SurfaceLinkRouter router(bus);

    SurfaceLink link;
    link.from.surface_kind = SurfaceKind::kEditor;
    link.from.file_path = "notes/main.md";
    link.to.surface_kind = SurfaceKind::kCanvas;
    link.to.board_id = "board-1";
    link.to.object_id = "node-7";

    auto result = router.resolve(link);
    REQUIRE(result.status == LinkResolveStatus::kResolved);
}

TEST_CASE("SurfaceLinkRouter can_route validates surface combinations", "[phase12][router]")
{
    EventBus bus;
    SurfaceLinkRouter router(bus);

    // Editor -> Preview: always routable
    CHECK(router.can_route(SurfaceKind::kEditor, SurfaceKind::kPreview));
    // Editor -> Canvas: always routable
    CHECK(router.can_route(SurfaceKind::kEditor, SurfaceKind::kCanvas));
    // Editor -> Graph: always routable
    CHECK(router.can_route(SurfaceKind::kEditor, SurfaceKind::kGraph));
    // Same surface: always routable
    CHECK(router.can_route(SurfaceKind::kEditor, SurfaceKind::kEditor));
}

TEST_CASE("SurfaceLinkRouter surface_name returns strings", "[phase12][router]")
{
    CHECK(SurfaceLinkRouter::surface_name(SurfaceKind::kEditor) == "Editor");
    CHECK(SurfaceLinkRouter::surface_name(SurfaceKind::kPreview) == "Preview");
    CHECK(SurfaceLinkRouter::surface_name(SurfaceKind::kCanvas) == "Canvas");
    CHECK(SurfaceLinkRouter::surface_name(SurfaceKind::kGraph) == "Graph");
    CHECK(SurfaceLinkRouter::surface_name(SurfaceKind::kNotebook) == "Notebook");
}

TEST_CASE("SurfaceLinkRouter emits events on resolve", "[phase12][router]")
{
    EventBus bus;
    SurfaceLinkRouter router(bus);

    int resolved_count = 0;
    auto sub = bus.subscribe<events::SurfaceLinkResolvedEvent>(
        [&](const events::SurfaceLinkResolvedEvent&) { ++resolved_count; });

    SurfaceLink link;
    link.from.surface_kind = SurfaceKind::kEditor;
    link.to.surface_kind = SurfaceKind::kPreview;
    link.to.file_path = "test.md";

    router.resolve(link);
    CHECK(resolved_count == 1);
}

// ============================================================================
// NavigationService surface link routing tests (Phase 35)
// ============================================================================

TEST_CASE("NavigationService navigate_via_link records cross-surface history",
          "[phase12][navigation]")
{
    EventBus bus;
    Config config;
    VaultService vault(bus, config);
    NavigationService nav(bus, vault);

    SurfaceLink link;
    link.from.surface_kind = SurfaceKind::kEditor;
    link.from.file_path = "source.md";
    link.to.surface_kind = SurfaceKind::kCanvas;
    link.to.file_path = "board.canvas";
    link.to.board_id = "board-1";
    link.to.object_id = "node-5";

    auto result = nav.navigate_via_link(0, link);
    CHECK(result.status == LinkResolveStatus::kResolved);

    // Check history contains cross-surface fields
    auto* entry = nav.history_for(0).current();
    REQUIRE(entry != nullptr);
    CHECK(entry->from_surface == "Editor");
    CHECK(entry->to_surface == "Canvas");
    CHECK(entry->board_id == "board-1");
    CHECK(entry->object_id == "node-5");
}

TEST_CASE("NavigationService global_timeline tracks traversals", "[phase12][navigation]")
{
    EventBus bus;
    Config config;
    VaultService vault(bus, config);
    NavigationService nav(bus, vault);

    // Navigate twice
    SurfaceLink link1;
    link1.from.surface_kind = SurfaceKind::kEditor;
    link1.to.surface_kind = SurfaceKind::kPreview;
    link1.to.file_path = "a.md";

    SurfaceLink link2;
    link2.from.surface_kind = SurfaceKind::kEditor;
    link2.to.surface_kind = SurfaceKind::kCanvas;
    link2.to.file_path = "b.canvas";

    nav.navigate_via_link(0, link1);
    nav.navigate_via_link(0, link2);

    CHECK(nav.global_timeline().size() == 2);
}

// ============================================================================
// SurfaceTransitionCoordinator tests (Phase 36)
// ============================================================================

TEST_CASE("SurfaceTransitionCoordinator begin/commit lifecycle", "[phase12][transition]")
{
    EventBus bus;
    Config config;
    SurfaceTransitionCoordinator coord(bus, config);

    CHECK_FALSE(coord.is_transitioning());

    bool started = coord.begin_transition(SurfaceKind::kEditor, SurfaceKind::kPreview, "test");
    REQUIRE(started);
    CHECK(coord.is_transitioning());
    CHECK(coord.from_surface() == SurfaceKind::kEditor);
    CHECK(coord.to_surface() == SurfaceKind::kPreview);

    coord.commit();
    CHECK_FALSE(coord.is_transitioning());
    CHECK(coord.completed_count() == 1);
    CHECK(coord.cancelled_count() == 0);
}

TEST_CASE("SurfaceTransitionCoordinator cancel lifecycle", "[phase12][transition]")
{
    EventBus bus;
    Config config;
    SurfaceTransitionCoordinator coord(bus, config);

    coord.begin_transition(SurfaceKind::kEditor, SurfaceKind::kCanvas, "test");
    coord.cancel("user cancelled");

    CHECK_FALSE(coord.is_transitioning());
    CHECK(coord.completed_count() == 0);
    CHECK(coord.cancelled_count() == 1);
}

TEST_CASE("SurfaceTransitionCoordinator rapid transitions cancel previous", "[phase12][transition]")
{
    EventBus bus;
    Config config;
    SurfaceTransitionCoordinator coord(bus, config);

    int cancel_events = 0;
    auto sub = bus.subscribe<events::SurfaceTransitionCancelEvent>(
        [&](const events::SurfaceTransitionCancelEvent&) { ++cancel_events; });

    coord.begin_transition(SurfaceKind::kEditor, SurfaceKind::kPreview, "first");
    coord.begin_transition(SurfaceKind::kPreview, SurfaceKind::kCanvas, "second");

    CHECK(cancel_events == 1); // First transition auto-cancelled
    CHECK(coord.is_transitioning());
    CHECK(coord.to_surface() == SurfaceKind::kCanvas);
}

TEST_CASE("SurfaceTransitionCoordinator emits events", "[phase12][transition]")
{
    EventBus bus;
    Config config;
    SurfaceTransitionCoordinator coord(bus, config);

    int start_count = 0;
    int complete_count = 0;
    auto sub1 = bus.subscribe<events::SurfaceTransitionStartEvent>(
        [&](const events::SurfaceTransitionStartEvent& evt)
        {
            ++start_count;
            CHECK(evt.from_surface == SurfaceKind::kEditor);
            CHECK(evt.to_surface == SurfaceKind::kPreview);
        });
    auto sub2 = bus.subscribe<events::SurfaceTransitionCompleteEvent>(
        [&](const events::SurfaceTransitionCompleteEvent&) { ++complete_count; });

    coord.begin_transition(SurfaceKind::kEditor, SurfaceKind::kPreview);
    coord.commit();

    CHECK(start_count == 1);
    CHECK(complete_count == 1);
}

// ============================================================================
// ThemeScopeMapper tests (Phase 42)
// ============================================================================

TEST_CASE("ThemeScopeMapper matches exact scope", "[phase12][scope]")
{
    ThemeScopeMapper mapper;
    mapper.add_rule({.selector = "keyword.control", .foreground = "#C678DD"});

    auto match = mapper.match_scope("keyword.control");
    CHECK(match.specificity == 1000);
    CHECK(match.foreground == "#C678DD");
}

TEST_CASE("ThemeScopeMapper matches prefix scopes", "[phase12][scope]")
{
    ThemeScopeMapper mapper;
    mapper.add_rule({.selector = "keyword", .foreground = "#C678DD"});
    mapper.add_rule({.selector = "keyword.control", .foreground = "#E06C75"});

    auto match = mapper.match_scope("keyword.control.flow");
    // "keyword.control" is more specific (2 segments) than "keyword" (1 segment)
    CHECK(match.foreground == "#E06C75");
    CHECK(match.specificity > 0);
}

TEST_CASE("ThemeScopeMapper semantic overrides take precedence", "[phase12][scope]")
{
    ThemeScopeMapper mapper;
    mapper.add_rule({.selector = "keyword", .foreground = "#C678DD"});
    mapper.add_semantic_override({.selector = "keyword", .foreground = "#FF0000"});

    auto match = mapper.match_scope("keyword");
    CHECK(match.foreground == "#FF0000");
    CHECK(match.priority == 1000);
}

TEST_CASE("ThemeScopeMapper parse_font_style", "[phase12][scope]")
{
    auto flags = ThemeScopeMapper::parse_font_style("italic bold underline");
    CHECK((flags & FontStyleFlag::kItalic) != FontStyleFlag::kNone);
    CHECK((flags & FontStyleFlag::kBold) != FontStyleFlag::kNone);
    CHECK((flags & FontStyleFlag::kUnderline) != FontStyleFlag::kNone);

    auto none = ThemeScopeMapper::parse_font_style("");
    CHECK(none == FontStyleFlag::kNone);
}

TEST_CASE("ThemeScopeMapper rule management", "[phase12][scope]")
{
    ThemeScopeMapper mapper;
    CHECK(mapper.rule_count() == 0);

    mapper.add_rule({.selector = "keyword", .foreground = "#C678DD"});
    mapper.add_rule({.selector = "string", .foreground = "#98C379"});
    CHECK(mapper.rule_count() == 2);

    mapper.add_semantic_override({.selector = "variable", .foreground = "#ABB2BF"});
    CHECK(mapper.semantic_override_count() == 1);

    mapper.clear();
    CHECK(mapper.rule_count() == 0);
    CHECK(mapper.semantic_override_count() == 0);
}

// ============================================================================
// VsCodeThemeAdapter tests (Phase 40)
// ============================================================================

TEST_CASE("VsCodeThemeAdapter maps known VSCode color keys", "[phase12][vscode]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_color("editor.background") == "editor_background");
    CHECK(VsCodeThemeAdapter::map_vscode_color("editor.foreground") == "editor_foreground");
    CHECK(VsCodeThemeAdapter::map_vscode_color("sideBar.background") == "sidebar_bg");
    CHECK(VsCodeThemeAdapter::map_vscode_color("tab.activeBackground") == "tab_active_bg");
}

TEST_CASE("VsCodeThemeAdapter returns empty for unknown keys", "[phase12][vscode]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_color("nonexistent.key").empty());
}

TEST_CASE("VsCodeThemeAdapter compatibility report — no entries", "[phase12][vscode]")
{
    std::vector<VsCodeColorEntry> colors;
    std::vector<VsCodeTokenRule> rules;

    auto report = VsCodeThemeAdapter::compatibility_report(colors, rules);
    CHECK(report.total_tokens == 0);
    CHECK(report.level == ThemeCompatibility::kPartial);
}

TEST_CASE("VsCodeThemeAdapter compatibility report — full coverage", "[phase12][vscode]")
{
    std::vector<VsCodeColorEntry> colors = {
        {"editor.background", "#282C34"},
        {"editor.foreground", "#ABB2BF"},
    };
    std::vector<VsCodeTokenRule> rules;

    auto report = VsCodeThemeAdapter::compatibility_report(colors, rules);
    CHECK(report.total_tokens == 2);
    CHECK(report.supported_tokens == 2);
    CHECK(report.level == ThemeCompatibility::kFull);
}

TEST_CASE("VsCodeThemeAdapter parse_json_string rejects empty", "[phase12][vscode]")
{
    auto result = VsCodeThemeAdapter::parse_json_string("");
    CHECK_FALSE(result.has_value());
    CHECK(result.error().find("Empty") != std::string::npos);
}

TEST_CASE("VsCodeThemeAdapter parse_json_string validates structure", "[phase12][vscode]")
{
    auto result = VsCodeThemeAdapter::parse_json_string("{\"foo\": \"bar\"}");
    CHECK_FALSE(result.has_value()); // No 'colors' or 'tokenColors' key
}

TEST_CASE("VsCodeThemeAdapter convert_to_theme sets metadata", "[phase12][vscode]")
{
    std::vector<VsCodeColorEntry> colors;
    std::vector<VsCodeTokenRule> rules;

    auto theme = VsCodeThemeAdapter::convert_to_theme("One Dark Pro", colors, rules);
    CHECK(theme.name == "One Dark Pro");
    CHECK(theme.author == "VSCode Import");
}

// ============================================================================
// Theme struct extensions tests (Phase 41)
// ============================================================================

TEST_CASE("Theme interactive state tokens have defaults", "[phase12][theme]")
{
    Theme theme;
    CHECK(theme.hover_bg_token == Color(50, 50, 80));
    CHECK(theme.active_bg_token == Color(40, 40, 70));
    CHECK(theme.pressed_bg_token == Color(35, 35, 65));
    CHECK(theme.focus_border_token == Color(100, 99, 255));
    CHECK(theme.error_fg_token == Color(220, 60, 60));
    CHECK(theme.warning_fg_token == Color(220, 180, 50));
    CHECK(theme.info_fg_token == Color(80, 160, 240));
    CHECK(theme.success_fg_token == Color(60, 180, 90));
}

TEST_CASE("Theme metadata fields", "[phase12][theme]")
{
    Theme theme;
    theme.author = "Test Author";
    theme.description = "Test theme";
    theme.source = "vscode-import";

    CHECK(theme.author == "Test Author");
    CHECK(theme.description == "Test theme");
    CHECK(theme.source == "vscode-import");
}

// ============================================================================
// Events tests (Phases 36, 37, 40, 43)
// ============================================================================

TEST_CASE("Phase 12 transition events fire correctly", "[phase12][events]")
{
    EventBus bus;

    int start_count = 0;
    int complete_count = 0;
    int cancel_count = 0;

    auto s1 = bus.subscribe<events::SurfaceTransitionStartEvent>(
        [&](const events::SurfaceTransitionStartEvent&) { ++start_count; });
    auto s2 = bus.subscribe<events::SurfaceTransitionCompleteEvent>(
        [&](const events::SurfaceTransitionCompleteEvent&) { ++complete_count; });
    auto s3 = bus.subscribe<events::SurfaceTransitionCancelEvent>(
        [&](const events::SurfaceTransitionCancelEvent&) { ++cancel_count; });

    events::SurfaceTransitionStartEvent start;
    start.from_surface = SurfaceKind::kEditor;
    start.to_surface = SurfaceKind::kPreview;
    bus.publish(start);

    events::SurfaceTransitionCompleteEvent complete;
    bus.publish(complete);

    events::SurfaceTransitionCancelEvent cancel;
    cancel.reason = "test";
    bus.publish(cancel);

    CHECK(start_count == 1);
    CHECK(complete_count == 1);
    CHECK(cancel_count == 1);
}

TEST_CASE("Phase 12 pair events fire correctly", "[phase12][events]")
{
    EventBus bus;

    int pair_req_count = 0;
    int pair_change_count = 0;

    auto s1 = bus.subscribe<events::PairSurfacesRequestEvent>(
        [&](const events::PairSurfacesRequestEvent&) { ++pair_req_count; });
    auto s2 = bus.subscribe<events::PairSurfacesChangedEvent>(
        [&](const events::PairSurfacesChangedEvent& evt)
        {
            ++pair_change_count;
            CHECK(evt.paired);
        });

    events::PairSurfacesRequestEvent req;
    req.primary = SurfaceKind::kEditor;
    req.secondary = SurfaceKind::kPreview;
    bus.publish(req);

    events::PairSurfacesChangedEvent changed;
    changed.paired = true;
    bus.publish(changed);

    CHECK(pair_req_count == 1);
    CHECK(pair_change_count == 1);
}

TEST_CASE("Phase 12 VSCode theme events fire correctly", "[phase12][events]")
{
    EventBus bus;

    int import_count = 0;
    int report_count = 0;
    int repair_count = 0;

    auto s1 = bus.subscribe<events::ImportVsCodeThemeRequestEvent>(
        [&](const events::ImportVsCodeThemeRequestEvent&) { ++import_count; });
    auto s2 = bus.subscribe<events::ThemeCompatibilityReportEvent>(
        [&](const events::ThemeCompatibilityReportEvent& evt)
        {
            ++report_count;
            CHECK(evt.supported_tokens == 42);
        });
    auto s3 = bus.subscribe<events::ThemeRepairRequestEvent>(
        [&](const events::ThemeRepairRequestEvent&) { ++repair_count; });

    events::ImportVsCodeThemeRequestEvent import_evt;
    import_evt.file_path = "/path/to/theme.json";
    bus.publish(import_evt);

    events::ThemeCompatibilityReportEvent report;
    report.supported_tokens = 42;
    report.total_tokens = 50;
    bus.publish(report);

    events::ThemeRepairRequestEvent repair;
    repair.theme_name = "One Dark Pro";
    bus.publish(repair);

    CHECK(import_count == 1);
    CHECK(report_count == 1);
    CHECK(repair_count == 1);
}
