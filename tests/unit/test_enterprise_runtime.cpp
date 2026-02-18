/// test_enterprise_runtime.cpp — V7 Phases 39-40: Enterprise runtime & chaos testing

#include "core/ChaosPlugin.h"
#include "core/RuntimePolicy.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// Runtime Policy
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("RuntimePolicy: not immutable by default", "[enterprise]")
{
    RuntimePolicy policy;
    REQUIRE_FALSE(policy.is_immutable());
}

TEST_CASE("RuntimePolicy: --immutable flag", "[enterprise]")
{
    RuntimePolicy policy;
    policy.process_args({"--immutable"});
    REQUIRE(policy.is_immutable());
}

TEST_CASE("RuntimePolicy: blocklist blocks extension", "[enterprise]")
{
    RuntimePolicy policy;
    REQUIRE(policy.is_extension_allowed("malicious-ext"));

    policy.block_extension("malicious-ext");
    REQUIRE_FALSE(policy.is_extension_allowed("malicious-ext"));

    policy.unblock_extension("malicious-ext");
    REQUIRE(policy.is_extension_allowed("malicious-ext"));
}

TEST_CASE("RuntimePolicy: allowlist restricts to listed", "[enterprise]")
{
    RuntimePolicy policy;
    policy.allow_extension("approved-ext");

    REQUIRE(policy.is_extension_allowed("approved-ext"));
    REQUIRE_FALSE(policy.is_extension_allowed("other-ext"));
}

TEST_CASE("RuntimePolicy: blocklist takes precedence over allowlist", "[enterprise]")
{
    RuntimePolicy policy;
    policy.allow_extension("ext-a");
    policy.block_extension("ext-a");

    REQUIRE_FALSE(policy.is_extension_allowed("ext-a"));
}

// ══════════════════════════════════════════════════════════════════════════════
// Network Interceptor
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("NetworkInterceptor: blocks by default", "[enterprise]")
{
    NetworkInterceptor interceptor;
    auto result = interceptor.intercept("some-ext", "https://api.example.com");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::PermissionDenied);
    REQUIRE(interceptor.blocked_count() == 1);
}

TEST_CASE("NetworkInterceptor: allows with permission", "[enterprise]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("trusted-ext");

    auto result = interceptor.intercept("trusted-ext", "https://api.example.com");
    REQUIRE(result.has_value());
    REQUIRE(interceptor.blocked_count() == 0);
}

TEST_CASE("NetworkInterceptor: audit log records all calls", "[enterprise]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-a");

    (void)interceptor.intercept("ext-a", "https://allowed.com");
    (void)interceptor.intercept("ext-b", "https://blocked.com");

    auto log = interceptor.audit_log();
    REQUIRE(log.size() == 2);
    REQUIRE(log[0].allowed);
    REQUIRE_FALSE(log[1].allowed);
}

TEST_CASE("NetworkInterceptor: revoke removes permission", "[enterprise]")
{
    NetworkInterceptor interceptor;
    interceptor.grant_permission("ext-x");
    interceptor.revoke_permission("ext-x");

    auto result = interceptor.intercept("ext-x", "https://api.com");
    REQUIRE_FALSE(result.has_value());
}

// ══════════════════════════════════════════════════════════════════════════════
// Chaos Plugin
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("ChaosPlugin: none behavior succeeds", "[enterprise]")
{
    ChaosPlugin plugin(ChaosBehavior::kNone);
    REQUIRE(plugin.activate().has_value());
    REQUIRE(plugin.deactivate().has_value());
    REQUIRE(plugin.on_event("test").has_value());
}

TEST_CASE("ChaosPlugin: throw on activate", "[enterprise]")
{
    ChaosPlugin plugin(ChaosBehavior::kThrowOnActivate);
    auto result = plugin.activate();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::PluginError);
    REQUIRE(plugin.activation_attempts() == 1);
}

TEST_CASE("ChaosPlugin: throw on deactivate", "[enterprise]")
{
    ChaosPlugin plugin(ChaosBehavior::kThrowOnDeactivate);
    auto result = plugin.deactivate();
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ChaosPlugin: throw on event", "[enterprise]")
{
    ChaosPlugin plugin(ChaosBehavior::kThrowOnEvent);
    auto result = plugin.on_event("user.input");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ChaosPlugin: event flood counting", "[enterprise]")
{
    ChaosPlugin plugin(ChaosBehavior::kEventFlood);
    REQUIRE(plugin.on_event("tick").has_value());
    REQUIRE(plugin.flood_count() == 1000);
}

TEST_CASE("ChaosPlugin: behavior names", "[enterprise]")
{
    REQUIRE(behavior_name(ChaosBehavior::kNone) == "None");
    REQUIRE(behavior_name(ChaosBehavior::kThrowOnActivate) == "Throw on Activate");
    REQUIRE(behavior_name(ChaosBehavior::kEventFlood) == "Event Flood");
}

TEST_CASE("ChaosPlugin: set_behavior changes mode", "[enterprise]")
{
    ChaosPlugin plugin;
    REQUIRE(plugin.behavior() == ChaosBehavior::kNone);
    plugin.set_behavior(ChaosBehavior::kBusyLoop);
    REQUIRE(plugin.behavior() == ChaosBehavior::kBusyLoop);
}
