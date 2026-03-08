/// test_http_client.cpp — Unit tests for HttpClient types
#include "core/HttpClient.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("HttpResponse: default values", "[http_client]")
{
    HttpResponse resp;
    REQUIRE(resp.status_code == 0);
    REQUIRE(resp.body.empty());
    REQUIRE(resp.headers.empty());
    REQUIRE(resp.error.empty());
}

TEST_CASE("HttpRequestOptions: default timeout is 30s", "[http_client]")
{
    HttpRequestOptions opts;
    REQUIRE(opts.timeout == std::chrono::seconds{30});
    REQUIRE(opts.headers.empty());
}

TEST_CASE("HttpResponse: can set fields", "[http_client]")
{
    HttpResponse resp;
    resp.status_code = 200;
    resp.body = "{\"ok\":true}";
    resp.headers["Content-Type"] = "application/json";
    REQUIRE(resp.status_code == 200);
    REQUIRE(resp.headers.size() == 1);
}

TEST_CASE("HttpRequestOptions: custom headers", "[http_client]")
{
    HttpRequestOptions opts;
    opts.headers["Authorization"] = "Bearer token123";
    opts.timeout = std::chrono::seconds{60};
    REQUIRE(opts.headers.count("Authorization") == 1);
    REQUIRE(opts.timeout == std::chrono::seconds{60});
}
