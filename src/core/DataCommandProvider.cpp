/// @file DataCommandProvider.cpp
/// @brief V9 Phase 48 — DataCommandProvider implementation.

#include "DataCommandProvider.h"

namespace markamp::core
{

void DataCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto DataCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "data.createTable",
        "data.deleteTable",
        "data.addColumn",
        "data.sortTable",
        "data.filterTable",
        "data.importCSV",
        "data.exportCSV",
        "data.clearData",
    };
}

auto DataCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto DataCommandProvider::command_count() -> int
{
    return 8;
}

auto DataCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    auto make = [&](const std::string& cmd_id,
                    const std::string& cmd_title,
                    const std::string& desc,
                    const std::string& icon)
    {
        CommandEntry cmd;
        cmd.id = cmd_id;
        cmd.title = cmd_title;
        cmd.category = "Data";
        cmd.description = desc;
        cmd.icon = icon;
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    };

    make("data.createTable", "Create Table", "Create a new data table", "table");
    make("data.deleteTable", "Delete Table", "Delete selected data table", "trash-2");
    make("data.addColumn", "Add Column", "Add column to table", "columns");
    make("data.sortTable", "Sort Table", "Sort table by column", "arrow-up-down");
    make("data.filterTable", "Filter Table", "Filter table rows", "filter");
    make("data.importCSV", "Import CSV", "Import data from CSV file", "upload");
    make("data.exportCSV", "Export CSV", "Export data to CSV file", "download");
    make("data.clearData", "Clear Data", "Clear all table data", "eraser");

    return entries;
}

} // namespace markamp::core
