/// @file test_v20_language_service.cpp
/// @brief V20 Phase 07 – LanguageService unit tests.

#include "core/LanguageService.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("LanguageService: construction with built-in languages", "[v20][language]")
{
    LanguageService service;
    REQUIRE(service.language_count() > 20);
}

TEST_CASE("LanguageService: resolve from path", "[v20][language]")
{
    LanguageService service;

    auto cpp = service.resolve_from_path("/project/main.cpp");
    REQUIRE(cpp.ok());
    REQUIRE(cpp.language_id == "cpp");
    REQUIRE(cpp.resolution_source == "extension");

    auto py = service.resolve_from_path("/scripts/setup.py");
    REQUIRE(py.ok());
    REQUIRE(py.language_id == "python");

    auto md = service.resolve_from_path("/docs/README.md");
    REQUIRE(md.ok());
    REQUIRE(md.language_id == "markdown");

    auto json = service.resolve_from_path("/config.json");
    REQUIRE(json.language_id == "json");

    auto ts = service.resolve_from_path("/app/index.ts");
    REQUIRE(ts.language_id == "typescript");
}

TEST_CASE("LanguageService: resolve from filename", "[v20][language]")
{
    LanguageService service;

    auto makefile = service.resolve_from_path("/project/Makefile");
    REQUIRE(makefile.ok());
    REQUIRE(makefile.language_id == "makefile");
    REQUIRE(makefile.resolution_source == "filename");

    auto dockerfile = service.resolve_from_path("/project/Dockerfile");
    REQUIRE(dockerfile.ok());
    REQUIRE(dockerfile.language_id == "dockerfile");
}

TEST_CASE("LanguageService: resolve CMakeLists.txt", "[v20][language]")
{
    LanguageService service;
    auto cmake = service.resolve_from_path("/project/CMakeLists.txt");
    REQUIRE(cmake.ok());
    REQUIRE(cmake.language_id == "cmake");
}

TEST_CASE("LanguageService: resolve unknown → fallback", "[v20][language]")
{
    LanguageService service;
    auto unknown = service.resolve_from_path("/file.xyz123");
    REQUIRE(unknown.ok());
    REQUIRE(unknown.language_id == "plaintext");
    REQUIRE(unknown.is_fallback);
}

TEST_CASE("LanguageService: resolve from ID", "[v20][language]")
{
    LanguageService service;

    auto rust = service.resolve_from_id("rust");
    REQUIRE(rust.ok());
    REQUIRE(rust.display_name == "Rust");

    auto unknown = service.resolve_from_id("brainfuck");
    REQUIRE(unknown.is_fallback);
}

TEST_CASE("LanguageService: resolve from first line", "[v20][language]")
{
    LanguageService service;

    auto py = service.resolve_from_first_line("#!/usr/bin/env python3");
    REQUIRE(py.ok());
    REQUIRE(py.language_id == "python");

    auto bash = service.resolve_from_first_line("#!/bin/bash");
    REQUIRE(bash.ok());
    REQUIRE(bash.language_id == "shell");

    auto node = service.resolve_from_first_line("#!/usr/bin/env node");
    REQUIRE(node.ok());
    REQUIRE(node.language_id == "javascript");
}

TEST_CASE("LanguageService: find_language", "[v20][language]")
{
    LanguageService service;
    auto* cpp = service.find_language("cpp");
    REQUIRE(cpp != nullptr);
    REQUIRE(cpp->display_name == "C++");
    REQUIRE(cpp->matches_extension("cpp"));
    REQUIRE(cpp->matches_extension("h"));
}

TEST_CASE("LanguageService: fenced_block_languages", "[v20][language]")
{
    LanguageService service;
    auto fenced = service.fenced_block_languages();
    REQUIRE(fenced.size() > 10);
}

TEST_CASE("LanguageService: all_languages", "[v20][language]")
{
    LanguageService service;
    auto all = service.all_languages();
    REQUIRE(all.size() == service.language_count());
}

TEST_CASE("Phase 07 events: LanguageResolvedEvent", "[v20][lang-events]")
{
    events::LanguageResolvedEvent evt;
    evt.language_id = "cpp";
    evt.resolution_source = "extension";
    REQUIRE(evt.language_id == "cpp");
}
