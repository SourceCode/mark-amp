// ============================================================================
// File: src/core/SecurityCommandProvider.h
// Phase 29: Security & Input Validation — Command palette commands
// ============================================================================
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// A security command for the command palette.
struct SecurityCommand
{
    std::string id;
    std::string title;
    std::string category;
    std::string keybinding;
    std::string description;
};

/// SecurityCommandProvider — registers and manages security-related commands.
class SecurityCommandProvider
{
public:
    SecurityCommandProvider();

    /// Get all registered commands.
    [[nodiscard]] auto commands() const -> const std::vector<SecurityCommand>&;

    /// Find a command by its ID.
    [[nodiscard]] auto find_command(const std::string& command_id) const
        -> std::optional<SecurityCommand>;

    /// Get commands in a specific category.
    [[nodiscard]] auto commands_in_category(const std::string& category) const
        -> std::vector<SecurityCommand>;

    /// Get all unique categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get command count.
    [[nodiscard]] auto command_count() const -> int32_t;

    /// Get all available commands.
    [[nodiscard]] auto available_commands() const -> std::vector<SecurityCommand>;

private:
    std::vector<SecurityCommand> commands_;

    /// Register all security commands.
    auto register_commands() -> void;
};

} // namespace markamp::core
