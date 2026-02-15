// ============================================================================
// File: src/core/WebDavClient.h
// Phase 38: Cloud Sync — WebDAV client implementation
// ============================================================================
#pragma once

#include "CloudSyncTypes.h"
#include "IWebDavClient.h"

namespace markamp::core
{

// WebDavClient — concrete IWebDavClient using libcurl.
class WebDavClient : public IWebDavClient
{
public:
    explicit WebDavClient(const WebDavConfig& config);

    [[nodiscard]] auto upload(const std::string& remote_path,
                              const std::filesystem::path& local_path)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto upload_data(const std::string& remote_path,
                                   const std::vector<uint8_t>& data,
                                   const std::string& content_type)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto download(const std::string& remote_path,
                                const std::filesystem::path& local_path)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto download_data(const std::string& remote_path)
        -> std::expected<std::vector<uint8_t>, std::string> override;

    [[nodiscard]] auto delete_resource(const std::string& remote_path)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto list(const std::string& remote_path) const
        -> std::expected<std::vector<WebDavResource>, std::string> override;

    [[nodiscard]] auto mkdir(const std::string& remote_path)
        -> std::expected<void, std::string> override;

    [[nodiscard]] auto exists(const std::string& remote_path) const -> bool override;

    [[nodiscard]] auto test_connection() -> std::expected<void, std::string> override;

private:
    WebDavConfig config_;

    // Build full URL from relative path.
    [[nodiscard]] auto build_url(const std::string& remote_path) const -> std::string;
};

} // namespace markamp::core
