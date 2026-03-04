/// @file test_shortcuts_editor.cpp
/// @brief Phase 39 — Unit tests for enhanced ShortcutManager features:
///        conflict detection, context filtering, persistence.

#include "core/EventBus.h"
#include "core/ShortcutManager.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// ShortcutManager — construction and registration
// ============================================================================

TEST_CASE("ShortcutManager: register and find shortcut", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut shortcut;
    shortcut.id = "editor.save";
    shortcut.description = "Save";
    shortcut.key_code = 'S';
    shortcut.modifiers = 0x1000; // wxMOD_CMD
    shortcut.context = "global";
    shortcut.category = "File";
    mgr.register_shortcut(shortcut);

    auto* found = mgr.find_shortcut("editor.save");
    REQUIRE(found != nullptr);
    REQUIRE(found->key_code == 'S');
    REQUIRE(found->description == "Save");
}

TEST_CASE("ShortcutManager: unregister shortcut", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut shortcut;
    shortcut.id = "editor.save";
    shortcut.key_code = 'S';
    mgr.register_shortcut(shortcut);

    mgr.unregister_shortcut("editor.save");
    REQUIRE(mgr.find_shortcut("editor.save") == nullptr);
}

// ============================================================================
// ShortcutManager — remap
// ============================================================================

TEST_CASE("ShortcutManager: remap shortcut", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut shortcut;
    shortcut.id = "editor.save";
    shortcut.key_code = 'S';
    shortcut.modifiers = 0x1000;
    mgr.register_shortcut(shortcut);

    mgr.remap_shortcut("editor.save", 'S', 0x1000 | 0x0001); // CMD+SHIFT+S
    auto* found = mgr.find_shortcut("editor.save");
    REQUIRE(found != nullptr);
    REQUIRE(found->modifiers == (0x1000 | 0x0001));
}

// ============================================================================
// ShortcutManager — conflict detection
// ============================================================================

TEST_CASE("ShortcutManager: detect conflict", "[shortcuts][conflict]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s1;
    s1.id = "editor.save";
    s1.key_code = 'S';
    s1.modifiers = 0x1000;
    s1.context = "global";
    mgr.register_shortcut(s1);

    // has_conflict only returns true when >1 shortcut has the same binding
    REQUIRE_FALSE(mgr.has_conflict('S', 0x1000, "global"));

    // Register a second shortcut with the same binding
    Shortcut s2;
    s2.id = "editor.search";
    s2.key_code = 'S';
    s2.modifiers = 0x1000;
    s2.context = "global";
    mgr.register_shortcut(s2);

    REQUIRE(mgr.has_conflict('S', 0x1000, "global"));
    REQUIRE_FALSE(mgr.has_conflict('X', 0x1000, "global"));
}

TEST_CASE("ShortcutManager: get_conflicts", "[shortcuts][conflict]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s1;
    s1.id = "cmd.a";
    s1.key_code = 'A';
    s1.modifiers = 0x1000;
    s1.context = "global";
    mgr.register_shortcut(s1);

    Shortcut s2;
    s2.id = "cmd.b";
    s2.key_code = 'A';
    s2.modifiers = 0x1000;
    s2.context = "global";
    mgr.register_shortcut(s2);

    auto conflicts = mgr.get_conflicts();
    REQUIRE(conflicts.size() >= 1);
}

// ============================================================================
// ShortcutManager — scope/context filtering
// ============================================================================

TEST_CASE("ShortcutManager: context-based filtering", "[shortcuts][scope]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s1;
    s1.id = "editor.save";
    s1.key_code = 'S';
    s1.context = "editor";
    s1.category = "File";
    mgr.register_shortcut(s1);

    Shortcut s2;
    s2.id = "terminal.clear";
    s2.key_code = 'K';
    s2.context = "terminal";
    s2.category = "Terminal";
    mgr.register_shortcut(s2);

    auto editor_shortcuts = mgr.get_shortcuts_for_context("editor");
    REQUIRE(editor_shortcuts.size() == 1);
    REQUIRE(editor_shortcuts[0].id == "editor.save");

    auto terminal_shortcuts = mgr.get_shortcuts_for_context("terminal");
    REQUIRE(terminal_shortcuts.size() == 1);
    REQUIRE(terminal_shortcuts[0].id == "terminal.clear");
}

TEST_CASE("ShortcutManager: category filtering", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s1;
    s1.id = "file.open";
    s1.category = "File";
    mgr.register_shortcut(s1);

    Shortcut s2;
    s2.id = "edit.copy";
    s2.category = "Edit";
    mgr.register_shortcut(s2);

    auto file_shortcuts = mgr.get_shortcuts_for_category("File");
    REQUIRE(file_shortcuts.size() == 1);
}

// ============================================================================
// ShortcutManager — formatting
// ============================================================================

TEST_CASE("ShortcutManager: format_shortcut", "[shortcuts]")
{
    auto text = ShortcutManager::format_shortcut('S', 0x1000);
    REQUIRE_FALSE(text.empty());
}

TEST_CASE("ShortcutManager: format_key_name", "[shortcuts]")
{
    auto name = ShortcutManager::format_key_name('A');
    REQUIRE_FALSE(name.empty());
}

TEST_CASE("ShortcutManager: platform_modifier_name", "[shortcuts]")
{
    auto mod = ShortcutManager::platform_modifier_name();
    REQUIRE_FALSE(mod.empty());
}

// ============================================================================
// ShortcutManager — persistence
// ============================================================================

TEST_CASE("ShortcutManager: export as JSON", "[shortcuts][persistence]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s1;
    s1.id = "editor.save";
    s1.key_code = 'S';
    s1.modifiers = 0x1000;
    s1.description = "Save File";
    mgr.register_shortcut(s1);

    auto json = mgr.export_as_json();
    REQUIRE_FALSE(json.empty());
}

TEST_CASE("ShortcutManager: export as markdown", "[shortcuts][persistence]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s1;
    s1.id = "editor.save";
    s1.key_code = 'S';
    s1.modifiers = 0x1000;
    s1.category = "File";
    s1.description = "Save";
    mgr.register_shortcut(s1);

    auto md = mgr.export_as_markdown();
    REQUIRE_FALSE(md.empty());
}

// ============================================================================
// ShortcutManager — categories
// ============================================================================

TEST_CASE("ShortcutManager: get_all_categories", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s1{.id = "a", .category = "File"};
    Shortcut s2{.id = "b", .category = "Edit"};
    Shortcut s3{.id = "c", .category = "File"};
    mgr.register_shortcut(s1);
    mgr.register_shortcut(s2);
    mgr.register_shortcut(s3);

    auto categories = mgr.get_all_categories();
    REQUIRE(categories.size() >= 2);
}

// ============================================================================
// ShortcutManager — bulk operations
// ============================================================================

TEST_CASE("ShortcutManager: get_all_shortcuts returns correct count", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    for (int i = 0; i < 10; ++i)
    {
        Shortcut s;
        s.id = "cmd." + std::to_string(i);
        s.key_code = 'A' + i;
        s.modifiers = 0x1000;
        mgr.register_shortcut(s);
    }
    auto& all = mgr.get_all_shortcuts();
    REQUIRE(all.size() >= 10);
}

TEST_CASE("ShortcutManager: shortcut_count", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);
    REQUIRE(mgr.shortcut_count() == 0);

    Shortcut s;
    s.id = "test";
    mgr.register_shortcut(s);
    REQUIRE(mgr.shortcut_count() == 1);
}

// ============================================================================
// ShortcutManager — get_shortcut_text
// ============================================================================

TEST_CASE("ShortcutManager: get_shortcut_text", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager mgr(bus);

    Shortcut s;
    s.id = "test.a";
    s.key_code = 'A';
    s.modifiers = 0x1000;
    mgr.register_shortcut(s);

    auto text = mgr.get_shortcut_text("test.a");
    REQUIRE_FALSE(text.empty());

    auto missing = mgr.get_shortcut_text("nonexistent");
    REQUIRE(missing.empty());
}
