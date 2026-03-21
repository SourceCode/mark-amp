/// @file CommandModel.cpp
/// @brief V20 P06-T01: Canonical command model implementation.

#include "CommandModel.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

CommandModel::CommandModel(EventBus& bus)
    : event_bus_(bus)
{
}

void CommandModel::register_command(const CommandDescriptor& descriptor, CommandHandler handler)
{
    CommandEntry entry;
    entry.descriptor = descriptor;
    entry.handler = std::move(handler);
    commands_[descriptor.id] = std::move(entry);

    MARKAMP_LOG_DEBUG("Command registered: {}", descriptor.id);
}

auto CommandModel::execute(const std::string& command_id, const std::string& source)
    -> CommandResult
{
    ++execution_count_;

    auto it = commands_.find(command_id);
    if (it == commands_.end())
    {
        CommandResult result;
        result.command_id = command_id;
        result.source = source;
        result.error_message = "Command not found: " + command_id;
        MARKAMP_LOG_WARN("Command not found: {} (source={})", command_id, source);
        return result;
    }

    if (!it->second.descriptor.is_enabled)
    {
        CommandResult result;
        result.command_id = command_id;
        result.source = source;
        result.error_message = "Command disabled: " + command_id;
        return result;
    }

    auto result = it->second.handler(source);
    result.command_id = command_id;
    result.source = source;

    events::CanonicalCommandExecutedEvent evt;
    evt.command_id = command_id;
    evt.source = source;
    evt.success = result.ok();
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Command executed: {} (source={}, ok={})", command_id, source, result.ok());
    return result;
}

auto CommandModel::find_command(const std::string& command_id) const -> const CommandDescriptor*
{
    auto it = commands_.find(command_id);
    if (it == commands_.end())
    {
        return nullptr;
    }
    return &it->second.descriptor;
}

auto CommandModel::all_commands() const -> std::vector<CommandDescriptor>
{
    std::vector<CommandDescriptor> result;
    result.reserve(commands_.size());
    for (const auto& [id, entry] : commands_)
    {
        result.push_back(entry.descriptor);
    }
    return result;
}

auto CommandModel::commands_by_category(CommandCategory category) const
    -> std::vector<CommandDescriptor>
{
    std::vector<CommandDescriptor> result;
    for (const auto& [id, entry] : commands_)
    {
        if (entry.descriptor.category == category)
        {
            result.push_back(entry.descriptor);
        }
    }
    return result;
}

auto CommandModel::is_enabled(const std::string& command_id) const -> bool
{
    auto it = commands_.find(command_id);
    if (it == commands_.end())
    {
        return false;
    }
    return it->second.descriptor.is_enabled;
}

} // namespace markamp::core
