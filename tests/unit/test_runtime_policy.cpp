// test_runtime_policy.cpp — 10 tests for RuntimePolicy and NetworkInterceptor
#include "core/RuntimePolicy.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("RuntimePolicy default is not immutable", "[policy][runtime]")
{
    RuntimePolicy policy;
    CHECK_FALSE(policy.is_immutable());
}

TEST_CASE("RuntimePolicy set_immutable toggles", "[policy][runtime]")
{
    RuntimePolicy policy;
    policy.set_immutable(true);
    CHECK(policy.is_immutable());
    policy.set_immutable(false);
    CHECK_FALSE(policy.is_immutable());
}

TEST_CASE("RuntimePolicy extension blocklist", "[policy][runtime]")
{
    RuntimePolicy policy;
    policy.block_extension("malicious-ext");
    CHECK_FALSE(policy.is_extension_allowed("malicious-ext"));
    auto blocked = policy.blocklist();
    CHECK_FALSE(blocked.empty());
}

TEST_CASE("RuntimePolicy unblock_extension", "[policy][runtime]")
{
    RuntimePolicy policy;
    policy.block_extension("ext-1");
    policy.unblock_extension("ext-1");
    CHECK(policy.is_extension_allowed("ext-1"));
}

TEST_CASE("RuntimePolicy extension allowlist", "[policy][runtime]")
{
    RuntimePolicy policy;
    policy.allow_extension("trusted-ext");
    CHECK(policy.is_extension_allowed("trusted-ext"));
    CHECK_FALSE(policy.is_extension_allowed("unknown-ext"));
}

TEST_CASE("RuntimePolicy process_args sets immutable", "[policy][runtime]")
{
    RuntimePolicy policy;
    policy.process_args({"--immutable"});
    CHECK(policy.is_immutable());
}

TEST_CASE("NetworkInterceptor default no permissions", "[policy][network]")
{
    NetworkInterceptor interceptor;
    CHECK_FALSE(interceptor.has_permission("any-ext"));
    CHECK(interceptor.blocked_count() == 0);
}

TEST_CASE("NetworkInterceptor grant and check permission", "[policy][network]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-1");
    CHECK(interceptor.has_permission("ext-1"));
}

TEST_CASE("NetworkInterceptor revoke_permission", "[policy][network]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-1");
    interceptor.revoke_permission("ext-1");
    CHECK_FALSE(interceptor.has_permission("ext-1"));
}

TEST_CASE("NetworkInterceptor intercept allowed produces audit entry", "[policy][network]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-1");
    auto result = interceptor.intercept("ext-1", "https://api.example.com");
    CHECK(result.has_value());
    auto log = interceptor.audit_log();
    CHECK_FALSE(log.empty());
    CHECK(log.back().allowed);
}
