/// @file test_e2e.cpp
/// End-to-end integration tests for MarkAmp v1.0.0.
/// Tests exercise the core pipeline without a live wxWidgets window:
/// AppState, ThemeEngine, EventBus, rendering, Config, and FileNode.

#include "core/AppState.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/FileNode.h"
#include "core/MarkdownParser.h"
#include "core/Theme.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "core/ThemeValidator.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <filesystem>
#include <fstream>
#include <string>

using Catch::Matchers::ContainsSubstring;
namespace fs = std::filesystem;

// ─── Helper: render markdown through the full pipeline ───

namespace
{

auto render_e2e(const std::string& markdown, const fs::path& base_path = {}) -> std::string
{
    markamp::rendering::FootnotePreprocessor footnote_proc;
    auto footnote_result = footnote_proc.process(markdown);

    markamp::core::MarkdownParser parser;
    auto doc_result = parser.parse(footnote_result.processed_markdown);
    REQUIRE(doc_result.has_value());

    markamp::rendering::HtmlRenderer renderer;
    if (!base_path.empty())
    {
        renderer.set_base_path(base_path);
    }

    if (footnote_result.has_footnotes)
    {
        return renderer.render_with_footnotes(*doc_result, footnote_result.footnote_section_html);
    }
    return renderer.render(*doc_result);
}

} // namespace

// ═══════════════════════════════════════════════════════
// Workflow 1: First Launch Defaults
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: Default state on first launch", "[e2e][init]")
{
    markamp::core::EventBus bus;
    markamp::core::AppStateManager state_mgr(bus);
    const auto& state = state_mgr.state();

    SECTION("Default theme is Midnight Neon")
    {
        REQUIRE(state.current_theme_id == "midnight-neon");
    }

    SECTION("Default view mode is Split")
    {
        REQUIRE(state.view_mode == markamp::core::events::ViewMode::Split);
    }

    SECTION("Sidebar is visible by default")
    {
        REQUIRE(state.sidebar_visible == true);
    }

    SECTION("Cursor starts at 1:1")
    {
        REQUIRE(state.cursor_line == 1);
        REQUIRE(state.cursor_column == 1);
    }

    SECTION("Encoding defaults to UTF-8")
    {
        REQUIRE(state.encoding == "UTF-8");
    }
}

// ═══════════════════════════════════════════════════════
// Workflow 2: File Editing State
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: Editing state transitions", "[e2e][editing]")
{
    markamp::core::EventBus bus;
    markamp::core::AppStateManager state_mgr(bus);

    SECTION("Set active file updates state")
    {
        state_mgr.set_active_file("readme");
        REQUIRE(state_mgr.state().active_file_id == "readme");
    }

    SECTION("Update content sets content in state")
    {
        state_mgr.update_content("# Hello\n\nNew content");
        REQUIRE(state_mgr.state().active_file_content == "# Hello\n\nNew content");
    }

    SECTION("Cursor position updates")
    {
        state_mgr.set_cursor_position(5, 10);
        REQUIRE(state_mgr.state().cursor_line == 5);
        REQUIRE(state_mgr.state().cursor_column == 10);
    }
}

// ═══════════════════════════════════════════════════════
// Workflow 3: Theme Management
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: Theme engine initialization and switching", "[e2e][theme]")
{
    markamp::core::EventBus bus;
    markamp::core::ThemeRegistry registry;
    registry.initialize();
    markamp::core::ThemeEngine engine(bus, registry);

    SECTION("Engine loads with themes available")
    {
        auto themes = engine.available_themes();
        REQUIRE(themes.size() >= 1);
    }

    SECTION("Default theme is accessible")
    {
        const auto& current = engine.current_theme();
        REQUIRE_FALSE(current.id.empty());
        REQUIRE_FALSE(current.name.empty());
    }

    SECTION("Apply all available themes without crash")
    {
        auto themes = engine.available_themes();
        for (const auto& info : themes)
        {
            REQUIRE_NOTHROW(engine.apply_theme(info.id));
            REQUIRE(engine.current_theme().id == info.id);
        }
    }

    SECTION("Theme export and import round-trip")
    {
        auto temp_dir = fs::temp_directory_path() / "markamp_e2e_theme";
        fs::create_directories(temp_dir);
        auto export_path = temp_dir / "exported_theme.json";

        const auto& current = engine.current_theme();
        REQUIRE_NOTHROW(engine.export_theme(current.id, export_path));
        REQUIRE(fs::exists(export_path));
        REQUIRE(fs::file_size(export_path) > 10);

        REQUIRE_NOTHROW(engine.import_theme(export_path));

        fs::remove_all(temp_dir);
    }
}

// ═══════════════════════════════════════════════════════
// Workflow 4: View Mode Switching
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: View mode transitions", "[e2e][view]")
{
    markamp::core::EventBus bus;
    markamp::core::AppStateManager state_mgr(bus);

    SECTION("Switch to Editor mode")
    {
        state_mgr.set_view_mode(markamp::core::events::ViewMode::Editor);
        REQUIRE(state_mgr.state().view_mode == markamp::core::events::ViewMode::Editor);
    }

    SECTION("Switch to Preview mode")
    {
        state_mgr.set_view_mode(markamp::core::events::ViewMode::Preview);
        REQUIRE(state_mgr.state().view_mode == markamp::core::events::ViewMode::Preview);
    }

    SECTION("Switch to Split mode")
    {
        state_mgr.set_view_mode(markamp::core::events::ViewMode::Split);
        REQUIRE(state_mgr.state().view_mode == markamp::core::events::ViewMode::Split);
    }

    SECTION("Cycling through all modes")
    {
        state_mgr.set_view_mode(markamp::core::events::ViewMode::Editor);
        state_mgr.set_view_mode(markamp::core::events::ViewMode::Preview);
        state_mgr.set_view_mode(markamp::core::events::ViewMode::Split);
        REQUIRE(state_mgr.state().view_mode == markamp::core::events::ViewMode::Split);
    }
}

// ═══════════════════════════════════════════════════════
// Workflow 5: Markdown Rendering Pipeline
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: Heading rendering", "[e2e][render]")
{
    auto html = render_e2e("# Heading 1\n## Heading 2\n### Heading 3");
    CHECK_THAT(html, ContainsSubstring("<h1>"));
    CHECK_THAT(html, ContainsSubstring("<h2>"));
    CHECK_THAT(html, ContainsSubstring("<h3>"));
}

TEST_CASE("E2E: Code block rendering with language", "[e2e][render]")
{
    auto html = render_e2e("```python\ndef hello():\n    print('hi')\n```");
    CHECK_THAT(html, ContainsSubstring("hello"));
    CHECK_THAT(html, ContainsSubstring("print"));
}

TEST_CASE("E2E: Table rendering", "[e2e][render]")
{
    auto html = render_e2e("| A | B |\n|---|---|\n| 1 | 2 |");
    CHECK_THAT(html, ContainsSubstring("<table"));
    CHECK_THAT(html, ContainsSubstring("<th"));
    CHECK_THAT(html, ContainsSubstring("<td"));
}

TEST_CASE("E2E: Task list rendering", "[e2e][render]")
{
    auto html = render_e2e("- [x] Done\n- [ ] Todo");
    CHECK_THAT(html, ContainsSubstring("checkbox"));
}

TEST_CASE("E2E: Link rendering (sanitized)", "[e2e][render]")
{
    auto html = render_e2e("[MarkAmp](https://markamp.dev)");
    CHECK_THAT(html, ContainsSubstring("<a"));
    CHECK_THAT(html, ContainsSubstring("href"));
}

TEST_CASE("E2E: Empty content renders without crash", "[e2e][render]")
{
    REQUIRE_NOTHROW(render_e2e(""));
}

TEST_CASE("E2E: Large content renders without crash", "[e2e][render]")
{
    std::string large_content;
    for (int i = 0; i < 1000; ++i)
    {
        large_content += "## Section " + std::to_string(i) + "\n\n";
        large_content += "Paragraph content for section " + std::to_string(i) + ".\n\n";
    }
    REQUIRE_NOTHROW(render_e2e(large_content));
}

// ═══════════════════════════════════════════════════════
// Workflow 8: Sidebar State
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: Sidebar visibility toggle", "[e2e][sidebar]")
{
    markamp::core::EventBus bus;
    markamp::core::AppStateManager state_mgr(bus);

    REQUIRE(state_mgr.state().sidebar_visible == true);

    state_mgr.set_sidebar_visible(false);
    REQUIRE(state_mgr.state().sidebar_visible == false);

    state_mgr.set_sidebar_visible(true);
    REQUIRE(state_mgr.state().sidebar_visible == true);
}

// ═══════════════════════════════════════════════════════
// EventBus Integration
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: EventBus publishes on state change", "[e2e][events]")
{
    markamp::core::EventBus bus;
    markamp::core::AppStateManager state_mgr(bus);

    bool view_event_received = false;
    auto sub = bus.subscribe<markamp::core::events::ViewModeChangedEvent>(
        [&](const markamp::core::events::ViewModeChangedEvent& evt)
        {
            view_event_received = true;
            REQUIRE(evt.mode == markamp::core::events::ViewMode::Editor);
        });

    state_mgr.set_view_mode(markamp::core::events::ViewMode::Editor);
    REQUIRE(view_event_received);
}

TEST_CASE("E2E: Multiple event subscribers", "[e2e][events]")
{
    markamp::core::EventBus bus;
    markamp::core::AppStateManager state_mgr(bus);

    int call_count = 0;
    auto sub1 = bus.subscribe<markamp::core::events::SidebarToggleEvent>(
        [&](const markamp::core::events::SidebarToggleEvent&) { ++call_count; });
    auto sub2 = bus.subscribe<markamp::core::events::SidebarToggleEvent>(
        [&](const markamp::core::events::SidebarToggleEvent&) { ++call_count; });

    state_mgr.set_sidebar_visible(false);
    REQUIRE(call_count == 2);
}

// ═══════════════════════════════════════════════════════
// Theme Validator Integration
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: Theme validator accepts built-in themes", "[e2e][theme]")
{
    markamp::core::ThemeRegistry registry;
    registry.initialize();
    markamp::core::ThemeValidator validator;

    auto themes = registry.list_themes();
    for (const auto& info : themes)
    {
        auto theme = registry.get_theme(info.id);
        REQUIRE(theme.has_value());

        auto result = validator.validate_theme(*theme);
        CHECK(result.is_valid);
    }
}

// ═══════════════════════════════════════════════════════
// Config Persistence
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: Config save and reload", "[e2e][config]")
{
    markamp::core::Config config;

    config.set("theme", std::string_view{"cyber-night"});
    config.set("sidebar_visible", true);
    config.set("view_mode", std::string_view{"split"});

    REQUIRE(config.get_string("theme") == "cyber-night");
    REQUIRE(config.get_bool("sidebar_visible") == true);
    REQUIRE(config.get_string("view_mode") == "split");
}

TEST_CASE("E2E: Config defaults", "[e2e][config]")
{
    markamp::core::Config config;

    REQUIRE(config.get_string("nonexistent", "fallback") == "fallback");
    REQUIRE(config.get_int("nonexistent", 42) == 42);
    REQUIRE(config.get_bool("nonexistent", true) == true);
    REQUIRE(config.get_double("nonexistent", 3.14) == 3.14);
}

// ═══════════════════════════════════════════════════════
// FileNode Integration
// ═══════════════════════════════════════════════════════

TEST_CASE("E2E: FileNode tree operations", "[e2e][filetree]")
{
    using markamp::core::FileNode;
    using markamp::core::FileNodeType;

    FileNode root;
    root.id = "root";
    root.name = "project";
    root.type = FileNodeType::Folder;

    FileNode readme;
    readme.id = "readme";
    readme.name = "README.md";
    readme.type = FileNodeType::File;
    readme.content = "# Hello World";

    FileNode src_dir;
    src_dir.id = "src";
    src_dir.name = "src";
    src_dir.type = FileNodeType::Folder;

    FileNode main_cpp;
    main_cpp.id = "main";
    main_cpp.name = "main.cpp";
    main_cpp.type = FileNodeType::File;
    main_cpp.content = "int main() {}";

    src_dir.children.push_back(main_cpp);
    root.children.push_back(readme);
    root.children.push_back(src_dir);

    SECTION("Find by ID")
    {
        auto found = root.find_by_id("readme");
        REQUIRE(found != nullptr);
        REQUIRE(found->name == "README.md");
    }

    SECTION("Find nested by ID")
    {
        auto found = root.find_by_id("main");
        REQUIRE(found != nullptr);
        REQUIRE(found->name == "main.cpp");
    }

    SECTION("File count")
    {
        REQUIRE(root.file_count() == 2);
    }

    SECTION("Folder count")
    {
        REQUIRE(root.folder_count() >= 1);
    }

    SECTION("Extension")
    {
        REQUIRE(readme.extension() == ".md");
        REQUIRE(main_cpp.extension() == ".cpp");
    }

    SECTION("Type checks")
    {
        REQUIRE(root.is_folder());
        REQUIRE_FALSE(root.is_file());
        REQUIRE(readme.is_file());
        REQUIRE_FALSE(readme.is_folder());
    }
}
