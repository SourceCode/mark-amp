// ============================================================================
// File: src/core/SyncCommandProvider.h
// Phase 27: Cloud Sync & Collaboration — Command palette commands
// ============================================================================
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// A sync command for the command palette.
struct SyncCommand
{
    std::string id;
    std::string title;
    std::string category;
    std::string keybinding;
    std::string description;
    bool requires_sync_configured{false}; // Disabled when no sync provider configured
};

// SyncCommandProvider — registers and manages sync-related commands.
class SyncCommandProvider
{
public:
    SyncCommandProvider();

    // Get all registered commands.
    [[nodiscard]] auto commands() const -> const std::vector<SyncCommand>&;

    // Find a command by its ID.
    [[nodiscard]] auto find_command(const std::string& command_id) const
        -> std::optional<SyncCommand>;

    // Get commands in a specific category.
    [[nodiscard]] auto commands_in_category(const std::string& category) const
        -> std::vector<SyncCommand>;

    // Get all unique categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    // Get command count.
    [[nodiscard]] auto command_count() const -> int32_t;

    // Filter commands based on sync configuration status.
    [[nodiscard]] auto available_commands(bool sync_configured) const -> std::vector<SyncCommand>;

private:
    std::vector<SyncCommand> commands_;

    // Register all sync commands.
    auto register_commands() -> void;
};

} // namespace markamp::core
