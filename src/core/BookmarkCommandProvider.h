/// @file BookmarkCommandProvider.h
/// @brief V9 Phase 37 — Bookmark commands for command palette.
#pragma once

#include "CommandRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provides 8 bookmark-related commands for the command palette.
///
/// Commands:
///   - bookmark.toggle — Toggle Bookmark (Cmd+Shift+B)
///   - bookmark.next — Next Bookmark (F2)
///   - bookmark.previous — Previous Bookmark (Shift+F2)
///   - bookmark.list — List Bookmarks
///   - bookmark.removeAll — Remove All Bookmarks
///   - bookmark.export — Export Bookmarks
///   - bookmark.renameLabel — Rename Bookmark Label
///   - bookmark.goTo — Go To Bookmark
class BookmarkCommandProvider
{
public:
    BookmarkCommandProvider() = default;

    /// Register all bookmark commands in the given registry.
    void register_commands(CommandRegistry& registry) const;

    /// Get the list of command IDs provided.
    [[nodiscard]] static auto command_ids() -> std::vector<std::string>;

    /// Get the command entry for a specific command.
    [[nodiscard]] auto get_command(const std::string& command_id) const -> CommandEntry;

    /// Total number of commands provided.
    [[nodiscard]] static auto command_count() -> int;

private:
    /// Build all command entries.
    [[nodiscard]] auto build_entries() const -> std::vector<CommandEntry>;
};

} // namespace markamp::core
