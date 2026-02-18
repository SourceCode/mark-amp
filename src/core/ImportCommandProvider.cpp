/// @file ImportCommandProvider.cpp
/// @brief V9 Phase 42 — ImportCommandProvider implementation.

#include "ImportCommandProvider.h"

namespace markamp::core
{

void ImportCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto ImportCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "import.file",
        "import.batchImport",
        "import.showHistory",
        "import.clearHistory",
        "import.showPresets",
        "import.createPreset",
        "import.detectFormat",
        "import.cancelImport",
    };
}

auto ImportCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto ImportCommandProvider::command_count() -> int
{
    return 8;
}

auto ImportCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "import.file";
        cmd.title = "Import Document";
        cmd.category = "Import";
        cmd.description = "Import a document and convert to Markdown";
        cmd.icon = "import";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "import.batchImport";
        cmd.title = "Batch Import";
        cmd.category = "Import";
        cmd.description = "Import multiple documents at once";
        cmd.icon = "files";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "import.showHistory";
        cmd.title = "Show Import History";
        cmd.category = "Import";
        cmd.description = "View past import operations";
        cmd.icon = "history";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "import.clearHistory";
        cmd.title = "Clear Import History";
        cmd.category = "Import";
        cmd.description = "Clear all import history records";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "import.showPresets";
        cmd.title = "Show Import Presets";
        cmd.category = "Import";
        cmd.description = "View saved import presets";
        cmd.icon = "list";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "import.createPreset";
        cmd.title = "Create Import Preset";
        cmd.category = "Import";
        cmd.description = "Save current import settings as a preset";
        cmd.icon = "save";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "import.detectFormat";
        cmd.title = "Detect Document Format";
        cmd.category = "Import";
        cmd.description = "Auto-detect the format of a document";
        cmd.icon = "search";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "import.cancelImport";
        cmd.title = "Cancel Import";
        cmd.category = "Import";
        cmd.description = "Cancel an in-progress import";
        cmd.icon = "cancel";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
