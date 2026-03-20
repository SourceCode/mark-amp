/// @file CommandFeedback.h
/// @brief P02-T05: Standardized command execution feedback.
///
/// Provides a helper class for consistent user feedback when commands
/// succeed, no-op, fail, or are disabled.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;

/// Result classification for command execution.
enum class CommandResult
{
    kSuccess,  ///< Command completed successfully
    kNoOp,     ///< Command had nothing to do (e.g., save with no changes)
    kError,    ///< Command failed with an error
    kDisabled, ///< Command was disabled due to unmet preconditions
};

/// Helper for consistent command feedback across all execution surfaces.
///
/// Usage:
///   CommandFeedbackHelper feedback(event_bus);
///   feedback.report_success("file.save", "File saved");
///   feedback.report_disabled("edit.bold", "No editor is active");
class CommandFeedbackHelper
{
public:
    explicit CommandFeedbackHelper(EventBus& event_bus);

    /// Report successful command execution — status bar flash message.
    void report_success(const std::string& command_id, const std::string& message);

    /// Report a no-op — lightweight status bar message for routine cases.
    void report_no_op(const std::string& command_id, const std::string& reason);

    /// Report a command error — notification toast with error level.
    void report_error(const std::string& command_id, const std::string& error);

    /// Report a disabled command — tooltip/palette hint with disabled reason.
    void report_disabled(const std::string& command_id, const std::string& reason);

    /// Report based on result enum.
    void report(const std::string& command_id, CommandResult result, const std::string& message);

private:
    EventBus& event_bus_;
};

} // namespace markamp::core
