# Phase 38 — Cloud Sync (S3/WebDAV Abstraction)

## Objective

Extend the local sync engine from Phase 37 to support cloud storage providers, specifically S3-compatible object storage and WebDAV servers. This phase introduces a storage provider abstraction layer (ISyncProvider interface) so new cloud backends can be added by implementing a single interface. Data is encrypted with AES-256-GCM before upload to protect user privacy, with the encryption key derived from a user-supplied passphrase using the Argon2id key derivation function.

The architecture separates transport concerns from sync logic. The SyncEngine from Phase 37 handles change detection, merge planning, and conflict resolution. The CloudSyncService in this phase wraps SyncEngine and replaces direct filesystem operations with ISyncProvider calls. For example, instead of `std::filesystem::copy_file()`, the cloud sync path calls `provider->upload_file()` or `provider->download_file()`. This design means all sync logic (change detection, conflict resolution, state management) is reused without modification.

Two concrete providers are implemented: S3SyncProvider (using libcurl with manual AWS Signature V4 signing or a lightweight S3 SDK) for S3-compatible services (AWS S3, MinIO, Backblaze B2, DigitalOcean Spaces), and WebDAVSyncProvider (using libcurl PROPFIND/GET/PUT/DELETE/MKCOL methods) for WebDAV-compatible services (Nextcloud, ownCloud, Box, generic WebDAV). Both providers support TLS/SSL, configurable timeouts, and proxy settings. The encryption layer sits between the sync engine and the provider: files are encrypted before upload and decrypted after download, with each file encrypted independently using a unique nonce.

## Prerequisites

- Phase 37 (Local Sync Engine)

## SiYuan Source Reference

- `kernel/model/sync.go` — Cloud sync functions, provider selection, encryption/decryption during sync
- `kernel/conf/sync.go` — SyncConf with S3 and WebDAV configuration structs (Endpoint, Region, Bucket, AccessKey, SecretKey, Timeout, SkipTLSVerify, PathStyle for S3; Endpoint, Username, Password, SkipTLSVerify, Timeout for WebDAV)
- `kernel/model/cloud.go` — Cloud provider abstraction, cloud storage operations
- `kernel/dejavu/cloud/` — S3 and WebDAV cloud storage implementations

## MarkAmp Integration Points

- New header: `src/core/ISyncProvider.h`
- New header: `src/core/S3SyncProvider.h`
- New source: `src/core/S3SyncProvider.cpp`
- New header: `src/core/WebDAVSyncProvider.h`
- New source: `src/core/WebDAVSyncProvider.cpp`
- New header: `src/core/CloudSyncService.h`
- New source: `src/core/CloudSyncService.cpp`
- New header: `src/core/SyncEncryption.h`
- New source: `src/core/SyncEncryption.cpp`
- Extends `Events.h` with cloud sync events
- Uses libcurl for HTTP operations (S3 signing, WebDAV methods)
- Uses OpenSSL or libsodium for AES-256-GCM encryption and Argon2id KDF
- Reuses SyncEngine from Phase 37 for change detection and merge logic

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Supported cloud storage provider types.
enum class SyncProviderType : uint8_t
{
    None,       // No cloud sync configured
    S3,         // S3-compatible object storage
    WebDAV      // WebDAV-compatible file storage
};

// Direction of a cloud file transfer.
enum class TransferDirection : uint8_t
{
    Upload,     // Local -> Cloud
    Download    // Cloud -> Local
};

// Information about a file in cloud storage.
struct RemoteFile
{
    std::string path;                            // Remote path (e.g., "/data/docs/note.sy")
    int64_t size_bytes{0};                       // File size
    std::string hash;                            // ETag or content hash
    int64_t modified{0};                         // Last modified time (Unix ms)
    bool is_directory{false};

    [[nodiscard]] auto filename() const -> std::string;
    [[nodiscard]] auto parent_path() const -> std::string;
    [[nodiscard]] auto formatted_size() const -> std::string;
};

// Status of the cloud sync connection.
struct SyncStatus
{
    SyncProviderType provider_type{SyncProviderType::None};
    bool connected{false};
    int64_t last_sync_time{0};                   // Unix ms of last successful sync
    SyncResult last_sync_result;                 // Result of last sync operation
    bool sync_in_progress{false};
    std::string provider_display_name;           // e.g., "AWS S3 (us-east-1)"
    int64_t remote_size_bytes{0};                // Total remote storage used

    [[nodiscard]] auto formatted_last_sync() const -> std::string;
    [[nodiscard]] auto is_configured() const -> bool
    {
        return provider_type != SyncProviderType::None;
    }
};

// Configuration for S3-compatible storage.
struct S3Config
{
    std::string endpoint;                        // e.g., "s3.amazonaws.com" or "minio.local:9000"
    std::string region;                          // e.g., "us-east-1"
    std::string bucket;                          // Bucket name
    std::string access_key;                      // AWS access key ID
    std::string secret_key;                      // AWS secret access key
    std::string prefix;                          // Key prefix within bucket (default: "markamp/")
    bool use_ssl{true};                          // Use HTTPS
    bool path_style{false};                      // Use path-style URLs (for MinIO)
    bool skip_tls_verify{false};                 // Skip TLS certificate verification
    int timeout_seconds{30};                     // Request timeout
};

// Configuration for WebDAV storage.
struct WebDAVConfig
{
    std::string url;                             // WebDAV server URL (e.g., "https://cloud.example.com/remote.php/dav/files/user/")
    std::string username;                        // Authentication username
    std::string password;                        // Authentication password
    std::string base_path;                       // Base path on the WebDAV server (default: "/markamp/")
    bool skip_tls_verify{false};                 // Skip TLS certificate verification
    int timeout_seconds{30};                     // Request timeout
};

// Encryption parameters for cloud sync.
struct EncryptionConfig
{
    bool enabled{true};                          // Enable encryption (default: true)
    std::string passphrase;                      // User passphrase (not stored, derived at runtime)
    // Argon2id parameters
    int argon2_time_cost{3};                     // Number of iterations
    int argon2_memory_cost{65536};               // Memory in KiB (64 MB)
    int argon2_parallelism{4};                   // Number of threads
};

// Result of an encryption or decryption operation.
struct CryptoResult
{
    std::vector<uint8_t> data;                   // Output data (encrypted or decrypted)
    bool success{false};
    std::string error_message;

    [[nodiscard]] auto size() const -> size_t
    {
        return data.size();
    }
};

// ISyncProvider — interface for cloud storage operations.
// Implementations handle transport-level details (HTTP, signing, auth).
class ISyncProvider
{
public:
    virtual ~ISyncProvider() = default;

    // Establish connection to the cloud storage.
    [[nodiscard]] virtual auto connect()
        -> std::expected<void, std::string> = 0;

    // Disconnect from the cloud storage.
    virtual auto disconnect() -> void = 0;

    // Upload a local file to remote storage.
    [[nodiscard]] virtual auto upload_file(const std::filesystem::path& local_path,
                                            const std::string& remote_path)
        -> std::expected<void, std::string> = 0;

    // Download a remote file to local storage.
    [[nodiscard]] virtual auto download_file(const std::string& remote_path,
                                              const std::filesystem::path& local_path)
        -> std::expected<void, std::string> = 0;

    // List files at a remote path/prefix.
    [[nodiscard]] virtual auto list_files(const std::string& prefix) const
        -> std::expected<std::vector<RemoteFile>, std::string> = 0;

    // Delete a remote file.
    [[nodiscard]] virtual auto delete_file(const std::string& remote_path)
        -> std::expected<void, std::string> = 0;

    // Get information about a remote file.
    [[nodiscard]] virtual auto get_file_info(const std::string& remote_path) const
        -> std::expected<RemoteFile, std::string> = 0;

    // Create a remote directory.
    [[nodiscard]] virtual auto mkdir(const std::string& remote_path)
        -> std::expected<void, std::string> = 0;

    // Check if the provider is currently connected.
    [[nodiscard]] virtual auto is_connected() const -> bool = 0;

    // Get the provider type.
    [[nodiscard]] virtual auto provider_type() const -> SyncProviderType = 0;

    // Get a human-readable display name.
    [[nodiscard]] virtual auto display_name() const -> std::string = 0;

    // Set a progress callback for upload/download operations.
    virtual auto set_progress_callback(
        std::function<void(TransferDirection, const std::string&, int64_t, int64_t)> callback)
        -> void = 0;
};

// SyncEncryption — handles AES-256-GCM encryption for cloud sync data.
class SyncEncryption
{
public:
    SyncEncryption() = default;

    // Derive an encryption key from a passphrase using Argon2id.
    [[nodiscard]] auto derive_key(const std::string& passphrase,
                                   const EncryptionConfig& config)
        -> std::expected<std::vector<uint8_t>, std::string>;

    // Encrypt data with AES-256-GCM using the derived key.
    // Prepends a random 12-byte nonce to the ciphertext.
    [[nodiscard]] auto encrypt(const std::vector<uint8_t>& plaintext,
                                const std::vector<uint8_t>& key)
        -> CryptoResult;

    // Encrypt a file, writing the encrypted output to the destination path.
    [[nodiscard]] auto encrypt_file(const std::filesystem::path& source,
                                     const std::filesystem::path& dest,
                                     const std::vector<uint8_t>& key)
        -> std::expected<int64_t, std::string>;

    // Decrypt data previously encrypted with encrypt().
    // Reads the nonce from the first 12 bytes of ciphertext.
    [[nodiscard]] auto decrypt(const std::vector<uint8_t>& ciphertext,
                                const std::vector<uint8_t>& key)
        -> CryptoResult;

    // Decrypt a file, writing the decrypted output to the destination path.
    [[nodiscard]] auto decrypt_file(const std::filesystem::path& source,
                                     const std::filesystem::path& dest,
                                     const std::vector<uint8_t>& key)
        -> std::expected<int64_t, std::string>;

    // Verify a passphrase by attempting to decrypt a known verification token.
    [[nodiscard]] auto verify_passphrase(const std::string& passphrase,
                                          const std::vector<uint8_t>& verification_token,
                                          const EncryptionConfig& config) const
        -> bool;

private:
    // Generate a cryptographically secure random nonce.
    [[nodiscard]] auto generate_nonce(size_t length = 12) const
        -> std::vector<uint8_t>;

    // Generate a random salt for Argon2id.
    [[nodiscard]] auto generate_salt(size_t length = 16) const
        -> std::vector<uint8_t>;
};

// CloudSyncService — orchestrates cloud sync using ISyncProvider and SyncEngine.
class CloudSyncService
{
public:
    CloudSyncService(EventBus& event_bus, Config& config,
                     SyncEngine& sync_engine);

    // Set the cloud provider type and configuration.
    // Creates and configures the appropriate ISyncProvider.
    [[nodiscard]] auto set_provider(SyncProviderType type,
                                     const std::string& config_json)
        -> std::expected<void, std::string>;

    // Perform a full cloud sync (uses SyncEngine internally).
    [[nodiscard]] auto sync()
        -> std::expected<SyncResult, std::string>;

    // Get the current sync status.
    [[nodiscard]] auto get_sync_status() const -> SyncStatus;

    // Set the encryption passphrase for cloud sync.
    [[nodiscard]] auto set_encryption_passphrase(const std::string& passphrase)
        -> std::expected<void, std::string>;

    // Check if encryption is configured and passphrase is set.
    [[nodiscard]] auto is_encryption_ready() const -> bool;

    // Test connectivity to the configured cloud provider.
    [[nodiscard]] auto test_connection()
        -> std::expected<void, std::string>;

    // Get the active provider (nullable).
    [[nodiscard]] auto provider() const -> ISyncProvider*;

private:
    EventBus& event_bus_;
    Config& config_;
    SyncEngine& sync_engine_;
    std::unique_ptr<ISyncProvider> provider_;
    SyncEncryption encryption_;
    std::vector<uint8_t> encryption_key_;
    SyncStatus status_;

    // Create a provider instance based on type.
    [[nodiscard]] auto create_provider(SyncProviderType type)
        -> std::unique_ptr<ISyncProvider>;

    // Download remote state to a temporary directory for SyncEngine comparison.
    [[nodiscard]] auto download_remote_snapshot(const std::filesystem::path& temp_dir)
        -> std::expected<void, std::string>;

    // Upload local changes to the cloud provider.
    [[nodiscard]] auto upload_changes(const std::vector<std::string>& files,
                                       const std::filesystem::path& local_dir)
        -> std::expected<int, std::string>;

    // Download remote changes from the cloud provider.
    [[nodiscard]] auto download_changes(const std::vector<std::string>& files,
                                         const std::filesystem::path& local_dir)
        -> std::expected<int, std::string>;

    // Encrypt a file if encryption is enabled, returning the path to the encrypted file.
    [[nodiscard]] auto encrypt_if_needed(const std::filesystem::path& source)
        -> std::expected<std::filesystem::path, std::string>;

    // Decrypt a file if encryption is enabled, returning the path to the decrypted file.
    [[nodiscard]] auto decrypt_if_needed(const std::filesystem::path& source)
        -> std::expected<std::filesystem::path, std::string>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. `CloudSyncService::set_provider()` — Parse config_json for provider-specific settings. Create the appropriate ISyncProvider (S3SyncProvider or WebDAVSyncProvider). Configure the provider with parsed settings. Store in provider_ unique_ptr. Update status_. Return error if config is invalid.
2. `CloudSyncService::sync()` — Verify provider is configured and connected. Create temporary directory for remote snapshot. Download remote file listing and state. Use SyncEngine::detect_changes() on both local and remote states. Merge changes. For uploads: encrypt_if_needed(), then provider_->upload_file(). For downloads: provider_->download_file(), then decrypt_if_needed(). Handle conflicts. Update status_. Publish CloudSyncCompletedEvent. Clean up temp directory.
3. `CloudSyncService::set_encryption_passphrase()` — Derive AES-256 key from passphrase using Argon2id via SyncEncryption::derive_key(). Store the key in memory. Create a verification token (encrypt a known string) and store it with the sync config. Update encryption readiness status.
4. `S3SyncProvider::connect()` — Perform a ListBuckets or HeadBucket request to verify credentials and bucket access. Use libcurl with AWS Signature V4 signing. Set connected_ flag on success.
5. `S3SyncProvider::upload_file()` — Read local file into memory. Compute Content-MD5 for integrity. Create PutObject request with AWS Sig V4. Send via libcurl. Verify response status code is 200. Handle multipart upload for files > 5MB.
6. `S3SyncProvider::list_files()` — Perform ListObjectsV2 request with the configured prefix. Parse XML response to extract object keys, sizes, ETags, and last modified times. Handle pagination via continuation tokens for large buckets.
7. `WebDAVSyncProvider::connect()` — Perform a PROPFIND request on the base URL to verify authentication. Parse the multistatus XML response. Set connected_ flag.
8. `WebDAVSyncProvider::upload_file()` — Read local file. Send PUT request to the WebDAV URL. Create parent directories via MKCOL if needed. Verify response status.
9. `WebDAVSyncProvider::list_files()` — Send PROPFIND request with Depth:1 header. Parse multistatus XML response to extract href, getcontentlength, getlastmodified, resourcetype. Return vector of RemoteFile.
10. `SyncEncryption::encrypt()` — Generate 12-byte random nonce. Encrypt data with AES-256-GCM using the derived key and nonce. Prepend nonce to ciphertext (nonce || ciphertext || tag). Return CryptoResult.
11. `SyncEncryption::decrypt()` — Extract 12-byte nonce from the first 12 bytes. Extract 16-byte auth tag from the last 16 bytes. Decrypt ciphertext with AES-256-GCM. Verify authentication tag. Return decrypted plaintext.
12. `SyncEncryption::derive_key()` — Use Argon2id with configured time/memory/parallelism parameters. Generate random 16-byte salt (or use stored salt for existing config). Derive 32-byte key. Return key bytes.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Cloud sync events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CloudSyncStartedEvent)
std::string provider_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CloudSyncCompletedEvent)
int32_t uploaded{0};
int32_t downloaded{0};
int32_t conflicts{0};
int32_t errors{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CloudSyncErrorEvent)
std::string error_message;
std::string provider_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CloudSyncProgressEvent)
TransferDirection direction{TransferDirection::Upload};
std::string file_path;
int progress_percent{0};
int64_t bytes_transferred{0};
int64_t bytes_total{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CloudProviderConnectedEvent)
std::string provider_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CloudProviderDisconnectedEvent)
std::string provider_name;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

- `knowledgebase.sync.provider` — Cloud provider type: `"none"`, `"s3"`, `"webdav"` (default: `"none"`)
- `knowledgebase.sync.encrypt` — Enable AES-256-GCM encryption for cloud data (default: `true`)
- `knowledgebase.sync.s3.endpoint` — S3 endpoint URL (default: `"s3.amazonaws.com"`)
- `knowledgebase.sync.s3.region` — AWS region (default: `"us-east-1"`)
- `knowledgebase.sync.s3.bucket` — S3 bucket name (default: `""`)
- `knowledgebase.sync.s3.access_key` — AWS access key ID (default: `""`)
- `knowledgebase.sync.s3.secret_key` — AWS secret access key (default: `""`)
- `knowledgebase.sync.s3.prefix` — Key prefix in bucket (default: `"markamp/"`)
- `knowledgebase.sync.s3.use_ssl` — Use HTTPS for S3 (default: `true`)
- `knowledgebase.sync.s3.path_style` — Use path-style S3 URLs (default: `false`)
- `knowledgebase.sync.s3.timeout_seconds` — S3 request timeout (default: `30`)
- `knowledgebase.sync.webdav.url` — WebDAV server URL (default: `""`)
- `knowledgebase.sync.webdav.username` — WebDAV username (default: `""`)
- `knowledgebase.sync.webdav.password` — WebDAV password (default: `""`)
- `knowledgebase.sync.webdav.base_path` — WebDAV base path (default: `"/markamp/"`)
- `knowledgebase.sync.webdav.timeout_seconds` — WebDAV request timeout (default: `30`)

## Test Cases (Catch2)

File: `tests/unit/test_cloud_sync.cpp`

Note: All tests use a MockSyncProvider that implements ISyncProvider with in-memory storage, avoiding actual network calls.

1. **Mock provider connect and disconnect** — Create MockSyncProvider. Call `connect()`, verify `is_connected()` returns true. Call `disconnect()`, verify `is_connected()` returns false.
2. **Upload file via mock provider** — Connect provider. Upload a test file. Verify `list_files()` includes the uploaded file with correct size and path.
3. **Download file via mock provider** — Connect provider. Upload a file. Download it to a different path. Verify downloaded file has identical content to the original.
4. **List files returns correct entries** — Upload 5 files with different paths. Call `list_files("")`. Verify 5 RemoteFile entries with correct paths, sizes, and non-empty hashes.
5. **Full sync cycle via CloudSyncService** — Create CloudSyncService with MockSyncProvider. Add 3 local files. Call `sync()`. Verify all 3 files uploaded. Add 2 more remote files via mock. Call `sync()` again. Verify 2 files downloaded locally.
6. **Conflict during cloud sync** — Upload file X via mock. Modify local file X. Modify remote file X (different content). Call `sync()`. Verify conflict detected. Verify conflict copy created (KeepBoth).
7. **Encryption roundtrip** — Create SyncEncryption. Derive key from "test-passphrase". Encrypt a 1KB test buffer. Decrypt the result. Verify decrypted data matches original. Verify encrypted data differs from original.
8. **Encryption file roundtrip** — Create a temp file with known content. Derive key. Encrypt file to encrypted path. Decrypt encrypted file to decrypted path. Verify decrypted file matches original byte-for-byte.
9. **Progress reporting during sync** — Subscribe to CloudSyncProgressEvent. Upload 3 files via CloudSyncService. Verify at least 3 progress events received, each with increasing progress_percent. Verify final event has progress >= 100%.
10. **Error handling on network failure** — Configure MockSyncProvider to fail on the 2nd upload. Call `sync()` with 3 files. Verify SyncResult has errors == 1. Verify CloudSyncErrorEvent published. Verify the other 2 files were still processed.

## Acceptance Criteria

- [ ] ISyncProvider interface abstracts cloud storage operations
- [ ] S3SyncProvider implements all ISyncProvider methods with AWS Sig V4 signing
- [ ] WebDAVSyncProvider implements all ISyncProvider methods with PROPFIND/PUT/GET/DELETE
- [ ] CloudSyncService reuses SyncEngine for change detection and merge logic
- [ ] AES-256-GCM encryption with Argon2id KDF protects uploaded data
- [ ] Each file encrypted with unique nonce, decryptable independently
- [ ] Progress events report transfer direction, file name, and percentage
- [ ] Provider switching (S3 to WebDAV) works without data loss
- [ ] Mock provider enables comprehensive testing without network
- [ ] All 10 test cases pass

## Files to Create/Modify

- CREATE: `src/core/ISyncProvider.h`
- CREATE: `src/core/S3SyncProvider.h`
- CREATE: `src/core/S3SyncProvider.cpp`
- CREATE: `src/core/WebDAVSyncProvider.h`
- CREATE: `src/core/WebDAVSyncProvider.cpp`
- CREATE: `src/core/CloudSyncService.h`
- CREATE: `src/core/CloudSyncService.cpp`
- CREATE: `src/core/SyncEncryption.h`
- CREATE: `src/core/SyncEncryption.cpp`
- MODIFY: `src/core/Events.h` (add CloudSyncStartedEvent, CloudSyncCompletedEvent, CloudSyncErrorEvent, CloudSyncProgressEvent, CloudProviderConnectedEvent, CloudProviderDisconnectedEvent)
- MODIFY: `src/core/PluginContext.h` (add `CloudSyncService*` pointer)
- MODIFY: `src/app/MarkAmpApp.cpp` (instantiate CloudSyncService, wire to PluginContext)
- MODIFY: `src/CMakeLists.txt` (add S3SyncProvider.cpp, WebDAVSyncProvider.cpp, CloudSyncService.cpp, SyncEncryption.cpp to source list; add libcurl, OpenSSL/libsodium link libraries)
- CREATE: `tests/unit/test_cloud_sync.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_cloud_sync target)
