// ============================================================================
// File: src/core/S3Client.cpp
// Phase 38: Cloud Sync — S3Client implementation (stub with curl skeleton)
// ============================================================================

#include "S3Client.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

S3Client::S3Client(const S3Config& config)
    : config_(config)
{
}

auto S3Client::put_object(const std::string& key,
                          const std::filesystem::path& local_path,
                          const std::string& content_type) -> std::expected<void, std::string>
{
    if (!std::filesystem::exists(local_path))
    {
        return std::unexpected("File not found: " + local_path.string());
    }

    // Read file into buffer.
    std::ifstream file(local_path, std::ios::binary);
    if (!file)
        return std::unexpected("Cannot open file: " + local_path.string());

    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    return put_object_data(key, data, content_type);
}

auto S3Client::put_object_data(const std::string& key,
                               const std::vector<uint8_t>& data,
                               const std::string& content_type) -> std::expected<void, std::string>
{
    auto url = build_url(key);
    auto payload_hash = sha256_hex(std::string(data.begin(), data.end()));
    auto headers = sign_request("PUT", url, payload_hash, content_type);

    // Stub: In production, use libcurl to perform PUT request.
    // curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    // curl_easy_setopt(curl, CURLOPT_READDATA, &data);
    // curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, data.size());
    // Set headers from sign_request().
    (void)headers;

    return {};
}

auto S3Client::get_object(const std::string& key, const std::filesystem::path& local_path)
    -> std::expected<void, std::string>
{
    auto data_result = get_object_data(key);
    if (!data_result)
        return std::unexpected(data_result.error());

    std::error_code ec;
    std::filesystem::create_directories(local_path.parent_path(), ec);

    std::ofstream file(local_path, std::ios::binary);
    if (!file)
        return std::unexpected("Cannot create file: " + local_path.string());
    file.write(reinterpret_cast<const char*>(data_result->data()),
               static_cast<std::streamsize>(data_result->size()));

    return {};
}

auto S3Client::get_object_data(const std::string& key)
    -> std::expected<std::vector<uint8_t>, std::string>
{
    auto url = build_url(key);
    auto headers = sign_request("GET", url, sha256_hex(""), "");

    // Stub: In production, use libcurl to perform GET request.
    (void)headers;

    return std::unexpected("S3 GET not yet implemented (requires libcurl)");
}

auto S3Client::delete_object(const std::string& key) -> std::expected<void, std::string>
{
    auto url = build_url(key);
    auto headers = sign_request("DELETE", url, sha256_hex(""), "");

    // Stub: curl DELETE.
    (void)headers;

    return {};
}

auto S3Client::list_objects(const std::string& prefix, int max_keys) const
    -> std::expected<std::vector<S3ObjectInfo>, std::string>
{
    // Stub: ListObjectsV2 via curl GET with query params.
    (void)prefix;
    (void)max_keys;
    return std::vector<S3ObjectInfo>{};
}

auto S3Client::head_object(const std::string& key) const -> std::expected<S3ObjectInfo, std::string>
{
    // Stub: HEAD request.
    (void)key;
    return std::unexpected("S3 HEAD not yet implemented");
}

auto S3Client::test_connection() -> std::expected<void, std::string>
{
    auto result = list_objects("", 1);
    if (!result)
        return std::unexpected(result.error());
    return {};
}

auto S3Client::build_url(const std::string& key) const -> std::string
{
    std::string protocol = config_.use_ssl ? "https" : "http";
    if (config_.path_style)
    {
        return protocol + "://" + config_.endpoint + "/" + config_.bucket + "/" + config_.prefix +
               key;
    }
    return protocol + "://" + config_.bucket + "." + config_.endpoint + "/" + config_.prefix + key;
}

auto S3Client::sign_request(const std::string& method,
                            const std::string& url,
                            const std::string& payload_hash,
                            const std::string& content_type) const -> std::vector<std::string>
{
    // Stub: AWS Signature V4 implementation.
    // In production: canonical request → string to sign → signing key → signature.
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream date_ss;
    date_ss << std::put_time(std::gmtime(&time_t), "%Y%m%dT%H%M%SZ");

    std::vector<std::string> headers;
    headers.push_back("x-amz-date: " + date_ss.str());
    headers.push_back("x-amz-content-sha256: " + payload_hash);
    if (!content_type.empty())
    {
        headers.push_back("Content-Type: " + content_type);
    }
    // Authorization header would be computed here with Sig V4.
    headers.push_back("Authorization: AWS4-HMAC-SHA256 Credential=" + config_.access_key_id +
                      "/...");

    (void)method;
    (void)url;
    return headers;
}

auto S3Client::sha256_hex(const std::string& data) -> std::string
{
    // Stub: FNV-1a hash as placeholder for SHA-256.
    uint64_t hash = 14695981039346656037ULL;
    for (char c : data)
    {
        hash ^= static_cast<uint8_t>(c);
        hash *= 1099511628211ULL;
    }
    std::ostringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

} // namespace markamp::core
