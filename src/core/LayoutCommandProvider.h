/// @file LayoutCommandProvider.h
/// @brief V9 Phase 46 — 8 layout commands for the command palette.
#pragma once

#include "CommandRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provides 8 layout-related commands.
class LayoutCommandProvider
{
public:
    LayoutCommandProvider() = default;

    void register_commands(CommandRegistry& registry) const;
    static auto command_count() -> int;
    static auto command_ids() -> std::vector<std::string>;
    [[nodiscard]] auto get_command(const std::string& command_id) const -> CommandEntry;

private:
    static auto build_entries() -> std::vector<CommandEntry>;
};

} // namespace markamp::core
