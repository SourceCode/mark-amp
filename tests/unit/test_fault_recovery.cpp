/// test_fault_recovery.cpp — V7 Phases 37-38: Subsystem reset & fault domain tests

#include "core/FaultDomain.h"
#include "core/SubsystemReset.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// Subsystem Reset
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("SubsystemReset: no handlers initially", "[fault_recovery]")
{
    SubsystemResetCoordinator coordinator;
    REQUIRE(coordinator.handler_count() == 0);
}

TEST_CASE("SubsystemReset: register and reset", "[fault_recovery]")
{
    SubsystemResetCoordinator coordinator;
    bool was_reset = false;

    coordinator.register_handler("Editor",
                                 [&was_reset]() -> Result<void>
                                 {
                                     was_reset = true;
                                     return {};
                                 });

    REQUIRE(coordinator.has_handler("Editor"));
    auto result = coordinator.reset_subsystem("Editor");
    REQUIRE(result.has_value());
    REQUIRE(was_reset);
}

TEST_CASE("SubsystemReset: reset unknown subsystem fails", "[fault_recovery]")
{
    SubsystemResetCoordinator coordinator;
    auto result = coordinator.reset_subsystem("Unknown");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::NotFound);
}

TEST_CASE("SubsystemReset: unregister removes handler", "[fault_recovery]")
{
    SubsystemResetCoordinator coordinator;
    coordinator.register_handler("X", []() -> Result<void> { return {}; });
    coordinator.unregister_handler("X");
    REQUIRE_FALSE(coordinator.has_handler("X"));
}

TEST_CASE("SubsystemReset: reset_all reports per-subsystem", "[fault_recovery]")
{
    SubsystemResetCoordinator coordinator;
    coordinator.register_handler("Ok", []() -> Result<void> { return {}; });
    coordinator.register_handler(
        "Fail",
        []() -> Result<void> {
            return std::unexpected(make_error(ErrorCode::IoError, SubsystemId::Core, "disk fail"));
        });

    auto results = coordinator.reset_all();
    REQUIRE(results.size() == 2);

    bool found_ok = false;
    bool found_fail = false;
    for (const auto& res : results)
    {
        if (res.subsystem_name == "Ok")
        {
            found_ok = res.success;
        }
        if (res.subsystem_name == "Fail")
        {
            found_fail = !res.success;
        }
    }
    REQUIRE(found_ok);
    REQUIRE(found_fail);
}

TEST_CASE("SubsystemReset: registered_subsystems list", "[fault_recovery]")
{
    SubsystemResetCoordinator coordinator;
    coordinator.register_handler("A", []() -> Result<void> { return {}; });
    coordinator.register_handler("B", []() -> Result<void> { return {}; });

    auto names = coordinator.registered_subsystems();
    REQUIRE(names.size() == 2);
}

// ══════════════════════════════════════════════════════════════════════════════
// Fault Domain Registry
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("FaultDomain: 8 pre-registered domains", "[fault_recovery]")
{
    FaultDomainRegistry registry;
    REQUIRE(registry.domain_count() == 8);
}

TEST_CASE("FaultDomain: health names", "[fault_recovery]")
{
    REQUIRE(health_name(DomainHealth::kHealthy) == "Healthy");
    REQUIRE(health_name(DomainHealth::kDegraded) == "Degraded");
    REQUIRE(health_name(DomainHealth::kFaulted) == "Faulted");
    REQUIRE(health_name(DomainHealth::kIsolated) == "Isolated");
}

TEST_CASE("FaultDomain: initially healthy", "[fault_recovery]")
{
    FaultDomainRegistry registry;
    REQUIRE(registry.health("Rendering") == DomainHealth::kHealthy);
    REQUIRE(registry.is_operational("Rendering"));
}

TEST_CASE("FaultDomain: degrades after errors", "[fault_recovery]")
{
    FaultDomainRegistry registry;
    registry.set_thresholds(2, 5);

    registry.record_error("Rendering", "error 1");
    REQUIRE(registry.health("Rendering") == DomainHealth::kHealthy);

    registry.record_error("Rendering", "error 2");
    REQUIRE(registry.health("Rendering") == DomainHealth::kDegraded);
    REQUIRE(registry.is_operational("Rendering")); // Still operational
}

TEST_CASE("FaultDomain: faults after more errors", "[fault_recovery]")
{
    FaultDomainRegistry registry;
    registry.set_thresholds(1, 3);

    for (int idx = 0; idx < 3; ++idx)
    {
        registry.record_error("Editor", "error");
    }

    REQUIRE(registry.health("Editor") == DomainHealth::kFaulted);
    REQUIRE_FALSE(registry.is_operational("Editor"));
}

TEST_CASE("FaultDomain: isolate and recover", "[fault_recovery]")
{
    FaultDomainRegistry registry;
    registry.isolate("Network");
    REQUIRE(registry.health("Network") == DomainHealth::kIsolated);
    REQUIRE_FALSE(registry.is_operational("Network"));

    registry.recover("Network");
    REQUIRE(registry.health("Network") == DomainHealth::kHealthy);
    REQUIRE(registry.is_operational("Network"));
}

TEST_CASE("FaultDomain: custom domain registration", "[fault_recovery]")
{
    FaultDomainRegistry registry;
    registry.register_domain("CustomDomain");
    REQUIRE(registry.domain_count() == 9);
    REQUIRE(registry.health("CustomDomain") == DomainHealth::kHealthy);
}

TEST_CASE("FaultDomain: all_domains snapshot", "[fault_recovery]")
{
    FaultDomainRegistry registry;
    auto domains = registry.all_domains();
    REQUIRE(domains.size() == 8);
}
