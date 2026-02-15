// ============================================================================
// File: src/core/S3Client.h
// Phase 38: Cloud Sync — S3 client implementation
// ============================================================================
#pragma once

#include "CloudSyncTypes.h"
#include "IS3Client.h"

namespace markamp::core
{

// S3Client — concrete IS3Client using libcurl.
// Implements AWS Signature V4 signing for authentication.
class S3Client : public IS3Client
{
public:
    explicit S3Client(const S3Config& config);

    [[nodiscard]] auto put_object(const std::string& key,
                                  const std::filesystem::path& local_path,
                                  const std::string& content_type)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto put_object_data(const std::string& key,
                                       const std::vector<uint8_t>& data,
                                       const std::string& content_type)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto get_object(const std::string& key, const std::filesystem::path& local_path)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto get_object_data(const std::string& key)
        -> std::expected<std::vector<uint8_t>, std::string> override;

    [[nodiscard]] auto delete_object(const std::string& key)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto list_objects(const std::string& prefix, int max_keys) const
        -> std::expected<std::vector<S3ObjectInfo>, std::string> override;

    [[nodiscard]] auto head_object(const std::string& key) const
        -> std::expected<S3ObjectInfo, std::string> override;

    [[nodiscard]] auto test_connection() -> std::expected<void, std::string> override;

private:
    S3Config config_;

    // Build the full URL for a key.
    [[nodiscard]] auto build_url(const std::string& key) const -> std::string;

    // Create AWS Signature V4 authorization header.
    [[nodiscard]] auto sign_request(const std::string& method,
                                    const std::string& url,
                                    const std::string& payload_hash,
                                    const std::string& content_type) const
        -> std::vector<std::string>;

    // Compute SHA-256 hex digest (stub — uses FNV-1a placeholder).
    [[nodiscard]] static auto sha256_hex(const std::string& data) -> std::string;
};

} // namespace markamp::core
