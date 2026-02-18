/// @file BookmarkCommandProvider.cpp
/// @brief V9 Phase 37 — BookmarkCommandProvider implementation.

#include "BookmarkCommandProvider.h"

namespace markamp::core
{

void BookmarkCommandProvider::register_commands(CommandRegistry& registry) const
{
    auto entries = build_entries();
    registry.register_commands(std::move(entries));
}

auto BookmarkCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "bookmark.toggle",
        "bookmark.next",
        "bookmark.previous",
        "bookmark.list",
        "bookmark.removeAll",
        "bookmark.export",
        "bookmark.renameLabel",
        "bookmark.goTo",
    };
}

auto BookmarkCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
{
    auto entries = build_entries();
    for (auto& entry : entries)
    {
        if (entry.id == command_id)
        {
            return entry;
        }
    }
    return {};
}

auto BookmarkCommandProvider::command_count() -> int
{
    return 8;
}

auto BookmarkCommandProvider::build_entries() const -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "bookmark.toggle";
        cmd.title = "Toggle Bookmark";
        cmd.category = "Bookmark";
        cmd.description = "Toggle a bookmark on the current block";
        cmd.shortcut = "Cmd+Shift+B";
        cmd.icon = "bookmark";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "bookmark.next";
        cmd.title = "Next Bookmark";
        cmd.category = "Bookmark";
        cmd.description = "Navigate to the next bookmark";
        cmd.shortcut = "F2";
        cmd.icon = "arrow-down";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "bookmark.previous";
        cmd.title = "Previous Bookmark";
        cmd.category = "Bookmark";
        cmd.description = "Navigate to the previous bookmark";
        cmd.shortcut = "Shift+F2";
        cmd.icon = "arrow-up";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "bookmark.list";
        cmd.title = "List Bookmarks";
        cmd.category = "Bookmark";
        cmd.description = "Show all bookmarks in a searchable list";
        cmd.icon = "list-tree";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "bookmark.removeAll";
        cmd.title = "Remove All Bookmarks";
        cmd.category = "Bookmark";
        cmd.description = "Remove all bookmarks from the workspace";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "bookmark.export";
        cmd.title = "Export Bookmarks";
        cmd.category = "Bookmark";
        cmd.description = "Export bookmarks to Markdown, JSON, CSV, or OPML";
        cmd.icon = "export";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "bookmark.renameLabel";
        cmd.title = "Rename Bookmark Label";
        cmd.category = "Bookmark";
        cmd.description = "Rename a bookmark label across all blocks";
        cmd.icon = "edit";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "bookmark.goTo";
        cmd.title = "Go To Bookmark";
        cmd.category = "Bookmark";
        cmd.description = "Jump to a specific bookmark by label";
        cmd.icon = "go-to-file";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
