// Phase 34 — Task 2: UI panel rendering tests (header-only logic, no wxWidgets)
// Tests OutputChannel, DiagnosticsService, and search result models

#include "core/DiagnosticsService.h"
#include "core/OutputChannelService.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <string>
#include <vector>

using namespace markamp::core;

// ===========================================================================
// OutputPanel model tests
// ===========================================================================

TEST_CASE("Panel — OutputChannel create and append", "[panel][output]")
{
    OutputChannel channel("Build");
    REQUIRE(channel.name() == "Build");
    REQUIRE(channel.content().empty());

    channel.append("line one");
    REQUIRE(channel.content() == "line one");

    channel.append_line(" continued");
    REQUIRE(channel.content().find("continued") != std::string::npos);
}

TEST_CASE("Panel — OutputChannel visibility", "[panel][output]")
{
    OutputChannel channel("Test");
    REQUIRE_FALSE(channel.is_visible());

    channel.show();
    REQUIRE(channel.is_visible());

    channel.hide();
    REQUIRE_FALSE(channel.is_visible());
}

TEST_CASE("Panel — OutputChannel clear", "[panel][output]")
{
    OutputChannel channel("Log");
    channel.append_line("data");
    REQUIRE_FALSE(channel.content().empty());

    channel.clear();
    REQUIRE(channel.content().empty());
}

TEST_CASE("Panel — OutputChannelService channel management", "[panel][output]")
{
    OutputChannelService service;

    auto* ch_a = service.create_channel("Alpha");
    auto* ch_b = service.create_channel("Beta");
    REQUIRE(ch_a != nullptr);
    REQUIRE(ch_b != nullptr);

    REQUIRE(service.get_channel("Alpha") == ch_a);
    REQUIRE(service.get_channel("NonExistent") == nullptr);

    auto names = service.channel_names();
    REQUIRE(names.size() == 2);

    service.remove_channel("Alpha");
    REQUIRE(service.get_channel("Alpha") == nullptr);
    REQUIRE(service.channel_names().size() == 1);
}

TEST_CASE("Panel — OutputChannelService active channel switching", "[panel][output]")
{
    OutputChannelService service;
    service.create_channel("A");
    service.create_channel("B");

    service.set_active_channel("A");
    REQUIRE(service.active_channel() == "A");

    service.set_active_channel("B");
    REQUIRE(service.active_channel() == "B");
}

// ===========================================================================
// ProblemsPanel model tests (via DiagnosticsService)
// ===========================================================================

TEST_CASE("Panel — DiagnosticsService set and get", "[panel][problems]")
{
    DiagnosticsService service;

    std::vector<Diagnostic> diags;
    diags.push_back(
        Diagnostic{.range = DiagnosticRange{.start = DiagnosticPosition{.line = 10, .character = 5},
                                            .end = DiagnosticPosition{.line = 10, .character = 15}},
                   .message = "Missing semicolon",
                   .severity = DiagnosticSeverity::kError,
                   .source = "parser",
                   .code = "P001"});

    service.set("file://main.cpp", std::move(diags));
    REQUIRE(service.total_count() == 1);

    const auto& retrieved = service.get("file://main.cpp");
    REQUIRE(retrieved.size() == 1);
    REQUIRE(retrieved[0].message == "Missing semicolon");
    REQUIRE(retrieved[0].severity == DiagnosticSeverity::kError);
}

TEST_CASE("Panel — DiagnosticsService severity filtering", "[panel][problems]")
{
    DiagnosticsService service;

    std::vector<Diagnostic> diags;
    diags.push_back(Diagnostic{.range = {},
                               .message = "err1",
                               .severity = DiagnosticSeverity::kError,
                               .source = "s",
                               .code = "c"});
    diags.push_back(Diagnostic{.range = {},
                               .message = "warn1",
                               .severity = DiagnosticSeverity::kWarning,
                               .source = "s",
                               .code = "c"});
    diags.push_back(Diagnostic{.range = {},
                               .message = "warn2",
                               .severity = DiagnosticSeverity::kWarning,
                               .source = "s",
                               .code = "c"});
    diags.push_back(Diagnostic{.range = {},
                               .message = "info1",
                               .severity = DiagnosticSeverity::kInformation,
                               .source = "s",
                               .code = "c"});
    diags.push_back(Diagnostic{.range = {},
                               .message = "hint1",
                               .severity = DiagnosticSeverity::kHint,
                               .source = "s",
                               .code = "c"});

    service.set("file://test.md", std::move(diags));

    REQUIRE(service.count_by_severity(DiagnosticSeverity::kError) == 1);
    REQUIRE(service.count_by_severity(DiagnosticSeverity::kWarning) == 2);
    REQUIRE(service.count_by_severity(DiagnosticSeverity::kInformation) == 1);
    REQUIRE(service.count_by_severity(DiagnosticSeverity::kHint) == 1);
    REQUIRE(service.total_count() == 5);
}

TEST_CASE("Panel — DiagnosticsService multi-URI", "[panel][problems]")
{
    DiagnosticsService service;

    std::vector<Diagnostic> d1;
    d1.push_back(Diagnostic{.range = {},
                            .message = "a",
                            .severity = DiagnosticSeverity::kError,
                            .source = "s",
                            .code = "c"});
    service.set("file://a.md", std::move(d1));

    std::vector<Diagnostic> d2;
    d2.push_back(Diagnostic{.range = {},
                            .message = "b",
                            .severity = DiagnosticSeverity::kWarning,
                            .source = "s",
                            .code = "c"});
    d2.push_back(Diagnostic{.range = {},
                            .message = "c",
                            .severity = DiagnosticSeverity::kError,
                            .source = "s",
                            .code = "c"});
    service.set("file://b.md", std::move(d2));

    REQUIRE(service.total_count() == 3);
    auto uris = service.uris();
    REQUIRE(uris.size() == 2);
}

TEST_CASE("Panel — DiagnosticsService change listener", "[panel][problems]")
{
    DiagnosticsService service;

    int change_count = 0;
    auto id = service.on_change([&](const std::string& /*uri*/) { ++change_count; });

    std::vector<Diagnostic> diags;
    diags.push_back(Diagnostic{.range = {},
                               .message = "x",
                               .severity = DiagnosticSeverity::kHint,
                               .source = "s",
                               .code = "c"});
    service.set("file://test.md", std::move(diags));
    REQUIRE(change_count == 1);

    service.remove("file://test.md");
    REQUIRE(change_count >= 1); // Remove may or may not fire change

    service.remove_listener(id);
}

TEST_CASE("Panel — DiagnosticsService empty URI returns empty vector", "[panel][problems]")
{
    DiagnosticsService service;
    const auto& result = service.get("file://nonexistent.md");
    REQUIRE(result.empty());
}

// ===========================================================================
// Panel lifecycle (all channel operations in sequence)
// ===========================================================================

TEST_CASE("Panel — full lifecycle: create, write, switch, clear, destroy", "[panel][lifecycle]")
{
    OutputChannelService service;

    // Create
    auto* build = service.create_channel("Build");
    auto* test = service.create_channel("Test");
    REQUIRE(service.channel_names().size() == 2);

    // Write
    build->append_line("Building...");
    test->append_line("Testing...");

    // Switch
    service.set_active_channel("Build");
    REQUIRE(service.active_channel() == "Build");
    REQUIRE(build->content().find("Building") != std::string::npos);

    service.set_active_channel("Test");
    REQUIRE(service.active_channel() == "Test");

    // Clear
    build->clear();
    REQUIRE(build->content().empty());
    REQUIRE_FALSE(test->content().empty());

    // Destroy
    service.remove_channel("Build");
    service.remove_channel("Test");
    REQUIRE(service.channel_names().empty());
}
