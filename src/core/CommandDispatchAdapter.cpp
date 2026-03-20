/// @file CommandDispatchAdapter.cpp
/// @brief V19 P02: Unified command dispatch adapter implementation.

#include "CommandDispatchAdapter.h"

#include "Logger.h"

namespace markamp::core
{

CommandDispatchAdapter::CommandDispatchAdapter(CommandRegistry& registry,
                                               CommandFeedbackHelper& feedback,
                                               EventBus& event_bus)
    : registry_(registry)
    , feedback_(feedback)
    , event_bus_(event_bus)
{
    MARKAMP_LOG_DEBUG("CommandDispatchAdapter initialized");
}

auto CommandDispatchAdapter::dispatch(const std::string& command_id,
                                      CommandSource source,
                                      const std::string& context_info) -> CommandResult
{
    const auto start = std::chrono::steady_clock::now();
    ++dispatch_count_;

    // Check if command exists
    const auto* entry = registry_.get_command(command_id);
    if (entry == nullptr)
    {
        ++error_count_;
        const std::string msg = "Command not found: " + command_id;
        feedback_.report_error(command_id, msg);
        record_dispatch(command_id, source, CommandResult::kError, msg, 0);
        MARKAMP_LOG_WARN("Dispatch failed: {} (source: {})", command_id, static_cast<int>(source));
        return CommandResult::kError;
    }

    // Execute the command
    const bool success = registry_.execute_command(command_id, source);

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed =
        static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    CommandResult result_val = success ? CommandResult::kSuccess : CommandResult::kError;

    if (success)
    {
        std::string msg = context_info.empty() ? "OK" : context_info;
        feedback_.report_success(command_id, msg);
        record_dispatch(command_id, source, CommandResult::kSuccess, msg, elapsed);

        MARKAMP_LOG_DEBUG("Dispatched: {} (source: {}, {}µs)",
                          command_id,
                          static_cast<int>(source),
                          elapsed);
    }
    else
    {
        ++error_count_;
        const std::string msg = "Execution failed";
        feedback_.report_error(command_id, msg);
        record_dispatch(command_id, source, CommandResult::kError, msg, elapsed);

        MARKAMP_LOG_WARN("Dispatch error: {} (source: {}, {}µs)",
                          command_id,
                          static_cast<int>(source),
                          elapsed);
    }

    return result_val;
}

auto CommandDispatchAdapter::dispatch_with_context(const std::string& command_id,
                                                    CommandSource source,
                                                    const ContextKeyService& context)
    -> CommandResult
{
    // Check when-clause before dispatching
    const auto* entry = registry_.get_command(command_id);
    if (entry == nullptr)
    {
        return dispatch(command_id, source);
    }

    if (!entry->is_available(context))
    {
        ++dispatch_count_;
        const std::string msg = "Command disabled: when-clause not satisfied";
        feedback_.report_disabled(command_id, msg);
        record_dispatch(command_id, source, CommandResult::kDisabled, msg, 0);

        MARKAMP_LOG_DEBUG("Dispatch skipped (when-clause): {} (source: {})",
                          command_id,
                          static_cast<int>(source));
        return CommandResult::kDisabled;
    }

    return dispatch(command_id, source);
}

auto CommandDispatchAdapter::has_command(const std::string& command_id) const -> bool
{
    return registry_.has_command(command_id);
}

auto CommandDispatchAdapter::recent_dispatches(int count) const
    -> std::vector<CommandDispatchRecord>
{
    if (count <= 0 || history_.empty())
    {
        return {};
    }

    const auto start_idx =
        static_cast<int>(history_.size()) > count
            ? static_cast<int>(history_.size()) - count
            : 0;

    return {history_.begin() + start_idx, history_.end()};
}

void CommandDispatchAdapter::record_dispatch(const std::string& command_id,
                                              CommandSource source,
                                              CommandResult result_val,
                                              const std::string& message,
                                              int elapsed_us)
{
    CommandDispatchRecord record;
    record.command_id = command_id;
    record.source = source;
    record.result = result_val;
    record.message = message;
    record.timestamp = std::chrono::steady_clock::now();
    record.elapsed_us = elapsed_us;

    if (static_cast<int>(history_.size()) >= kMaxHistory)
    {
        history_.erase(history_.begin());
    }
    history_.push_back(std::move(record));
}

} // namespace markamp::core
