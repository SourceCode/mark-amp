// Phase 34 — Task 1: Integration test — workspace lifecycle
// Tests cross-subsystem workflows: document edit → backlink → event flow

#include "core/Config.h"
#include "core/DiagnosticsService.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/MarkdownParser.h"
#include "core/OutputChannelService.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <string>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

namespace
{
auto make_config_event(const std::string& key_val,
                       const std::string& old_val,
                       const std::string& new_val) -> ConfigChangedEvent
{
    ConfigChangedEvent evt;
    evt.key = key_val;
    evt.old_value = old_val;
    evt.new_value = new_val;
    return evt;
}
} // namespace

// ---------------------------------------------------------------------------
// Workspace lifecycle
// ---------------------------------------------------------------------------

TEST_CASE("Integration — Config then EventBus round-trip", "[integration][lifecycle]")
{
    Config config;
    EventBus bus;

    bool config_changed = false;
    auto sub = bus.subscribe<ConfigChangedEvent>([&](const ConfigChangedEvent& /*ev*/)
                                                 { config_changed = true; });

    // Simulate config change + event
    config.set("editor.font_size", 16);
    bus.publish(make_config_event("editor.font_size", "14", "16"));

    REQUIRE(config_changed);
    REQUIRE(config.get_int("editor.font_size") == 16);
}

TEST_CASE("Integration — OutputChannel lifecycle", "[integration][lifecycle]")
{
    OutputChannelService service;

    auto* channel = service.create_channel("Test Channel");
    REQUIRE(channel != nullptr);
    REQUIRE(channel->name() == "Test Channel");

    channel->append_line("Hello");
    channel->append_line("World");
    REQUIRE(channel->content().find("Hello") != std::string::npos);
    REQUIRE(channel->content().find("World") != std::string::npos);

    channel->clear();
    REQUIRE(channel->content().empty());

    service.remove_channel("Test Channel");
    REQUIRE(service.get_channel("Test Channel") == nullptr);
}

TEST_CASE("Integration — multiple channels and active switching", "[integration][lifecycle]")
{
    OutputChannelService service;
    service.create_channel("Alpha");
    service.create_channel("Beta");
    service.create_channel("Gamma");

    auto names = service.channel_names();
    REQUIRE(names.size() == 3);

    service.set_active_channel("Beta");
    REQUIRE(service.active_channel() == "Beta");

    service.set_active_channel("Gamma");
    REQUIRE(service.active_channel() == "Gamma");
}

TEST_CASE("Integration — OutputChannel content change listener", "[integration][lifecycle]")
{
    OutputChannelService service;
    auto* channel = service.create_channel("Log");

    int change_count = 0;
    auto listener_id =
        channel->on_content_change([&](const OutputChannel& /*ch*/) { ++change_count; });

    channel->append("first ");
    channel->append("second ");
    channel->append_line("third");
    REQUIRE(change_count == 3);

    channel->remove_content_listener(listener_id);
    channel->append("after remove");
    REQUIRE(change_count == 3); // Listener removed, no new calls
}

// ---------------------------------------------------------------------------
// Markdown parse → render → verify
// ---------------------------------------------------------------------------

TEST_CASE("Integration — Markdown parse and render", "[integration][pipeline]")
{
    MarkdownParser parser;
    auto result = parser.parse("# Hello\n\nParagraph text\n");
    REQUIRE(result.has_value());

    auto html = parser.render_html(result.value());
    REQUIRE(html.find("<h1") != std::string::npos);
    REQUIRE(html.find("Paragraph text") != std::string::npos);
}

TEST_CASE("Integration — Markdown code block render", "[integration][pipeline]")
{
    MarkdownParser parser;
    auto result = parser.parse("```cpp\nint x = 42;\n```\n");
    REQUIRE(result.has_value());

    auto html = parser.render_html(result.value());
    REQUIRE(html.find("code") != std::string::npos);
    REQUIRE(html.find("42") != std::string::npos);
}

TEST_CASE("Integration — Markdown list render", "[integration][pipeline]")
{
    MarkdownParser parser;
    auto result = parser.parse("- item one\n- item two\n- item three\n");
    REQUIRE(result.has_value());

    auto html = parser.render_html(result.value());
    REQUIRE(html.find("<li") != std::string::npos);
    REQUIRE(html.find("item one") != std::string::npos);
    REQUIRE(html.find("item three") != std::string::npos);
}

// ---------------------------------------------------------------------------
// Diagnostics service
// ---------------------------------------------------------------------------

TEST_CASE("Integration — DiagnosticsService lifecycle", "[integration][diagnostics]")
{
    DiagnosticsService diag_service;

    std::vector<Diagnostic> diags;
    diags.push_back(
        Diagnostic{.range = DiagnosticRange{.start = DiagnosticPosition{.line = 1, .character = 0},
                                            .end = DiagnosticPosition{.line = 1, .character = 10}},
                   .message = "Unused variable",
                   .severity = DiagnosticSeverity::kWarning,
                   .source = "lint",
                   .code = "W001"});
    diags.push_back(
        Diagnostic{.range = DiagnosticRange{.start = DiagnosticPosition{.line = 5, .character = 0},
                                            .end = DiagnosticPosition{.line = 5, .character = 20}},
                   .message = "Syntax error",
                   .severity = DiagnosticSeverity::kError,
                   .source = "parser",
                   .code = "E001"});

    diag_service.set("file://test.md", std::move(diags));

    REQUIRE(diag_service.total_count() == 2);
    REQUIRE(diag_service.count_by_severity(DiagnosticSeverity::kWarning) == 1);
    REQUIRE(diag_service.count_by_severity(DiagnosticSeverity::kError) == 1);

    auto uris = diag_service.uris();
    REQUIRE(uris.size() == 1);
    REQUIRE(uris[0] == "file://test.md");

    const auto& retrieved = diag_service.get("file://test.md");
    REQUIRE(retrieved.size() == 2);
    REQUIRE(retrieved[0].message == "Unused variable");
    REQUIRE(retrieved[1].message == "Syntax error");
}

TEST_CASE("Integration — DiagnosticsService change listener", "[integration][diagnostics]")
{
    DiagnosticsService diag_service;

    std::string changed_uri;
    auto listener_id = diag_service.on_change([&](const std::string& uri) { changed_uri = uri; });

    std::vector<Diagnostic> diags;
    diags.push_back(Diagnostic{.range = {},
                               .message = "test",
                               .severity = DiagnosticSeverity::kHint,
                               .source = "test",
                               .code = "T1"});
    diag_service.set("file://readme.md", std::move(diags));
    REQUIRE(changed_uri == "file://readme.md");

    diag_service.remove("file://readme.md");
    REQUIRE(diag_service.total_count() == 0);

    diag_service.remove_listener(listener_id);
}

TEST_CASE("Integration — DiagnosticsService clear all", "[integration][diagnostics]")
{
    DiagnosticsService diag_service;

    std::vector<Diagnostic> diags1;
    diags1.push_back(Diagnostic{.range = {},
                                .message = "a",
                                .severity = DiagnosticSeverity::kError,
                                .source = "s",
                                .code = "c"});
    diag_service.set("file://a.md", std::move(diags1));

    std::vector<Diagnostic> diags2;
    diags2.push_back(Diagnostic{.range = {},
                                .message = "b",
                                .severity = DiagnosticSeverity::kWarning,
                                .source = "s",
                                .code = "c"});
    diag_service.set("file://b.md", std::move(diags2));

    REQUIRE(diag_service.total_count() == 2);
    diag_service.clear();
    REQUIRE(diag_service.total_count() == 0);
    REQUIRE(diag_service.uris().empty());
}

// ---------------------------------------------------------------------------
// PluginOutputRouter
// ---------------------------------------------------------------------------

TEST_CASE("Integration — PluginOutputRouter per-plugin channels", "[integration][plugins]")
{
    OutputChannelService service;
    PluginOutputRouter router(service);

    router.append_line("my-plugin", "Hello from plugin");
    router.append_line("other-plugin", "Other output");

    auto* ch1 = service.get_channel("Plugin: my-plugin");
    auto* ch2 = service.get_channel("Plugin: other-plugin");
    REQUIRE(ch1 != nullptr);
    REQUIRE(ch2 != nullptr);
    REQUIRE(ch1->content().find("Hello from plugin") != std::string::npos);
    REQUIRE(ch2->content().find("Other output") != std::string::npos);

    router.clear("my-plugin");
    REQUIRE(ch1->content().empty());
    REQUIRE_FALSE(ch2->content().empty());
}

// ---------------------------------------------------------------------------
// EventBus basic flow
// ---------------------------------------------------------------------------

TEST_CASE("Integration — EventBus publish and subscribe", "[integration][eventbus]")
{
    EventBus bus;

    int count = 0;
    auto sub = bus.subscribe<ConfigChangedEvent>(
        [&](const ConfigChangedEvent& ev)
        {
            REQUIRE(ev.key == "test.key");
            ++count;
        });

    bus.publish(make_config_event("test.key", "old", "new"));
    bus.publish(make_config_event("test.key", "new", "newer"));
    REQUIRE(count == 2);
}

TEST_CASE("Integration — EventBus RAII unsubscribe", "[integration][eventbus]")
{
    EventBus bus;
    int count = 0;
    {
        auto sub =
            bus.subscribe<ConfigChangedEvent>([&](const ConfigChangedEvent& /*ev*/) { ++count; });
        bus.publish(make_config_event("k", "a", "b"));
        REQUIRE(count == 1);
    } // sub goes out of scope

    bus.publish(make_config_event("k", "b", "c"));
    REQUIRE(count == 1); // No longer subscribed
}
