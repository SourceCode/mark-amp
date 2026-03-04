/**
 * @file test_context_menu_v2.cpp
 * @brief Phase 28: Unit tests for ContextMenuBuilder, ContextMenuRegistry,
 *        and built-in context menu factories.
 */

#include "ui/ContextMenuBuilder.h"
#include "ui/ContextMenuModel.h"
#include "ui/ContextMenuRegistry.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <set>
#include <string>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// ContextMenuBuilder — basics
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - empty builder", "[context_menu][builder]")
{
    ContextMenuBuilder builder;
    CHECK(builder.item_count() == 0);
    CHECK(builder.separator_count() == 0);
    CHECK_FALSE(builder.has_predicates());

    auto model = builder.build();
    CHECK(model.action_count() == 0);
}

TEST_CASE("ContextMenuBuilder - add items", "[context_menu][builder]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut", "Ctrl+X")
                       .item(MenuActionCategory::kEdit, "copy", "Copy", "Ctrl+C")
                       .item(MenuActionCategory::kEdit, "paste", "Paste", "Ctrl+V");

    CHECK(builder.item_count() == 3);
    CHECK(builder.separator_count() == 0);

    auto model = builder.build();
    CHECK(model.action_count() == 3);
}

TEST_CASE("ContextMenuBuilder - items with separators", "[context_menu][builder]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut")
                       .item(MenuActionCategory::kEdit, "copy", "Copy")
                       .separator()
                       .item(MenuActionCategory::kNavigation, "open", "Open");

    CHECK(builder.item_count() == 3);
    CHECK(builder.separator_count() == 1);
}

TEST_CASE("ContextMenuBuilder - clear", "[context_menu][builder]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut")
                       .separator()
                       .when("cut", [](const ContextKeys&) { return true; });

    CHECK(builder.item_count() == 1);
    CHECK(builder.separator_count() == 1);
    CHECK(builder.has_predicates());

    builder.clear();
    CHECK(builder.item_count() == 0);
    CHECK(builder.separator_count() == 0);
    CHECK_FALSE(builder.has_predicates());
}

// ═══════════════════════════════════════════════════════
// ContextMenuBuilder — dynamic enablement
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - enablement predicates", "[context_menu][builder][enablement]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "paste", "Paste")
                       .item(MenuActionCategory::kEdit, "cut", "Cut")
                       .when("paste",
                             [](const ContextKeys& keys)
                             {
                                 auto iter = keys.find("hasClipboard");
                                 return iter != keys.end() && iter->second;
                             })
                       .when("cut",
                             [](const ContextKeys& keys)
                             {
                                 auto iter = keys.find("hasSelection");
                                 return iter != keys.end() && iter->second;
                             });

    CHECK(builder.has_predicates());

    SECTION("All enabled")
    {
        ContextKeys keys = {{"hasClipboard", true}, {"hasSelection", true}};
        auto model = builder.build(keys);
        auto items = model.visible_items();
        CHECK(items.size() == 2);
        for (const auto& menu_item : items)
        {
            CHECK(menu_item.is_enabled);
        }
    }

    SECTION("Paste disabled")
    {
        ContextKeys keys = {{"hasClipboard", false}, {"hasSelection", true}};
        auto model = builder.build(keys);
        auto items = model.sorted_items();
        // Find paste
        bool found_paste = false;
        for (const auto& menu_item : items)
        {
            if (menu_item.action_id == "paste")
            {
                CHECK_FALSE(menu_item.is_enabled);
                found_paste = true;
            }
        }
        CHECK(found_paste);
    }

    SECTION("No context = no predicates applied")
    {
        auto model = builder.build();
        auto items = model.sorted_items();
        // All should be enabled (no predicates applied)
        for (const auto& menu_item : items)
        {
            if (!menu_item.is_separator)
            {
                CHECK(menu_item.is_enabled);
            }
        }
    }
}

// ═══════════════════════════════════════════════════════
// ContextMenuRegistry — basics
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuRegistry - register and get", "[context_menu][registry]")
{
    auto& registry = ContextMenuRegistry::instance();
    registry.clear();

    registry.register_menu("test_context",
                           []()
                           {
                               return ContextMenuBuilder()
                                   .item(MenuActionCategory::kEdit, "test.cut", "Cut")
                                   .item(MenuActionCategory::kEdit, "test.copy", "Copy");
                           });

    CHECK(registry.has("test_context"));
    CHECK_FALSE(registry.has("nonexistent"));
    CHECK(registry.count() == 1);

    auto builder = registry.get("test_context");
    CHECK(builder.item_count() == 2);

    registry.clear();
}

TEST_CASE("ContextMenuRegistry - unregister", "[context_menu][registry]")
{
    auto& registry = ContextMenuRegistry::instance();
    registry.clear();

    registry.register_menu(
        "temp",
        []() { return ContextMenuBuilder().item(MenuActionCategory::kEdit, "temp", "Temp"); });

    CHECK(registry.has("temp"));
    registry.unregister_menu("temp");
    CHECK_FALSE(registry.has("temp"));

    registry.clear();
}

TEST_CASE("ContextMenuRegistry - get nonexistent returns empty", "[context_menu][registry]")
{
    auto& registry = ContextMenuRegistry::instance();
    registry.clear();

    auto builder = registry.get("does_not_exist");
    CHECK(builder.item_count() == 0);

    registry.clear();
}

TEST_CASE("ContextMenuRegistry - registered types", "[context_menu][registry]")
{
    auto& registry = ContextMenuRegistry::instance();
    registry.clear();

    registry.register_menu("beta", []() { return ContextMenuBuilder(); });
    registry.register_menu("alpha", []() { return ContextMenuBuilder(); });
    registry.register_menu("gamma", []() { return ContextMenuBuilder(); });

    auto types = registry.registered_types();
    REQUIRE(types.size() == 3);
    // Should be sorted
    CHECK(types[0] == "alpha");
    CHECK(types[1] == "beta");
    CHECK(types[2] == "gamma");

    registry.clear();
}

// ═══════════════════════════════════════════════════════
// Built-in menu factories
// ═══════════════════════════════════════════════════════

TEST_CASE("Editor context menu", "[context_menu][builtin]")
{
    auto builder = context_menus::editor_menu();
    CHECK(builder.item_count() > 10);
    CHECK(builder.separator_count() >= 3);

    auto model = builder.build();
    auto items = model.sorted_items();
    // Verify cut/copy/paste are present
    bool has_cut = false;
    bool has_copy = false;
    bool has_paste = false;
    for (const auto& menu_item : items)
    {
        if (menu_item.action_id == "editor.cut")
        {
            has_cut = true;
        }
        if (menu_item.action_id == "editor.copy")
        {
            has_copy = true;
        }
        if (menu_item.action_id == "editor.paste")
        {
            has_paste = true;
        }
    }
    CHECK(has_cut);
    CHECK(has_copy);
    CHECK(has_paste);
}

TEST_CASE("File tree file menu", "[context_menu][builtin]")
{
    auto builder = context_menus::file_tree_file_menu();
    CHECK(builder.item_count() > 5);

    auto model = builder.build();
    auto items = model.sorted_items();
    bool has_open = false;
    bool has_rename = false;
    bool has_delete = false;
    for (const auto& menu_item : items)
    {
        if (menu_item.action_id == "file.open")
        {
            has_open = true;
        }
        if (menu_item.action_id == "file.rename")
        {
            has_rename = true;
        }
        if (menu_item.action_id == "file.delete")
        {
            has_delete = true;
        }
    }
    CHECK(has_open);
    CHECK(has_rename);
    CHECK(has_delete);
}

TEST_CASE("Tab bar menu", "[context_menu][builtin]")
{
    auto builder = context_menus::tab_bar_menu();
    CHECK(builder.item_count() > 5);

    auto model = builder.build();
    auto items = model.sorted_items();
    bool has_close = false;
    for (const auto& menu_item : items)
    {
        if (menu_item.action_id == "tab.close")
        {
            has_close = true;
        }
    }
    CHECK(has_close);
}

TEST_CASE("Terminal menu", "[context_menu][builtin]")
{
    auto builder = context_menus::terminal_menu();
    CHECK(builder.item_count() > 3);
}

TEST_CASE("register_all_defaults", "[context_menu][registry][defaults]")
{
    auto& registry = ContextMenuRegistry::instance();
    registry.clear();

    context_menus::register_all_defaults();

    CHECK(registry.count() == 9);
    CHECK(registry.has("editor"));
    CHECK(registry.has("file_tree.file"));
    CHECK(registry.has("file_tree.folder"));
    CHECK(registry.has("file_tree.empty"));
    CHECK(registry.has("tab_bar"));
    CHECK(registry.has("terminal"));
    CHECK(registry.has("output_panel"));
    CHECK(registry.has("problems_panel"));
    CHECK(registry.has("breadcrumb"));

    registry.clear();
}

// ═══════════════════════════════════════════════════════
// Action ID uniqueness across all built-in menus
// ═══════════════════════════════════════════════════════

TEST_CASE("All built-in menu action IDs are unique within each menu",
          "[context_menu][builtin][uniqueness]")
{
    auto check_unique = [](const ContextMenuBuilder& builder, const std::string& menu_name)
    {
        auto model = builder.build();
        auto items = model.sorted_items();
        std::set<std::string> seen;
        for (const auto& menu_item : items)
        {
            if (menu_item.is_separator)
            {
                continue;
            }
            INFO("Menu: " << menu_name << ", Action: " << menu_item.action_id);
            CHECK(seen.find(menu_item.action_id) == seen.end());
            seen.insert(menu_item.action_id);
        }
    };

    check_unique(context_menus::editor_menu(), "editor");
    check_unique(context_menus::file_tree_file_menu(), "file_tree.file");
    check_unique(context_menus::file_tree_folder_menu(), "file_tree.folder");
    check_unique(context_menus::file_tree_empty_menu(), "file_tree.empty");
    check_unique(context_menus::tab_bar_menu(), "tab_bar");
    check_unique(context_menus::terminal_menu(), "terminal");
    check_unique(context_menus::output_panel_menu(), "output_panel");
    check_unique(context_menus::problems_panel_menu(), "problems_panel");
    check_unique(context_menus::breadcrumb_menu(), "breadcrumb");
}

// ═══════════════════════════════════════════════════════
// All items have labels
// ═══════════════════════════════════════════════════════

TEST_CASE("All built-in items have labels", "[context_menu][builtin][labels]")
{
    auto check_labels = [](const ContextMenuBuilder& builder, const std::string& menu_name)
    {
        auto model = builder.build();
        auto items = model.sorted_items();
        for (const auto& menu_item : items)
        {
            if (menu_item.is_separator)
            {
                continue;
            }
            INFO("Menu: " << menu_name << ", Action: " << menu_item.action_id);
            CHECK_FALSE(menu_item.label.empty());
        }
    };

    check_labels(context_menus::editor_menu(), "editor");
    check_labels(context_menus::file_tree_file_menu(), "file_tree.file");
    check_labels(context_menus::file_tree_folder_menu(), "file_tree.folder");
    check_labels(context_menus::file_tree_empty_menu(), "file_tree.empty");
    check_labels(context_menus::tab_bar_menu(), "tab_bar");
    check_labels(context_menus::terminal_menu(), "terminal");
    check_labels(context_menus::output_panel_menu(), "output_panel");
    check_labels(context_menus::problems_panel_menu(), "problems_panel");
    check_labels(context_menus::breadcrumb_menu(), "breadcrumb");
}

// ═══════════════════════════════════════════════════════
// Phase 28: Sub-menu nesting
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - sub-menu nesting", "[context_menu][builder][submenu]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut")
                       .separator()
                       .begin_submenu("Refactor")
                       .item(MenuActionCategory::kRefactor, "extract", "Extract Method")
                       .item(MenuActionCategory::kRefactor, "rename", "Rename Symbol")
                       .end_submenu()
                       .item(MenuActionCategory::kNavigation, "goto", "Go To Definition");

    // Top-level: cut + submenu.Refactor + goto = 3 items (extract/rename are nested)
    auto model = builder.build();
    auto items = model.sorted_items();

    // Find the submenu item
    bool found_submenu = false;
    for (const auto& menu_item : items)
    {
        if (menu_item.has_submenu && menu_item.label == "Refactor")
        {
            found_submenu = true;
            CHECK(menu_item.children.size() == 2);
            CHECK(menu_item.children[0].action_id == "extract");
            CHECK(menu_item.children[1].action_id == "rename");
        }
    }
    CHECK(found_submenu);
}

TEST_CASE("ContextMenuBuilder - submenu depth tracking", "[context_menu][builder][submenu]")
{
    ContextMenuBuilder builder;
    CHECK(builder.submenu_depth() == 0);

    builder.begin_submenu("Level 1");
    CHECK(builder.submenu_depth() == 1);

    builder.begin_submenu("Level 2");
    CHECK(builder.submenu_depth() == 2);

    builder.end_submenu();
    CHECK(builder.submenu_depth() == 1);

    builder.end_submenu();
    CHECK(builder.submenu_depth() == 0);

    // Extra end_submenu is a no-op
    builder.end_submenu();
    CHECK(builder.submenu_depth() == 0);
}

// ═══════════════════════════════════════════════════════
// Phase 28: Icon and checked state
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - icon on item", "[context_menu][builder][icon]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut", "Ctrl+X")
                       .icon("cut", "scissors")
                       .item(MenuActionCategory::kEdit, "paste", "Paste");

    auto model = builder.build();
    auto items = model.sorted_items();
    for (const auto& menu_item : items)
    {
        if (menu_item.action_id == "cut")
        {
            CHECK(menu_item.icon_name == "scissors");
        }
        else if (menu_item.action_id == "paste")
        {
            CHECK(menu_item.icon_name.empty());
        }
    }
}

TEST_CASE("ContextMenuBuilder - checked state", "[context_menu][builder][checked]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kView, "word_wrap", "Word Wrap")
                       .checked("word_wrap", true)
                       .item(MenuActionCategory::kView, "minimap", "Minimap")
                       .checked("minimap", false);

    auto model = builder.build();
    auto items = model.sorted_items();
    for (const auto& menu_item : items)
    {
        if (menu_item.action_id == "word_wrap")
        {
            CHECK(menu_item.is_checked);
        }
        else if (menu_item.action_id == "minimap")
        {
            CHECK_FALSE(menu_item.is_checked);
        }
    }
}

// ═══════════════════════════════════════════════════════
// Phase 28: Disabled tooltip
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - disabled tooltip", "[context_menu][builder][tooltip]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "paste", "Paste")
                       .disabled_tooltip("paste", "Clipboard is empty");

    auto model = builder.build();
    auto items = model.sorted_items();
    bool found = false;
    for (const auto& menu_item : items)
    {
        if (menu_item.action_id == "paste")
        {
            CHECK(menu_item.disabled_tooltip == "Clipboard is empty");
            found = true;
        }
    }
    CHECK(found);
}

// ═══════════════════════════════════════════════════════
// Phase 28: Dynamic item generation
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - dynamic item generation", "[context_menu][builder][dynamic]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut")
                       .dynamic(
                           [](const ContextKeys& keys) -> std::vector<ContextMenuItem>
                           {
                               std::vector<ContextMenuItem> dynamic_items;
                               auto iter = keys.find("hasRecentFiles");
                               if (iter != keys.end() && iter->second)
                               {
                                   ContextMenuItem recent;
                                   recent.action_id = "recent.file1";
                                   recent.label = "Recent: file1.md";
                                   recent.category = MenuActionCategory::kNavigation;
                                   dynamic_items.push_back(recent);
                               }
                               return dynamic_items;
                           });

    SECTION("Dynamic items injected when condition met")
    {
        ContextKeys keys = {{"hasRecentFiles", true}};
        auto model = builder.build(keys);
        auto items = model.sorted_items();
        bool found_dynamic = false;
        for (const auto& menu_item : items)
        {
            if (menu_item.action_id == "recent.file1")
            {
                found_dynamic = true;
            }
        }
        CHECK(found_dynamic);
    }

    SECTION("Dynamic items not injected when condition not met")
    {
        ContextKeys keys = {{"hasRecentFiles", false}};
        auto model = builder.build(keys);
        auto items = model.sorted_items();
        bool found_dynamic = false;
        for (const auto& menu_item : items)
        {
            if (menu_item.action_id == "recent.file1")
            {
                found_dynamic = true;
            }
        }
        CHECK_FALSE(found_dynamic);
    }
}

// ═══════════════════════════════════════════════════════
// Phase 28: Action callbacks
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - action callbacks", "[context_menu][builder][callbacks]")
{
    bool cut_invoked = false;
    bool copy_invoked = false;

    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut")
                       .on_action("cut", [&cut_invoked]() { cut_invoked = true; })
                       .item(MenuActionCategory::kEdit, "copy", "Copy")
                       .on_action("copy", [&copy_invoked]() { copy_invoked = true; });

    auto callbacks = builder.build_callbacks();
    CHECK(callbacks.size() == 2);
    CHECK(callbacks.count("cut") == 1);
    CHECK(callbacks.count("copy") == 1);

    callbacks["cut"]();
    CHECK(cut_invoked);
    CHECK_FALSE(copy_invoked);

    callbacks["copy"]();
    CHECK(copy_invoked);
}

// ═══════════════════════════════════════════════════════
// Phase 28: Clear resets extended state
// ═══════════════════════════════════════════════════════

TEST_CASE("ContextMenuBuilder - clear resets Phase 28 state", "[context_menu][builder][clear]")
{
    auto builder = ContextMenuBuilder()
                       .item(MenuActionCategory::kEdit, "cut", "Cut")
                       .on_action("cut", []() {})
                       .dynamic([](const ContextKeys&) { return std::vector<ContextMenuItem>{}; })
                       .begin_submenu("Sub")
                       .item(MenuActionCategory::kEdit, "inner", "Inner");

    CHECK(builder.submenu_depth() == 1);

    builder.clear();
    CHECK(builder.item_count() == 0);
    CHECK(builder.submenu_depth() == 0);
    CHECK(builder.build_callbacks().empty());
}
