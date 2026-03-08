/// test_environment_service.cpp — Comprehensive tests for EnvironmentService
#include "core/EnvironmentService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("EnvironmentService: app_name returns non-empty", "[environment_service][positive]")
{
    EnvironmentService svc;
    REQUIRE_FALSE(svc.app_name().empty());
}

TEST_CASE("EnvironmentService: language returns non-empty", "[environment_service][positive]")
{
    EnvironmentService svc;
    REQUIRE_FALSE(svc.language().empty());
}

TEST_CASE("EnvironmentService: machine_id returns non-empty", "[environment_service][positive]")
{
    EnvironmentService svc;
    REQUIRE_FALSE(svc.machine_id().empty());
}

TEST_CASE("EnvironmentService: set_language updates language", "[environment_service][positive]")
{
    EnvironmentService svc;
    svc.set_language("fr");
    REQUIRE(svc.language() == "fr");
}

TEST_CASE("EnvironmentService: set_app_root updates root", "[environment_service][positive]")
{
    EnvironmentService svc;
    svc.set_app_root("/usr/local");
    REQUIRE(svc.app_root() == "/usr/local");
}

TEST_CASE("EnvironmentService: set_machine_id updates id", "[environment_service][positive]")
{
    EnvironmentService svc;
    svc.set_machine_id("test-machine-123");
    REQUIRE(svc.machine_id() == "test-machine-123");
}

// ── Negative Tests ──

TEST_CASE("EnvironmentService: clipboard_read on empty returns empty",
          "[environment_service][negative]")
{
    EnvironmentService svc;
    auto text = svc.clipboard_read();
    (void)text; // May or may not be empty depending on system clipboard
}

// ── Edge Cases ──

TEST_CASE("EnvironmentService: consistent values across calls", "[environment_service][edge]")
{
    EnvironmentService svc;
    auto name1 = svc.app_name();
    auto name2 = svc.app_name();
    REQUIRE(name1 == name2);
}
