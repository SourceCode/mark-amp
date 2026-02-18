/// @file VaultEncryptionManager.h
/// @brief V9 Phase 44 — Vault-level encryption with lock/unlock and auto-lock.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Vault encryption state.
enum class VaultEncryptionState : uint8_t
{
    kUnlocked = 0,
    kLocked = 1,
    kEncrypting = 2,
    kDecrypting = 3,
};

/// Per-vault encryption configuration.
struct VaultEncryptionConfig
{
    std::string encryption_algorithm{"AES-256-GCM"};
    std::string key_derivation_method{"PBKDF2"};
    int iteration_count{100000};
    int auto_lock_minutes{15};
    bool require_on_export{true};
};

/// Tracked vault encryption info.
struct VaultEncryptionInfo
{
    std::string vault_id;
    VaultEncryptionState state{VaultEncryptionState::kUnlocked};
    VaultEncryptionConfig config;
    bool is_encrypted{false};
    int encrypted_note_count{0};
    std::chrono::system_clock::time_point last_unlocked;
};

/// Manages vault-wide encryption, lock/unlock, and auto-lock.
class VaultEncryptionManager
{
public:
    VaultEncryptionManager() = default;

    // ── Vault operations ──────────────────────────────────────────────
    auto encrypt_vault(const std::string& vault_id, const std::string& password) -> bool;
    auto decrypt_vault(const std::string& vault_id, const std::string& password) -> bool;
    auto lock_vault(const std::string& vault_id) -> bool;
    auto unlock_vault(const std::string& vault_id, const std::string& password) -> bool;

    // ── State queries ─────────────────────────────────────────────────
    [[nodiscard]] auto vault_state(const std::string& vault_id) const -> VaultEncryptionState;
    [[nodiscard]] auto is_vault_encrypted(const std::string& vault_id) const -> bool;
    [[nodiscard]] auto find_vault(const std::string& vault_id) const -> const VaultEncryptionInfo*;

    // ── Configuration ─────────────────────────────────────────────────
    auto set_config(const std::string& vault_id, VaultEncryptionConfig config) -> bool;
    [[nodiscard]] auto get_config(const std::string& vault_id) const -> VaultEncryptionConfig;

    // ── Statistics ─────────────────────────────────────────────────────
    [[nodiscard]] auto encrypted_vault_count() const -> int;
    [[nodiscard]] auto locked_vault_count() const -> int;
    [[nodiscard]] auto vault_count() const -> int;
    void clear_all();

private:
    std::vector<VaultEncryptionInfo> vaults_;

    auto find_vault_mut(const std::string& vault_id) -> VaultEncryptionInfo*;
    auto find_or_create_vault(const std::string& vault_id) -> VaultEncryptionInfo&;
};

} // namespace markamp::core
