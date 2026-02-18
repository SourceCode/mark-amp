/// @file AutomationCommandProvider.cpp
/// @brief V9 Phase 40 — AutomationCommandProvider implementation.

#include "AutomationCommandProvider.h"

namespace markamp::core
{

void AutomationCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto AutomationCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "automation.showRules",
        "automation.addRule",
        "automation.removeRule",
        "automation.toggleRule",
        "automation.executeRule",
        "automation.exportRules",
        "automation.importRules",
        "automation.loadDefaults",
    };
}

auto AutomationCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto AutomationCommandProvider::command_count() -> int
{
    return 8;
}

auto AutomationCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "automation.showRules";
        cmd.title = "Show Automation Rules";
        cmd.category = "Automation";
        cmd.description = "View all workspace automation rules";
        cmd.shortcut = "Cmd+Shift+R";
        cmd.icon = "robot";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "automation.addRule";
        cmd.title = "Add Automation Rule";
        cmd.category = "Automation";
        cmd.description = "Create a new automation rule";
        cmd.icon = "add";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "automation.removeRule";
        cmd.title = "Remove Automation Rule";
        cmd.category = "Automation";
        cmd.description = "Delete an existing automation rule";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "automation.toggleRule";
        cmd.title = "Toggle Automation Rule";
        cmd.category = "Automation";
        cmd.description = "Enable or disable an automation rule";
        cmd.icon = "toggle";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "automation.executeRule";
        cmd.title = "Execute Automation Rule";
        cmd.category = "Automation";
        cmd.description = "Manually trigger an automation rule";
        cmd.icon = "play";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "automation.exportRules";
        cmd.title = "Export Automation Rules";
        cmd.category = "Automation";
        cmd.description = "Export all rules as JSON";
        cmd.icon = "export";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "automation.importRules";
        cmd.title = "Import Automation Rules";
        cmd.category = "Automation";
        cmd.description = "Import rules from a JSON file";
        cmd.icon = "import";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "automation.loadDefaults";
        cmd.title = "Load Default Rules";
        cmd.category = "Automation";
        cmd.description = "Reset to default automation rules";
        cmd.icon = "refresh";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
