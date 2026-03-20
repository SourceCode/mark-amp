/// @file WorkbenchCommands.cpp
/// @brief P02-T02: Central registration of core workbench commands.

#include "WorkbenchCommands.h"

#include "CommandRegistry.h"
#include "EventBus.h"
#include "Events.h"

namespace markamp::core
{

void register_workbench_commands(CommandRegistry& registry, EventBus& event_bus)
{
    std::vector<CommandEntry> commands;

    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // File commands
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

    commands.push_back({.id = "file.newFile",
                        .title = "New File",
                        .category = "File",
                        .description = "Create a new untitled file",
                        .shortcut = "Ctrl+N",
                        .icon = "ui.toolbar.new",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::NewFileRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "file.save",
                        .title = "Save",
                        .category = "File",
                        .description = "Save the active file",
                        .shortcut = "Ctrl+S",
                        .icon = "ui.toolbar.save",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::TabSaveRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "file.print",
                        .title = "Print...",
                        .category = "File",
                        .description = "Print the active document",
                        .shortcut = "Ctrl+P",
                        .icon = "ui.toolbar.print",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::PrintDocumentRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "file.copyPath",
                        .title = "Copy File Path",
                        .category = "File",
                        .description = "Copy the active file path to clipboard",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::CopyFilePathRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "file.revealInFinder",
                        .title = "Reveal in Finder",
                        .category = "File",
                        .description = "Reveal the active file in the system file manager",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::RevealInFinderRequestEvent{});
                            return true;
                        }});

    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // Edit commands
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

    commands.push_back({.id = "edit.find",
                        .title = "Find",
                        .category = "Edit",
                        .description = "Open the find bar",
                        .shortcut = "Ctrl+F",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::FindRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.replace",
                        .title = "Replace",
                        .category = "Edit",
                        .description = "Open the find-and-replace bar",
                        .shortcut = "Ctrl+H",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ReplaceRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.findNext",
                        .title = "Find Next",
                        .category = "Edit",
                        .description = "Go to the next search match",
                        .shortcut = "Ctrl+G",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::SearchNextMatchRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.findPrevious",
                        .title = "Find Previous",
                        .category = "Edit",
                        .description = "Go to the previous search match",
                        .shortcut = "Ctrl+Shift+G",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::SearchPrevMatchRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.duplicateLine",
                        .title = "Duplicate Line",
                        .category = "Edit",
                        .description = "Duplicate the current line or selection",
                        .shortcut = "Ctrl+Shift+D",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::DuplicateLineRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.toggleComment",
                        .title = "Toggle Comment",
                        .category = "Edit",
                        .description = "Toggle line comment on the current line or selection",
                        .shortcut = "Ctrl+/",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ToggleCommentRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.deleteLine",
                        .title = "Delete Line",
                        .category = "Edit",
                        .description = "Delete the current line",
                        .shortcut = "Ctrl+Shift+K",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::DeleteLineRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.moveLineUp",
                        .title = "Move Line Up",
                        .category = "Edit",
                        .description = "Move the current line up",
                        .shortcut = "Alt+Up",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::MoveLineUpRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.moveLineDown",
                        .title = "Move Line Down",
                        .category = "Edit",
                        .description = "Move the current line down",
                        .shortcut = "Alt+Down",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::MoveLineDownRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.goToLine",
                        .title = "Go to Line...",
                        .category = "Edit",
                        .description = "Jump to a specific line number",
                        .shortcut = "Ctrl+L",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::GoToLineRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.toggleWordWrap",
                        .title = "Toggle Word Wrap",
                        .category = "Edit",
                        .description = "Toggle word wrapping in the editor",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::WrapToggleRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.sortLinesAsc",
                        .title = "Sort Lines Ascending",
                        .category = "Edit",
                        .description = "Sort selected lines in ascending order",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::SortLinesAscRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.sortLinesDesc",
                        .title = "Sort Lines Descending",
                        .category = "Edit",
                        .description = "Sort selected lines in descending order",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::SortLinesDescRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.joinLines",
                        .title = "Join Lines",
                        .category = "Edit",
                        .description = "Join the current line with the next line",
                        .shortcut = "Ctrl+J",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::JoinLinesRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.transformUpperCase",
                        .title = "Transform to Upper Case",
                        .category = "Edit",
                        .description = "Convert selected text to upper case",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::TransformUpperRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.transformLowerCase",
                        .title = "Transform to Lower Case",
                        .category = "Edit",
                        .description = "Convert selected text to lower case",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::TransformLowerRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "edit.insertDateTime",
                        .title = "Insert Date/Time",
                        .category = "Edit",
                        .description = "Insert the current date and time at cursor",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::InsertDateTimeRequestEvent{});
                            return true;
                        }});

    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // View commands
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

    commands.push_back({.id = "view.toggleSidebar",
                        .title = "Toggle Sidebar",
                        .category = "View",
                        .description = "Show or hide the sidebar",
                        .shortcut = "Ctrl+B",
                        .icon = "ui.panel.sidebar",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::SidebarToggleEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.toggleBottomPanel",
                        .title = "Toggle Bottom Panel",
                        .category = "View",
                        .description = "Show or hide the bottom panel",
                        .shortcut = "Ctrl+J",
                        .icon = "ui.panel.bottom",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ToggleBottomPanelRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.toggleMinimap",
                        .title = "Toggle Minimap",
                        .category = "View",
                        .description = "Show or hide the minimap",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ToggleMinimapRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.toggleLineNumbers",
                        .title = "Toggle Line Numbers",
                        .category = "View",
                        .description = "Show or hide line numbers in the editor",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ToggleLineNumbersRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.toggleWhitespace",
                        .title = "Toggle Whitespace",
                        .category = "View",
                        .description = "Show or hide whitespace characters in the editor",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ToggleWhitespaceRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.foldAll",
                        .title = "Fold All",
                        .category = "View",
                        .description = "Collapse all foldable regions",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::FoldAllRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.unfoldAll",
                        .title = "Unfold All",
                        .category = "View",
                        .description = "Expand all foldable regions",
                        .when_clause = "editorFocus",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::UnfoldAllRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.zoomIn",
                        .title = "Zoom In",
                        .category = "View",
                        .description = "Increase the editor zoom level",
                        .shortcut = "Ctrl+=",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ZoomInRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.zoomOut",
                        .title = "Zoom Out",
                        .category = "View",
                        .description = "Decrease the editor zoom level",
                        .shortcut = "Ctrl+-",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ZoomOutRequestEvent{});
                            return true;
                        }});

    commands.push_back({.id = "view.zoomReset",
                        .title = "Zoom Reset",
                        .category = "View",
                        .description = "Reset the editor zoom level to default",
                        .shortcut = "Ctrl+0",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::ZoomResetRequestEvent{});
                            return true;
                        }});

    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // Window commands
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

    commands.push_back({.id = "window.showCommandPalette",
                        .title = "Command Palette",
                        .category = "Window",
                        .description = "Open the command palette",
                        .shortcut = "Ctrl+Shift+P",
                        .icon = "ui.toolbar.command",
                        .execute_fn = []() -> bool
                        {
                            // Handled directly by MainFrame accelerator
                            return true;
                        }});

    commands.push_back({.id = "window.themeGallery",
                        .title = "Theme Gallery",
                        .category = "Window",
                        .description = "Open the theme gallery",
                        .shortcut = "Ctrl+T",
                        .icon = "ui.toolbar.themes",
                        .execute_fn = [&event_bus]() -> bool
                        {
                            event_bus.publish(events::SettingsOpenRequestEvent{});
                            return true;
                        }});

    registry.register_commands(std::move(commands));
}

} // namespace markamp::core
