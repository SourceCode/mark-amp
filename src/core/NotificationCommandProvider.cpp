/// @file NotificationCommandProvider.cpp
/// @brief V9 Phase 39 — NotificationCommandProvider implementation.

#include "NotificationCommandProvider.h"

namespace markamp::core
{

void NotificationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto NotificationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "notifications.showCenter",
        "notifications.markAllRead",
        "notifications.dismissAll",
        "notifications.filterRules",
        "notifications.clearHistory",
        "notifications.toggleDoNotDisturb",
        "notifications.showByPriority",
        "notifications.archiveAll",
    };
}

auto NotificationCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
{
    for (auto& entry : build_entries())
    {
        if (entry.id == command_id)
        {
            return entry;
        }
    }
    return {};
}

auto NotificationCommandProvider::command_count() -> int
{
    return 8;
}

auto NotificationCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "notifications.showCenter";
        cmd.title = "Show Notification Center";
        cmd.category = "Notifications";
        cmd.description = "Open the notification center panel";
        cmd.shortcut = "Cmd+Shift+N";
        cmd.icon = "bell";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "notifications.markAllRead";
        cmd.title = "Mark All as Read";
        cmd.category = "Notifications";
        cmd.description = "Mark all notifications as read";
        cmd.icon = "check-all";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "notifications.dismissAll";
        cmd.title = "Dismiss All Notifications";
        cmd.category = "Notifications";
        cmd.description = "Dismiss all active notifications";
        cmd.icon = "close-all";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "notifications.filterRules";
        cmd.title = "Notification Filter Rules";
        cmd.category = "Notifications";
        cmd.description = "Manage notification filter rules";
        cmd.icon = "filter";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "notifications.clearHistory";
        cmd.title = "Clear Notification History";
        cmd.category = "Notifications";
        cmd.description = "Clear all dismissed notifications from history";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "notifications.toggleDoNotDisturb";
        cmd.title = "Toggle Do Not Disturb";
        cmd.category = "Notifications";
        cmd.description = "Toggle do-not-disturb mode for notifications";
        cmd.icon = "bell-slash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "notifications.showByPriority";
        cmd.title = "Show by Priority";
        cmd.category = "Notifications";
        cmd.description = "Filter notifications to show only high priority";
        cmd.icon = "warning";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "notifications.archiveAll";
        cmd.title = "Archive All";
        cmd.category = "Notifications";
        cmd.description = "Archive all current notifications";
        cmd.icon = "archive";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
