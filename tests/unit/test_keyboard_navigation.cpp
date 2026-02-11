#include "core/EventBus.h"
#include "core/ShortcutManager.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using markamp::core::EventBus;
using markamp::core::Shortcut;
using markamp::core::ShortcutManager;

// ═══════════════════════════════════════════════════════
// wxWidgets key/modifier constants (same values used in ShortcutManager)
// ═══════════════════════════════════════════════════════

// Modifier flags
constexpr int kModAlt = 0x0001;
constexpr int kModControl = 0x0002;
constexpr int kModShift = 0x0004;
constexpr int kModMeta = 0x0008; // Cmd on macOS

// On macOS, wxMOD_CMD = wxMOD_META. On others, wxMOD_CMD = wxMOD_CONTROL.
#ifdef __APPLE__
constexpr int kModCmd = kModMeta;
#else
constexpr int kModCmd = kModControl;
#endif

// Key codes (wxWidgets values)
constexpr int kKeyEscape = 0x1B; // WXK_ESCAPE = 27
constexpr int kKeyReturn = 0x0D; // WXK_RETURN = 13
constexpr int kKeyF1 = 340;      // WXK_F1

// ═══════════════════════════════════════════════════════
// Registration and retrieval
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: register and retrieve shortcut", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save file", 'S', kModCmd, "global", "File", nullptr});

    auto& all = manager.get_all_shortcuts();
    REQUIRE(all.size() == 1);
    REQUIRE(all[0].id == "file.save");
    REQUIRE(all[0].description == "Save file");
    REQUIRE(all[0].key_code == 'S');
    REQUIRE(all[0].modifiers == kModCmd);
    REQUIRE(all[0].context == "global");
    REQUIRE(all[0].category == "File");
}

TEST_CASE("ShortcutManager: unregister shortcut removes it", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save file", 'S', kModCmd, "global", "File", nullptr});
    manager.register_shortcut({"file.open", "Open file", 'O', kModCmd, "global", "File", nullptr});

    REQUIRE(manager.get_all_shortcuts().size() == 2);

    manager.unregister_shortcut("file.save");
    REQUIRE(manager.get_all_shortcuts().size() == 1);
    REQUIRE(manager.get_all_shortcuts()[0].id == "file.open");
}

TEST_CASE("ShortcutManager: register with same ID overwrites", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save file", 'S', kModCmd, "global", "File", nullptr});
    manager.register_shortcut(
        {"file.save", "Save (updated)", 'S', kModCmd | kModShift, "global", "File", nullptr});

    REQUIRE(manager.get_all_shortcuts().size() == 1);
    REQUIRE(manager.get_all_shortcuts()[0].description == "Save (updated)");
    REQUIRE(manager.get_all_shortcuts()[0].modifiers == (kModCmd | kModShift));
}

// ═══════════════════════════════════════════════════════
// Context filtering
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: get_shortcuts_for_context filters correctly", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save", 'S', kModCmd, "global", "File", nullptr});
    manager.register_shortcut({"edit.bold", "Bold", 'B', kModCmd, "editor", "Markdown", nullptr});
    manager.register_shortcut({"nav.up", "Up", 'K', 0, "sidebar", "Navigation", nullptr});

    auto editor_shortcuts = manager.get_shortcuts_for_context("editor");
    REQUIRE(editor_shortcuts.size() == 1);
    REQUIRE(editor_shortcuts[0].id == "edit.bold");

    auto global_shortcuts = manager.get_shortcuts_for_context("global");
    REQUIRE(global_shortcuts.size() == 1);
    REQUIRE(global_shortcuts[0].id == "file.save");
}

TEST_CASE("ShortcutManager: get_shortcuts_for_category groups correctly", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save", 'S', kModCmd, "global", "File", nullptr});
    manager.register_shortcut({"file.open", "Open", 'O', kModCmd, "global", "File", nullptr});
    manager.register_shortcut({"edit.bold", "Bold", 'B', kModCmd, "editor", "Markdown", nullptr});

    auto file_shortcuts = manager.get_shortcuts_for_category("File");
    REQUIRE(file_shortcuts.size() == 2);

    auto md_shortcuts = manager.get_shortcuts_for_category("Markdown");
    REQUIRE(md_shortcuts.size() == 1);
}

// ═══════════════════════════════════════════════════════
// Key event processing
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: process_key_event matches key+mods+context", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    bool fired = false;
    manager.register_shortcut(
        {"file.save", "Save", 'S', kModCmd, "global", "File", [&fired]() { fired = true; }});

    bool result = manager.process_key_event('S', kModCmd, "global");
    REQUIRE(result);
    REQUIRE(fired);
}

TEST_CASE("ShortcutManager: process_key_event rejects non-matching key", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    bool fired = false;
    manager.register_shortcut(
        {"file.save", "Save", 'S', kModCmd, "global", "File", [&fired]() { fired = true; }});

    bool result = manager.process_key_event('O', kModCmd, "global");
    REQUIRE_FALSE(result);
    REQUIRE_FALSE(fired);
}

TEST_CASE("ShortcutManager: context-specific takes priority over global", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    std::string which_fired;
    manager.register_shortcut(
        {"view.sidebar", "Toggle sidebar", 'B', kModCmd, "global", "View", [&which_fired]() {
             which_fired = "global";
         }});
    manager.register_shortcut(
        {"edit.bold", "Bold", 'B', kModCmd, "editor", "Markdown", [&which_fired]() {
             which_fired = "editor";
         }});

    // In editor context: editor shortcut should fire, not global
    bool result = manager.process_key_event('B', kModCmd, "editor");
    REQUIRE(result);
    REQUIRE(which_fired == "editor");
}

TEST_CASE("ShortcutManager: global shortcut fires from any context without context override",
          "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    bool fired = false;
    manager.register_shortcut(
        {"file.save", "Save", 'S', kModCmd, "global", "File", [&fired]() { fired = true; }});

    // Fire from an unrelated context ("sidebar") — global should still match
    bool result = manager.process_key_event('S', kModCmd, "sidebar");
    REQUIRE(result);
    REQUIRE(fired);
}

TEST_CASE("ShortcutManager: context-specific does NOT fire from wrong context", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    bool fired = false;
    manager.register_shortcut(
        {"edit.bold", "Bold", 'B', kModCmd, "editor", "Markdown", [&fired]() { fired = true; }});

    // From sidebar context: editor shortcut should NOT fire
    bool result = manager.process_key_event('B', kModCmd, "sidebar");
    REQUIRE_FALSE(result);
    REQUIRE_FALSE(fired);
}

// ═══════════════════════════════════════════════════════
// Formatting
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: format_shortcut with Cmd modifier", "[shortcuts]")
{
    auto text = ShortcutManager::format_shortcut('S', kModCmd);
#ifdef __APPLE__
    REQUIRE(text == "\xE2\x8C\x98+S"); // ⌘+S
#else
    REQUIRE(text == "Ctrl+S");
#endif
}

TEST_CASE("ShortcutManager: format_shortcut with Cmd+Shift", "[shortcuts]")
{
    auto text = ShortcutManager::format_shortcut('S', kModCmd | kModShift);
#ifdef __APPLE__
    REQUIRE(text == "\xE2\x8C\x98+Shift+S"); // ⌘+Shift+S
#else
    REQUIRE(text == "Ctrl+Shift+S");
#endif
}

TEST_CASE("ShortcutManager: format_shortcut with Alt modifier", "[shortcuts]")
{
    auto text = ShortcutManager::format_shortcut('Z', kModAlt);
#ifdef __APPLE__
    REQUIRE(text == "\xE2\x8C\xA5+Z"); // ⌥+Z
#else
    REQUIRE(text == "Alt+Z");
#endif
}

TEST_CASE("ShortcutManager: format_key_name for F1", "[shortcuts]")
{
    auto name = ShortcutManager::format_key_name(kKeyF1);
    REQUIRE(name == "F1");
}

TEST_CASE("ShortcutManager: format_key_name for Escape", "[shortcuts]")
{
    auto name = ShortcutManager::format_key_name(kKeyEscape);
    REQUIRE(name == "Escape");
}

TEST_CASE("ShortcutManager: format_key_name for Enter", "[shortcuts]")
{
    auto name = ShortcutManager::format_key_name(kKeyReturn);
    REQUIRE(name == "Enter");
}

TEST_CASE("ShortcutManager: format_key_name for letters", "[shortcuts]")
{
    // Lowercase input gets displayed as uppercase
    auto name = ShortcutManager::format_key_name('a');
    REQUIRE(name == "A");
}

// ═══════════════════════════════════════════════════════
// Shortcut text and find
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: get_shortcut_text returns formatted text", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save", 'S', kModCmd, "global", "File", nullptr});

    auto text = manager.get_shortcut_text("file.save");
#ifdef __APPLE__
    REQUIRE(text == "\xE2\x8C\x98+S");
#else
    REQUIRE(text == "Ctrl+S");
#endif
}

TEST_CASE("ShortcutManager: get_shortcut_text returns empty for unknown id", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    auto text = manager.get_shortcut_text("nonexistent");
    REQUIRE(text.empty());
}

TEST_CASE("ShortcutManager: find_shortcut returns pointer or nullptr", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save", 'S', kModCmd, "global", "File", nullptr});

    auto* found = manager.find_shortcut("file.save");
    REQUIRE(found != nullptr);
    REQUIRE(found->id == "file.save");

    auto* not_found = manager.find_shortcut("nonexistent");
    REQUIRE(not_found == nullptr);
}

// ═══════════════════════════════════════════════════════
// Conflict detection
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: has_conflict detects duplicate bindings", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save", 'S', kModCmd, "global", "File", nullptr});
    manager.register_shortcut({"file.save_as", "Save As", 'S', kModCmd, "global", "File", nullptr});

    REQUIRE(manager.has_conflict('S', kModCmd, "global"));
}

TEST_CASE("ShortcutManager: has_conflict returns false for unique bindings", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    manager.register_shortcut({"file.save", "Save", 'S', kModCmd, "global", "File", nullptr});
    manager.register_shortcut({"file.open", "Open", 'O', kModCmd, "global", "File", nullptr});

    REQUIRE_FALSE(manager.has_conflict('S', kModCmd, "global"));
    REQUIRE_FALSE(manager.has_conflict('O', kModCmd, "global"));
}

// ═══════════════════════════════════════════════════════
// Remapping
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: remap_shortcut changes binding", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    bool fired = false;
    manager.register_shortcut(
        {"file.save", "Save", 'S', kModCmd, "global", "File", [&fired]() { fired = true; }});

    manager.remap_shortcut("file.save", 'W', kModCmd);

    // Old binding should not fire
    REQUIRE_FALSE(manager.process_key_event('S', kModCmd, "global"));
    REQUIRE_FALSE(fired);

    // New binding should fire
    REQUIRE(manager.process_key_event('W', kModCmd, "global"));
    REQUIRE(fired);
}

TEST_CASE("ShortcutManager: reset_to_defaults restores original bindings", "[shortcuts]")
{
    EventBus bus;
    ShortcutManager manager(bus);

    bool fired = false;
    manager.register_shortcut(
        {"file.save", "Save", 'S', kModCmd, "global", "File", [&fired]() { fired = true; }});

    // Remap and verify
    manager.remap_shortcut("file.save", 'W', kModCmd);
    REQUIRE_FALSE(manager.process_key_event('S', kModCmd, "global"));

    // Reset and verify original binding works
    manager.reset_to_defaults();
    REQUIRE(manager.process_key_event('S', kModCmd, "global"));
    REQUIRE(fired);
}

// ═══════════════════════════════════════════════════════
// Platform modifier name
// ═══════════════════════════════════════════════════════

TEST_CASE("ShortcutManager: platform_modifier_name returns correct platform string", "[shortcuts]")
{
    auto name = ShortcutManager::platform_modifier_name();
#ifdef __APPLE__
    REQUIRE(name == "\xE2\x8C\x98"); // ⌘
#else
    REQUIRE(name == "Ctrl");
#endif
}
