#include "core/DecorationService.h"
#include "core/DiagnosticsService.h"
#include "core/FileSystemProviderRegistry.h"
#include "core/LanguageProviderRegistry.h"
#include "core/OutputChannelService.h"
#include "core/PluginContext.h"
#include "core/TreeDataProviderRegistry.h"
#include "core/WebviewService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ══════════════════════════════════════════
// OutputChannelService Tests
// ══════════════════════════════════════════

TEST_CASE("OutputChannelService: create and get channel", "[services]")
{
    OutputChannelService svc;
    auto* ch = svc.create_channel("Test Output");
    REQUIRE(ch != nullptr);
    REQUIRE(ch->name() == "Test Output");
    REQUIRE(svc.get_channel("Test Output") == ch);
    REQUIRE(svc.get_channel("Nonexistent") == nullptr);
}

TEST_CASE("OutputChannel: append and clear", "[services]")
{
    OutputChannelService svc;
    auto* ch = svc.create_channel("Log");
    ch->append("Hello ");
    ch->append_line("World");
    REQUIRE(ch->content() == "Hello World\n");
    ch->clear();
    REQUIRE(ch->content().empty());
}

TEST_CASE("OutputChannel: show and hide", "[services]")
{
    OutputChannelService svc;
    auto* ch = svc.create_channel("Log");
    REQUIRE_FALSE(ch->is_visible());
    ch->show();
    REQUIRE(ch->is_visible());
    ch->hide();
    REQUIRE_FALSE(ch->is_visible());
}

TEST_CASE("OutputChannelService: channel names and remove", "[services]")
{
    OutputChannelService svc;
    svc.create_channel("A");
    svc.create_channel("B");
    REQUIRE(svc.channel_names().size() == 2);
    svc.remove_channel("A");
    REQUIRE(svc.channel_names().size() == 1);
    REQUIRE(svc.get_channel("A") == nullptr);
}

// ══════════════════════════════════════════
// DiagnosticsService Tests
// ══════════════════════════════════════════

TEST_CASE("DiagnosticsService: set and get", "[services]")
{
    DiagnosticsService svc;
    std::vector<Diagnostic> diags = {
        {.range = {}, .message = "Error 1", .severity = DiagnosticSeverity::kError},
        {.range = {}, .message = "Warning 1", .severity = DiagnosticSeverity::kWarning},
    };
    svc.set("file:///test.md", std::move(diags));

    const auto& result = svc.get("file:///test.md");
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].message == "Error 1");
}

TEST_CASE("DiagnosticsService: count by severity", "[services]")
{
    DiagnosticsService svc;
    svc.set("file:///a.md",
            {{.range = {}, .message = "E1", .severity = DiagnosticSeverity::kError},
             {.range = {}, .message = "E2", .severity = DiagnosticSeverity::kError}});
    svc.set("file:///b.md",
            {{.range = {}, .message = "W1", .severity = DiagnosticSeverity::kWarning}});

    REQUIRE(svc.count_by_severity(DiagnosticSeverity::kError) == 2);
    REQUIRE(svc.count_by_severity(DiagnosticSeverity::kWarning) == 1);
    REQUIRE(svc.total_count() == 3);
}

TEST_CASE("DiagnosticsService: remove and clear", "[services]")
{
    DiagnosticsService svc;
    svc.set("file:///a.md",
            {{.range = {}, .message = "E", .severity = DiagnosticSeverity::kError}});
    svc.remove("file:///a.md");
    REQUIRE(svc.get("file:///a.md").empty());
    REQUIRE(svc.total_count() == 0);
}

// ══════════════════════════════════════════
// TreeDataProviderRegistry Tests
// ══════════════════════════════════════════

namespace
{
class MockTreeProvider : public ITreeDataProvider
{
public:
    [[nodiscard]] auto get_children(const std::string& /*parent_id*/) const
        -> std::vector<TreeItem> override
    {
        return {{.label = "Item 1"}, {.label = "Item 2"}};
    }

    [[nodiscard]] auto get_tree_item(const std::string& /*item_id*/) const -> TreeItem override
    {
        return {.label = "Mock Item"};
    }

    auto on_did_change_tree_data(ChangeListener /*listener*/) -> std::size_t override
    {
        return 0;
    }
};
} // namespace

TEST_CASE("TreeDataProviderRegistry: register and get", "[services]")
{
    TreeDataProviderRegistry reg;
    auto provider = std::make_shared<MockTreeProvider>();
    reg.register_provider("myExtension.treeView", provider);

    REQUIRE(reg.has_provider("myExtension.treeView"));
    REQUIRE(reg.get_provider("myExtension.treeView") == provider);
    REQUIRE_FALSE(reg.has_provider("nonexistent"));

    auto children = reg.get_provider("myExtension.treeView")->get_children("");
    REQUIRE(children.size() == 2);
}

TEST_CASE("TreeDataProviderRegistry: unregister", "[services]")
{
    TreeDataProviderRegistry reg;
    reg.register_provider("view1", std::make_shared<MockTreeProvider>());
    reg.unregister_provider("view1");
    REQUIRE_FALSE(reg.has_provider("view1"));
}

// ══════════════════════════════════════════
// WebviewService Tests
// ══════════════════════════════════════════

TEST_CASE("WebviewService: create and get panel", "[services]")
{
    WebviewService svc;
    auto* panel = svc.create_panel("myType", "My Panel", {.enable_scripts = true});
    REQUIRE(panel != nullptr);
    REQUIRE(panel->view_type() == "myType");
    REQUIRE(panel->title() == "My Panel");
    REQUIRE(panel->options().enable_scripts);
    REQUIRE(panel->is_visible());
}

TEST_CASE("WebviewPanel: HTML and messaging", "[services]")
{
    WebviewService svc;
    auto* panel = svc.create_panel("editor", "Editor");
    panel->set_html("<h1>Hello</h1>");
    REQUIRE(panel->html() == "<h1>Hello</h1>");

    std::string received;
    panel->on_did_receive_message([&received](const std::string& msg) { received = msg; });
    panel->post_message(R"({"command":"save"})");
    REQUIRE(received == R"({"command":"save"})");
}

TEST_CASE("WebviewPanel: dispose", "[services]")
{
    WebviewService svc;
    auto* panel = svc.create_panel("editor", "Editor");
    bool disposed = false;
    panel->on_did_dispose([&disposed]() { disposed = true; });
    panel->dispose();
    REQUIRE(disposed);
    REQUIRE_FALSE(panel->is_visible());
}

// ══════════════════════════════════════════
// DecorationService Tests
// ══════════════════════════════════════════

TEST_CASE("DecorationService: create type and set decorations", "[services]")
{
    DecorationService svc;
    auto handle = svc.create_decoration_type({.background_color = "#ff0000"});
    REQUIRE(handle > 0);

    svc.set_decorations(
        "file:///test.md",
        handle,
        {{.start_line = 1, .start_character = 0, .end_line = 1, .end_character = 10}});

    const auto& decos = svc.get_decorations("file:///test.md", handle);
    REQUIRE(decos.size() == 1);
    REQUIRE(decos[0].start_line == 1);
}

TEST_CASE("DecorationService: dispose type", "[services]")
{
    DecorationService svc;
    auto handle = svc.create_decoration_type({});
    svc.set_decorations("file:///a.md", handle, {{.start_line = 0}});
    svc.dispose_decoration_type(handle);
    REQUIRE(svc.get_options(handle) == nullptr);
    REQUIRE(svc.get_decorations("file:///a.md", handle).empty());
}

// ══════════════════════════════════════════
// PluginContext Tests
// ══════════════════════════════════════════

TEST_CASE("PluginContext: wire up all services", "[services]")
{
    ContextKeyService ctx_keys;
    OutputChannelService output;
    DiagnosticsService diagnostics;
    TreeDataProviderRegistry tree_reg;
    WebviewService webviews;
    DecorationService decorations;
    FileSystemProviderRegistry fs_reg;
    LanguageProviderRegistry lang_reg;

    PluginContext plugin_ctx{
        .extension_id = "publisher.my-extension",
        .extension_path = "/extensions/my-extension",
        .global_storage_path = "/storage/global",
        .workspace_storage_path = "/storage/workspace",
        .log_path = "/logs",
        .context_key_service = &ctx_keys,
        .output_channel_service = &output,
        .diagnostics_service = &diagnostics,
        .tree_data_provider_registry = &tree_reg,
        .webview_service = &webviews,
        .decoration_service = &decorations,
        .file_system_provider_registry = &fs_reg,
        .language_provider_registry = &lang_reg,
    };

    REQUIRE(plugin_ctx.extension_id == "publisher.my-extension");
    REQUIRE(plugin_ctx.context_key_service != nullptr);
    REQUIRE(plugin_ctx.output_channel_service != nullptr);
    REQUIRE(plugin_ctx.diagnostics_service != nullptr);
    REQUIRE(plugin_ctx.tree_data_provider_registry != nullptr);
    REQUIRE(plugin_ctx.webview_service != nullptr);
    REQUIRE(plugin_ctx.decoration_service != nullptr);
    REQUIRE(plugin_ctx.file_system_provider_registry != nullptr);
    REQUIRE(plugin_ctx.language_provider_registry != nullptr);

    // Verify services are usable through context
    plugin_ctx.context_key_service->set_context("activated", true);
    REQUIRE(plugin_ctx.context_key_service->get_bool("activated"));

    auto* ch = plugin_ctx.output_channel_service->create_channel("My Ext");
    ch->append_line("Extension activated");
    REQUIRE(ch->content() == "Extension activated\n");
}
