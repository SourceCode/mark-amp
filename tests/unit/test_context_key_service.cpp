/// test_context_key_service.cpp — Comprehensive tests for ContextKeyService
#include "core/ContextKeyService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("ContextKeyService: set and get bool", "[context_key_service][positive]")
{
    ContextKeyService svc;
    svc.set_context("editor.readonly", true);
    auto* val = svc.get_context("editor.readonly");
    REQUIRE(val != nullptr);
    REQUIRE(std::get<bool>(*val) == true);
}

TEST_CASE("ContextKeyService: set and get int", "[context_key_service][positive]")
{
    ContextKeyService svc;
    svc.set_context("editor.tabSize", 4);
    auto* val = svc.get_context("editor.tabSize");
    REQUIRE(val != nullptr);
    REQUIRE(std::get<int>(*val) == 4);
}

TEST_CASE("ContextKeyService: set and get double", "[context_key_service][positive]")
{
    ContextKeyService svc;
    svc.set_context("editor.fontSize", 14.5);
    auto* val = svc.get_context("editor.fontSize");
    REQUIRE(val != nullptr);
    REQUIRE(std::get<double>(*val) == 14.5);
}

TEST_CASE("ContextKeyService: set and get string", "[context_key_service][positive]")
{
    ContextKeyService svc;
    svc.set_context("editor.mode", std::string("insert"));
    auto* val = svc.get_context("editor.mode");
    REQUIRE(val != nullptr);
    REQUIRE(std::get<std::string>(*val) == "insert");
}

TEST_CASE("ContextKeyService: overwrite replaces value", "[context_key_service][positive]")
{
    ContextKeyService svc;
    svc.set_context("key", true);
    svc.set_context("key", false);
    auto* val = svc.get_context("key");
    REQUIRE(val != nullptr);
    REQUIRE(std::get<bool>(*val) == false);
}

// ── Negative Tests ──

TEST_CASE("ContextKeyService: get missing key returns nullptr", "[context_key_service][negative]")
{
    ContextKeyService svc;
    REQUIRE(svc.get_context("nonexistent") == nullptr);
}

TEST_CASE("ContextKeyService: get empty-string key returns nullptr",
          "[context_key_service][negative]")
{
    ContextKeyService svc;
    REQUIRE(svc.get_context("") == nullptr);
}

// ── Edge Cases ──

TEST_CASE("ContextKeyService: multiple keys coexist", "[context_key_service][edge]")
{
    ContextKeyService svc;
    svc.set_context("a", true);
    svc.set_context("b", 42);
    svc.set_context("c", std::string("hello"));
    REQUIRE(svc.get_context("a") != nullptr);
    REQUIRE(svc.get_context("b") != nullptr);
    REQUIRE(svc.get_context("c") != nullptr);
    REQUIRE(std::get<bool>(*svc.get_context("a")) == true);
    REQUIRE(std::get<int>(*svc.get_context("b")) == 42);
    REQUIRE(std::get<std::string>(*svc.get_context("c")) == "hello");
}

TEST_CASE("ContextKeyService: change type of existing key", "[context_key_service][edge]")
{
    ContextKeyService svc;
    svc.set_context("key", true);
    svc.set_context("key", 42); // Change from bool to int
    auto* val = svc.get_context("key");
    REQUIRE(val != nullptr);
    REQUIRE(std::get<int>(*val) == 42);
}
