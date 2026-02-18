// ============================================================================
// File: src/rendering/FxCommandProvider.h
// Phase 28: FX Visual Effects System — Command palette integration
// ============================================================================
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::rendering
{

/// An FX command for the command palette.
struct FxCommand
{
    std::string id;
    std::string title;
    std::string category;
    std::string keybinding;
    std::string description;
    bool requires_fx_enabled{false}; ///< Disabled when FX master is off
};

/// Registers and manages FX-related commands for the command palette.
class FxCommandProvider
{
public:
    FxCommandProvider();

    /// Get all registered FX commands.
    [[nodiscard]] auto commands() const -> const std::vector<FxCommand>&;

    /// Find a command by its ID.
    [[nodiscard]] auto find_command(const std::string& command_id) const
        -> std::optional<FxCommand>;

    /// Get commands in a specific category.
    [[nodiscard]] auto commands_in_category(const std::string& category) const
        -> std::vector<FxCommand>;

    /// Get all unique categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Get command count.
    [[nodiscard]] auto command_count() const -> int32_t;

    /// Filter commands based on FX master enabled status.
    [[nodiscard]] auto available_commands(bool fx_enabled) const -> std::vector<FxCommand>;

private:
    std::vector<FxCommand> commands_;

    /// Register all FX commands.
    auto register_commands() -> void;
};

} // namespace markamp::rendering
