/// @file test_phase44_encryption_privacy.cpp
/// @brief Comprehensive tests for Phase 44 — Encryption & Privacy.

#include "core/CommandRegistry.h"
#include "core/DataRedactionEngine.h"
#include "core/EncryptionCommandProvider.h"
#include "core/Events.h"
#include "core/KeyManager.h"
#include "core/PrivacyCommandProvider.h"
#include "core/PrivacyManager.h"
#include "core/VaultEncryptionManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// VaultEncryptionManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("VaultEncryptionManager: encrypt and query", "[phase44][vault-encrypt]")
{
    VaultEncryptionManager mgr;
    REQUIRE(mgr.encrypt_vault("vault_1", "password123") == true);
    REQUIRE(mgr.is_vault_encrypted("vault_1") == true);
    REQUIRE(mgr.vault_state("vault_1") == VaultEncryptionState::kUnlocked);
    REQUIRE(mgr.encrypted_vault_count() == 1);
}

TEST_CASE("VaultEncryptionManager: encrypt requires password", "[phase44][vault-encrypt]")
{
    VaultEncryptionManager mgr;
    REQUIRE(mgr.encrypt_vault("vault_1", "") == false);
    REQUIRE(mgr.is_vault_encrypted("vault_1") == false);
}

TEST_CASE("VaultEncryptionManager: decrypt vault", "[phase44][vault-encrypt]")
{
    VaultEncryptionManager mgr;
    mgr.encrypt_vault("vault_1", "password");
    REQUIRE(mgr.decrypt_vault("vault_1", "password") == true);
    REQUIRE(mgr.is_vault_encrypted("vault_1") == false);
    REQUIRE(mgr.encrypted_vault_count() == 0);
}

TEST_CASE("VaultEncryptionManager: lock and unlock", "[phase44][vault-encrypt]")
{
    VaultEncryptionManager mgr;
    mgr.encrypt_vault("vault_1", "password");

    REQUIRE(mgr.lock_vault("vault_1") == true);
    REQUIRE(mgr.vault_state("vault_1") == VaultEncryptionState::kLocked);
    REQUIRE(mgr.locked_vault_count() == 1);

    REQUIRE(mgr.unlock_vault("vault_1", "password") == true);
    REQUIRE(mgr.vault_state("vault_1") == VaultEncryptionState::kUnlocked);
    REQUIRE(mgr.locked_vault_count() == 0);
}

TEST_CASE("VaultEncryptionManager: cannot lock unencrypted vault", "[phase44][vault-encrypt]")
{
    VaultEncryptionManager mgr;
    REQUIRE(mgr.lock_vault("vault_1") == false);
}

TEST_CASE("VaultEncryptionManager: config", "[phase44][vault-encrypt]")
{
    VaultEncryptionManager mgr;
    VaultEncryptionConfig config;
    config.auto_lock_minutes = 30;
    config.iteration_count = 200000;
    mgr.set_config("vault_1", config);

    auto retrieved = mgr.get_config("vault_1");
    REQUIRE(retrieved.auto_lock_minutes == 30);
    REQUIRE(retrieved.iteration_count == 200000);
}

TEST_CASE("VaultEncryptionManager: find_vault and clear", "[phase44][vault-encrypt]")
{
    VaultEncryptionManager mgr;
    mgr.encrypt_vault("v1", "pwd");
    mgr.encrypt_vault("v2", "pwd");
    REQUIRE(mgr.vault_count() == 2);
    REQUIRE(mgr.find_vault("v1") != nullptr);

    mgr.clear_all();
    REQUIRE(mgr.vault_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// KeyManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("KeyManager: generate and find key", "[phase44][keys]")
{
    KeyManager mgr;
    auto kid = mgr.generate_key();
    REQUIRE(mgr.key_count() == 1);

    const auto* key = mgr.find_key(kid);
    REQUIRE(key != nullptr);
    REQUIRE(key->algorithm == "AES-256-GCM");
    REQUIRE(key->is_active == true);
}

TEST_CASE("KeyManager: revoke key", "[phase44][keys]")
{
    KeyManager mgr;
    auto kid = mgr.generate_key();
    REQUIRE(mgr.revoke_key(kid) == true);
    REQUIRE(mgr.find_key(kid)->is_revoked == true);
    REQUIRE(mgr.revoked_count() == 1);
    REQUIRE(mgr.active_keys().empty());

    // Cannot revoke again
    REQUIRE(mgr.revoke_key(kid) == false);
}

TEST_CASE("KeyManager: rotate key", "[phase44][keys]")
{
    KeyManager mgr;
    auto old_kid = mgr.generate_key();
    auto new_kid = mgr.rotate_key(old_kid);

    REQUIRE(!new_kid.empty());
    REQUIRE(new_kid != old_kid);
    REQUIRE(mgr.find_key(old_kid)->is_active == false);
    REQUIRE(mgr.find_key(new_kid)->is_active == true);
    REQUIRE(mgr.key_count() == 2);
}

TEST_CASE("KeyManager: rotation policy", "[phase44][keys]")
{
    KeyManager mgr;
    KeyRotationPolicy policy;
    policy.rotation_interval_days = 30;
    policy.auto_rotate = true;
    mgr.set_rotation_policy(policy);

    auto retrieved = mgr.get_rotation_policy();
    REQUIRE(retrieved.rotation_interval_days == 30);
    REQUIRE(retrieved.auto_rotate == true);
}

TEST_CASE("KeyManager: clear all", "[phase44][keys]")
{
    KeyManager mgr;
    mgr.generate_key();
    mgr.generate_key();
    mgr.clear_all();
    REQUIRE(mgr.key_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// PrivacyManager Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("PrivacyManager: classify and query", "[phase44][privacy]")
{
    PrivacyManager mgr;
    mgr.classify_document("doc_1", PrivacyLevel::kConfidential);
    REQUIRE(mgr.classification_count() == 1);

    const auto* cls = mgr.get_classification("doc_1");
    REQUIRE(cls != nullptr);
    REQUIRE(cls->level == PrivacyLevel::kConfidential);
}

TEST_CASE("PrivacyManager: reclassify document", "[phase44][privacy]")
{
    PrivacyManager mgr;
    mgr.classify_document("doc_1", PrivacyLevel::kPublic);
    mgr.classify_document("doc_1", PrivacyLevel::kSecret);
    REQUIRE(mgr.classification_count() == 1);
    REQUIRE(mgr.get_classification("doc_1")->level == PrivacyLevel::kSecret);
}

TEST_CASE("PrivacyManager: remove classification", "[phase44][privacy]")
{
    PrivacyManager mgr;
    mgr.classify_document("doc_1", PrivacyLevel::kInternal);
    REQUIRE(mgr.remove_classification("doc_1") == true);
    REQUIRE(mgr.classification_count() == 0);
    REQUIRE(mgr.remove_classification("nonexistent") == false);
}

TEST_CASE("PrivacyManager: documents at level", "[phase44][privacy]")
{
    PrivacyManager mgr;
    mgr.classify_document("doc_1", PrivacyLevel::kSecret);
    mgr.classify_document("doc_2", PrivacyLevel::kSecret);
    mgr.classify_document("doc_3", PrivacyLevel::kPublic);

    REQUIRE(mgr.documents_at_level(PrivacyLevel::kSecret).size() == 2);
    REQUIRE(mgr.documents_at_level(PrivacyLevel::kPublic).size() == 1);
}

TEST_CASE("PrivacyManager: policy", "[phase44][privacy]")
{
    PrivacyManager mgr;
    PrivacyPolicy policy;
    policy.redact_on_export = true;
    policy.retention_days = 90;
    mgr.set_policy(policy);

    auto retrieved = mgr.get_policy();
    REQUIRE(retrieved.redact_on_export == true);
    REQUIRE(retrieved.retention_days == 90);
}

TEST_CASE("PrivacyManager: level names", "[phase44][privacy]")
{
    REQUIRE(privacy_level_name(PrivacyLevel::kPublic) == "public");
    REQUIRE(privacy_level_name(PrivacyLevel::kInternal) == "internal");
    REQUIRE(privacy_level_name(PrivacyLevel::kConfidential) == "confidential");
    REQUIRE(privacy_level_name(PrivacyLevel::kSecret) == "secret");
}

TEST_CASE("PrivacyManager: clear all", "[phase44][privacy]")
{
    PrivacyManager mgr;
    mgr.classify_document("doc_1", PrivacyLevel::kPublic);
    mgr.clear_all();
    REQUIRE(mgr.classification_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// DataRedactionEngine Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("DataRedactionEngine: add and find rule", "[phase44][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "test_rule";
    rule.pattern = "secret";
    rule.replacement = "[REDACTED]";
    engine.add_rule(rule);

    REQUIRE(engine.rule_count() == 1);
    REQUIRE(engine.find_rule("test_rule") != nullptr);
}

TEST_CASE("DataRedactionEngine: remove rule", "[phase44][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "r1";
    rule.pattern = "test";
    engine.add_rule(rule);
    REQUIRE(engine.remove_rule("r1") == true);
    REQUIRE(engine.rule_count() == 0);
    REQUIRE(engine.remove_rule("nonexistent") == false);
}

TEST_CASE("DataRedactionEngine: load defaults", "[phase44][redaction]")
{
    DataRedactionEngine engine;
    engine.load_defaults();
    REQUIRE(engine.rule_count() == 5);
    REQUIRE(engine.active_rule_count() == 5);
}

TEST_CASE("DataRedactionEngine: redact content", "[phase44][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "email_rule";
    rule.pattern = "user@example.com";
    rule.replacement = "[EMAIL]";
    engine.add_rule(rule);

    auto result = engine.redact("Contact user@example.com for help");
    REQUIRE(result.redactions_applied == 1);
    REQUIRE(result.rules_matched == 1);
    REQUIRE(result.redacted_text == "Contact [EMAIL] for help");
}

TEST_CASE("DataRedactionEngine: multiple redactions", "[phase44][redaction]")
{
    DataRedactionEngine engine;
    RedactionRule rule;
    rule.rule_id = "word_rule";
    rule.pattern = "secret";
    rule.replacement = "***";
    engine.add_rule(rule);

    auto result = engine.redact("This is secret and also secret info");
    REQUIRE(result.redactions_applied == 2);
    REQUIRE(result.redacted_text == "This is *** and also *** info");
}

TEST_CASE("DataRedactionEngine: scan without redacting", "[phase44][redaction]")
{
    DataRedactionEngine engine;
    engine.load_defaults();

    auto matches = engine.scan("Email: user@test.com, IP: 192.168.1.1");
    REQUIRE(matches.size() >= 2); // @ match and 192.168. match
}

TEST_CASE("DataRedactionEngine: rule type names", "[phase44][redaction]")
{
    REQUIRE(redaction_rule_type_name(RedactionRuleType::kEmail) == "email");
    REQUIRE(redaction_rule_type_name(RedactionRuleType::kPhone) == "phone");
    REQUIRE(redaction_rule_type_name(RedactionRuleType::kSsn) == "ssn");
    REQUIRE(redaction_rule_type_name(RedactionRuleType::kCreditCard) == "credit_card");
    REQUIRE(redaction_rule_type_name(RedactionRuleType::kIpAddress) == "ip_address");
}

TEST_CASE("DataRedactionEngine: clear", "[phase44][redaction]")
{
    DataRedactionEngine engine;
    engine.load_defaults();
    engine.clear_rules();
    REQUIRE(engine.rule_count() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// Command Provider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("EncryptionCommandProvider: count and ids", "[phase44][encrypt-commands]")
{
    REQUIRE(EncryptionCommandProvider::command_count() == 8);
    auto ids = EncryptionCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "encrypt.vault");
}

TEST_CASE("EncryptionCommandProvider: register to registry", "[phase44][encrypt-commands]")
{
    CommandRegistry registry;
    const EncryptionCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.get_command("encrypt.vault") != nullptr);
    REQUIRE(registry.get_command("encrypt.vault")->category == "Encryption");
}

TEST_CASE("PrivacyCommandProvider: count and ids", "[phase44][privacy-commands]")
{
    REQUIRE(PrivacyCommandProvider::command_count() == 8);
    auto ids = PrivacyCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "privacy.classify");
}

TEST_CASE("PrivacyCommandProvider: register to registry", "[phase44][privacy-commands]")
{
    CommandRegistry registry;
    const PrivacyCommandProvider provider;
    provider.register_commands(registry);

    REQUIRE(registry.get_command("privacy.classify") != nullptr);
    REQUIRE(registry.get_command("privacy.classify")->category == "Privacy");
}

// ═══════════════════════════════════════════════════════════════════
// Phase 44 Events Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Phase 44 Events: VaultEncryptionChangedEvent", "[phase44][events]")
{
    VaultEncryptionChangedEvent event;
    event.vault_id = "vault_1";
    event.new_state = "encrypted";
    REQUIRE(event.new_state == "encrypted");
}

TEST_CASE("Phase 44 Events: VaultLockedEvent", "[phase44][events]")
{
    VaultLockedEvent event;
    event.vault_id = "vault_1";
    event.is_locked = true;
    REQUIRE(event.is_locked == true);
}

TEST_CASE("Phase 44 Events: KeyRotatedEvent", "[phase44][events]")
{
    KeyRotatedEvent event;
    event.old_key_id = "key_1";
    event.new_key_id = "key_2";
    event.algorithm = "AES-256-GCM";
    REQUIRE(event.algorithm == "AES-256-GCM");
}

TEST_CASE("Phase 44 Events: DocumentClassifiedEvent", "[phase44][events]")
{
    DocumentClassifiedEvent event;
    event.document_id = "doc_1";
    event.level = "secret";
    REQUIRE(event.level == "secret");
}

TEST_CASE("Phase 44 Events: DataRedactedEvent", "[phase44][events]")
{
    DataRedactedEvent event;
    event.document_id = "doc_1";
    event.redactions_applied = 5;
    event.rules_matched = 3;
    REQUIRE(event.redactions_applied == 5);
}

TEST_CASE("Phase 44 Events: PrivacyPolicyChangedEvent", "[phase44][events]")
{
    PrivacyPolicyChangedEvent event;
    event.setting = "redact_on_export";
    event.new_value = "true";
    REQUIRE(event.setting == "redact_on_export");
}
