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

// ============================================================================
// Phase 27: Cloud Sync & Collaboration — Extended types
// ============================================================================

// Auto-sync schedule interval.
enum class SyncScheduleInterval : uint8_t
{
    kManual, // No auto-sync
    k5Min,   // Every 5 minutes
    k15Min,  // Every 15 minutes
    k30Min,  // Every 30 minutes
    k60Min   // Every 60 minutes
};

// Default sync ignore patterns (like .gitignore).
struct SyncIgnorePatterns
{
    std::vector<std::string> patterns;
    bool use_defaults{true}; // Include default patterns

    // Built-in defaults.
    static auto default_patterns() -> std::vector<std::string>
    {
        return {"*.tmp",
                "*.swp",
                "*.swo",
                ".DS_Store",
                "Thumbs.db",
                "node_modules/",
                ".git/",
                ".markamp/sync-state.json",
                "*.log",
                "~$*"};
    }

    // Check if a path matches any ignore pattern.
    [[nodiscard]] auto should_ignore(const std::string& relative_path) const -> bool;
};

// Information about a syncing device.
struct DeviceInfo
{
    std::string device_id;
    std::string device_name;
    int64_t last_sync_at{0};
    bool is_current{false};
    std::string platform; // "macOS", "Windows", "Linux"
};

// Bandwidth management configuration.
struct BandwidthConfig
{
    int64_t upload_limit_kbps{0};      // 0 = unlimited
    int64_t download_limit_kbps{0};    // 0 = unlimited
    bool pause_on_metered{false};      // Pause sync on metered connections
    bool prioritize_small_files{true}; // Sync <100KB files first
};

// Sync notification type.
enum class SyncNotificationType : uint8_t
{
    kSyncComplete,     // Sync finished successfully
    kSyncError,        // Sync encountered an error
    kConflictDetected, // File conflicts found
    kOfflineQueued,    // Changes queued while offline
    kShareActivity     // Collaborator made changes
};

// Sync notification payload.
struct SyncNotification
{
    SyncNotificationType type{SyncNotificationType::kSyncComplete};
    std::string message;
    int32_t file_count{0};
    bool has_retry_action{false};
};

// Workspace share permission level.
enum class SharePermission : uint8_t
{
    kReadOnly, // View only
    kReadWrite // Full edit access
};

// Workspace share metadata.
struct WorkspaceShareInfo
{
    std::string share_id;
    std::string workspace_path;
    SharePermission permission{SharePermission::kReadOnly};
    std::string shared_by;
    std::vector<std::string> shared_with;
    int64_t created_at{0};
    bool is_active{true};
};

// Sync history entry — one sync operation log.
struct SyncHistoryEntry
{
    std::string entry_id;
    int64_t timestamp{0};
    std::string operation; // "sync", "push", "pull"
    int32_t files_uploaded{0};
    int32_t files_downloaded{0};
    int32_t files_deleted{0};
    int32_t conflicts{0};
    int64_t elapsed_ms{0};
    std::vector<std::string> errors;
    std::string device_name;
    CloudSyncStatus status{CloudSyncStatus::Completed};
};

} // namespace markamp::core
