#include "ContextMenuRegistry.h"

#include <algorithm>

namespace markamp::ui
{

auto ContextMenuRegistry::instance() -> ContextMenuRegistry&
{
    static ContextMenuRegistry registry;
    return registry;
}

void ContextMenuRegistry::register_menu(const std::string& context_type, MenuFactory factory)
{
    factories_[context_type] = std::move(factory);
}

void ContextMenuRegistry::unregister_menu(const std::string& context_type)
{
    factories_.erase(context_type);
}

auto ContextMenuRegistry::get(const std::string& context_type) const -> ContextMenuBuilder
{
    auto iter = factories_.find(context_type);
    if (iter != factories_.end())
    {
        return iter->second();
    }
    return {}; // Return empty builder
}

auto ContextMenuRegistry::has(const std::string& context_type) const -> bool
{
    return factories_.find(context_type) != factories_.end();
}

auto ContextMenuRegistry::registered_types() const -> std::vector<std::string>
{
    std::vector<std::string> types;
    types.reserve(factories_.size());
    for (const auto& [type_name, factory_func] : factories_)
    {
        types.push_back(type_name);
    }
    std::sort(types.begin(), types.end());
    return types;
}

auto ContextMenuRegistry::count() const -> int
{
    return static_cast<int>(factories_.size());
}

void ContextMenuRegistry::clear()
{
    factories_.clear();
}

// ═══════════════════════════════════════════════════════
// Built-in context menu factories
// ═══════════════════════════════════════════════════════

namespace context_menus
{

auto editor_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kEdit, "editor.cut", "Cut", "Ctrl+X")
        .item(MenuActionCategory::kEdit, "editor.copy", "Copy", "Ctrl+C")
        .item(MenuActionCategory::kEdit, "editor.paste", "Paste", "Ctrl+V")
        .separator()
        .item(MenuActionCategory::kEdit, "editor.select_all", "Select All", "Ctrl+A")
        .separator()
        .item(MenuActionCategory::kNavigation, "editor.go_to_definition", "Go to Definition", "F12")
        .item(
            MenuActionCategory::kNavigation, "editor.peek_definition", "Peek Definition", "Alt+F12")
        .item(MenuActionCategory::kNavigation,
              "editor.go_to_references",
              "Go to References",
              "Shift+F12")
        .separator()
        .item(MenuActionCategory::kRefactor, "editor.rename_symbol", "Rename Symbol", "F2")
        .item(MenuActionCategory::kRefactor, "editor.extract_function", "Extract Function")
        .item(MenuActionCategory::kRefactor, "editor.extract_variable", "Extract Variable")
        .separator()
        .item(MenuActionCategory::kView, "editor.format_document", "Format Document", "Shift+Alt+F")
        .item(MenuActionCategory::kView, "editor.fold_region", "Fold Region")
        .item(MenuActionCategory::kView, "editor.unfold_region", "Unfold Region");
}

auto file_tree_file_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kNavigation, "file.open", "Open", "Enter")
        .item(MenuActionCategory::kNavigation, "file.open_to_side", "Open to the Side")
        .item(MenuActionCategory::kNavigation, "file.reveal_in_finder", "Reveal in Finder")
        .separator()
        .item(MenuActionCategory::kEdit, "file.cut", "Cut", "Ctrl+X")
        .item(MenuActionCategory::kEdit, "file.copy", "Copy", "Ctrl+C")
        .item(MenuActionCategory::kEdit, "file.copy_path", "Copy Path")
        .item(MenuActionCategory::kEdit, "file.copy_relative_path", "Copy Relative Path")
        .separator()
        .item(MenuActionCategory::kEdit, "file.rename", "Rename", "F2")
        .item(MenuActionCategory::kEdit, "file.delete", "Delete", "Delete")
        .separator()
        .item(MenuActionCategory::kCreate, "file.new_file", "New File...")
        .item(MenuActionCategory::kCreate, "file.new_folder", "New Folder...");
}

auto file_tree_folder_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kNavigation, "folder.open", "Open Folder")
        .item(MenuActionCategory::kNavigation, "folder.reveal_in_finder", "Reveal in Finder")
        .item(MenuActionCategory::kNavigation, "folder.open_terminal", "Open in Terminal")
        .separator()
        .item(MenuActionCategory::kCreate, "folder.new_file", "New File...", "Ctrl+N")
        .item(MenuActionCategory::kCreate, "folder.new_folder", "New Folder...")
        .separator()
        .item(MenuActionCategory::kEdit, "folder.cut", "Cut", "Ctrl+X")
        .item(MenuActionCategory::kEdit, "folder.copy", "Copy", "Ctrl+C")
        .item(MenuActionCategory::kEdit, "folder.paste", "Paste", "Ctrl+V")
        .separator()
        .item(MenuActionCategory::kEdit, "folder.rename", "Rename", "F2")
        .item(MenuActionCategory::kEdit, "folder.delete", "Delete");
}

auto file_tree_empty_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kCreate, "empty.new_file", "New File...", "Ctrl+N")
        .item(MenuActionCategory::kCreate, "empty.new_folder", "New Folder...")
        .separator()
        .item(MenuActionCategory::kEdit, "empty.paste", "Paste", "Ctrl+V")
        .separator()
        .item(MenuActionCategory::kView, "empty.refresh", "Refresh Explorer")
        .item(MenuActionCategory::kView, "empty.collapse_all", "Collapse All");
}

auto tab_bar_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kNavigation, "tab.close", "Close", "Ctrl+W")
        .item(MenuActionCategory::kNavigation, "tab.close_others", "Close Others")
        .item(MenuActionCategory::kNavigation, "tab.close_right", "Close to the Right")
        .item(MenuActionCategory::kNavigation, "tab.close_all", "Close All")
        .separator()
        .item(MenuActionCategory::kNavigation, "tab.copy_path", "Copy Path")
        .item(MenuActionCategory::kNavigation, "tab.copy_relative_path", "Copy Relative Path")
        .separator()
        .item(MenuActionCategory::kNavigation, "tab.reveal_in_explorer", "Reveal in Explorer")
        .item(MenuActionCategory::kNavigation, "tab.reveal_in_finder", "Reveal in Finder")
        .separator()
        .item(MenuActionCategory::kView, "tab.pin", "Pin Tab")
        .item(MenuActionCategory::kView, "tab.split_right", "Split Right")
        .item(MenuActionCategory::kView, "tab.split_down", "Split Down");
}

auto terminal_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kEdit, "terminal.copy", "Copy", "Ctrl+C")
        .item(MenuActionCategory::kEdit, "terminal.paste", "Paste", "Ctrl+V")
        .separator()
        .item(MenuActionCategory::kEdit, "terminal.select_all", "Select All")
        .item(MenuActionCategory::kEdit, "terminal.clear", "Clear Terminal")
        .separator()
        .item(MenuActionCategory::kView, "terminal.split", "Split Terminal")
        .item(MenuActionCategory::kNavigation, "terminal.new", "New Terminal")
        .item(MenuActionCategory::kNavigation, "terminal.kill", "Kill Terminal");
}

auto output_panel_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kEdit, "output.copy", "Copy", "Ctrl+C")
        .item(MenuActionCategory::kEdit, "output.select_all", "Select All", "Ctrl+A")
        .separator()
        .item(MenuActionCategory::kEdit, "output.clear", "Clear Output")
        .separator()
        .item(MenuActionCategory::kView, "output.word_wrap", "Toggle Word Wrap")
        .item(MenuActionCategory::kView, "output.timestamps", "Toggle Timestamps");
}

auto problems_panel_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kNavigation, "problems.go_to", "Go to Problem", "Enter")
        .item(MenuActionCategory::kNavigation, "problems.peek", "Peek Problem")
        .separator()
        .item(MenuActionCategory::kEdit, "problems.copy", "Copy Message", "Ctrl+C")
        .item(MenuActionCategory::kRefactor, "problems.quick_fix", "Quick Fix...", "Ctrl+.")
        .separator()
        .item(MenuActionCategory::kView, "problems.filter", "Filter Problems")
        .item(MenuActionCategory::kView, "problems.collapse_all", "Collapse All");
}

auto breadcrumb_menu() -> ContextMenuBuilder
{
    return ContextMenuBuilder()
        .item(MenuActionCategory::kNavigation, "breadcrumb.go_to", "Go to Symbol")
        .item(MenuActionCategory::kNavigation, "breadcrumb.copy_path", "Copy Path")
        .item(MenuActionCategory::kNavigation, "breadcrumb.reveal", "Reveal in Explorer")
        .separator()
        .item(MenuActionCategory::kView, "breadcrumb.toggle_icons", "Toggle Icons")
        .item(MenuActionCategory::kView, "breadcrumb.toggle_path", "Toggle File Path");
}

void register_all_defaults()
{
    auto& registry = ContextMenuRegistry::instance();

    registry.register_menu("editor", editor_menu);
    registry.register_menu("file_tree.file", file_tree_file_menu);
    registry.register_menu("file_tree.folder", file_tree_folder_menu);
    registry.register_menu("file_tree.empty", file_tree_empty_menu);
    registry.register_menu("tab_bar", tab_bar_menu);
    registry.register_menu("terminal", terminal_menu);
    registry.register_menu("output_panel", output_panel_menu);
    registry.register_menu("problems_panel", problems_panel_menu);
    registry.register_menu("breadcrumb", breadcrumb_menu);
}

} // namespace context_menus

} // namespace markamp::ui
