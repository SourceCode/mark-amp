/// @file CommandDispatchAdapter.h
/// @brief V19 P02: Unified command dispatch adapter.
///
/// Wraps CommandRegistry to provide a single dispatch surface that
/// automatically logs the invocation source (palette, shortcut, menu, extension,
/// toolbar, context-menu), records timing, and routes feedback through
/// CommandFeedbackHelper. This replaces scattered direct invocations that
/// bypass usage tracking and feedback.
#pragma once

#include "CommandFeedback.h"
#include "CommandRegistry.h"
#include "EventBus.h"

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

/// Execution log entry for debugging and telemetry.
struct CommandDispatchRecord
{
    std::string command_id;
    CommandSource source{CommandSource::kInternal};
    CommandResult result{CommandResult::kSuccess};
    std::string message;
    std::chrono::steady_clock::time_point timestamp;
    int elapsed_us{0}; ///< Execution duration in microseconds
};

/// Unified command dispatch adapter.
///
/// Centralizes all command execution through CommandRegistry with automatic
/// source tracking, feedback reporting, and execution logging.
///
/// Example:
/// ```cpp
/// CommandDispatchAdapter adapter(registry, feedback, event_bus);
/// adapter.dispatch("file.save", CommandSource::kShortcut);
/// adapter.dispatch("editor.formatDocument", CommandSource::kPalette);
/// ```
class CommandDispatchAdapter
{
public:
    CommandDispatchAdapter(CommandRegistry& registry,
                          CommandFeedbackHelper& feedback,
                          EventBus& event_bus);

    /// Execute a command with source tracking and feedback.
    /// Returns the CommandResult of the execution.
    auto dispatch(const std::string& command_id,
                  CommandSource source = CommandSource::kInternal,
                  const std::string& context_info = "") -> CommandResult;

    /// Execute a command with when-clause context filtering.
    /// Returns kDisabled if the when-clause does not match.
    auto dispatch_with_context(const std::string& command_id,
                               CommandSource source,
                               const ContextKeyService& context) -> CommandResult;

    /// Check whether a command exists in the registry.
    [[nodiscard]] auto has_command(const std::string& command_id) const -> bool;

    /// Get the last N dispatch records for debugging.
    [[nodiscard]] auto recent_dispatches(int count = 20) const
        -> std::vector<CommandDispatchRecord>;

    /// Total dispatch count since creation.
    [[nodiscard]] auto dispatch_count() const -> int { return dispatch_count_; }

    /// Number of failed dispatches.
    [[nodiscard]] auto error_count() const -> int { return error_count_; }

private:
    CommandRegistry& registry_;
    CommandFeedbackHelper& feedback_;
    [[maybe_unused]] EventBus& event_bus_;

    int dispatch_count_{0};
    int error_count_{0};

    static constexpr int kMaxHistory = 100;
    std::vector<CommandDispatchRecord> history_;

    void record_dispatch(const std::string& command_id,
                         CommandSource source,
                         CommandResult result_val,
                         const std::string& message,
                         int elapsed_us);
};

} // namespace markamp::core
