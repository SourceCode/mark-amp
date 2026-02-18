// ============================================================================
// File: src/core/PerformanceCommandProvider.h
// Phase 30: Performance Optimization — Command palette commands
// ============================================================================
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// A performance command for the command palette.
struct PerformanceCommand
{
    std::string id;
    std::string title;
    std::string category;
    std::string keybinding;
    std::string description;
};

/// PerformanceCommandProvider — registers and manages performance-related commands.
///
/// Phase 30 Task 18: 7 performance commands.
class PerformanceCommandProvider
{
public:
    PerformanceCommandProvider();

    /// Get all registered commands.
    [[nodiscard]] auto commands() const -> const std::vector<PerformanceCommand>&;

    /// Find a command by its ID.
    [[nodiscard]] auto find_command(const std::string& command_id) const
        -> std::optional<PerformanceCommand>;

    /// Get commands in a specific category.
    [[nodiscard]] auto commands_in_category(const std::string& category) const
        -> std::vector<PerformanceCommand>;

    /// Get all unique categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get command count.
    [[nodiscard]] auto command_count() const -> int32_t;

    /// Get all available commands.
    [[nodiscard]] auto available_commands() const -> std::vector<PerformanceCommand>;

private:
    std::vector<PerformanceCommand> commands_;

    /// Register all performance commands.
    auto register_commands() -> void;
};

} // namespace markamp::core
