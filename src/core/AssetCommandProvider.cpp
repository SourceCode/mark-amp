/// @file AssetCommandProvider.cpp
/// @brief V9 Phase 37 — AssetCommandProvider implementation.

#include "AssetCommandProvider.h"

namespace markamp::core
{

void AssetCommandProvider::register_commands(CommandRegistry& registry) const
{
    auto entries = build_entries();
    registry.register_commands(std::move(entries));
}

auto AssetCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "asset.import",
        "asset.findOrphans",
        "asset.optimize",
        "asset.storageReport",
        "asset.deduplicate",
        "asset.rename",
        "asset.delete",
        "asset.checkLinks",
    };
}

auto AssetCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
{
    auto entries = build_entries();
    for (auto& entry : entries)
    {
        if (entry.id == command_id)
        {
            return entry;
        }
    }
    return {};
}

auto AssetCommandProvider::command_count() -> int
{
    return 8;
}

auto AssetCommandProvider::build_entries() const -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "asset.import";
        cmd.title = "Import Asset";
        cmd.category = "Asset";
        cmd.description = "Import a file as a managed asset";
        cmd.shortcut = "Cmd+Shift+I";
        cmd.icon = "cloud-upload";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "asset.findOrphans";
        cmd.title = "Find Orphaned Assets";
        cmd.category = "Asset";
        cmd.description = "Find assets not referenced by any document";
        cmd.icon = "search";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "asset.optimize";
        cmd.title = "Optimize Images";
        cmd.category = "Asset";
        cmd.description = "Optimize image assets to reduce storage";
        cmd.icon = "zap";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "asset.storageReport";
        cmd.title = "Show Storage Report";
        cmd.category = "Asset";
        cmd.description = "Show asset storage usage and optimization opportunities";
        cmd.icon = "graph";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "asset.deduplicate";
        cmd.title = "Deduplicate Assets";
        cmd.category = "Asset";
        cmd.description = "Find and merge duplicate asset files";
        cmd.icon = "files";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "asset.rename";
        cmd.title = "Rename Asset";
        cmd.category = "Asset";
        cmd.description = "Rename an asset and update all references";
        cmd.icon = "edit";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "asset.delete";
        cmd.title = "Delete Asset";
        cmd.category = "Asset";
        cmd.description = "Delete an asset from the workspace";
        cmd.icon = "trash";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }
    {
        CommandEntry cmd;
        cmd.id = "asset.checkLinks";
        cmd.title = "Check Link Integrity";
        cmd.category = "Asset";
        cmd.description = "Scan documents for broken asset links";
        cmd.icon = "link";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
