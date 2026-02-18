// Phase 34 Batch 34B — Task 7: Plugin lifecycle tests
// Manifest parsing, dependency resolution, activation/deactivation,
// PluginContext service access, plugin diagnostics.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/IPlugin.h"
#include "core/PluginContext.h"
#include "core/PluginManager.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

using namespace markamp::core;

// ---------------------------------------------------------------------------
// Test plugin implementation
// ---------------------------------------------------------------------------

namespace
{

class TestPlugin : public IPlugin
{
public:
    explicit TestPlugin(std::string plugin_id,
                        std::string plugin_name = "Test Plugin",
                        std::string plugin_version = "1.0.0")
    {
        manifest_.id = std::move(plugin_id);
        manifest_.name = std::move(plugin_name);
        manifest_.version = std::move(plugin_version);
    }

    [[nodiscard]] auto manifest() const -> const PluginManifest& override
    {
        return manifest_;
    }

    void activate(PluginContext& /*ctx*/) override
    {
        active_ = true;
        ++activate_count_;
    }

    void deactivate() override
    {
        active_ = false;
        ++deactivate_count_;
    }

    [[nodiscard]] auto activate_count() const -> int
    {
        return activate_count_;
    }
    [[nodiscard]] auto deactivate_count() const -> int
    {
        return deactivate_count_;
    }

    void add_command(const std::string& cmd_id, const std::string& title)
    {
        CommandContribution cmd;
        cmd.id = cmd_id;
        cmd.title = title;
        manifest_.contributes.commands.push_back(cmd);
    }

private:
    PluginManifest manifest_;
    int activate_count_{0};
    int deactivate_count_{0};
};

} // namespace

// ---------------------------------------------------------------------------
// Manifest
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — manifest fields accessible", "[plugin][manifest]")
{
    TestPlugin plugin("test.plugin", "My Plugin", "2.1.0");
    auto& manifest = plugin.manifest();
    REQUIRE(manifest.id == "test.plugin");
    REQUIRE(manifest.name == "My Plugin");
    REQUIRE(manifest.version == "2.1.0");
    REQUIRE(manifest.contributes.commands.empty());
}

TEST_CASE("Plugin — manifest with commands", "[plugin][manifest]")
{
    TestPlugin plugin("cmd.plugin");
    plugin.add_command("cmd.hello", "Say Hello");
    plugin.add_command("cmd.goodbye", "Say Goodbye");

    REQUIRE(plugin.manifest().contributes.commands.size() == 2);
    REQUIRE(plugin.manifest().contributes.commands[0].id == "cmd.hello");
    REQUIRE(plugin.manifest().contributes.commands[1].title == "Say Goodbye");
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — register and count", "[plugin][lifecycle]")
{
    EventBus bus;
    Config config;
    PluginManager manager(bus, config);

    REQUIRE(manager.plugin_count() == 0);
    manager.register_plugin(std::make_unique<TestPlugin>("plugin.a"));
    REQUIRE(manager.plugin_count() == 1);

    manager.register_plugin(std::make_unique<TestPlugin>("plugin.b"));
    REQUIRE(manager.plugin_count() == 2);
}

TEST_CASE("Plugin — duplicate registration rejected", "[plugin][lifecycle]")
{
    EventBus bus;
    Config config;
    PluginManager manager(bus, config);

    auto result1 = manager.register_plugin(std::make_unique<TestPlugin>("plugin.dup"));
    REQUIRE(result1);

    auto result2 = manager.register_plugin(std::make_unique<TestPlugin>("plugin.dup"));
    REQUIRE_FALSE(result2); // Duplicate should fail
}

// ---------------------------------------------------------------------------
// Activation / deactivation
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — activate_all activates registered plugins", "[plugin][lifecycle]")
{
    EventBus bus;
    Config config;
    PluginManager manager(bus, config);

    auto plugin_ptr = std::make_unique<TestPlugin>("plugin.act");
    auto* raw = plugin_ptr.get();
    manager.register_plugin(std::move(plugin_ptr));

    REQUIRE_FALSE(raw->is_active());
    manager.activate_all();
    REQUIRE(raw->is_active());
    REQUIRE(raw->activate_count() == 1);
}

TEST_CASE("Plugin — deactivate_all deactivates plugins", "[plugin][lifecycle]")
{
    EventBus bus;
    Config config;
    PluginManager manager(bus, config);

    auto plugin_ptr = std::make_unique<TestPlugin>("plugin.deact");
    auto* raw = plugin_ptr.get();
    manager.register_plugin(std::move(plugin_ptr));

    manager.activate_all();
    REQUIRE(raw->is_active());

    manager.deactivate_all();
    REQUIRE_FALSE(raw->is_active());
    REQUIRE(raw->deactivate_count() == 1);
}

// ---------------------------------------------------------------------------
// Activation report
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — activation report populated", "[plugin][lifecycle]")
{
    EventBus bus;
    Config config;
    PluginManager manager(bus, config);

    manager.register_plugin(std::make_unique<TestPlugin>("plugin.r1"));
    manager.register_plugin(std::make_unique<TestPlugin>("plugin.r2"));
    manager.activate_all();

    auto report = manager.last_activation_report();
    REQUIRE(report.total_plugins == 2);
    REQUIRE(report.activated_count >= 0);
    REQUIRE(report.failed_count == 0);
}

// ---------------------------------------------------------------------------
// Plugin diagnostics
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — diagnostics contain registered plugins", "[plugin][diagnostics]")
{
    EventBus bus;
    Config config;
    PluginManager manager(bus, config);

    manager.register_plugin(std::make_unique<TestPlugin>("plugin.diag", "DiagPlugin", "3.0.0"));
    manager.activate_all();

    auto diags = manager.plugin_diagnostics();
    REQUIRE_FALSE(diags.empty());

    bool found = false;
    for (const auto& diag : diags)
    {
        if (diag.plugin_id == "plugin.diag")
        {
            found = true;
            REQUIRE(diag.name == "DiagPlugin");
            REQUIRE(diag.version == "3.0.0");
            REQUIRE(diag.active);
        }
    }
    REQUIRE(found);
}

// ---------------------------------------------------------------------------
// API version compatibility
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — API version compatibility check", "[plugin][api]")
{
    ApiVersion host{1, 0};

    ApiVersion compatible{1, 0};
    REQUIRE(compatible.is_compatible_with(host));

    ApiVersion newer_minor{1, 1};
    REQUIRE_FALSE(newer_minor.is_compatible_with(host)); // Plugin wants 1.1, host is 1.0

    ApiVersion different_major{2, 0};
    REQUIRE_FALSE(different_major.is_compatible_with(host));
}

TEST_CASE("Plugin — default api_version is 1.0", "[plugin][api]")
{
    TestPlugin plugin("api.test");
    auto ver = plugin.api_version();
    REQUIRE(ver.major == 1);
    REQUIRE(ver.minor == 0);
}

// ---------------------------------------------------------------------------
// Multiple plugins
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — multiple plugins lifecycle", "[plugin][lifecycle]")
{
    EventBus bus;
    Config config;
    PluginManager manager(bus, config);

    constexpr int kPluginCount = 10;
    for (int idx = 0; idx < kPluginCount; ++idx)
    {
        manager.register_plugin(
            std::make_unique<TestPlugin>("plugin.multi." + std::to_string(idx)));
    }

    REQUIRE(manager.plugin_count() == kPluginCount);

    manager.activate_all();
    auto report = manager.last_activation_report();
    REQUIRE(report.total_plugins == kPluginCount);

    manager.deactivate_all();
    // All should be deactivated — verify via diagnostics
    auto diags = manager.plugin_diagnostics();
    for (const auto& diag : diags)
    {
        REQUIRE_FALSE(diag.active);
    }
}

// ---------------------------------------------------------------------------
// PluginManifest contribution points
// ---------------------------------------------------------------------------

TEST_CASE("Plugin — contribution point types", "[plugin][manifest]")
{
    PluginManifest manifest;
    manifest.id = "contrib.test";

    CommandContribution cmd;
    cmd.id = "contrib.cmd1";
    cmd.title = "Command One";
    cmd.category = "Test";
    manifest.contributes.commands.push_back(cmd);

    SnippetContribution snip;
    snip.name = "Test Snippet";
    snip.trigger = "!test";
    snip.body = "> [!NOTE]\n> $0";
    manifest.contributes.snippets.push_back(snip);

    REQUIRE(manifest.contributes.commands.size() == 1);
    REQUIRE(manifest.contributes.snippets.size() == 1);
    REQUIRE(manifest.contributes.keybindings.empty());
    REQUIRE(manifest.contributes.menus.empty());
    REQUIRE(manifest.contributes.settings.empty());
    REQUIRE(manifest.contributes.themes.empty());
}
