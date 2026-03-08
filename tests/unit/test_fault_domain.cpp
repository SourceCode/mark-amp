/// test_fault_domain.cpp — Comprehensive tests for FaultDomainRegistry
#include "core/FaultDomain.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("FaultDomainRegistry: register and count", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    auto initial = registry.domain_count();
    registry.register_domain("test-domain");
    REQUIRE(registry.domain_count() == initial + 1);
}

TEST_CASE("FaultDomainRegistry: new domain is healthy", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    registry.register_domain("custom");
    REQUIRE(registry.health("custom") == DomainHealth::kHealthy);
    REQUIRE(registry.is_operational("custom"));
}

TEST_CASE("FaultDomainRegistry: record_error transitions health", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    registry.set_thresholds(2, 5); // degraded at 2, faulted at 5
    registry.record_error("test", "err1");
    REQUIRE(registry.health("test") == DomainHealth::kHealthy);
    registry.record_error("test", "err2");
    registry.record_error("test", "err3");
    REQUIRE(registry.health("test") == DomainHealth::kDegraded);
}

TEST_CASE("FaultDomainRegistry: isolate sets kIsolated", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    registry.isolate("test");
    REQUIRE(registry.health("test") == DomainHealth::kIsolated);
    REQUIRE_FALSE(registry.is_operational("test"));
}

TEST_CASE("FaultDomainRegistry: recover resets to healthy", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    registry.isolate("test");
    registry.recover("test");
    REQUIRE(registry.health("test") == DomainHealth::kHealthy);
    REQUIRE(registry.is_operational("test"));
}

TEST_CASE("FaultDomainRegistry: domain_names lists all", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    registry.register_domain("a");
    registry.register_domain("b");
    auto names = registry.domain_names();
    REQUIRE(names.size() >= 2); // May include pre-registered domains
}

TEST_CASE("FaultDomainRegistry: all_domains returns snapshots", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    auto domains = registry.all_domains();
    REQUIRE_FALSE(domains.empty());
}

TEST_CASE("FaultDomainRegistry: set_health works", "[fault_domain][positive]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    registry.set_health("test", DomainHealth::kDegraded);
    REQUIRE(registry.health("test") == DomainHealth::kDegraded);
    REQUIRE(registry.is_operational("test")); // Degraded is still operational
}

// ── Negative Tests ──

TEST_CASE("FaultDomainRegistry: isolated domain is not operational", "[fault_domain][negative]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    registry.set_health("test", DomainHealth::kIsolated);
    REQUIRE_FALSE(registry.is_operational("test"));
}

TEST_CASE("FaultDomainRegistry: faulted domain is not operational", "[fault_domain][negative]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    registry.set_health("test", DomainHealth::kFaulted);
    REQUIRE_FALSE(registry.is_operational("test"));
}

// ── Edge Cases ──

TEST_CASE("FaultDomainRegistry: recover after multiple errors", "[fault_domain][edge]")
{
    FaultDomainRegistry registry;
    registry.register_domain("test");
    registry.set_thresholds(1, 3);
    for (int i = 0; i < 5; ++i)
    {
        registry.record_error("test", "err");
    }
    registry.recover("test");
    REQUIRE(registry.health("test") == DomainHealth::kHealthy);
}

TEST_CASE("FaultDomain struct: defaults", "[fault_domain][edge]")
{
    FaultDomain domain;
    REQUIRE(domain.name.empty());
    REQUIRE(domain.health == DomainHealth::kHealthy);
    REQUIRE(domain.error_count == 0);
    REQUIRE(domain.last_error_message.empty());
}

TEST_CASE("DomainHealth: health_name returns string", "[fault_domain][edge]")
{
    REQUIRE_FALSE(health_name(DomainHealth::kHealthy).empty());
    REQUIRE_FALSE(health_name(DomainHealth::kDegraded).empty());
    REQUIRE_FALSE(health_name(DomainHealth::kFaulted).empty());
    REQUIRE_FALSE(health_name(DomainHealth::kIsolated).empty());
}
