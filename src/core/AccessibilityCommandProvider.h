// ============================================================================
// File: src/core/AccessibilityCommandProvider.h
// Phase 31: Accessibility System — Command palette commands
// ============================================================================
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// An accessibility command for the command palette.
struct AccessibilityCommand
{
    std::string id;
    std::string title;
    std::string category;
    std::string keybinding;
    std::string description;
};

/// Provides accessibility commands for the command palette.
///
/// Phase 31 Tasks 10, 19: 8 accessibility commands covering high contrast,
/// reduced motion, audit, zoom, screen reader, and heading navigation.
class AccessibilityCommandProvider
{
public:
    AccessibilityCommandProvider();

    /// Get all available accessibility commands.
    [[nodiscard]] auto commands() const -> const std::vector<AccessibilityCommand>&;

    /// Find a command by its ID.
    [[nodiscard]] auto find_command(const std::string& cmd_id) const
        -> std::optional<AccessibilityCommand>;

    /// Get all commands in a given category.
    [[nodiscard]] auto commands_in_category(const std::string& category) const
        -> std::vector<AccessibilityCommand>;

    /// Get all unique categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get the total number of registered commands.
    [[nodiscard]] auto command_count() const -> std::size_t;

private:
    std::vector<AccessibilityCommand> commands_;
    void register_commands();
};

} // namespace markamp::core
