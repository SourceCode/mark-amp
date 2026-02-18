/// @file NotificationCommandProvider.h
/// @brief V9 Phase 39 — Notification commands for command palette.
#pragma once

#include "CommandRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provides 8 notification-related commands for the command palette.
class NotificationCommandProvider
{
public:
    NotificationCommandProvider() = default;

    void register_commands(CommandRegistry& registry) const;
    [[nodiscard]] static auto command_ids() -> std::vector<std::string>;
    [[nodiscard]] auto get_command(const std::string& command_id) const -> CommandEntry;
    [[nodiscard]] static auto command_count() -> int;

private:
    [[nodiscard]] static auto build_entries() -> std::vector<CommandEntry>;
};

} // namespace markamp::core
