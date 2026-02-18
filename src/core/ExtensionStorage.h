#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Source from which an extension was installed.
enum class ExtensionSource
{
    kGallery, // Installed from marketplace
    kVsix,    // Installed from a .vsix file
    kBuiltin  // Bundled with the application
};

/// Metadata about an installed extension, persisted to extensions.json.
struct ExtensionMetadata
{
    std::string extension_id; // "publisher.name" format
    std::string version;
    ExtensionSource source{ExtensionSource::kBuiltin};
    std::string installed_at; // ISO-8601 timestamp
    bool enabled{true};
    std::string location; // Filesystem path to extension directory
};

/// Service to persist and retrieve extension metadata.
/// Stores data in a JSON file (e.g. `~/.markamp/extensions.json`).
class ExtensionStorageService
{
public:
    /// Construct with a specific storage file path.
    explicit ExtensionStorageService(std::filesystem::path storage_path);

    /// Load metadata from disk.
    void load();

    /// Save metadata to disk.
    void save() const;

    /// Add or update metadata for an extension.
    void upsert(const ExtensionMetadata& metadata);

    /// Remove metadata for an extension by ID.
    void remove(const std::string& extension_id);

    /// Get metadata for a specific extension.
    [[nodiscard]] auto get(const std::string& extension_id) const -> const ExtensionMetadata*;

    /// Get all stored metadata.
    [[nodiscard]] auto get_all() const -> std::vector<ExtensionMetadata>;

    /// Check if an extension is tracked.
    [[nodiscard]] auto contains(const std::string& extension_id) const -> bool;

    /// Get total count of tracked extensions.
    [[nodiscard]] auto count() const -> size_t;

    // ── V9 Phase 04 Task 9: Secret storage with encryption ──

    /// Set the encryption key for secret storage.
    void set_encryption_key(const std::string& key);

    /// Store an encrypted secret for an extension.
    void store_secret(const std::string& extension_id,
                      const std::string& secret_key,
                      const std::string& secret_value);

    /// Retrieve a decrypted secret for an extension.
    [[nodiscard]] auto retrieve_secret(const std::string& extension_id,
                                       const std::string& secret_key) const -> std::string;

    /// Delete a stored secret.
    void delete_secret(const std::string& extension_id, const std::string& secret_key);

    /// Check if encryption is configured.
    [[nodiscard]] auto has_encryption_key() const -> bool
    {
        return !encryption_key_.empty();
    }

private:
    std::filesystem::path storage_path_;
    std::unordered_map<std::string, ExtensionMetadata> entries_;

    // Task 9: Encryption support
    std::string encryption_key_;
    // Secrets stored as: extension_id -> { secret_key -> encrypted_value }
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> secrets_;
    [[nodiscard]] static auto xor_cipher(const std::string& data, const std::string& key)
        -> std::string;
};

} // namespace markamp::core
