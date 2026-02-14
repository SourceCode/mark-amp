/// @file test_panel_data_flow.cpp
/// Tests that OutputChannelService, DiagnosticsService, and TreeDataProviderRegistry
/// correctly manage data flow to panels.

#include "core/DiagnosticsService.h"
#include "core/OutputChannelService.h"
#include "core/TreeDataProviderRegistry.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>

using namespace markamp::core;

// ── OutputChannelService tests ──────────────────────────────────────────────

TEST_CASE("PanelDataFlow: OutputChannel create and write", "[panel][output]")
{
    OutputChannelService service;

    auto* channel = service.create_channel("Extension Log");
    REQUIRE(channel != nullptr);
    REQUIRE(channel->name() == "Extension Log");

    channel->append_line("Starting extension...");
    channel->append_line("Extension activated.");

    auto content = channel->content();
    REQUIRE(content.find("Starting extension...") != std::string::npos);
    REQUIRE(content.find("Extension activated.") != std::string::npos);
}

TEST_CASE("PanelDataFlow: OutputChannel content change listener", "[panel][output]")
{
    OutputChannelService service;
    auto* channel = service.create_channel("Build");

    int change_count = 0;
    channel->on_content_change([&](const OutputChannel&) { change_count++; });

    channel->append("line 1");
    channel->append_line("line 2");
    channel->append("line 3");

    REQUIRE(change_count == 3);
}

TEST_CASE("PanelDataFlow: OutputChannel clear resets content", "[panel][output]")
{
    OutputChannelService service;
    auto* channel = service.create_channel("Test");

    channel->append_line("data");
    REQUIRE_FALSE(channel->content().empty());

    channel->clear();
    REQUIRE(channel->content().empty());
}

TEST_CASE("PanelDataFlow: OutputChannelService manages multiple channels", "[panel][output]")
{
    OutputChannelService service;

    service.create_channel("Log");
    service.create_channel("Build");
    service.create_channel("Debug");

    auto names = service.channel_names();
    REQUIRE(names.size() == 3);

    service.remove_channel("Build");
    names = service.channel_names();
    REQUIRE(names.size() == 2);
    REQUIRE(service.get_channel("Build") == nullptr);
}

TEST_CASE("PanelDataFlow: active channel tracking", "[panel][output]")
{
    OutputChannelService service;

    service.create_channel("Alpha");
    service.create_channel("Beta");

    service.set_active_channel("Alpha");
    REQUIRE(service.active_channel() == "Alpha");

    service.set_active_channel("Beta");
    REQUIRE(service.active_channel() == "Beta");
}

// ── DiagnosticsService tests ────────────────────────────────────────────────

TEST_CASE("PanelDataFlow: DiagnosticsService set and get", "[panel][diagnostics]")
{
    DiagnosticsService service;

    std::vector<Diagnostic> diags;
    diags.push_back({{{1, 0}, {1, 10}}, "Unused variable", DiagnosticSeverity::kWarning, "lint"});
    diags.push_back({{{5, 0}, {5, 20}}, "Syntax error", DiagnosticSeverity::kError, "parser"});

    service.set("file:///test.md", std::move(diags));

    const auto& result = service.get("file:///test.md");
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].message == "Unused variable");
    REQUIRE(result[1].severity == DiagnosticSeverity::kError);
}

TEST_CASE("PanelDataFlow: DiagnosticsService count by severity", "[panel][diagnostics]")
{
    DiagnosticsService service;

    std::vector<Diagnostic> diags_a;
    diags_a.push_back({{{0, 0}, {0, 0}}, "err1", DiagnosticSeverity::kError});
    diags_a.push_back({{{0, 0}, {0, 0}}, "warn1", DiagnosticSeverity::kWarning});

    std::vector<Diagnostic> diags_b;
    diags_b.push_back({{{0, 0}, {0, 0}}, "err2", DiagnosticSeverity::kError});
    diags_b.push_back({{{0, 0}, {0, 0}}, "info1", DiagnosticSeverity::kInformation});

    service.set("file:///a.md", std::move(diags_a));
    service.set("file:///b.md", std::move(diags_b));

    REQUIRE(service.count_by_severity(DiagnosticSeverity::kError) == 2);
    REQUIRE(service.count_by_severity(DiagnosticSeverity::kWarning) == 1);
    REQUIRE(service.count_by_severity(DiagnosticSeverity::kInformation) == 1);
    REQUIRE(service.total_count() == 4);
}

TEST_CASE("PanelDataFlow: DiagnosticsService change listener", "[panel][diagnostics]")
{
    DiagnosticsService service;

    std::vector<std::string> changed_uris;
    service.on_change([&](const std::string& uri) { changed_uris.push_back(uri); });

    std::vector<Diagnostic> diags;
    diags.push_back({{{0, 0}, {0, 0}}, "test", DiagnosticSeverity::kHint});
    service.set("file:///test.md", std::move(diags));

    REQUIRE(changed_uris.size() == 1);
    REQUIRE(changed_uris[0] == "file:///test.md");

    service.remove("file:///test.md");
    REQUIRE(changed_uris.size() == 2);
}

TEST_CASE("PanelDataFlow: DiagnosticsService clear all", "[panel][diagnostics]")
{
    DiagnosticsService service;

    std::vector<Diagnostic> diags;
    diags.push_back({{{0, 0}, {0, 0}}, "x", DiagnosticSeverity::kError});
    service.set("file:///a.md", std::move(diags));

    std::vector<Diagnostic> diags2;
    diags2.push_back({{{0, 0}, {0, 0}}, "y", DiagnosticSeverity::kError});
    service.set("file:///b.md", std::move(diags2));

    REQUIRE(service.total_count() == 2);
    service.clear();
    REQUIRE(service.total_count() == 0);
    REQUIRE(service.uris().empty());
}

// ── TreeDataProviderRegistry tests ──────────────────────────────────────────

/// Simple test tree data provider for testing.
class TestTreeProvider : public ITreeDataProvider
{
public:
    [[nodiscard]] auto get_children(const std::string& /*parent_id*/) const
        -> std::vector<TreeItem> override
    {
        return {{"Root Item", "desc", "tooltip", "", "", false, "root-1", ""}};
    }

    [[nodiscard]] auto get_tree_item(const std::string& /*item_id*/) const -> TreeItem override
    {
        return {"Root Item", "desc", "tooltip", "", "", false, "root-1", ""};
    }

    auto on_did_change_tree_data(ChangeListener /*listener*/) -> std::size_t override
    {
        return 0;
    }
};

TEST_CASE("PanelDataFlow: TreeDataProviderRegistry register and get", "[panel][tree]")
{
    TreeDataProviderRegistry registry;

    auto provider = std::make_shared<TestTreeProvider>();
    registry.register_provider("explorer", provider);

    REQUIRE(registry.has_provider("explorer"));
    REQUIRE(registry.get_provider("explorer") != nullptr);
    REQUIRE_FALSE(registry.has_provider("search"));
}

TEST_CASE("PanelDataFlow: TreeDataProvider returns data", "[panel][tree]")
{
    TreeDataProviderRegistry registry;
    auto provider = std::make_shared<TestTreeProvider>();
    registry.register_provider("test-view", provider);

    auto retreived = registry.get_provider("test-view");
    auto children = retreived->get_children("");
    REQUIRE(children.size() == 1);
    REQUIRE(children[0].label == "Root Item");
    REQUIRE(children[0].item_id == "root-1");
}

TEST_CASE("PanelDataFlow: TreeDataProviderRegistry unregister", "[panel][tree]")
{
    TreeDataProviderRegistry registry;
    auto provider = std::make_shared<TestTreeProvider>();
    registry.register_provider("temp-view", provider);

    REQUIRE(registry.has_provider("temp-view"));
    registry.unregister_provider("temp-view");
    REQUIRE_FALSE(registry.has_provider("temp-view"));
}

TEST_CASE("PanelDataFlow: TreeDataProviderRegistry view_ids", "[panel][tree]")
{
    TreeDataProviderRegistry registry;

    registry.register_provider("view-a", std::make_shared<TestTreeProvider>());
    registry.register_provider("view-b", std::make_shared<TestTreeProvider>());

    auto ids = registry.view_ids();
    REQUIRE(ids.size() == 2);
}
