#include "HttpClient.h"

#include "Logger.h"

#include <filesystem>
#include <fstream>
#include <httplib.h>
#include <regex>

namespace markamp::core
{

namespace
{

/// Parse a URL into host and path components.
struct UrlParts
{
    std::string scheme;
    std::string host;
    std::string path;
};

auto parse_url(const std::string& url) -> UrlParts
{
    UrlParts parts;

    // Simple URL parsing: scheme://host/path
    auto scheme_end = url.find("://");
    if (scheme_end == std::string::npos)
    {
        parts.scheme = "http";
        scheme_end = 0;
    }
    else
    {
        parts.scheme = url.substr(0, scheme_end);
        scheme_end += 3;
    }

    auto path_start = url.find('/', scheme_end);
    if (path_start == std::string::npos)
    {
        parts.host = url.substr(scheme_end);
        parts.path = "/";
    }
    else
    {
        parts.host = url.substr(scheme_end, path_start - scheme_end);
        parts.path = url.substr(path_start);
    }

    return parts;
}

auto make_client(const UrlParts& url_parts, const HttpRequestOptions& options)
    -> std::unique_ptr<httplib::Client>
{
    const auto base_url = url_parts.scheme + "://" + url_parts.host;
    auto client = std::make_unique<httplib::Client>(base_url);

    client->set_connection_timeout(options.timeout);
    client->set_read_timeout(options.timeout);
    client->set_write_timeout(options.timeout);
    client->set_follow_location(true);

    return client;
}

void apply_headers(httplib::Headers& target,
                   const std::unordered_map<std::string, std::string>& source)
{
    for (const auto& [key, value] : source)
    {
        target.emplace(key, value);
    }
}

} // anonymous namespace

auto HttpClient::get(const std::string& url, const HttpRequestOptions& options) -> HttpResponse
{
    HttpResponse response;
    const auto url_parts = parse_url(url);
    auto client = make_client(url_parts, options);

    httplib::Headers request_headers;
    apply_headers(request_headers, options.headers);

    auto result = client->Get(url_parts.path, request_headers);

    if (!result)
    {
        response.error = "HTTP GET failed: " + httplib::to_string(result.error());
        MARKAMP_LOG_WARN("HttpClient GET error: {}", response.error);
        return response;
    }

    response.status_code = result->status;
    response.body = result->body;
    for (const auto& [key, value] : result->headers)
    {
        response.headers[key] = value;
    }

    return response;
}

auto HttpClient::post_json(const std::string& url,
                           const std::string& json_body,
                           const HttpRequestOptions& options) -> HttpResponse
{
    HttpResponse response;
    const auto url_parts = parse_url(url);
    auto client = make_client(url_parts, options);

    httplib::Headers request_headers;
    request_headers.emplace("Content-Type", "application/json");
    request_headers.emplace("Accept", "application/json;api-version=6.1-preview.1");
    apply_headers(request_headers, options.headers);

    auto result = client->Post(url_parts.path, request_headers, json_body, "application/json");

    if (!result)
    {
        response.error = "HTTP POST failed: " + httplib::to_string(result.error());
        MARKAMP_LOG_WARN("HttpClient POST error: {}", response.error);
        return response;
    }

    response.status_code = result->status;
    response.body = result->body;
    for (const auto& [key, value] : result->headers)
    {
        response.headers[key] = value;
    }

    return response;
}

auto HttpClient::download(const std::string& url,
                          const std::string& dest_path,
                          const HttpRequestOptions& options) -> std::expected<void, std::string>
{
    const auto url_parts = parse_url(url);
    auto client = make_client(url_parts, options);

    httplib::Headers request_headers;
    apply_headers(request_headers, options.headers);

    // Ensure parent directory exists
    std::error_code mkdir_err;
    std::filesystem::create_directories(std::filesystem::path(dest_path).parent_path(), mkdir_err);

    std::ofstream file_stream(dest_path, std::ios::binary);
    if (!file_stream.is_open())
    {
        return std::unexpected("Cannot create download file: " + dest_path);
    }

    auto result =
        client->Get(url_parts.path,
                    request_headers,
                    [&file_stream](const char* data, size_t data_length) -> bool
                    {
                        file_stream.write(data, static_cast<std::streamsize>(data_length));
                        return true;
                    });

    file_stream.close();

    if (!result)
    {
        // Clean up partial download
        std::error_code remove_err;
        std::filesystem::remove(dest_path, remove_err);
        return std::unexpected("Download failed: " + httplib::to_string(result.error()));
    }

    if (result->status != 200)
    {
        std::error_code remove_err;
        std::filesystem::remove(dest_path, remove_err);
        return std::unexpected("Download failed with status: " + std::to_string(result->status));
    }

    return {};
}

} // namespace markamp::core
