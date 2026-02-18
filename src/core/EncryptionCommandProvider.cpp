/// @file EncryptionCommandProvider.cpp
/// @brief V9 Phase 44 — EncryptionCommandProvider implementation.

#include "EncryptionCommandProvider.h"

namespace markamp::core
{

void EncryptionCommandProvider::register_commands(CommandRegistry& registry) const
{
    registry.register_commands(build_entries());
}

auto EncryptionCommandProvider::command_ids() -> std::vector<std::string>
{
    return {
        "encrypt.vault",
        "encrypt.decryptVault",
        "encrypt.lock",
        "encrypt.unlock",
        "encrypt.rotateKeys",
        "encrypt.changePassword",
        "encrypt.keyStatus",
        "encrypt.exportBackup",
    };
}

auto EncryptionCommandProvider::get_command(const std::string& command_id) const -> CommandEntry
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

auto EncryptionCommandProvider::command_count() -> int
{
    return 8;
}

auto EncryptionCommandProvider::build_entries() -> std::vector<CommandEntry>
{
    std::vector<CommandEntry> entries;
    entries.reserve(8);

    {
        CommandEntry cmd;
        cmd.id = "encrypt.vault";
        cmd.title = "Encrypt Vault";
        cmd.category = "Encryption";
        cmd.description = "Encrypt all notes in the current vault";
        cmd.icon = "lock";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "encrypt.decryptVault";
        cmd.title = "Decrypt Vault";
        cmd.category = "Encryption";
        cmd.description = "Remove encryption from the current vault";
        cmd.icon = "unlock";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "encrypt.lock";
        cmd.title = "Lock Vault";
        cmd.category = "Encryption";
        cmd.description = "Lock the vault requiring password to access";
        cmd.icon = "shield-lock";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "encrypt.unlock";
        cmd.title = "Unlock Vault";
        cmd.category = "Encryption";
        cmd.description = "Unlock the vault with password";
        cmd.icon = "shield-unlock";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "encrypt.rotateKeys";
        cmd.title = "Rotate Keys";
        cmd.category = "Encryption";
        cmd.description = "Rotate encryption keys for enhanced security";
        cmd.icon = "refresh";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "encrypt.changePassword";
        cmd.title = "Change Password";
        cmd.category = "Encryption";
        cmd.description = "Change the vault encryption password";
        cmd.icon = "key";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "encrypt.keyStatus";
        cmd.title = "View Key Status";
        cmd.category = "Encryption";
        cmd.description = "View encryption key status and rotation info";
        cmd.icon = "info";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    {
        CommandEntry cmd;
        cmd.id = "encrypt.exportBackup";
        cmd.title = "Export Encrypted Backup";
        cmd.category = "Encryption";
        cmd.description = "Export an encrypted backup of the vault";
        cmd.icon = "download";
        cmd.execute_fn = []() { return true; };
        entries.push_back(std::move(cmd));
    }

    return entries;
}

} // namespace markamp::core
