// test_network_interceptor.cpp — 10 tests for NetworkInterceptor
#include "core/RuntimePolicy.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("NetworkInterceptor starts with no permissions", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    CHECK_FALSE(interceptor.has_permission("any-ext"));
}

TEST_CASE("NetworkInterceptor grant_permission enables access", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-network");
    CHECK(interceptor.has_permission("ext-network"));
}

TEST_CASE("NetworkInterceptor revoke_permission removes access", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-1");
    interceptor.revoke_permission("ext-1");
    CHECK_FALSE(interceptor.has_permission("ext-1"));
}

TEST_CASE("NetworkInterceptor intercept allowed returns success", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-1");
    auto result = interceptor.intercept("ext-1", "https://api.example.com/data");
    CHECK(result.has_value());
}

TEST_CASE("NetworkInterceptor intercept blocked returns error", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    auto result = interceptor.intercept("blocked-ext", "https://evil.com");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("NetworkInterceptor blocked_count tracks blocks", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    (void)interceptor.intercept("no-perm", "https://a.com");
    (void)interceptor.intercept("no-perm", "https://b.com");
    CHECK(interceptor.blocked_count() == 2);
}

TEST_CASE("NetworkInterceptor audit_log records all attempts", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("allowed");
    (void)interceptor.intercept("allowed", "https://safe.com");
    (void)interceptor.intercept("denied", "https://blocked.com");
    auto log = interceptor.audit_log();
    CHECK(log.size() == 2);
}

TEST_CASE("NetworkInterceptor AuditEntry defaults", "[policy][interceptor]")
{
    NetworkInterceptor::AuditEntry entry;
    CHECK(entry.extension_id.empty());
    CHECK(entry.url.empty());
    CHECK_FALSE(entry.allowed);
    CHECK(entry.timestamp_ms == 0);
}

TEST_CASE("NetworkInterceptor multiple extensions", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-a");
    interceptor.grant_permission("ext-b");
    CHECK(interceptor.has_permission("ext-a"));
    CHECK(interceptor.has_permission("ext-b"));
    CHECK_FALSE(interceptor.has_permission("ext-c"));
}

TEST_CASE("NetworkInterceptor allowed audit entry has allowed flag", "[policy][interceptor]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-1");
    (void)interceptor.intercept("ext-1", "https://api.example.com");
    auto log = interceptor.audit_log();
    REQUIRE_FALSE(log.empty());
    CHECK(log.back().allowed);
    CHECK(log.back().url == "https://api.example.com");
}
