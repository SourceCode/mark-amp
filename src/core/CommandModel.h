/// @file CommandModel.h
/// @brief V20 P06-T01: Canonical command model for all creation and lifecycle commands.
///
/// Consolidates creation and lifecycle commands into one model. Replaces
/// scattered command registrations across MainFrame, palette, and menus
/// with centralized command descriptors and enablement rules.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Command category for organization and discoverability.
enum class CommandCategory
{
    kFile,       ///< File operations (new, open, save, close)
    kEdit,       ///< Edit operations (undo, redo, cut, copy, paste)
    kView,       ///< View operations (toggle panels, zoom)
    kCanvas,     ///< Canvas-specific operations
    kNotebook,   ///< Notebook-specific operations
    kNavigation, ///< Navigation operations
    kHelp        ///< Help operations
};

/// Describes a canonical command.
struct CommandDescriptor
{
    std::string id;                          ///< e.g. "workbench.action.files.newFile"
    std::string label;                       ///< Display label
    CommandCategory category{CommandCategory::kFile};
    std::optional<std::string> shortcut;     ///< Keyboard shortcut
    std::optional<std::string> icon;         ///< Icon identifier
    std::optional<std::string> when_clause;  ///< Context key enablement expression
    bool is_enabled{true};

    [[nodiscard]] auto has_shortcut() const noexcept -> bool { return shortcut.has_value(); }
    [[nodiscard]] auto has_when_clause() const noexcept -> bool { return when_clause.has_value(); }
};

/// Result of command execution.
struct CommandResult
{
    bool success{false};
    std::string command_id;
    std::string source;                      ///< menu, palette, shortcut, context-menu
    std::optional<ArtifactId> target_artifact;
    std::string error_message;

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// Command handler function type.
using CommandHandler = std::function<CommandResult(const std::string& source)>;

/// Centralized command model and registry.
class CommandModel
{
public:
    explicit CommandModel(EventBus& bus);

    /// Register a canonical command.
    void register_command(const CommandDescriptor& descriptor, CommandHandler handler);

    /// Execute a command by ID from a given source.
    [[nodiscard]] auto execute(const std::string& command_id,
                                const std::string& source = "unknown") -> CommandResult;

    /// Look up a command descriptor.
    [[nodiscard]] auto find_command(const std::string& command_id) const
        -> const CommandDescriptor*;

    /// List all registered commands.
    [[nodiscard]] auto all_commands() const -> std::vector<CommandDescriptor>;

    /// List commands by category.
    [[nodiscard]] auto commands_by_category(CommandCategory category) const
        -> std::vector<CommandDescriptor>;

    /// Check if a command is enabled.
    [[nodiscard]] auto is_enabled(const std::string& command_id) const -> bool;

    /// Total registered commands.
    [[nodiscard]] auto command_count() const noexcept -> int
    {
        return static_cast<int>(commands_.size());
    }

    /// Total executions.
    [[nodiscard]] auto execution_count() const noexcept -> int { return execution_count_; }

private:
    struct CommandEntry
    {
        CommandDescriptor descriptor;
        CommandHandler handler;
    };

    EventBus& event_bus_;
    std::unordered_map<std::string, CommandEntry> commands_;
    int execution_count_{0};
};

} // namespace markamp::core
