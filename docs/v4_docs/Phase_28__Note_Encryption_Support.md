# Phase 28 -- Note Encryption Support

## Objective

Implement per-document encryption: users can encrypt sensitive documents with a password. Encrypted documents are stored as encrypted blobs on disk and decrypted in-memory only when the correct password is provided. Uses AES-256-GCM for encryption. Supports encrypt/decrypt workflows, password caching for the session, and visual indicators for encrypted documents.

## Prerequisites

- Phase 02 (VaultService -- document I/O)
- Existing FileSystem

## Feature References (PRD)

- PRD #42: Note Encryption Support

## Data Structures to Implement

### File: `src/core/EncryptionService.h`

```cpp
#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

struct EncryptedDocument
{
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> iv;        // 12 bytes for AES-GCM
    std::vector<uint8_t> salt;      // 32 bytes for key derivation
    std::vector<uint8_t> auth_tag;  // 16 bytes for GCM authentication
    std::string algorithm{"AES-256-GCM"};
    int kdf_iterations{100000};
    std::string version{"1"};
};

class EncryptionService
{
public:
    EncryptionService(EventBus& event_bus, Config& config);

    /// Encrypt a document's content with a password.
    [[nodiscard]] auto encrypt(const std::string& plaintext,
                                const std::string& password) const
        -> std::expected<EncryptedDocument, std::string>;

    /// Decrypt an encrypted document.
    [[nodiscard]] auto decrypt(const EncryptedDocument& doc,
                                const std::string& password) const
        -> std::expected<std::string, std::string>;

    /// Encrypt and write a document file.
    [[nodiscard]] auto encrypt_file(const std::string& document_id,
                                     const std::string& password)
        -> std::expected<void, std::string>;

    /// Read and decrypt a document file.
    [[nodiscard]] auto decrypt_file(const std::string& file_path,
                                     const std::string& password) const
        -> std::expected<std::string, std::string>;

    /// Check if a file is encrypted (magic bytes header).
    [[nodiscard]] auto is_encrypted(const std::string& file_path) const -> bool;

    /// Cache a password for the current session (keyed by document ID).
    auto cache_password(const std::string& document_id,
                         const std::string& password) -> void;

    /// Get a cached password.
    [[nodiscard]] auto get_cached_password(const std::string& document_id) const
        -> std::optional<std::string>;

    /// Clear all cached passwords.
    auto clear_password_cache() -> void;

    /// Change the password on an encrypted document.
    [[nodiscard]] auto change_password(const std::string& document_id,
                                        const std::string& old_password,
                                        const std::string& new_password)
        -> std::expected<void, std::string>;

private:
    EventBus& event_bus_;
    Config& config_;

    std::unordered_map<std::string, std::string> password_cache_;

    [[nodiscard]] auto derive_key(const std::string& password,
                                   const std::vector<uint8_t>& salt,
                                   int iterations) const
        -> std::vector<uint8_t>;
    [[nodiscard]] auto generate_random_bytes(int count) const -> std::vector<uint8_t>;

    auto serialize_encrypted(const EncryptedDocument& doc) const -> std::vector<uint8_t>;
    [[nodiscard]] auto deserialize_encrypted(const std::vector<uint8_t>& data) const
        -> std::expected<EncryptedDocument, std::string>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`encrypt(plaintext, password)`** -- Generate random salt (32 bytes). Derive key using PBKDF2-SHA256 with salt and iterations. Generate random IV (12 bytes). Encrypt plaintext with AES-256-GCM. Return EncryptedDocument with ciphertext, IV, salt, and auth tag.

2. **`decrypt(doc, password)`** -- Derive key from password + doc.salt. Decrypt ciphertext with AES-256-GCM using IV and auth tag. If auth tag verification fails, return error (wrong password or tampered data).

3. **`encrypt_file(document_id, password)`** -- Load document content. Encrypt. Serialize to binary format with magic header. Write to disk replacing the .md file with .md.enc.

4. **`is_encrypted(file_path)`** -- Read first 8 bytes. Check for magic header "MARKENC\0".

5. **`derive_key(password, salt, iterations)`** -- PBKDF2-HMAC-SHA256 key derivation. Output 32-byte key. Use platform crypto library (CommonCrypto on macOS, CNG on Windows, OpenSSL on Linux).

6. **`cache_password(doc_id, password)`** -- Store password in memory for the session. Clear on app exit or explicit clear.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentEncryptedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentDecryptedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PasswordRequiredEvent)
std::string document_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_encryption_service.cpp`

1. **Encrypt and decrypt round-trip** -- Encrypt text. Decrypt with same password. Verify identical.
2. **Wrong password fails** -- Encrypt. Decrypt with wrong password. Verify error.
3. **Different salts** -- Encrypt same text twice. Verify ciphertext differs (different salt).
4. **Empty content** -- Encrypt empty string. Decrypt. Verify empty string returned.
5. **Large content** -- Encrypt 1MB text. Decrypt. Verify correct.
6. **Is encrypted detection** -- Write encrypted file. is_encrypted() returns true. Plain file returns false.
7. **File encrypt/decrypt** -- Encrypt file to disk. Decrypt from disk. Verify content matches.
8. **Password caching** -- Cache password. Retrieve. Verify matches. Clear. Verify gone.
9. **Change password** -- Encrypt with pw1. Change to pw2. Decrypt with pw2. Verify works.
10. **Tampered data fails** -- Modify one byte of ciphertext. Decrypt fails (auth tag mismatch).

## Acceptance Criteria

- [ ] AES-256-GCM encryption produces authenticated ciphertext
- [ ] PBKDF2 key derivation with configurable iterations
- [ ] Wrong password returns clear error (not crash or garbage)
- [ ] Encrypted files detected by magic header
- [ ] Password cache stores credentials for session only
- [ ] Password change re-encrypts with new key
- [ ] Tampered data detected via auth tag verification
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/EncryptionService.h` | EncryptionService, EncryptedDocument |
| CREATE | `src/core/EncryptionService.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 3 encryption events |
| MODIFY | `src/core/PluginContext.h` | Add `EncryptionService* encryption_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add EncryptionService.cpp, link crypto library |
| CREATE | `tests/unit/test_encryption_service.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_encryption_service target |

## Architecture Notes

- Uses platform crypto: CommonCrypto (macOS), BCrypt (Windows), OpenSSL (Linux)
- Password is NEVER stored on disk -- only a session cache in memory
- Encrypted files use `.md.enc` extension to distinguish from plain Markdown
- VaultService must detect .enc files and route through EncryptionService
- Constructor injection: EncryptionService(EventBus&, Config&)

## Estimated Complexity

**L** -- Platform crypto APIs, key derivation, file format, tamper detection, 10 tests.
