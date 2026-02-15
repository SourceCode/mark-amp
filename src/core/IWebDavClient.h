// ============================================================================
// File: src/core/IWebDavClient.h
// Phase 38: Cloud Sync — WebDAV client interface
// ============================================================================
#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// WebDAV resource metadata.
struct WebDavResource
{
    std::string path;
    std::string display_name;
    int64_t size_bytes{0};
    std::string last_modified;
    std::string content_type;
    std::string etag;
    bool is_collection{false}; // true = directory
};

// IWebDavClient — abstract interface for WebDAV operations.
class IWebDavClient
{
public:
    virtual ~IWebDavClient() = default;

    // Upload a file to WebDAV.
    [[nodiscard]] virtual auto upload(const std::string& remote_path,
                                      const std::filesystem::path& local_path)
        -> std::expected<void, std::string> = 0;

    // Upload raw data.
    [[nodiscard]] virtual auto
    upload_data(const std::string& remote_path,
                const std::vector<uint8_t>& data,
                const std::string& content_type = "application/octet-stream")
        -> std::expected<void, std::string> = 0;

    // Download a file from WebDAV.
    [[nodiscard]] virtual auto download(const std::string& remote_path,
                                        const std::filesystem::path& local_path)
        -> std::expected<void, std::string> = 0;

    // Download as raw data.
    [[nodiscard]] virtual auto download_data(const std::string& remote_path)
        -> std::expected<std::vector<uint8_t>, std::string> = 0;

    // Delete a resource.
    [[nodiscard]] virtual auto delete_resource(const std::string& remote_path)
        -> std::expected<void, std::string> = 0;

    // List resources in a directory (PROPFIND).
    [[nodiscard]] virtual auto list(const std::string& remote_path) const
        -> std::expected<std::vector<WebDavResource>, std::string> = 0;

    // Create a directory (MKCOL).
    [[nodiscard]] virtual auto mkdir(const std::string& remote_path)
        -> std::expected<void, std::string> = 0;

    // Check if a resource exists.
    [[nodiscard]] virtual auto exists(const std::string& remote_path) const -> bool = 0;

    // Test connectivity.
    [[nodiscard]] virtual auto test_connection() -> std::expected<void, std::string> = 0;
};

} // namespace markamp::core
