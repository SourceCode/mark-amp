// ============================================================================
// File: src/core/S3Client.cpp
// Phase 38: Cloud Sync — S3Client implementation (stub with curl skeleton)
// ============================================================================

#include "S3Client.h"

#include <array>
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
    if (url.empty())
    {
        return std::unexpected("Empty URL for GET");
    }

    // Build AWS v4-signed GET request.
    auto payload_hash = sha256_hex("");
    auto headers = sign_request("GET", url, payload_hash, "");

    // libcurl integration: GET request with AWS auth headers.
    // curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    // Set signed headers from sign_request().
    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    (void)headers;

    return std::unexpected("S3 GET requires libcurl runtime");
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
    auto url = build_url(key);
    if (url.empty())
    {
        return std::unexpected("Empty URL for HEAD");
    }

    // Build AWS v4-signed HEAD request to check object existence and metadata.
    auto payload_hash = sha256_hex("");
    auto headers = sign_request("HEAD", url, payload_hash, "");

    // libcurl integration: HEAD request.
    // curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    // Parse response headers for Content-Length, Last-Modified, ETag.
    (void)headers;

    // Return object info from response headers.
    S3ObjectInfo info;
    info.key = key;
    info.size_bytes = 0;  // Would come from Content-Length header.
    info.etag = "";       // Would come from ETag header.

    return std::unexpected("S3 HEAD requires libcurl runtime");
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
    // SHA-256 implementation for AWS Signature V4.
    // This is a simplified but correct SHA-256 per FIPS 180-4.
    // For production, link against OpenSSL or a dedicated crypto library.

    // Initial hash values (first 32 bits of fractional parts of sqrt of first 8 primes).
    std::array<uint32_t, 8> hash_state = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    // Round constants.
    static constexpr std::array<uint32_t, 64> kRoundConstants = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4,
        0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe,
        0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f,
        0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
        0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
        0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116,
        0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
        0xc67178f2};

    // Pad message per SHA-256 spec.
    auto padded = std::vector<uint8_t>(data.begin(), data.end());
    const uint64_t original_bits = padded.size() * 8;
    padded.push_back(0x80);
    while (padded.size() % 64 != 56)
    {
        padded.push_back(0x00);
    }
    for (int shift = 56; shift >= 0; shift -= 8)
    {
        padded.push_back(static_cast<uint8_t>((original_bits >> shift) & 0xFF));
    }

    auto rotr = [](uint32_t val, unsigned int n) -> uint32_t
    { return (val >> n) | (val << (32 - n)); };

    // Process each 512-bit block.
    for (size_t block = 0; block < padded.size(); block += 64)
    {
        std::array<uint32_t, 64> schedule{};
        for (size_t idx = 0; idx < 16; ++idx)
        {
            schedule[idx] =
                (static_cast<uint32_t>(padded[block + idx * 4]) << 24) |
                (static_cast<uint32_t>(padded[block + idx * 4 + 1]) << 16) |
                (static_cast<uint32_t>(padded[block + idx * 4 + 2]) << 8) |
                static_cast<uint32_t>(padded[block + idx * 4 + 3]);
        }
        for (size_t idx = 16; idx < 64; ++idx)
        {
            const uint32_t s0_val = rotr(schedule[idx - 15], 7) ^ rotr(schedule[idx - 15], 18) ^ (schedule[idx - 15] >> 3);
            const uint32_t s1_val = rotr(schedule[idx - 2], 17) ^ rotr(schedule[idx - 2], 19) ^ (schedule[idx - 2] >> 10);
            schedule[idx] = schedule[idx - 16] + s0_val + schedule[idx - 7] + s1_val;
        }

        auto a_val = hash_state[0];
        auto b_val = hash_state[1];
        auto c_val = hash_state[2];
        auto d_val = hash_state[3];
        auto e_val = hash_state[4];
        auto f_val = hash_state[5];
        auto g_val = hash_state[6];
        auto h_val = hash_state[7];

        for (size_t idx = 0; idx < 64; ++idx)
        {
            const uint32_t sig1 = rotr(e_val, 6) ^ rotr(e_val, 11) ^ rotr(e_val, 25);
            const uint32_t choose = (e_val & f_val) ^ (~e_val & g_val);
            const uint32_t temp1 = h_val + sig1 + choose + kRoundConstants[idx] + schedule[idx];
            const uint32_t sig0 = rotr(a_val, 2) ^ rotr(a_val, 13) ^ rotr(a_val, 22);
            const uint32_t maj = (a_val & b_val) ^ (a_val & c_val) ^ (b_val & c_val);
            const uint32_t temp2 = sig0 + maj;

            h_val = g_val;
            g_val = f_val;
            f_val = e_val;
            e_val = d_val + temp1;
            d_val = c_val;
            c_val = b_val;
            b_val = a_val;
            a_val = temp1 + temp2;
        }

        hash_state[0] += a_val;
        hash_state[1] += b_val;
        hash_state[2] += c_val;
        hash_state[3] += d_val;
        hash_state[4] += e_val;
        hash_state[5] += f_val;
        hash_state[6] += g_val;
        hash_state[7] += h_val;
    }

    // Format as 64-char hex string.
    std::ostringstream ss;
    for (uint32_t word : hash_state)
    {
        ss << std::hex << std::setfill('0') << std::setw(8) << word;
    }
    return ss.str();
}

} // namespace markamp::core
