// ============================================================================
// File: src/core/CloudSyncTypes.h
// Phase 38: Cloud Sync — Types, configs, encryption
// ============================================================================
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// Cloud storage provider type.
enum class CloudProvider : uint8_t
{
    S3,    // Amazon S3 / S3-compatible (MinIO, Wasabi, etc.)
    WebDAV // WebDAV (Nextcloud, ownCloud, Synology, etc.)
};

// Current cloud sync status.
enum class CloudSyncStatus : uint8_t
{
    Idle,
    Connecting,
    Uploading,
    Downloading,
    Encrypting,
    Decrypting,
    Completed,
    Failed
};

// S3-specific configuration.
struct S3Config
{
    std::string endpoint;                // S3 endpoint URL
    std::string region{"us-east-1"};     // AWS region
    std::string bucket;                  // Bucket name
    std::string prefix;                  // Object key prefix
    std::string access_key_id;           // AWS access key
    std::string secret_access_key;       // AWS secret key
    bool use_ssl{true};                  // Use HTTPS
    bool path_style{false};              // Path-style addressing
    int64_t multipart_threshold_mb{100}; // Multipart upload threshold
};

// WebDAV-specific configuration.
struct WebDavConfig
{
    std::string url; // WebDAV server URL
    std::string username;
    std::string password;
    std::string remote_path{"/markamp/"}; // Remote directory path
    bool use_ssl{true};
    int timeout_seconds{30};
};

// Encryption configuration for cloud sync.
struct EncryptionConfig
{
    bool enabled{false};    // Enable end-to-end encryption
    std::string passphrase; // User passphrase (for KDF)
    // Derived key parameters (Argon2id).
    uint32_t argon2_memory_kb{65536}; // 64 MB
    uint32_t argon2_iterations{3};    // Time cost
    uint32_t argon2_parallelism{4};   // Parallelism
    // AES-256-GCM parameters.
    int32_t nonce_size{12}; // 96-bit nonce
    int32_t tag_size{16};   // 128-bit authentication tag
};

// Combined cloud sync configuration.
struct CloudSyncConfig
{
    CloudProvider provider{CloudProvider::S3};
    S3Config s3;
    WebDavConfig webdav;
    EncryptionConfig encryption;
    std::filesystem::path local_data_dir; // Local workspace data path
    int sync_interval_minutes{30};        // Auto-sync interval (0 = manual)
    bool sync_on_close{true};             // Sync before app close
    int64_t bandwidth_limit_kbps{0};      // 0 = unlimited
};

// Result of a cloud sync operation.
struct CloudSyncResult
{
    CloudSyncStatus status{CloudSyncStatus::Completed};
    int32_t files_uploaded{0};
    int32_t files_downloaded{0};
    int32_t files_deleted{0};
    int64_t bytes_uploaded{0};
    int64_t bytes_downloaded{0};
    int64_t elapsed_ms{0};
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    [[nodiscard]] auto succeeded() const -> bool
    {
        return status == CloudSyncStatus::Completed && errors.empty();
    }
};

} // namespace markamp::core
