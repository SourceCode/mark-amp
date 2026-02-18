/// @file VaultEncryptionManager.cpp
/// @brief V9 Phase 44 — VaultEncryptionManager implementation.

#include "VaultEncryptionManager.h"

#include <algorithm>

namespace markamp::core
{

auto VaultEncryptionManager::encrypt_vault(const std::string& vault_id, const std::string& password)
    -> bool
{
    if (password.empty())
    {
        return false;
    }
    auto& vault = find_or_create_vault(vault_id);
    if (vault.is_encrypted)
    {
        return false;
    }
    vault.state = VaultEncryptionState::kEncrypting;
    vault.is_encrypted = true;
    vault.encrypted_note_count = 10; // simulated
    vault.state = VaultEncryptionState::kUnlocked;
    vault.last_unlocked = std::chrono::system_clock::now();
    return true;
}

auto VaultEncryptionManager::decrypt_vault(const std::string& vault_id, const std::string& password)
    -> bool
{
    if (password.empty())
    {
        return false;
    }
    auto* vault = find_vault_mut(vault_id);
    if (vault == nullptr || !vault->is_encrypted)
    {
        return false;
    }
    vault->state = VaultEncryptionState::kDecrypting;
    vault->is_encrypted = false;
    vault->encrypted_note_count = 0;
    vault->state = VaultEncryptionState::kUnlocked;
    return true;
}

auto VaultEncryptionManager::lock_vault(const std::string& vault_id) -> bool
{
    auto* vault = find_vault_mut(vault_id);
    if (vault == nullptr || !vault->is_encrypted)
    {
        return false;
    }
    if (vault->state == VaultEncryptionState::kLocked)
    {
        return false;
    }
    vault->state = VaultEncryptionState::kLocked;
    return true;
}

auto VaultEncryptionManager::unlock_vault(const std::string& vault_id, const std::string& password)
    -> bool
{
    if (password.empty())
    {
        return false;
    }
    auto* vault = find_vault_mut(vault_id);
    if (vault == nullptr || vault->state != VaultEncryptionState::kLocked)
    {
        return false;
    }
    vault->state = VaultEncryptionState::kUnlocked;
    vault->last_unlocked = std::chrono::system_clock::now();
    return true;
}

auto VaultEncryptionManager::vault_state(const std::string& vault_id) const -> VaultEncryptionState
{
    const auto* vault = find_vault(vault_id);
    if (vault == nullptr)
    {
        return VaultEncryptionState::kUnlocked;
    }
    return vault->state;
}

auto VaultEncryptionManager::is_vault_encrypted(const std::string& vault_id) const -> bool
{
    const auto* vault = find_vault(vault_id);
    return vault != nullptr && vault->is_encrypted;
}

auto VaultEncryptionManager::find_vault(const std::string& vault_id) const
    -> const VaultEncryptionInfo*
{
    for (const auto& vault : vaults_)
    {
        if (vault.vault_id == vault_id)
        {
            return &vault;
        }
    }
    return nullptr;
}

auto VaultEncryptionManager::set_config(const std::string& vault_id, VaultEncryptionConfig config)
    -> bool
{
    auto& vault = find_or_create_vault(vault_id);
    vault.config = config;
    return true;
}

auto VaultEncryptionManager::get_config(const std::string& vault_id) const -> VaultEncryptionConfig
{
    const auto* vault = find_vault(vault_id);
    if (vault == nullptr)
    {
        return {};
    }
    return vault->config;
}

auto VaultEncryptionManager::encrypted_vault_count() const -> int
{
    return static_cast<int>(std::count_if(vaults_.begin(),
                                          vaults_.end(),
                                          [](const VaultEncryptionInfo& info)
                                          { return info.is_encrypted; }));
}

auto VaultEncryptionManager::locked_vault_count() const -> int
{
    return static_cast<int>(std::count_if(vaults_.begin(),
                                          vaults_.end(),
                                          [](const VaultEncryptionInfo& info)
                                          { return info.state == VaultEncryptionState::kLocked; }));
}

auto VaultEncryptionManager::vault_count() const -> int
{
    return static_cast<int>(vaults_.size());
}

void VaultEncryptionManager::clear_all()
{
    vaults_.clear();
}

auto VaultEncryptionManager::find_vault_mut(const std::string& vault_id) -> VaultEncryptionInfo*
{
    for (auto& vault : vaults_)
    {
        if (vault.vault_id == vault_id)
        {
            return &vault;
        }
    }
    return nullptr;
}

auto VaultEncryptionManager::find_or_create_vault(const std::string& vault_id)
    -> VaultEncryptionInfo&
{
    for (auto& vault : vaults_)
    {
        if (vault.vault_id == vault_id)
        {
            return vault;
        }
    }
    VaultEncryptionInfo info;
    info.vault_id = vault_id;
    vaults_.push_back(std::move(info));
    return vaults_.back();
}

} // namespace markamp::core
