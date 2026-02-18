// ============================================================================
// File: src/core/CloudSyncService.h
// Phase 38: Cloud Sync — Cloud sync orchestration with encryption
// ============================================================================
#pragma once

#include "CloudSyncTypes.h"
#include "IS3Client.h"
#include "IWebDavClient.h"
#include "SyncTypes.h"

#include <expected>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class SyncEngine;

// Progress callback: (current_file, percent, status).
using CloudProgressCallback = std::function<void(const std::string&, int, CloudSyncStatus)>;

// CloudSyncService — orchestrates cloud sync with end-to-end encryption.
class CloudSyncService
{
public:
    CloudSyncService(EventBus& event_bus, Config& config, SyncEngine& sync_engine);

    // Perform a full cloud sync.
    [[nodiscard]] auto sync(const CloudSyncConfig& cloud_config,
                            CloudProgressCallback progress = nullptr) -> CloudSyncResult;

    // Upload the entire local workspace to cloud.
    [[nodiscard]] auto push(const CloudSyncConfig& cloud_config,
                            CloudProgressCallback progress = nullptr) -> CloudSyncResult;

    // Download the entire cloud workspace to local.
    [[nodiscard]] auto pull(const CloudSyncConfig& cloud_config,
                            CloudProgressCallback progress = nullptr) -> CloudSyncResult;

    // Test connectivity to the cloud provider.
    [[nodiscard]] auto test_connection(const CloudSyncConfig& cloud_config)
        -> std::expected<void, std::string>;

    // Encrypt data using AES-256-GCM with key derived from passphrase.
    [[nodiscard]] auto encrypt(const std::vector<uint8_t>& plaintext,
                               const EncryptionConfig& enc_config) const
        -> std::expected<std::vector<uint8_t>, std::string>;

    // Decrypt data using AES-256-GCM.
    [[nodiscard]] auto decrypt(const std::vector<uint8_t>& ciphertext,
                               const EncryptionConfig& enc_config) const
        -> std::expected<std::vector<uint8_t>, std::string>;

    // Derive encryption key from passphrase using Argon2id.
    [[nodiscard]] auto derive_key(const std::string& passphrase,
                                  const EncryptionConfig& enc_config) const
        -> std::expected<std::vector<uint8_t>, std::string>;

    // Get the last sync result.
    [[nodiscard]] auto last_result() const -> const CloudSyncResult&;

private:
    [[maybe_unused]] EventBus& event_bus_;
    [[maybe_unused]] Config& config_;
    SyncEngine& sync_engine_;
    CloudSyncResult last_result_;
    mutable std::mutex mutex_;

    // Create the appropriate cloud client based on provider.
    [[nodiscard]] auto create_s3_client(const S3Config& s3_config) const
        -> std::unique_ptr<IS3Client>;
    [[nodiscard]] auto create_webdav_client(const WebDavConfig& webdav_config) const
        -> std::unique_ptr<IWebDavClient>;

    // Upload a snapshot to cloud storage.
    [[nodiscard]] auto upload_snapshot(const SyncSnapshot& snapshot,
                                       const CloudSyncConfig& cloud_config,
                                       CloudProgressCallback progress)
        -> std::expected<int32_t, std::string>;

    // Download a snapshot from cloud storage.
    [[nodiscard]] auto download_snapshot(const CloudSyncConfig& cloud_config,
                                         const std::filesystem::path& target_dir,
                                         CloudProgressCallback progress)
        -> std::expected<int32_t, std::string>;

    // Encrypt a file if encryption is enabled.
    [[nodiscard]] auto encrypt_file(const std::filesystem::path& source,
                                    const std::filesystem::path& target,
                                    const EncryptionConfig& enc_config) const
        -> std::expected<void, std::string>;

    // Decrypt a file if encryption is enabled.
    [[nodiscard]] auto decrypt_file(const std::filesystem::path& source,
                                    const std::filesystem::path& target,
                                    const EncryptionConfig& enc_config) const
        -> std::expected<void, std::string>;
};

} // namespace markamp::core
