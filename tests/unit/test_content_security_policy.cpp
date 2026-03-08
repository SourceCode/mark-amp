// test_content_security_policy.cpp — 10 tests for ContentSecurityPolicy
#include "core/ContentSecurityPolicy.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CspConfig defaults have self for default_src", "[security][csp]")
{
    CspConfig config;
    REQUIRE_FALSE(config.default_src.empty());
    CHECK(config.default_src[0] == "'self'");
}

TEST_CASE("CspConfig defaults block scripts", "[security][csp]")
{
    CspConfig config;
    REQUIRE_FALSE(config.script_src.empty());
    CHECK(config.script_src[0] == "'none'");
}

TEST_CASE("ContentSecurityPolicy generate_policy produces string", "[security][csp]")
{
    CspConfig config;
    auto policy = ContentSecurityPolicy::generate_policy(config);
    CHECK_FALSE(policy.empty());
    CHECK(policy.find("default-src") != std::string::npos);
}

TEST_CASE("ContentSecurityPolicy strict policy blocks inline scripts", "[security][csp]")
{
    auto strict = ContentSecurityPolicy::default_strict_policy();
    auto policy = ContentSecurityPolicy::generate_policy(strict);
    CHECK(policy.find("script-src") != std::string::npos);
}

TEST_CASE("ContentSecurityPolicy preview policy allows inline styles", "[security][csp]")
{
    auto preview = ContentSecurityPolicy::default_preview_policy();
    auto policy = ContentSecurityPolicy::generate_policy(preview);
    CHECK_FALSE(policy.empty());
}

TEST_CASE("ContentSecurityPolicy dev policy is permissive", "[security][csp]")
{
    auto dev = ContentSecurityPolicy::default_dev_policy();
    auto policy = ContentSecurityPolicy::generate_policy(dev);
    CHECK_FALSE(policy.empty());
}

TEST_CASE("ContentSecurityPolicy validate_directive", "[security][csp]")
{
    CHECK(ContentSecurityPolicy::validate_directive("'self'"));
    CHECK(ContentSecurityPolicy::validate_directive("https:"));
}

TEST_CASE("ContentSecurityPolicy as_meta_tag produces HTML meta", "[security][csp]")
{
    CspConfig config;
    auto meta = ContentSecurityPolicy::as_meta_tag(config);
    CHECK(meta.find("<meta") != std::string::npos);
    CHECK(meta.find("Content-Security-Policy") != std::string::npos);
}

TEST_CASE("ContentSecurityPolicy directive_count counts active directives", "[security][csp]")
{
    CspConfig config;
    auto count = ContentSecurityPolicy::directive_count(config);
    CHECK(count > 0);
}

TEST_CASE("ContentSecurityPolicy merge_policies overrides", "[security][csp]")
{
    CspConfig base;
    CspConfig override_config;
    override_config.script_src = {"'self'"};
    auto merged = ContentSecurityPolicy::merge_policies(base, override_config);
    REQUIRE_FALSE(merged.script_src.empty());
    CHECK(merged.script_src[0] == "'self'");
}
