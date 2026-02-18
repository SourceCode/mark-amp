/// @file test_phase20_file_management.cpp
/// @brief Phase 20 – Comprehensive tests for File Management & Workspace services.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/FileTemplateEngine.h"
#include "core/FileTreeModel.h"
#include "core/FileWatcher.h"
#include "core/MultiRootWorkspace.h"
#include "core/TrashManager.h"
#include "core/WorkspaceSettings.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// FileWatcher Tests
// ============================================================================

TEST_CASE("FileWatcher: construction", "[phase20][file-watcher]")
{
    EventBus bus;
    FileWatcher watcher(bus);

    REQUIRE(watcher.watched_count() == 0);
    REQUIRE(watcher.total_changes_detected() == 0);
    REQUIRE(watcher.debounce_ms() == 100);
    REQUIRE(watcher.is_recursive());
}

TEST_CASE("FileWatcher: watch and unwatch paths", "[phase20][file-watcher]")
{
    EventBus bus;
    FileWatcher watcher(bus);

    // Watching a non-existent path should fail.
    REQUIRE_FALSE(watcher.watch("/nonexistent/path/abc123"));
    REQUIRE(watcher.watched_count() == 0);

    // Cannot unwatch what isn't watched.
    REQUIRE_FALSE(watcher.unwatch("/nonexistent/path/abc123"));
}

TEST_CASE("FileWatcher: configuration", "[phase20][file-watcher]")
{
    EventBus bus;
    FileWatcher watcher(bus);

    watcher.set_debounce_ms(500);
    REQUIRE(watcher.debounce_ms() == 500);

    watcher.set_recursive(false);
    REQUIRE_FALSE(watcher.is_recursive());
}

TEST_CASE("FileWatcher: callbacks", "[phase20][file-watcher]")
{
    EventBus bus;
    FileWatcher watcher(bus);

    int call_count = 0;
    auto callback_id =
        watcher.on_change([&call_count](const FileChangeEvent& /*evt*/) { ++call_count; });

    REQUIRE(callback_id == 0);

    watcher.remove_callback(callback_id);
    // Callback removed, should not be called.
}

TEST_CASE("FileWatcher: clear", "[phase20][file-watcher]")
{
    EventBus bus;
    FileWatcher watcher(bus);

    watcher.clear();
    REQUIRE(watcher.watched_count() == 0);
}

TEST_CASE("FileWatcher: poll with no watches", "[phase20][file-watcher]")
{
    EventBus bus;
    FileWatcher watcher(bus);

    REQUIRE(watcher.poll() == 0);
}

// ============================================================================
// FileTreeModel Tests
// ============================================================================

TEST_CASE("FileTreeModel: construction", "[phase20][file-tree]")
{
    EventBus bus;
    FileTreeModel model(bus);

    REQUIRE(model.root_path().empty());
    REQUIRE(model.node_count() == 1); // Root node exists.
    REQUIRE(model.sort_order() == FileTreeSortOrder::kNameAsc);
    REQUIRE_FALSE(model.filter().show_hidden);
}

TEST_CASE("FileTreeModel: load non-existent directory fails", "[phase20][file-tree]")
{
    EventBus bus;
    FileTreeModel model(bus);

    REQUIRE_FALSE(model.load_directory("/nonexistent/dir/abc123"));
    REQUIRE(model.root_path().empty());
}

TEST_CASE("FileTreeModel: sort order", "[phase20][file-tree]")
{
    EventBus bus;
    FileTreeModel model(bus);

    model.set_sort_order(FileTreeSortOrder::kNameDesc);
    REQUIRE(model.sort_order() == FileTreeSortOrder::kNameDesc);

    model.set_sort_order(FileTreeSortOrder::kTypeGrouped);
    REQUIRE(model.sort_order() == FileTreeSortOrder::kTypeGrouped);
}

TEST_CASE("FileTreeModel: filter configuration", "[phase20][file-tree]")
{
    EventBus bus;
    FileTreeModel model(bus);

    FileTreeFilter filter;
    filter.show_hidden = true;
    filter.show_dotfiles = true;
    filter.include_extensions = {"md", "txt"};
    filter.exclude_patterns = {"node_modules"};

    model.set_filter(filter);

    REQUIRE(model.filter().show_hidden);
    REQUIRE(model.filter().show_dotfiles);
    REQUIRE(model.filter().include_extensions.size() == 2);
}

TEST_CASE("FileTreeModel: expand and collapse", "[phase20][file-tree]")
{
    EventBus bus;
    FileTreeModel model(bus);

    model.expand_node("/some/path");
    REQUIRE(model.is_expanded("/some/path"));
    REQUIRE(model.expanded_paths().size() == 1);

    model.collapse_node("/some/path");
    REQUIRE_FALSE(model.is_expanded("/some/path"));

    model.toggle_node("/some/path");
    REQUIRE(model.is_expanded("/some/path"));
}

TEST_CASE("FileTreeModel: clear", "[phase20][file-tree]")
{
    EventBus bus;
    FileTreeModel model(bus);

    model.expand_node("/some/path");
    model.clear();

    REQUIRE(model.root_path().empty());
    REQUIRE(model.expanded_paths().empty());
}

TEST_CASE("FileTreeModel: refresh callbacks", "[phase20][file-tree]")
{
    EventBus bus;
    FileTreeModel model(bus);

    int refresh_count = 0;
    auto callback_id = model.on_refresh([&refresh_count]() { ++refresh_count; });

    REQUIRE(callback_id == 0);

    model.remove_refresh_callback(callback_id);
}

// ============================================================================
// WorkspaceSettings Tests
// ============================================================================

TEST_CASE("WorkspaceSettings: construction", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    REQUIRE(settings.count(SettingScope::kDefault) == 0);
    REQUIRE(settings.count(SettingScope::kUser) == 0);
    REQUIRE(settings.count(SettingScope::kWorkspace) == 0);
    REQUIRE(settings.count(SettingScope::kFolder) == 0);
}

TEST_CASE("WorkspaceSettings: set and get string", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    settings.set("editor.fontSize", "14", SettingScope::kUser);
    auto value = settings.get_string("editor.fontSize", SettingScope::kUser);
    REQUIRE(value.has_value());
    REQUIRE(value.value() == "14");
}

TEST_CASE("WorkspaceSettings: set and get int", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    settings.set_int("editor.tabSize", 4, SettingScope::kUser);
    auto value = settings.get_int("editor.tabSize", SettingScope::kUser);
    REQUIRE(value.has_value());
    REQUIRE(value.value() == 4);
}

TEST_CASE("WorkspaceSettings: set and get bool", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    settings.set_bool("editor.wordWrap", true, SettingScope::kWorkspace);
    auto value = settings.get_bool("editor.wordWrap", SettingScope::kWorkspace);
    REQUIRE(value.has_value());
    REQUIRE(value.value());
}

TEST_CASE("WorkspaceSettings: cascade resolution", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    // Set at Default.
    settings.set("editor.fontSize", "12", SettingScope::kDefault);
    // Override at User.
    settings.set("editor.fontSize", "14", SettingScope::kUser);
    // Override at Workspace.
    settings.set("editor.fontSize", "16", SettingScope::kWorkspace);

    // Effective should be the highest scope (Workspace).
    auto effective = settings.effective_value("editor.fontSize");
    REQUIRE(effective.has_value());
    REQUIRE(effective.value().string_value == "16");
    REQUIRE(effective.value().scope == SettingScope::kWorkspace);
}

TEST_CASE("WorkspaceSettings: cascade with folder override", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    settings.set("theme.name", "dark", SettingScope::kUser);
    settings.set("theme.name", "light", SettingScope::kFolder);

    auto effective = settings.effective_value("theme.name");
    REQUIRE(effective.has_value());
    REQUIRE(effective.value().string_value == "light");
    REQUIRE(effective.value().scope == SettingScope::kFolder);
}

TEST_CASE("WorkspaceSettings: remove and has", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    settings.set("key1", "value1", SettingScope::kUser);
    REQUIRE(settings.has("key1", SettingScope::kUser));
    REQUIRE(settings.remove("key1", SettingScope::kUser));
    REQUIRE_FALSE(settings.has("key1", SettingScope::kUser));
}

TEST_CASE("WorkspaceSettings: all keys", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    settings.set("b_key", "val", SettingScope::kUser);
    settings.set("a_key", "val", SettingScope::kUser);

    auto keys = settings.all_keys(SettingScope::kUser);
    REQUIRE(keys.size() == 2);
    REQUIRE(keys[0] == "a_key"); // Sorted.
    REQUIRE(keys[1] == "b_key");
}

TEST_CASE("WorkspaceSettings: reset to defaults", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    settings.set("key1", "val", SettingScope::kUser);
    settings.set("key2", "val", SettingScope::kWorkspace);
    settings.set("key3", "val", SettingScope::kFolder);

    settings.reset_to_defaults();
    REQUIRE(settings.count(SettingScope::kUser) == 0);
    REQUIRE(settings.count(SettingScope::kWorkspace) == 0);
    REQUIRE(settings.count(SettingScope::kFolder) == 0);
}

TEST_CASE("WorkspaceSettings: persistence stubs", "[phase20][workspace-settings]")
{
    EventBus bus;
    WorkspaceSettings settings(bus);

    auto load_result = settings.load_workspace_settings("/some/path");
    REQUIRE(load_result.has_value());

    auto load_empty = settings.load_workspace_settings("");
    REQUIRE_FALSE(load_empty.has_value());
}

// ============================================================================
// FileTemplateEngine Tests
// ============================================================================

TEST_CASE("FileTemplateEngine: built-in templates", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    REQUIRE(engine.template_count() >= 5); // At least 5 built-ins.

    auto templates = engine.list_templates();
    REQUIRE_FALSE(templates.empty());
}

TEST_CASE("FileTemplateEngine: find template by ID", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    auto* tmpl = engine.find_template("blank-md");
    REQUIRE(tmpl != nullptr);
    REQUIRE(tmpl->name == "Blank Document");
    REQUIRE(tmpl->extension == "md");
    REQUIRE(tmpl->is_builtin);
}

TEST_CASE("FileTemplateEngine: create from template", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    auto result =
        engine.create_from_template("blank-md", "/path/to/doc.md", {{"title", "Hello World"}});

    REQUIRE(result.has_value());
    REQUIRE(result.value() == "# Hello World\n\n");
}

TEST_CASE("FileTemplateEngine: template defaults", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    // Create with no variables — should use defaults.
    auto result = engine.create_from_template("blank-md", "/path/to/doc.md");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == "# Untitled\n\n");
}

TEST_CASE("FileTemplateEngine: nonexistent template", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    auto result = engine.create_from_template("nonexistent", "/path/to/doc.md");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("FileTemplateEngine: search templates", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    auto results = engine.search_templates("blank");
    REQUIRE(results.size() >= 2); // Blank Document and Blank Canvas at minimum.
}

TEST_CASE("FileTemplateEngine: templates for extension", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    auto md_templates = engine.templates_for_extension("md");
    REQUIRE(md_templates.size() >= 2); // blank-md, meeting-notes, daily-journal.
}

TEST_CASE("FileTemplateEngine: user template", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    auto initial_count = engine.template_count();
    auto user_id = engine.add_user_template("Custom Note", "# {{title}}\nCustom", "md");
    REQUIRE_FALSE(user_id.empty());
    REQUIRE(engine.template_count() == initial_count + 1);

    REQUIRE(engine.unregister_template(user_id));
    REQUIRE(engine.template_count() == initial_count);
}

TEST_CASE("FileTemplateEngine: categories", "[phase20][file-templates]")
{
    EventBus bus;
    FileTemplateEngine engine(bus);

    auto cats = engine.categories();
    REQUIRE(cats.size() >= 3); // Document, Notebook, Canvas.
}

TEST_CASE("FileTemplateEngine: variable substitution", "[phase20][file-templates]")
{
    auto result = FileTemplateEngine::substitute_variables(
        "Hello {{name}}, welcome to {{place}}!", {{"name", "Alice"}, {"place", "Wonderland"}});

    REQUIRE(result == "Hello Alice, welcome to Wonderland!");
}

// ============================================================================
// TrashManager Tests
// ============================================================================

TEST_CASE("TrashManager: construction", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    REQUIRE(trash.trashed_count() == 0);
    REQUIRE(trash.total_trash_size() == 0);
}

TEST_CASE("TrashManager: trash and restore", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    auto result = trash.trash("/path/to/file.md");
    REQUIRE(result.has_value());
    REQUIRE(trash.trashed_count() == 1);

    auto trash_id = result.value();
    auto restore_result = trash.restore(trash_id);
    REQUIRE(restore_result.has_value());
    REQUIRE(trash.trashed_count() == 0);
}

TEST_CASE("TrashManager: trash empty path fails", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    auto result = trash.trash("");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("TrashManager: restore nonexistent fails", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    auto result = trash.restore("nonexistent-id");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("TrashManager: permanent delete", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    auto result = trash.trash("/path/to/file.md");
    REQUIRE(result.has_value());

    REQUIRE(trash.permanent_delete(result.value()));
    REQUIRE(trash.trashed_count() == 0);
}

TEST_CASE("TrashManager: list and find", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    auto result_a = trash.trash("/path/a.md");
    auto result_b = trash.trash("/path/b.md");
    REQUIRE(result_a.has_value());
    REQUIRE(result_b.has_value());

    auto list = trash.list_trashed();
    REQUIRE(list.size() == 2);

    auto found = trash.find_by_original_path("/path/a.md");
    REQUIRE(found.has_value());
    REQUIRE(found.value().original_path == "/path/a.md");

    auto found_by_id = trash.find_by_id(result_a.value());
    REQUIRE(found_by_id.has_value());
}

TEST_CASE("TrashManager: empty trash", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    trash.trash("/path/a.md");
    trash.trash("/path/b.md");
    trash.trash("/path/c.md");

    auto emptied = trash.empty_trash();
    REQUIRE(emptied == 3);
    REQUIRE(trash.trashed_count() == 0);
}

TEST_CASE("TrashManager: configuration", "[phase20][trash]")
{
    EventBus bus;
    TrashManager trash(bus);

    TrashConfig cfg;
    cfg.auto_purge_days = 7;
    cfg.max_trash_bytes = 1024 * 1024;
    cfg.trash_directory = "/custom/trash";

    trash.set_config(cfg);

    REQUIRE(trash.config().auto_purge_days == 7);
    REQUIRE(trash.config().max_trash_bytes == 1024 * 1024);
    REQUIRE(trash.config().trash_directory == "/custom/trash");
}

// ============================================================================
// MultiRootWorkspace Tests
// ============================================================================

TEST_CASE("MultiRootWorkspace: construction", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    REQUIRE(workspace.root_count() == 0);
    REQUIRE_FALSE(workspace.active_root().has_value());
    REQUIRE(workspace.name().empty());
}

TEST_CASE("MultiRootWorkspace: add and remove roots", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    auto result = workspace.add_root("/path/to/docs", "Documentation");
    REQUIRE(result.has_value());
    REQUIRE(workspace.root_count() == 1);

    // First root becomes active.
    auto active = workspace.active_root();
    REQUIRE(active.has_value());
    REQUIRE(active.value().display_name == "Documentation");

    auto root_id = result.value();
    REQUIRE(workspace.remove_root(root_id));
    REQUIRE(workspace.root_count() == 0);
}

TEST_CASE("MultiRootWorkspace: duplicate root fails", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    workspace.add_root("/path/to/docs");
    auto duplicate = workspace.add_root("/path/to/docs");
    REQUIRE_FALSE(duplicate.has_value());
}

TEST_CASE("MultiRootWorkspace: add empty path fails", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    auto result = workspace.add_root("");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("MultiRootWorkspace: set active root", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    auto root1 = workspace.add_root("/path/docs", "Docs");
    auto root2 = workspace.add_root("/path/notes", "Notes");
    REQUIRE(root1.has_value());
    REQUIRE(root2.has_value());

    workspace.set_active_root(root2.value());
    auto active = workspace.active_root();
    REQUIRE(active.has_value());
    REQUIRE(active.value().display_name == "Notes");
}

TEST_CASE("MultiRootWorkspace: reorder roots", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    auto root1 = workspace.add_root("/path/a", "A");
    auto root2 = workspace.add_root("/path/b", "B");
    auto root3 = workspace.add_root("/path/c", "C");

    // Reverse order.
    workspace.reorder_roots({root3.value(), root2.value(), root1.value()});

    auto roots = workspace.all_roots();
    REQUIRE(roots.size() == 3);
    REQUIRE(roots[0].display_name == "C");
    REQUIRE(roots[1].display_name == "B");
    REQUIRE(roots[2].display_name == "A");
}

TEST_CASE("MultiRootWorkspace: find root for path", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    workspace.add_root("/path/to/docs", "Docs");
    workspace.add_root("/path/to/notes", "Notes");

    auto found = workspace.find_root_for_path("/path/to/docs/file.md");
    REQUIRE(found.has_value());
    REQUIRE(found.value().display_name == "Docs");

    auto not_found = workspace.find_root_for_path("/other/path/file.md");
    REQUIRE_FALSE(not_found.has_value());
}

TEST_CASE("MultiRootWorkspace: merge file trees", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    workspace.set_name("My Workspace");
    workspace.add_root("/path/docs", "Docs");
    workspace.add_root("/path/notes", "Notes");

    auto merged = workspace.merge_file_trees();
    REQUIRE(merged.name == "My Workspace");
    REQUIRE(merged.is_folder());
    REQUIRE(merged.children.size() == 2);
    REQUIRE(merged.children[0].name == "Docs");
    REQUIRE(merged.children[1].name == "Notes");
}

TEST_CASE("MultiRootWorkspace: workspace settings", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    workspace.set_setting("theme", "dark");
    auto value = workspace.get_setting("theme");
    REQUIRE(value.has_value());
    REQUIRE(value.value() == "dark");

    auto missing = workspace.get_setting("nonexistent");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("MultiRootWorkspace: workspace file I/O stubs", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    auto save_result = workspace.save_workspace_file("/path/to/file.markamp-workspace");
    REQUIRE(save_result.has_value());

    auto load_result = workspace.load_workspace_file("/path/to/file.markamp-workspace");
    REQUIRE(load_result.has_value());

    auto empty_result = workspace.save_workspace_file("");
    REQUIRE_FALSE(empty_result.has_value());
}

TEST_CASE("MultiRootWorkspace: clear", "[phase20][multi-root]")
{
    EventBus bus;
    MultiRootWorkspace workspace(bus);

    workspace.set_name("Test");
    workspace.add_root("/path/docs");
    workspace.set_setting("key", "val");

    workspace.clear();
    REQUIRE(workspace.root_count() == 0);
    REQUIRE(workspace.name().empty());
    REQUIRE_FALSE(workspace.get_setting("key").has_value());
}
