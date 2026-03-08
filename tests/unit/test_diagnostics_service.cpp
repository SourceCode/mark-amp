/// test_diagnostics_service.cpp — Comprehensive tests for DiagnosticsService
#include "core/DiagnosticsService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("DiagnosticsService: set and get diagnostics", "[diagnostics_service][positive]")
{
    DiagnosticsService svc;
    Diagnostic diag;
    diag.message = "Unused variable";
    diag.severity = DiagnosticSeverity::kWarning;
    diag.range.start.line = 10;
    svc.set("file:///main.cpp", {diag});
    const auto& result = svc.get("file:///main.cpp");
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].message == "Unused variable");
}

TEST_CASE("DiagnosticsService: uris returns tracked files", "[diagnostics_service][positive]")
{
    DiagnosticsService svc;
    svc.set("file:///a.cpp", {});
    svc.set("file:///b.cpp", {});
    auto uris = svc.uris();
    REQUIRE(uris.size() == 2);
}

TEST_CASE("DiagnosticsService: remove deletes one URI", "[diagnostics_service][positive]")
{
    DiagnosticsService svc;
    Diagnostic diag;
    diag.message = "err";
    svc.set("file:///a.cpp", {diag});
    svc.set("file:///b.cpp", {diag});
    svc.remove("file:///a.cpp");
    REQUIRE(svc.uris().size() == 1);
}

TEST_CASE("DiagnosticsService: clear removes everything", "[diagnostics_service][positive]")
{
    DiagnosticsService svc;
    Diagnostic diag;
    diag.message = "err";
    svc.set("file:///a.cpp", {diag});
    svc.clear();
    REQUIRE(svc.uris().empty());
}

TEST_CASE("DiagnosticsService: set replaces existing diagnostics",
          "[diagnostics_service][positive]")
{
    DiagnosticsService svc;
    Diagnostic diag1;
    diag1.message = "first";
    svc.set("file:///a.cpp", {diag1});
    Diagnostic diag2;
    diag2.message = "second";
    svc.set("file:///a.cpp", {diag2});
    const auto& result = svc.get("file:///a.cpp");
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].message == "second");
}

// ── Negative Tests ──

TEST_CASE("DiagnosticsService: get for unknown URI returns empty",
          "[diagnostics_service][negative]")
{
    DiagnosticsService svc;
    const auto& result = svc.get("file:///nonexistent.cpp");
    REQUIRE(result.empty());
}

TEST_CASE("DiagnosticsService: remove on unknown URI safe", "[diagnostics_service][negative]")
{
    DiagnosticsService svc;
    svc.remove("file:///nonexistent.cpp");
    REQUIRE(svc.uris().empty());
}

TEST_CASE("DiagnosticsService: clear on empty is safe", "[diagnostics_service][negative]")
{
    DiagnosticsService svc;
    svc.clear();
    REQUIRE(svc.uris().empty());
}

// ── Edge Cases ──

TEST_CASE("DiagnosticSeverity: all enum values exist", "[diagnostics_service][edge]")
{
    REQUIRE(DiagnosticSeverity::kError != DiagnosticSeverity::kWarning);
    REQUIRE(DiagnosticSeverity::kWarning != DiagnosticSeverity::kInformation);
    REQUIRE(DiagnosticSeverity::kInformation != DiagnosticSeverity::kHint);
}

TEST_CASE("Diagnostic struct: default values", "[diagnostics_service][edge]")
{
    Diagnostic diag;
    REQUIRE(diag.message.empty());
    REQUIRE(diag.range.start.line == 0);
    REQUIRE(diag.range.start.character == 0);
    REQUIRE(diag.source.empty());
    REQUIRE(diag.code.empty());
}

TEST_CASE("DiagnosticsService: set empty vector clears URI", "[diagnostics_service][edge]")
{
    DiagnosticsService svc;
    Diagnostic diag;
    diag.message = "err";
    svc.set("file:///a.cpp", {diag});
    svc.set("file:///a.cpp", {});
    const auto& result = svc.get("file:///a.cpp");
    REQUIRE(result.empty());
}

TEST_CASE("DiagnosticsService: multiple diagnostics per URI", "[diagnostics_service][edge]")
{
    DiagnosticsService svc;
    Diagnostic diag1;
    diag1.message = "err1";
    diag1.range.start.line = 1;
    Diagnostic diag2;
    diag2.message = "err2";
    diag2.range.start.line = 5;
    Diagnostic diag3;
    diag3.message = "warn";
    diag3.range.start.line = 10;
    svc.set("file:///a.cpp", {diag1, diag2, diag3});
    const auto& result = svc.get("file:///a.cpp");
    REQUIRE(result.size() == 3);
}
