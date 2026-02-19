// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ShortcutOverlayModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_overlay() -> ShortcutOverlayModel
{
    ShortcutOverlayModel model;
    model.set_entries({
        {"cmd.save", "Save", "Cmd+S", "File", "editor"},
        {"cmd.open", "Open File", "Cmd+O", "File", ""},
        {"cmd.find", "Find", "Cmd+F", "Editor", "editor"},
        {"cmd.replace", "Replace", "Cmd+H", "Editor", "editor"},
        {"cmd.zoom_in", "Zoom In", "Cmd++", "View", ""},
        {"cmd.canvas_select", "Select Tool", "V", "Canvas", "canvas"},
    });
    return model;
}

TEST_CASE("Search matches label", "[shortcut][search]")
{
    auto model = make_test_overlay();
    const auto results = model.search("Save");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].command_id == "cmd.save");
}

TEST_CASE("Search matches shortcut", "[shortcut][search]")
{
    auto model = make_test_overlay();
    const auto results = model.search("Cmd+F");
    REQUIRE(!results.empty());
    REQUIRE(results[0].command_id == "cmd.find");
}

TEST_CASE("Context filter returns mode-relevant entries", "[shortcut][context]")
{
    auto model = make_test_overlay();
    const auto editor_shortcuts = model.for_context("editor");
    // editor context items + global (empty context)
    REQUIRE(editor_shortcuts.size() == 5); // save, open, find, replace, zoom_in
}

TEST_CASE("Context filter excludes other modes", "[shortcut][context]")
{
    auto model = make_test_overlay();
    const auto canvas_shortcuts = model.for_context("canvas");
    // canvas context items + global
    for (const auto& entry : canvas_shortcuts)
    {
        REQUIRE((entry.context == "canvas" || entry.context.empty()));
    }
}

TEST_CASE("Categories returns sorted unique list", "[shortcut][category]")
{
    auto model = make_test_overlay();
    const auto cats = model.categories();
    REQUIRE(cats.size() == 4); // Canvas, Editor, File, View
    REQUIRE(cats[0] == "Canvas");
}

TEST_CASE("By category filters correctly", "[shortcut][category]")
{
    auto model = make_test_overlay();
    const auto file_shortcuts = model.by_category("File");
    REQUIRE(file_shortcuts.size() == 2);
}

TEST_CASE("Tooltip formatting", "[shortcut][tooltip]")
{
    REQUIRE(ShortcutOverlayModel::format_tooltip("Save", "Cmd+S") == "Save (Cmd+S)");
    REQUIRE(ShortcutOverlayModel::format_tooltip("Save", "") == "Save");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
