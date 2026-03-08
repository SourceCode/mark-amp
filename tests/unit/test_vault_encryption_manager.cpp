// test_vault_encryption_manager.cpp — 10 tests for VaultEncryptionManager
#include "core/VaultEncryptionManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("VaultEncryptionConfig defaults", "[vault][encryption]")
{
    VaultEncryptionConfig config;
    CHECK(config.encryption_algorithm == "AES-256-GCM");
    CHECK(config.key_derivation_method == "PBKDF2");
    CHECK(config.iteration_count == 100000);
    CHECK(config.auto_lock_minutes == 15);
    CHECK(config.require_on_export);
}

TEST_CASE("VaultEncryptionInfo defaults", "[vault][encryption]")
{
    VaultEncryptionInfo info;
    CHECK(info.vault_id.empty());
    CHECK(info.state == VaultEncryptionState::kUnlocked);
    CHECK_FALSE(info.is_encrypted);
    CHECK(info.encrypted_note_count == 0);
}

TEST_CASE("VaultEncryptionState enum values", "[vault][encryption]")
{
    CHECK(VaultEncryptionState::kUnlocked != VaultEncryptionState::kLocked);
    CHECK(VaultEncryptionState::kEncrypting != VaultEncryptionState::kDecrypting);
}

TEST_CASE("VaultEncryptionManager starts empty", "[vault][encryption]")
{
    VaultEncryptionManager manager;
    CHECK(manager.vault_count() == 0);
    CHECK(manager.encrypted_vault_count() == 0);
}

TEST_CASE("VaultEncryptionManager encrypt_vault creates vault", "[vault][encryption]")
{
    VaultEncryptionManager manager;
    CHECK(manager.encrypt_vault("vault-1", "password123"));
    CHECK(manager.vault_count() >= 1);
    CHECK(manager.is_vault_encrypted("vault-1"));
}

TEST_CASE("VaultEncryptionManager lock and unlock vault", "[vault][encryption]")
{
    VaultEncryptionManager manager;
    manager.encrypt_vault("vault-1", "pass");
    CHECK(manager.lock_vault("vault-1"));
    CHECK(manager.vault_state("vault-1") == VaultEncryptionState::kLocked);
    CHECK(manager.unlock_vault("vault-1", "pass"));
    CHECK(manager.vault_state("vault-1") == VaultEncryptionState::kUnlocked);
}

TEST_CASE("VaultEncryptionManager find_vault", "[vault][encryption]")
{
    VaultEncryptionManager manager;
    manager.encrypt_vault("vault-2", "pass");
    auto found = manager.find_vault("vault-2");
    REQUIRE(found != nullptr);
    CHECK(found->vault_id == "vault-2");
}

TEST_CASE("VaultEncryptionManager set_config", "[vault][encryption]")
{
    VaultEncryptionManager manager;
    VaultEncryptionConfig config;
    config.auto_lock_minutes = 30;
    config.iteration_count = 200000;
    CHECK(manager.set_config("vault-3", config));
    auto retrieved = manager.get_config("vault-3");
    CHECK(retrieved.auto_lock_minutes == 30);
}

TEST_CASE("VaultEncryptionManager clear_all", "[vault][encryption]")
{
    VaultEncryptionManager manager;
    manager.encrypt_vault("v1", "p1");
    manager.encrypt_vault("v2", "p2");
    manager.clear_all();
    CHECK(manager.vault_count() == 0);
}

TEST_CASE("VaultEncryptionManager locked_vault_count", "[vault][encryption]")
{
    VaultEncryptionManager manager;
    manager.encrypt_vault("v1", "p1");
    manager.encrypt_vault("v2", "p2");
    manager.lock_vault("v1");
    CHECK(manager.locked_vault_count() >= 1);
}
