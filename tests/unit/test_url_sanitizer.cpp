// test_url_sanitizer.cpp — 10 tests for UrlSanitizer
#include "core/UrlSanitizer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("UrlSanitizer allows https URLs", "[security][url]")
{
    UrlSanitizer sanitizer;
    CHECK(sanitizer.is_safe_url("https://example.com"));
}

TEST_CASE("UrlSanitizer allows http URLs", "[security][url]")
{
    UrlSanitizer sanitizer;
    CHECK(sanitizer.is_safe_url("http://example.com"));
}

TEST_CASE("UrlSanitizer blocks javascript scheme", "[security][url]")
{
    UrlSanitizer sanitizer;
    CHECK_FALSE(sanitizer.is_safe_url("javascript:alert(1)"));
}

TEST_CASE("UrlSanitizer extract_scheme extracts correctly", "[security][url]")
{
    auto scheme = UrlSanitizer::extract_scheme("https://example.com/path");
    CHECK(scheme == "https");
    auto ftp = UrlSanitizer::extract_scheme("ftp://files.example.com");
    CHECK(ftp == "ftp");
}

TEST_CASE("UrlSanitizer is_local_url detects localhost", "[security][url]")
{
    CHECK(UrlSanitizer::is_local_url("http://localhost:8080"));
    CHECK(UrlSanitizer::is_local_url("http://127.0.0.1"));
    CHECK_FALSE(UrlSanitizer::is_local_url("https://example.com"));
}

TEST_CASE("UrlSanitizer is_protocol_relative", "[security][url]")
{
    CHECK(UrlSanitizer::is_protocol_relative("//example.com/style.css"));
    CHECK_FALSE(UrlSanitizer::is_protocol_relative("https://example.com"));
}

TEST_CASE("UrlSanitizer normalize lowercases scheme", "[security][url]")
{
    auto normalized = UrlSanitizer::normalize("HTTPS://EXAMPLE.COM");
    CHECK(normalized.substr(0, 5) == "https");
}

TEST_CASE("UrlSanitizer allow_scheme adds custom scheme", "[security][url]")
{
    UrlSanitizer sanitizer;
    sanitizer.allow_scheme("custom");
    auto schemes = sanitizer.allowed_schemes();
    bool found = false;
    for (const auto& scheme : schemes)
    {
        if (scheme == "custom")
        {
            found = true;
            break;
        }
    }
    CHECK(found);
}

TEST_CASE("UrlSanitizer blocked_schemes includes dangerous ones", "[security][url]")
{
    auto blocked = UrlSanitizer::blocked_schemes();
    CHECK_FALSE(blocked.empty());
}

TEST_CASE("UrlSanitizer sanitize returns safe URL", "[security][url]")
{
    UrlSanitizer sanitizer;
    auto result = sanitizer.sanitize("https://example.com/page?q=test");
    CHECK(result.has_value());
}
