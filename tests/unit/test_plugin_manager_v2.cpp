#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ExtensionManifest.h"
#include "core/IPlugin.h"
#include "core/PluginManager.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>

using namespace markamp::core;

namespace
{

/// Minimal test plugin for unit tests.
class TestPlugin : public IPlugin
{
public:
    explicit TestPlugin(PluginManifest pm)
        : manifest_(std::move(pm))
    {
    }

    [[nodiscard]] auto manifest() const -> const PluginManifest& override
    {
        return manifest_;
    }

    void activate(PluginContext& /*ctx*/) override
    {
        active_ = true;
    }
    void deactivate() override
    {
        active_ = false;
    }

private:
    PluginManifest manifest_;
};

auto make_test_plugin(const std::string& id,
                      const std::string& name = "Test",
                      const std::string& version = "1.0.0") -> std::unique_ptr<TestPlugin>
{
    PluginManifest pm;
    pm.id = id;
    pm.name = name;
    pm.version = version;
    return std::make_unique<TestPlugin>(std::move(pm));
}

auto make_ext_manifest(const std::string& name,
                       const std::string& publisher,
                       const std::vector<std::string>& activation_events = {},
                       const std::vector<std::string>& deps = {},
                       const std::vector<std::string>& pack = {}) -> ExtensionManifest
{
    ExtensionManifest em;
    em.name = name;
    em.publisher = publisher;
    em.version = "1.0.0";

    for (const auto& evt_str : activation_events)
    {
        em.activation_events.push_back(ActivationEvent::parse(evt_str));
    }

    em.extension_dependencies = deps;
    em.extension_pack = pack;
    return em;
}

} // anonymous namespace

// ── Legacy behavior (backward compatibility) ──

TEST_CASE("PluginManager V2: legacy registration and activation", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto plugin = make_test_plugin("test.legacy", "Legacy Plugin");
    mgr.register_plugin(std::move(plugin));

    REQUIRE(mgr.plugin_count() == 1);
    REQUIRE_FALSE(mgr.is_plugin_active("test.legacy"));

    mgr.activate_all();
    REQUIRE(mgr.is_plugin_active("test.legacy"));
}

TEST_CASE("PluginManager V2: deactivate all", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    mgr.register_plugin(make_test_plugin("test.one"));
    mgr.register_plugin(make_test_plugin("test.two"));
    mgr.activate_all();

    REQUIRE(mgr.is_plugin_active("test.one"));
    REQUIRE(mgr.is_plugin_active("test.two"));

    mgr.deactivate_all();
    REQUIRE_FALSE(mgr.is_plugin_active("test.one"));
    REQUIRE_FALSE(mgr.is_plugin_active("test.two"));
}

// ── Lazy activation ──

TEST_CASE("PluginManager V2: star activation event activates immediately", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest("star-plugin", "pub", {"*"});
    mgr.register_plugin(make_test_plugin("pub.star-plugin", "Star Plugin"), std::move(em));

    mgr.activate_all();
    REQUIRE(mgr.is_plugin_active("pub.star-plugin"));
    REQUIRE_FALSE(mgr.is_pending_activation("pub.star-plugin"));
}

TEST_CASE("PluginManager V2: onLanguage activation defers until triggered", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest("lang-plugin", "pub", {"onLanguage:markdown"});
    mgr.register_plugin(make_test_plugin("pub.lang-plugin", "Lang Plugin"), std::move(em));

    mgr.activate_all();
    REQUIRE_FALSE(mgr.is_plugin_active("pub.lang-plugin"));
    REQUIRE(mgr.is_pending_activation("pub.lang-plugin"));

    // Trigger the activation event
    mgr.trigger_activation_event("onLanguage:markdown");
    REQUIRE(mgr.is_plugin_active("pub.lang-plugin"));
    REQUIRE_FALSE(mgr.is_pending_activation("pub.lang-plugin"));
}

TEST_CASE("PluginManager V2: trigger unrelated event does nothing", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest("lang-plugin", "pub", {"onLanguage:markdown"});
    mgr.register_plugin(make_test_plugin("pub.lang-plugin"), std::move(em));
    mgr.activate_all();

    mgr.trigger_activation_event("onLanguage:python");
    REQUIRE_FALSE(mgr.is_plugin_active("pub.lang-plugin"));
    REQUIRE(mgr.is_pending_activation("pub.lang-plugin"));
}

TEST_CASE("PluginManager V2: multiple plugins on same event", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em1 = make_ext_manifest("plugin-a", "pub", {"onLanguage:markdown"});
    auto em2 = make_ext_manifest("plugin-b", "pub", {"onLanguage:markdown"});
    mgr.register_plugin(make_test_plugin("pub.plugin-a"), std::move(em1));
    mgr.register_plugin(make_test_plugin("pub.plugin-b"), std::move(em2));

    mgr.activate_all();
    REQUIRE_FALSE(mgr.is_plugin_active("pub.plugin-a"));
    REQUIRE_FALSE(mgr.is_plugin_active("pub.plugin-b"));

    mgr.trigger_activation_event("onLanguage:markdown");
    REQUIRE(mgr.is_plugin_active("pub.plugin-a"));
    REQUIRE(mgr.is_plugin_active("pub.plugin-b"));
}

// ── Dependency resolution ──

TEST_CASE("PluginManager V2: resolve dependencies - no deps", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest("no-deps", "pub");
    mgr.register_plugin(make_test_plugin("pub.no-deps"), std::move(em));

    const auto deps = mgr.resolve_dependencies("pub.no-deps");
    REQUIRE(deps.empty());
}

TEST_CASE("PluginManager V2: resolve dependencies - linear chain", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    // C depends on B, B depends on A
    auto em_a = make_ext_manifest("dep-a", "pub");
    auto em_b = make_ext_manifest("dep-b", "pub", {}, {"pub.dep-a"});
    auto em_c = make_ext_manifest("dep-c", "pub", {}, {"pub.dep-b"});

    mgr.register_plugin(make_test_plugin("pub.dep-a"), std::move(em_a));
    mgr.register_plugin(make_test_plugin("pub.dep-b"), std::move(em_b));
    mgr.register_plugin(make_test_plugin("pub.dep-c"), std::move(em_c));

    const auto deps = mgr.resolve_dependencies("pub.dep-c");
    REQUIRE(deps.size() == 2);
    // A should come before B in topological order
    REQUIRE(deps[0] == "pub.dep-a");
    REQUIRE(deps[1] == "pub.dep-b");
}

TEST_CASE("PluginManager V2: circular dependency detection", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    // A depends on B, B depends on A → circular
    auto em_a = make_ext_manifest("circ-a", "pub", {}, {"pub.circ-b"});
    auto em_b = make_ext_manifest("circ-b", "pub", {}, {"pub.circ-a"});

    mgr.register_plugin(make_test_plugin("pub.circ-a"), std::move(em_a));
    mgr.register_plugin(make_test_plugin("pub.circ-b"), std::move(em_b));

    REQUIRE_THROWS_AS(mgr.resolve_dependencies("pub.circ-a"), std::runtime_error);
}

TEST_CASE("PluginManager V2: dependency activation order", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    // B depends on A, both have * activation
    auto em_a = make_ext_manifest("dep-a", "pub", {"*"});
    auto em_b = make_ext_manifest("dep-b", "pub", {"*"}, {"pub.dep-a"});

    mgr.register_plugin(make_test_plugin("pub.dep-a"), std::move(em_a));
    mgr.register_plugin(make_test_plugin("pub.dep-b"), std::move(em_b));

    // Activate B directly — A should be activated first as a dependency
    mgr.activate_plugin("pub.dep-b");

    REQUIRE(mgr.is_plugin_active("pub.dep-a"));
    REQUIRE(mgr.is_plugin_active("pub.dep-b"));
}

// ── Extension pack expansion ──

TEST_CASE("PluginManager V2: expand extension pack", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest(
        "my-pack", "pub", {}, {}, {"pub.member-a", "pub.member-b", "pub.member-c"});
    mgr.register_plugin(make_test_plugin("pub.my-pack"), std::move(em));

    const auto members = mgr.expand_extension_pack("pub.my-pack");
    REQUIRE(members.size() == 3);
    REQUIRE(members[0] == "pub.member-a");
    REQUIRE(members[1] == "pub.member-b");
    REQUIRE(members[2] == "pub.member-c");
}

TEST_CASE("PluginManager V2: expand non-pack returns empty", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest("regular", "pub");
    mgr.register_plugin(make_test_plugin("pub.regular"), std::move(em));

    const auto members = mgr.expand_extension_pack("pub.regular");
    REQUIRE(members.empty());
}

// ── Enhanced PluginContext ──

TEST_CASE("PluginManager V2: extension manifest query", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest("my-ext", "pub");
    em.description = "Test extension";
    mgr.register_plugin(make_test_plugin("pub.my-ext"), std::move(em));

    const auto* manifest = mgr.get_extension_manifest("pub.my-ext");
    REQUIRE(manifest != nullptr);
    REQUIRE(manifest->name == "my-ext");
    REQUIRE(manifest->publisher == "pub");
    REQUIRE(manifest->description == "Test extension");
}

TEST_CASE("PluginManager V2: unknown plugin manifest returns nullptr", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    REQUIRE(mgr.get_extension_manifest("nonexistent") == nullptr);
}

TEST_CASE("PluginManager V2: unregister cleans up pending", "[plugin-manager-v2]")
{
    EventBus bus;
    Config cfg;
    PluginManager mgr(bus, cfg);

    auto em = make_ext_manifest("lazy-ext", "pub", {"onCommand:foo"});
    mgr.register_plugin(make_test_plugin("pub.lazy-ext"), std::move(em));
    mgr.activate_all();

    REQUIRE(mgr.is_pending_activation("pub.lazy-ext"));

    mgr.unregister_plugin("pub.lazy-ext");
    REQUIRE(mgr.plugin_count() == 0);
    REQUIRE_FALSE(mgr.is_pending_activation("pub.lazy-ext"));
}
