/// @file EncryptionService.h
/// @brief V4 Phase 28 – Note Encryption Support.
/// Per-document encryption with password-based key derivation,
/// password session caching, tamper detection, and serialization.

#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// An encrypted document blob.
struct EncryptedDocument
{
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> iv;       ///< 12 bytes for nonce
    std::vector<uint8_t> salt;     ///< 32 bytes for key derivation
    std::vector<uint8_t> auth_tag; ///< 16 bytes for authentication
    std::string algorithm{"AES-256-GCM"};
    int kdf_iterations{100000};
    std::string version{"1"};
};

/// Encryption service for per-document encryption.
class EncryptionService
{
public:
    EncryptionService(EventBus& event_bus, Config& config);

    /// Encrypt plaintext content with a password.
    [[nodiscard]] auto encrypt(const std::string& plaintext, const std::string& password) const
        -> std::expected<EncryptedDocument, std::string>;

    /// Decrypt an encrypted document with a password.
    [[nodiscard]] auto decrypt(const EncryptedDocument& doc, const std::string& password) const
        -> std::expected<std::string, std::string>;

    /// Check if raw bytes represent encrypted content (magic header).
    [[nodiscard]] auto is_encrypted_content(const std::vector<uint8_t>& data) const -> bool;

    /// Cache a password for the current session (keyed by document ID).
    auto cache_password(const std::string& document_id, const std::string& password) -> void;

    /// Get a cached password.
    [[nodiscard]] auto get_cached_password(const std::string& document_id) const
        -> std::optional<std::string>;

    /// Clear all cached passwords.
    auto clear_password_cache() -> void;

    /// Change the password on an encrypted document.
    [[nodiscard]] auto change_password(const EncryptedDocument& doc,
                                       const std::string& old_password,
                                       const std::string& new_password) const
        -> std::expected<EncryptedDocument, std::string>;

    /// Serialize an EncryptedDocument to a binary blob (with magic header).
    [[nodiscard]] auto serialize(const EncryptedDocument& doc) const -> std::vector<uint8_t>;

    /// Deserialize a binary blob to an EncryptedDocument.
    [[nodiscard]] auto deserialize(const std::vector<uint8_t>& data) const
        -> std::expected<EncryptedDocument, std::string>;

    /// The magic header for encrypted files.
    static constexpr const char* kMagicHeader = "MARKENC";
    static constexpr size_t kMagicHeaderSize = 7;

private:
    EventBus& event_bus_;
    Config& config_;

    std::unordered_map<std::string, std::string> password_cache_;

    /// Key derivation (password + salt → key bytes).
    [[nodiscard]] auto derive_key(const std::string& password,
                                  const std::vector<uint8_t>& salt,
                                  int iterations) const -> std::vector<uint8_t>;

    /// Generate pseudo-random bytes (simplified for testability).
    [[nodiscard]] auto generate_random_bytes(size_t count) const -> std::vector<uint8_t>;

    /// Compute authentication tag over (ciphertext || iv || salt).
    [[nodiscard]] auto compute_auth_tag(const std::vector<uint8_t>& key,
                                        const std::vector<uint8_t>& ciphertext,
                                        const std::vector<uint8_t>& iv_bytes,
                                        const std::vector<uint8_t>& salt) const
        -> std::vector<uint8_t>;

    /// XOR-based stream cipher (for testable layer; real AES in platform).
    [[nodiscard]] auto xor_crypt(const std::vector<uint8_t>& data,
                                 const std::vector<uint8_t>& key,
                                 const std::vector<uint8_t>& iv_bytes) const
        -> std::vector<uint8_t>;
};

} // namespace markamp::core
