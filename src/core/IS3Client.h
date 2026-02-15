// ============================================================================
// File: src/core/IS3Client.h
// Phase 38: Cloud Sync — S3 client interface
// ============================================================================
#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// S3 object metadata.
struct S3ObjectInfo
{
    std::string key;
    int64_t size_bytes{0};
    std::string etag;
    std::string last_modified;
    std::string content_type;
};

// IS3Client — abstract interface for S3 operations.
class IS3Client
{
public:
    virtual ~IS3Client() = default;

    // Upload a file to S3.
    [[nodiscard]] virtual auto
    put_object(const std::string& key,
               const std::filesystem::path& local_path,
               const std::string& content_type = "application/octet-stream")
        -> std::expected<void, std::string> = 0;

    // Upload raw data to S3.
    [[nodiscard]] virtual auto
    put_object_data(const std::string& key,
                    const std::vector<uint8_t>& data,
                    const std::string& content_type = "application/octet-stream")
        -> std::expected<void, std::string> = 0;

    // Download an object from S3 to local file.
    [[nodiscard]] virtual auto get_object(const std::string& key,
                                          const std::filesystem::path& local_path)
        -> std::expected<void, std::string> = 0;

    // Download an object as raw data.
    [[nodiscard]] virtual auto get_object_data(const std::string& key)
        -> std::expected<std::vector<uint8_t>, std::string> = 0;

    // Delete an object.
    [[nodiscard]] virtual auto delete_object(const std::string& key)
        -> std::expected<void, std::string> = 0;

    // List objects with a given prefix.
    [[nodiscard]] virtual auto list_objects(const std::string& prefix, int max_keys = 1000) const
        -> std::expected<std::vector<S3ObjectInfo>, std::string> = 0;

    // Check if an object exists and get its metadata.
    [[nodiscard]] virtual auto head_object(const std::string& key) const
        -> std::expected<S3ObjectInfo, std::string> = 0;

    // Test connectivity to the S3 endpoint.
    [[nodiscard]] virtual auto test_connection() -> std::expected<void, std::string> = 0;
};

} // namespace markamp::core
