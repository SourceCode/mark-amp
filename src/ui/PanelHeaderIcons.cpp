#include "PanelHeaderIcons.h"

namespace markamp::ui::panel_actions
{

auto explorer_actions() -> std::vector<PanelHeaderAction>
{
    return {
        {"explorer.new_file", "file-add", "New File", "Ctrl+N", false, false, true, false},
        {"explorer.new_folder", "folder-add", "New Folder", "", false, false, true, false},
        {"explorer.collapse_all", "collapse-all", "Collapse All", "", false, false, true, true},
        {"explorer.refresh", "refresh", "Refresh Explorer", "", false, false, true, false},
        {"explorer.filter", "filter", "Filter Files", "", true, false, true, true},
        {"explorer.open_terminal", "terminal", "Open in Terminal", "", false, false, true, false},
    };
}

auto search_actions() -> std::vector<PanelHeaderAction>
{
    return {
        {"search.clear", "clear-all", "Clear Results", "", false, false, true, false},
        {"search.collapse", "collapse-all", "Collapse All", "", false, false, true, false},
        {"search.regex", "regex", "Use Regular Expression", "Alt+R", true, false, true, true},
        {"search.case", "case-sensitive", "Match Case", "Alt+C", true, false, true, false},
        {"search.whole_word", "whole-word", "Match Whole Word", "Alt+W", true, false, true, false},
        {"search.open_editor", "open-editor", "Open in Editor", "", false, false, true, true},
    };
}

auto output_actions() -> std::vector<PanelHeaderAction>
{
    return {
        {"output.clear", "clear-all", "Clear Output", "", false, false, true, false},
        {"output.lock_scroll", "lock", "Lock Scroll", "", true, false, true, true},
        {"output.word_wrap", "word-wrap", "Toggle Word Wrap", "", true, false, true, false},
        {"output.timestamps", "clock", "Show Timestamps", "", true, false, true, false},
        {"output.open_editor", "open-editor", "Open in Editor", "", false, false, true, true},
    };
}

auto problems_actions() -> std::vector<PanelHeaderAction>
{
    return {
        {"problems.filter_error", "error", "Show Errors", "", true, true, true, false},
        {"problems.filter_warning", "warning", "Show Warnings", "", true, true, true, false},
        {"problems.filter_info", "info", "Show Info", "", true, true, true, false},
        {"problems.collapse_all", "collapse-all", "Collapse All", "", false, false, true, true},
        {"problems.quick_fix_all", "lightbulb", "Quick Fix All", "", false, false, true, false},
    };
}

auto terminal_actions() -> std::vector<PanelHeaderAction>
{
    return {
        {"terminal.new", "add", "New Terminal", "Ctrl+Shift+`", false, false, true, false},
        {"terminal.split", "split-horizontal", "Split Terminal", "", false, false, true, false},
        {"terminal.kill", "trash", "Kill Terminal", "", false, false, true, true},
        {"terminal.scroll_bottom", "arrow-down", "Scroll to Bottom", "", false, false, true, false},
        {"terminal.clear", "clear-all", "Clear Terminal", "", false, false, true, false},
    };
}

auto build_actions() -> std::vector<PanelHeaderAction>
{
    return {
        {"build.build", "debug-start", "Build", "Ctrl+Shift+B", false, false, true, false},
        {"build.rebuild", "refresh", "Rebuild All", "", false, false, true, false},
        {"build.clean", "trash", "Clean", "", false, false, true, true},
        {"build.configure", "gear", "Configure", "", false, false, true, false},
        {"build.stop", "debug-stop", "Stop Build", "", false, false, false, true},
    };
}

auto debug_console_actions() -> std::vector<PanelHeaderAction>
{
    return {
        {"debug_console.clear", "clear-all", "Clear Console", "", false, false, true, false},
        {"debug_console.log_level", "filter", "Log Level", "", false, false, true, true},
        {"debug_console.timestamps", "clock", "Show Timestamps", "", true, false, true, false},
    };
}

} // namespace markamp::ui::panel_actions
