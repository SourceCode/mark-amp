/// @file EncryptionService.cpp
/// @brief V4 Phase 28 – Note Encryption Support implementation.
/// Uses simplified XOR cipher with PBKDF2-style key stretching for the
/// testable layer. Real AES-256-GCM would be in a platform-specific layer.

#include "core/EncryptionService.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <functional>
#include <numeric>
#include <random>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

EncryptionService::EncryptionService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

// ============================================================================
// Simple hash helper (FNV-1a variant for key derivation)
// ============================================================================

namespace
{
auto simple_hash(const std::vector<uint8_t>& data, uint64_t seed) -> uint64_t
{
    uint64_t hash_val = 14695981039346656037ULL ^ seed;
    for (auto byte : data)
    {
        hash_val ^= static_cast<uint64_t>(byte);
        hash_val *= 1099511628211ULL;
    }
    return hash_val;
}

void write_u32(std::vector<uint8_t>& buf, uint32_t val)
{
    buf.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buf.push_back(static_cast<uint8_t>(val & 0xFF));
}

auto read_u32(const std::vector<uint8_t>& buf, size_t offset) -> uint32_t
{
    return (static_cast<uint32_t>(buf[offset]) << 24) |
           (static_cast<uint32_t>(buf[offset + 1]) << 16) |
           (static_cast<uint32_t>(buf[offset + 2]) << 8) | static_cast<uint32_t>(buf[offset + 3]);
}
} // namespace

// ============================================================================
// Key derivation
// ============================================================================

auto EncryptionService::derive_key(const std::string& password,
                                   const std::vector<uint8_t>& salt,
                                   int iterations) const -> std::vector<uint8_t>
{
    // Combine password + salt.
    std::vector<uint8_t> combined;
    combined.reserve(password.size() + salt.size());
    for (char chr : password)
    {
        combined.push_back(static_cast<uint8_t>(chr));
    }
    combined.insert(combined.end(), salt.begin(), salt.end());

    // Iterative hashing to stretch the key.
    uint64_t hash_a = simple_hash(combined, 0);
    uint64_t hash_b = simple_hash(combined, hash_a);
    uint64_t hash_c = simple_hash(combined, hash_b);
    uint64_t hash_d = simple_hash(combined, hash_c);

    for (int iter = 0; iter < iterations; ++iter)
    {
        hash_a = simple_hash(combined, hash_a ^ hash_d);
        hash_b = simple_hash(combined, hash_b ^ hash_a);
        hash_c = simple_hash(combined, hash_c ^ hash_b);
        hash_d = simple_hash(combined, hash_d ^ hash_c);
    }

    // Build 32-byte key from the four hash values.
    std::vector<uint8_t> key(32);
    for (int idx = 0; idx < 8; ++idx)
    {
        key[static_cast<size_t>(idx)] = static_cast<uint8_t>((hash_a >> (idx * 8)) & 0xFF);
        key[static_cast<size_t>(idx + 8)] = static_cast<uint8_t>((hash_b >> (idx * 8)) & 0xFF);
        key[static_cast<size_t>(idx + 16)] = static_cast<uint8_t>((hash_c >> (idx * 8)) & 0xFF);
        key[static_cast<size_t>(idx + 24)] = static_cast<uint8_t>((hash_d >> (idx * 8)) & 0xFF);
    }

    return key;
}

// ============================================================================
// Random bytes
// ============================================================================

auto EncryptionService::generate_random_bytes(size_t count) const -> std::vector<uint8_t>
{
    std::vector<uint8_t> bytes(count);
    auto seed = static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t idx = 0; idx < count; ++idx)
    {
        bytes[idx] = static_cast<uint8_t>(dist(rng));
    }
    return bytes;
}

// ============================================================================
// XOR stream cipher
// ============================================================================

auto EncryptionService::xor_crypt(const std::vector<uint8_t>& data,
                                  const std::vector<uint8_t>& key,
                                  const std::vector<uint8_t>& iv_bytes) const
    -> std::vector<uint8_t>
{
    // Build a keystream by hashing key+IV repeatedly.
    std::vector<uint8_t> keystream;
    keystream.reserve(data.size());

    std::vector<uint8_t> seed_data;
    seed_data.insert(seed_data.end(), key.begin(), key.end());
    seed_data.insert(seed_data.end(), iv_bytes.begin(), iv_bytes.end());

    uint64_t counter = 0;
    while (keystream.size() < data.size())
    {
        uint64_t hash_val = simple_hash(seed_data, counter);
        for (int byte_idx = 0; byte_idx < 8 && keystream.size() < data.size(); ++byte_idx)
        {
            keystream.push_back(static_cast<uint8_t>((hash_val >> (byte_idx * 8)) & 0xFF));
        }
        ++counter;
    }

    // XOR data with keystream.
    std::vector<uint8_t> result(data.size());
    for (size_t idx = 0; idx < data.size(); ++idx)
    {
        result[idx] = data[idx] ^ keystream[idx];
    }
    return result;
}

// ============================================================================
// Auth tag computation
// ============================================================================

auto EncryptionService::compute_auth_tag(const std::vector<uint8_t>& key,
                                         const std::vector<uint8_t>& ciphertext,
                                         const std::vector<uint8_t>& iv_bytes,
                                         const std::vector<uint8_t>& salt) const
    -> std::vector<uint8_t>
{
    // Combine all inputs.
    std::vector<uint8_t> combined;
    combined.insert(combined.end(), key.begin(), key.end());
    combined.insert(combined.end(), ciphertext.begin(), ciphertext.end());
    combined.insert(combined.end(), iv_bytes.begin(), iv_bytes.end());
    combined.insert(combined.end(), salt.begin(), salt.end());

    // Compute 16-byte tag via double hashing.
    uint64_t tag_a = simple_hash(combined, 0xDEADBEEF);
    uint64_t tag_b = simple_hash(combined, tag_a);

    std::vector<uint8_t> tag(16);
    for (int idx = 0; idx < 8; ++idx)
    {
        tag[static_cast<size_t>(idx)] = static_cast<uint8_t>((tag_a >> (idx * 8)) & 0xFF);
        tag[static_cast<size_t>(idx + 8)] = static_cast<uint8_t>((tag_b >> (idx * 8)) & 0xFF);
    }
    return tag;
}

// ============================================================================
// Encrypt
// ============================================================================

auto EncryptionService::encrypt(const std::string& plaintext, const std::string& password) const
    -> std::expected<EncryptedDocument, std::string>
{
    if (password.empty())
    {
        return std::unexpected("Password cannot be empty");
    }

    // Generate random salt (32 bytes) and IV (12 bytes).
    auto salt = generate_random_bytes(32);
    auto iv_bytes = generate_random_bytes(12);

    // Derive key.
    auto key = derive_key(password, salt, 100);

    // Encrypt.
    std::vector<uint8_t> plaintext_bytes(plaintext.begin(), plaintext.end());
    auto ciphertext = xor_crypt(plaintext_bytes, key, iv_bytes);

    // Compute auth tag.
    auto auth_tag = compute_auth_tag(key, ciphertext, iv_bytes, salt);

    EncryptedDocument doc;
    doc.ciphertext = std::move(ciphertext);
    doc.iv = std::move(iv_bytes);
    doc.salt = std::move(salt);
    doc.auth_tag = std::move(auth_tag);

    return doc;
}

// ============================================================================
// Decrypt
// ============================================================================

auto EncryptionService::decrypt(const EncryptedDocument& doc, const std::string& password) const
    -> std::expected<std::string, std::string>
{
    if (password.empty())
    {
        return std::unexpected("Password cannot be empty");
    }

    // Derive key.
    auto key = derive_key(password, doc.salt, 100);

    // Verify auth tag.
    auto expected_tag = compute_auth_tag(key, doc.ciphertext, doc.iv, doc.salt);
    if (expected_tag != doc.auth_tag)
    {
        return std::unexpected("Authentication failed: wrong password or tampered data");
    }

    // Decrypt.
    auto plaintext_bytes = xor_crypt(doc.ciphertext, key, doc.iv);
    return std::string(plaintext_bytes.begin(), plaintext_bytes.end());
}

// ============================================================================
// Is encrypted
// ============================================================================

auto EncryptionService::is_encrypted_content(const std::vector<uint8_t>& data) const -> bool
{
    if (data.size() < kMagicHeaderSize)
    {
        return false;
    }
    return std::memcmp(data.data(), kMagicHeader, kMagicHeaderSize) == 0;
}

// ============================================================================
// Password cache
// ============================================================================

auto EncryptionService::cache_password(const std::string& document_id, const std::string& password)
    -> void
{
    password_cache_[document_id] = password;
}

auto EncryptionService::get_cached_password(const std::string& document_id) const
    -> std::optional<std::string>
{
    auto iter = password_cache_.find(document_id);
    if (iter == password_cache_.end())
    {
        return std::nullopt;
    }
    return iter->second;
}

auto EncryptionService::clear_password_cache() -> void
{
    password_cache_.clear();
}

// ============================================================================
// Change password
// ============================================================================

auto EncryptionService::change_password(const EncryptedDocument& doc,
                                        const std::string& old_password,
                                        const std::string& new_password) const
    -> std::expected<EncryptedDocument, std::string>
{
    // Decrypt with old password.
    auto plaintext_result = decrypt(doc, old_password);
    if (!plaintext_result.has_value())
    {
        return std::unexpected(plaintext_result.error());
    }

    // Re-encrypt with new password (new salt + IV).
    return encrypt(*plaintext_result, new_password);
}

// ============================================================================
// Serialization
// ============================================================================

auto EncryptionService::serialize(const EncryptedDocument& doc) const -> std::vector<uint8_t>
{
    std::vector<uint8_t> result;

    // Magic header (7 bytes).
    for (size_t idx = 0; idx < kMagicHeaderSize; ++idx)
    {
        result.push_back(static_cast<uint8_t>(kMagicHeader[idx]));
    }
    result.push_back(0); // Null terminator for 8-byte header.

    // Version (4 bytes).
    write_u32(result, 1);

    // Salt length + salt.
    write_u32(result, static_cast<uint32_t>(doc.salt.size()));
    result.insert(result.end(), doc.salt.begin(), doc.salt.end());

    // IV length + IV.
    write_u32(result, static_cast<uint32_t>(doc.iv.size()));
    result.insert(result.end(), doc.iv.begin(), doc.iv.end());

    // Auth tag length + auth tag.
    write_u32(result, static_cast<uint32_t>(doc.auth_tag.size()));
    result.insert(result.end(), doc.auth_tag.begin(), doc.auth_tag.end());

    // Ciphertext length + ciphertext.
    write_u32(result, static_cast<uint32_t>(doc.ciphertext.size()));
    result.insert(result.end(), doc.ciphertext.begin(), doc.ciphertext.end());

    return result;
}

auto EncryptionService::deserialize(const std::vector<uint8_t>& data) const
    -> std::expected<EncryptedDocument, std::string>
{
    if (data.size() < kMagicHeaderSize + 1 + 4) // header + null + version
    {
        return std::unexpected("Data too short for encrypted document");
    }

    // Verify magic header.
    if (std::memcmp(data.data(), kMagicHeader, kMagicHeaderSize) != 0)
    {
        return std::unexpected("Invalid magic header: not an encrypted document");
    }

    size_t offset = kMagicHeaderSize + 1; // Skip header + null.

    // Version.
    if (offset + 4 > data.size())
    {
        return std::unexpected("Truncated version field");
    }
    uint32_t file_version = read_u32(data, offset);
    offset += 4;

    if (file_version != 1)
    {
        return std::unexpected("Unsupported encryption format version");
    }

    auto read_field =
        [&](const std::string& name) -> std::expected<std::vector<uint8_t>, std::string>
    {
        if (offset + 4 > data.size())
        {
            return std::unexpected("Truncated " + name + " length");
        }
        uint32_t length = read_u32(data, offset);
        offset += 4;
        if (offset + length > data.size())
        {
            return std::unexpected("Truncated " + name + " data");
        }
        std::vector<uint8_t> field(data.begin() + static_cast<ptrdiff_t>(offset),
                                   data.begin() + static_cast<ptrdiff_t>(offset + length));
        offset += length;
        return field;
    };

    EncryptedDocument doc;

    auto salt_result = read_field("salt");
    if (!salt_result.has_value())
    {
        return std::unexpected(salt_result.error());
    }
    doc.salt = std::move(*salt_result);

    auto iv_result = read_field("iv");
    if (!iv_result.has_value())
    {
        return std::unexpected(iv_result.error());
    }
    doc.iv = std::move(*iv_result);

    auto tag_result = read_field("auth_tag");
    if (!tag_result.has_value())
    {
        return std::unexpected(tag_result.error());
    }
    doc.auth_tag = std::move(*tag_result);

    auto ct_result = read_field("ciphertext");
    if (!ct_result.has_value())
    {
        return std::unexpected(ct_result.error());
    }
    doc.ciphertext = std::move(*ct_result);

    return doc;
}

} // namespace markamp::core
