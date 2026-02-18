/// @file AssetCommandProvider.h
/// @brief V9 Phase 37 — Asset commands for command palette.
#pragma once

#include "CommandRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Provides 8 asset-related commands for the command palette.
///
/// Commands:
///   - asset.import — Import Asset (Cmd+Shift+I)
///   - asset.findOrphans — Find Orphaned Assets
///   - asset.optimize — Optimize Images
///   - asset.storageReport — Show Storage Report
///   - asset.deduplicate — Deduplicate Assets
///   - asset.rename — Rename Asset
///   - asset.delete — Delete Asset
///   - asset.checkLinks — Check Link Integrity
class AssetCommandProvider
{
public:
    AssetCommandProvider() = default;

    /// Register all asset commands in the given registry.
    void register_commands(CommandRegistry& registry) const;

    /// Get the list of command IDs provided.
    [[nodiscard]] static auto command_ids() -> std::vector<std::string>;

    /// Get the command entry for a specific command.
    [[nodiscard]] auto get_command(const std::string& command_id) const -> CommandEntry;

    /// Total number of commands provided.
    [[nodiscard]] static auto command_count() -> int;

private:
    /// Build all command entries.
    [[nodiscard]] auto build_entries() const -> std::vector<CommandEntry>;
};

} // namespace markamp::core
