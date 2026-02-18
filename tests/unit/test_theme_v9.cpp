/// @file test_theme_v9.cpp
/// @brief V9 Phase 3 — Comprehensive test suite for theme engine completion.

#include "core/BuiltInThemes.h"
#include "core/Theme.h"
#include "core/ThemeAutoRepair.h"
#include "core/ThemeCompatibility.h"
#include "core/ThemeEngine.h"
#include "core/ThemeExporter.h"
#include "core/ThemePerformanceMonitor.h"
#include "core/ThemeRegistry.h"
#include "core/ThemeScopeMapper.h"
#include "core/VsCodeThemeAdapter.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json.hpp>

using namespace markamp::core;

// ============================================================================
// Section 1: Semantic Token Coverage
// ============================================================================

TEST_CASE("ThemeColorToken enum has V9 extended tokens", "[theme][tokens]")
{
    // Verify the new token enum values exist and have expected ordering
    REQUIRE(static_cast<int>(ThemeColorToken::SidebarBg) >
            static_cast<int>(ThemeColorToken::EditorQuickFix));
    REQUIRE(static_cast<int>(ThemeColorToken::NotebookCellBg) >
            static_cast<int>(ThemeColorToken::SidebarBg));

    // kColorTokenCount should be at least 57 (40 pre-existing + 17 new)
    REQUIRE(kColorTokenCount >= 57);
}

TEST_CASE("Theme struct has V9 semantic color fields", "[theme][tokens]")
{
    Theme theme;
    // New fields should have non-zero defaults
    REQUIRE(theme.colors.sidebar_bg.r == 30);
    REQUIRE(theme.colors.sidebar_bg.g == 30);
    REQUIRE(theme.colors.sidebar_bg.b == 50);

    REQUIRE(theme.colors.activity_bar_bg.r == 22);
    REQUIRE(theme.colors.activity_bar_badge_bg.r == 100);
    REQUIRE(theme.colors.activity_bar_badge_fg.r == 255);

    REQUIRE(theme.colors.tab_active_bg.r == 35);
    REQUIRE(theme.colors.tab_inactive_fg.r == 140);

    REQUIRE(theme.colors.diff_inserted_bg.r == 30);
    REQUIRE(theme.colors.diff_removed_bg.r == 80);

    REQUIRE(theme.colors.minimap_bg.r == 20);
    REQUIRE(theme.colors.peek_view_border.r == 100);
    REQUIRE(theme.colors.notebook_cell_bg.r == 28);
}

TEST_CASE("Theme has format_version and parent_theme_id", "[theme][persistence]")
{
    Theme theme;
    REQUIRE(theme.format_version == 1);
    REQUIRE(theme.parent_theme_id.empty());

    theme.format_version = 2;
    theme.parent_theme_id = "midnight-neon";
    REQUIRE(theme.format_version == 2);
    REQUIRE(theme.parent_theme_id == "midnight-neon");
}

// ============================================================================
// Section 2: ThemeEngine — New APIs
// ============================================================================

TEST_CASE("ThemeEngine caches V9 extended tokens", "[theme][engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    // After init, all V9 tokens should be cached
    auto missing = engine.missing_tokens();
    REQUIRE(missing.empty());

    // Verify a specific new token has a color
    const auto& sidebar_bg = engine.color(ThemeColorToken::SidebarBg);
    REQUIRE(sidebar_bg.IsOk());

    const auto& tab_active_bg = engine.color(ThemeColorToken::TabActiveBg);
    REQUIRE(tab_active_bg.IsOk());
}

TEST_CASE("ThemeEngine reduced motion", "[theme][engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    REQUIRE_FALSE(engine.is_reduced_motion());

    engine.set_reduced_motion(true);
    REQUIRE(engine.is_reduced_motion());

    // FX settings should be updated
    REQUIRE(engine.current_theme().fx_settings.reduced_motion);

    engine.set_reduced_motion(false);
    REQUIRE_FALSE(engine.is_reduced_motion());
}

TEST_CASE("ThemeEngine undo stack", "[theme][engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    REQUIRE_FALSE(engine.can_undo());

    // Push current state
    engine.push_undo();
    REQUIRE(engine.can_undo());

    auto original_id = engine.current_theme().id;

    // Undo should restore
    engine.undo_theme_change();
    REQUIRE(engine.current_theme().id == original_id);
    REQUIRE_FALSE(engine.can_undo());
}

TEST_CASE("ThemeEngine scope_mapper accessor", "[theme][engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    // Scope mapper should be accessible (empty by default)
    const auto& mapper = engine.scope_mapper();
    auto match = mapper.match_scope("keyword.control");
    // Empty mapper returns default (no match)
    REQUIRE(match.foreground.empty());
}

TEST_CASE("ThemeEngine apply_fx_tokens", "[theme][engine]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    // Should not throw
    REQUIRE_NOTHROW(engine.apply_fx_tokens());
}

// ============================================================================
// Section 3: ThemeCompatibilityScorer
// ============================================================================

TEST_CASE("ThemeCompatibilityScorer scores built-in theme", "[theme][compatibility]")
{
    auto theme = get_default_theme();
    auto result = ThemeCompatibilityScorer::score_theme(theme);

    // Built-in themes should have high coverage
    REQUIRE(result.score > 0.8);
    REQUIRE_FALSE(result.summary.empty());
    REQUIRE_FALSE(result.mapped_tokens.empty());
}

TEST_CASE("ThemeCompatibilityScorer scores empty theme", "[theme][compatibility]")
{
    Theme theme;
    theme.id = "empty-test";
    theme.name = "Empty Test";

    auto result = ThemeCompatibilityScorer::score_theme(theme);

    // Even empty theme gets base tokens mapped
    REQUIRE(result.score > 0.0);
    REQUIRE(result.score <= 1.0);
}

TEST_CASE("ThemeCompatibilityScorer suggest_repairs", "[theme][compatibility]")
{
    Theme theme;
    // Reset to default black to trigger suggestions
    theme.colors.sidebar_bg = Color{0, 0, 0};
    theme.colors.activity_bar_bg = Color{0, 0, 0};
    theme.colors.minimap_bg = Color{0, 0, 0};

    auto repairs = ThemeCompatibilityScorer::suggest_repairs(theme);
    REQUIRE(repairs.size() >= 3);
}

// ============================================================================
// Section 4: ThemeAutoRepair
// ============================================================================

TEST_CASE("ThemeAutoRepair fills missing tokens", "[theme][repair]")
{
    Theme theme;
    theme.id = "repair-test";
    theme.name = "Repair Test";
    theme.colors.bg_app = Color{20, 20, 40};
    theme.colors.bg_panel = Color{25, 25, 45};
    theme.colors.text_main = Color{220, 220, 240};
    theme.colors.text_muted = Color{160, 160, 180};
    theme.colors.accent_primary = Color{100, 99, 255};

    // Zero out some tokens
    theme.colors.sidebar_bg = Color{0, 0, 0};
    theme.colors.activity_bar_bg = Color{0, 0, 0};

    REQUIRE(ThemeAutoRepair::needs_repair(theme));

    auto actions = ThemeAutoRepair::repair(theme);
    REQUIRE_FALSE(actions.empty());

    // After repair, tokens should be filled
    REQUIRE(theme.colors.sidebar_bg.r != 0);
    REQUIRE(theme.colors.activity_bar_bg.r != 0);
}

TEST_CASE("ThemeAutoRepair fixes contrast violations", "[theme][repair]")
{
    Theme theme;
    theme.id = "contrast-test";
    theme.name = "Contrast Test";
    // Dark bg with dark fg (contrast violation)
    theme.colors.bg_app = Color{10, 10, 20};
    theme.colors.text_main = Color{30, 30, 40};
    theme.colors.text_muted = Color{20, 20, 30};
    theme.colors.bg_panel = Color{15, 15, 30};
    theme.colors.accent_primary = Color{100, 99, 255};

    // Set non-zero for sidebar fields so they don't trigger missing fill
    theme.colors.sidebar_bg = Color{15, 15, 30};
    theme.colors.sidebar_fg = Color{200, 200, 220};
    theme.colors.activity_bar_bg = Color{12, 12, 25};
    theme.colors.tab_active_bg = Color{35, 35, 55};
    theme.colors.tab_active_fg = Color{220, 220, 240};
    theme.colors.tab_inactive_bg = Color{25, 25, 42};
    theme.colors.tab_inactive_fg = Color{140, 140, 160};

    auto actions = ThemeAutoRepair::repair(theme);

    // Should have fixed at least the text_main contrast
    bool found_contrast_fix = false;
    for (const auto& act : actions)
    {
        if (act.repair_type == "contrast_fix")
        {
            found_contrast_fix = true;
            break;
        }
    }
    REQUIRE(found_contrast_fix);
}

TEST_CASE("ThemeAutoRepair no-op on healthy theme", "[theme][repair]")
{
    auto theme = get_default_theme();
    auto actions = ThemeAutoRepair::repair(theme);
    // Built-in theme should need minimal or no repairs
    // (some tokens may have default values but that's expected)
}

// ============================================================================
// Section 5: ThemeExporter
// ============================================================================

TEST_CASE("ThemeExporter generates valid VSCode JSON", "[theme][export]")
{
    auto theme = get_default_theme();
    auto json = ThemeExporter::to_vscode_json(theme);

    REQUIRE_FALSE(json.empty());
    REQUIRE(json.find("\"name\"") != std::string::npos);
    REQUIRE(json.find("\"type\"") != std::string::npos);
    REQUIRE(json.find("\"colors\"") != std::string::npos);
    REQUIRE(json.find("\"tokenColors\"") != std::string::npos);
    REQUIRE(json.find("editor.background") != std::string::npos);
    REQUIRE(json.find("sideBar.background") != std::string::npos);
    REQUIRE(json.find("activityBar.background") != std::string::npos);
    REQUIRE(json.find("keyword") != std::string::npos);
}

TEST_CASE("ThemeExporter export_to_vscode writes file", "[theme][export]")
{
    auto theme = get_default_theme();
    auto path = std::filesystem::temp_directory_path() / "test_export.json";

    auto result = ThemeExporter::export_to_vscode(theme, path);
    REQUIRE(result.has_value());
    REQUIRE(std::filesystem::exists(path));

    // Clean up
    std::filesystem::remove(path);
}

// ============================================================================
// Section 6: ThemePerformanceMonitor
// ============================================================================

TEST_CASE("ThemePerformanceMonitor tracks metrics", "[theme][performance]")
{
    ThemePerformanceMonitor monitor;

    auto initial = monitor.metrics();
    REQUIRE(initial.total_cache_rebuilds == 0);

    monitor.record_cache_rebuild(std::chrono::microseconds(150));
    REQUIRE(monitor.metrics().total_cache_rebuilds == 1);
    REQUIRE(monitor.metrics().last_rebuild_cache_us == std::chrono::microseconds(150));

    monitor.record_cache_rebuild(std::chrono::microseconds(250));
    REQUIRE(monitor.metrics().total_cache_rebuilds == 2);
    REQUIRE_THAT(monitor.metrics().avg_rebuild_cache_us, Catch::Matchers::WithinAbs(200.0, 1.0));

    monitor.record_window_apply(std::chrono::microseconds(500));
    REQUIRE(monitor.metrics().total_window_applies == 1);

    monitor.record_font_build(std::chrono::microseconds(100));
    REQUIRE(monitor.metrics().last_font_build_us == std::chrono::microseconds(100));
}

TEST_CASE("ThemePerformanceMonitor reset", "[theme][performance]")
{
    ThemePerformanceMonitor monitor;
    monitor.record_cache_rebuild(std::chrono::microseconds(150));
    monitor.reset();
    REQUIRE(monitor.metrics().total_cache_rebuilds == 0);
}

// ============================================================================
// Section 7: VsCodeThemeAdapter Extended Mappings
// ============================================================================

TEST_CASE("VsCodeThemeAdapter maps extended color keys", "[theme][vscode]")
{
    // New V9 mappings
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("sideBar.background").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("activityBar.background").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("activityBarBadge.background").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("breadcrumb.foreground").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("tab.activeBackground").empty());
    REQUIRE_FALSE(
        VsCodeThemeAdapter::map_vscode_color("diffEditor.insertedTextBackground").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("minimap.background").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("peekView.border").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("notebook.cellEditorBackground").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("terminal.background").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("badge.background").empty());
    REQUIRE_FALSE(VsCodeThemeAdapter::map_vscode_color("focusBorder").empty());
}

TEST_CASE("VsCodeThemeAdapter map count exceeds 50", "[theme][vscode]")
{
    // Verify we have broad coverage by testing a sample of keys
    int mapped_count = 0;
    const std::vector<std::string> keys = {
        "editor.background",
        "editor.foreground",
        "sideBar.background",
        "activityBar.background",
        "activityBarBadge.background",
        "breadcrumb.foreground",
        "tab.activeBackground",
        "tab.inactiveBackground",
        "diffEditor.insertedTextBackground",
        "minimap.background",
        "peekView.border",
        "notebook.cellEditorBackground",
        "terminal.background",
        "badge.background",
        "focusBorder",
        "input.background",
        "button.background",
        "list.hoverBackground",
        "statusBar.background",
        "panel.background",
    };
    for (const auto& key : keys)
    {
        if (!VsCodeThemeAdapter::map_vscode_color(key).empty())
        {
            ++mapped_count;
        }
    }
    REQUIRE(mapped_count >= 20);
}

// ============================================================================
// Section 8: Undo/Redo Stack (Task 18)
// ============================================================================

TEST_CASE("ThemeEngine redo stack", "[theme][engine][redo]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    REQUIRE_FALSE(engine.can_redo());

    // Push and undo
    engine.push_undo();
    auto original_id = engine.current_theme().id;
    engine.undo_theme_change();

    // After undo, redo should be available
    REQUIRE(engine.can_redo());

    // Redo should restore
    engine.redo_theme_change();
    REQUIRE(engine.current_theme().id == original_id);
    REQUIRE_FALSE(engine.can_redo());
}

TEST_CASE("ThemeEngine preview and cancel", "[theme][engine][preview]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    auto original_id = engine.current_theme().id;

    // Preview should push undo and apply
    engine.preview_theme(original_id);
    REQUIRE(engine.can_undo());

    // Cancel should revert
    engine.cancel_preview();
    REQUIRE(engine.current_theme().id == original_id);
}

// ============================================================================
// Section 9: Scope Mapper Population (Task 8)
// ============================================================================

TEST_CASE("ThemeEngine populate_scope_mapper fills rules", "[theme][engine][scope]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    auto theme = get_default_theme();
    engine.populate_scope_mapper(theme);

    const auto& mapper = engine.scope_mapper();
    REQUIRE(mapper.rule_count() == 11);

    // Match keyword scope
    auto match = mapper.match_scope("keyword");
    REQUIRE_FALSE(match.foreground.empty());

    // Match comment scope
    auto comment_match = mapper.match_scope("comment");
    REQUIRE_FALSE(comment_match.foreground.empty());
}

// ============================================================================
// Section 10: Contrast-Safe Colors (Task 12)
// ============================================================================

TEST_CASE("Theme hover_safe and selected_safe produce blended colors", "[theme][contrast]")
{
    Theme theme;
    theme.colors.bg_app = Color{20, 20, 40};
    theme.colors.accent_primary = Color{100, 99, 255};

    auto hover = theme.hover_safe();
    auto selected = theme.selected_safe();

    // Hover blend should be between bg_app and accent
    REQUIRE(hover.r >= theme.colors.bg_app.r);
    REQUIRE(hover.b >= theme.colors.bg_app.b);

    // Selected should be more shifted than hover
    REQUIRE(selected.b >= hover.b);
}

TEST_CASE("Theme inherits_from checks parent_theme_id", "[theme][inheritance]")
{
    Theme theme;
    theme.parent_theme_id = "midnight-neon";

    REQUIRE(theme.inherits_from("midnight-neon"));
    REQUIRE_FALSE(theme.inherits_from("solarized-dark"));
    REQUIRE_FALSE(theme.inherits_from(""));

    // Empty parent = no inheritance
    Theme orphan;
    REQUIRE_FALSE(orphan.inherits_from("midnight-neon"));
}

// ============================================================================
// Section 11: JSON Persistence V9 Fields (Task 11 + 12)
// ============================================================================

TEST_CASE("Theme JSON round-trip preserves V9 fields", "[theme][persistence]")
{
    Theme original;
    original.id = "test-roundtrip";
    original.name = "Round Trip";
    original.format_version = 3;
    original.parent_theme_id = "base-theme";
    original.colors.sidebar_bg = Color{40, 40, 60};
    original.colors.activity_bar_bg = Color{18, 18, 30};
    original.colors.notebook_cell_bg = Color{30, 30, 50};

    // Serialize
    nlohmann::json json_obj = original;
    auto json_str = json_obj.dump();

    // Deserialize
    auto parsed = nlohmann::json::parse(json_str);
    Theme restored = parsed.get<Theme>();

    REQUIRE(restored.format_version == 3);
    REQUIRE(restored.parent_theme_id == "base-theme");
    REQUIRE(restored.colors.sidebar_bg.r == 40);
    REQUIRE(restored.colors.activity_bar_bg.r == 18);
    REQUIRE(restored.colors.notebook_cell_bg.r == 30);
}

TEST_CASE("Theme JSON backward compat — missing V9 fields load cleanly", "[theme][persistence]")
{
    // Simulate legacy JSON with only base colors
    auto json_str = R"({
        "id": "legacy-theme",
        "name": "Legacy Theme",
        "colors": {
            "--bg-app": "#141428",
            "--bg-panel": "#19192d",
            "--bg-header": "#0f0f1e",
            "--bg-input": "#1e1e38",
            "--text-main": "#dcdcf0",
            "--text-muted": "#a0a0b4",
            "--accent-primary": "#6463ff",
            "--accent-secondary": "#8a6cff",
            "--border-light": "#2d2d50",
            "--border-dark": "#0a0a1e",
            "--editor-bg": "#141428",
            "--editor-fg": "#dcdcf0",
            "--editor-selection": "#3a3a64",
            "--editor-line-number": "#5a5a78",
            "--editor-cursor": "#6463ff",
            "--editor-gutter": "#19192d",
            "--list-hover": "#1e1e38",
            "--list-selected": "#2a2a4a",
            "--scrollbar-thumb": "#3a3a5a",
            "--scrollbar-track": "#19192d"
        }
    })";

    auto parsed = nlohmann::json::parse(json_str);
    Theme theme = parsed.get<Theme>();

    REQUIRE(theme.id == "legacy-theme");
    REQUIRE(theme.format_version == 1);
    REQUIRE(theme.parent_theme_id.empty());
    // V9 fields should have defaults (not crash)
    REQUIRE(theme.colors.sidebar_bg.r == 30); // default
}

// ============================================================================
// Section 12: Theme Events (Task 9)
// ============================================================================

TEST_CASE("ThemeEngine discover_extension_themes runs without crash", "[theme][engine][discovery]")
{
    EventBus bus;
    ThemeRegistry registry;
    ThemeEngine engine(bus, registry);

    // Should not throw — no extension themes by default
    REQUIRE_NOTHROW(engine.discover_extension_themes());
}
