#pragma once

#include <chrono>
#include <expected>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// HTTP response from a request.
struct HttpResponse
{
    int status_code{0};
    std::string body;
    std::unordered_map<std::string, std::string> headers;
    std::string error; // Non-empty if request failed at transport level
};

/// Configuration for HTTP requests.
struct HttpRequestOptions
{
    std::chrono::seconds timeout{30};
    std::unordered_map<std::string, std::string> headers;
};

/// Thin HTTP client wrapper. Currently uses cpp-httplib internally.
/// Thread-safe: each method creates its own client instance.
class HttpClient
{
public:
    /// Perform a GET request.
    [[nodiscard]] static auto get(const std::string& url, const HttpRequestOptions& options = {})
        -> HttpResponse;

    /// Perform a POST request with a JSON body.
    [[nodiscard]] static auto post_json(const std::string& url,
                                        const std::string& json_body,
                                        const HttpRequestOptions& options = {}) -> HttpResponse;

    /// Download a file from a URL to a local path.
    [[nodiscard]] static auto download(const std::string& url,
                                       const std::string& dest_path,
                                       const HttpRequestOptions& options = {})
        -> std::expected<void, std::string>;
};

} // namespace markamp::core
