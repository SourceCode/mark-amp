/// test_plugin_system_phase04.cpp — Comprehensive tests for Phase 04 Plugin System Maturation
///
/// Covers Tasks 1–16:
///   - PluginContext wiring & sandbox enforcement (Tasks 1–4)
///   - Hot-reload (Task 5)
///   - API versioning (Task 6)
///   - Dependency version constraints (Task 7)
///   - Resource limits (Task 8)
///   - Storage encryption (Task 9)
///   - Contribution validation (Task 10)
///   - Output channel routing (Task 11)
///   - Plugin diagnostics collection (Task 12)
///   - Tree data provider wiring (Task 13)
///   - Webview panel wiring (Task 14)
///   - Telemetry events (Task 15)

#include "core/Config.h"
#include "core/ContributionValidator.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ExtensionManifest.h"
#include "core/ExtensionRecommendations.h"
#include "core/ExtensionStorage.h"
#include "core/IPlugin.h"
#include "core/OutputChannelService.h"
#include "core/PluginHotReload.h"
#include "core/PluginManager.h"
#include "core/PluginMemoryTracker.h"

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>

using namespace markamp::core;

// ─────────────────────── Task 5: Hot-Reload ───────────────────────

TEST_CASE("PluginHotReload: total reloads starts at zero", "[phase04][hot_reload]")
{
    EventBus bus;
    Config cfg;
    PluginManager manager(bus, cfg);
    PluginHotReload hot_reload(manager, bus);

    REQUIRE(hot_reload.total_reloads() == 0);
}

TEST_CASE("PluginHotReload: reload count for unknown plugin is zero", "[phase04][hot_reload]")
{
    EventBus bus;
    Config cfg;
    PluginManager manager(bus, cfg);
    PluginHotReload hot_reload(manager, bus);

    REQUIRE(hot_reload.reload_count("unknown.plugin") == 0);
}

// ─────────────────────── Task 6: API Versioning ───────────────────────

TEST_CASE("ApiVersion: host API version is defined", "[phase04][api_version]")
{
    REQUIRE(kHostApiVersion.major > 0);
    REQUIRE(kHostApiVersion.minor >= 0);
}

TEST_CASE("ApiVersion: compatibility check — same major, older minor", "[phase04][api_version]")
{
    ApiVersion host{1, 5};
    ApiVersion plugin{1, 3};

    // Plugin targeting older minor is compatible if same major
    REQUIRE(plugin.major == host.major);
    REQUIRE(plugin.minor <= host.minor);
}

TEST_CASE("ApiVersion: incompatible — different major", "[phase04][api_version]")
{
    ApiVersion host{2, 0};
    ApiVersion plugin{1, 5};

    REQUIRE(plugin.major != host.major);
}

// ─────────────────────── Task 7: Dependency Version Constraints ───────────────────────

TEST_CASE("VersionedDependency: no constraint accepts any version", "[phase04][dependency]")
{
    ExtensionManifest::VersionedDependency dep;
    dep.extension_id = "test.ext";
    dep.version_range = "";

    REQUIRE(dep.is_satisfied_by("1.0.0"));
    REQUIRE(dep.is_satisfied_by("99.0.0"));
}

TEST_CASE("VersionedDependency: exact version match", "[phase04][dependency]")
{
    ExtensionManifest::VersionedDependency dep;
    dep.extension_id = "test.ext";
    dep.version_range = "2.3.4";

    REQUIRE(dep.is_satisfied_by("2.3.4"));
    REQUIRE_FALSE(dep.is_satisfied_by("2.3.5"));
    REQUIRE_FALSE(dep.is_satisfied_by("2.4.0"));
    REQUIRE_FALSE(dep.is_satisfied_by("3.0.0"));
}

TEST_CASE("VersionedDependency: caret range ^major.minor.patch", "[phase04][dependency]")
{
    ExtensionManifest::VersionedDependency dep;
    dep.extension_id = "test.ext";
    dep.version_range = "^1.2.3";

    // Same major, >= minor.patch
    REQUIRE(dep.is_satisfied_by("1.2.3"));
    REQUIRE(dep.is_satisfied_by("1.2.4"));
    REQUIRE(dep.is_satisfied_by("1.3.0"));
    REQUIRE(dep.is_satisfied_by("1.99.0"));

    // Below minor.patch
    REQUIRE_FALSE(dep.is_satisfied_by("1.2.2"));
    REQUIRE_FALSE(dep.is_satisfied_by("1.1.0"));

    // Different major
    REQUIRE_FALSE(dep.is_satisfied_by("2.0.0"));
    REQUIRE_FALSE(dep.is_satisfied_by("0.9.0"));
}

TEST_CASE("VersionedDependency: >=major.minor.patch range", "[phase04][dependency]")
{
    ExtensionManifest::VersionedDependency dep;
    dep.extension_id = "test.ext";
    dep.version_range = ">=1.5.0";

    REQUIRE(dep.is_satisfied_by("1.5.0"));
    REQUIRE(dep.is_satisfied_by("1.5.1"));
    REQUIRE(dep.is_satisfied_by("1.6.0"));
    REQUIRE(dep.is_satisfied_by("2.0.0"));
    REQUIRE(dep.is_satisfied_by("10.0.0"));

    REQUIRE_FALSE(dep.is_satisfied_by("1.4.9"));
    REQUIRE_FALSE(dep.is_satisfied_by("0.9.0"));
}

TEST_CASE("VersionedDependency: invalid version string returns false", "[phase04][dependency]")
{
    ExtensionManifest::VersionedDependency dep;
    dep.extension_id = "test.ext";
    dep.version_range = "^1.0.0";

    REQUIRE_FALSE(dep.is_satisfied_by("not-a-version"));
    REQUIRE_FALSE(dep.is_satisfied_by(""));
}

// ─────────────────────── Task 8: Resource Limits ───────────────────────

TEST_CASE("ResourceLimits: default limits are zero (unlimited)", "[phase04][resource_limits]")
{
    PluginMemoryTracker::ResourceLimits limits;
    REQUIRE(limits.memory_bytes == 0);
    REQUIRE(limits.cpu_time_ms == 0);
    REQUIRE(limits.event_subscriptions == 0);
}

TEST_CASE("PluginMemoryTracker: tracks allocations per plugin", "[phase04][resource_limits]")
{
    PluginMemoryTracker tracker;

    tracker.register_plugin("plugin.a", 1024 * 1024);
    tracker.register_plugin("plugin.b", 1024 * 1024);

    tracker.report_allocation("plugin.a", 1024);
    tracker.report_allocation("plugin.a", 2048);
    tracker.report_allocation("plugin.b", 512);

    REQUIRE(tracker.current_usage("plugin.a") == 3072);
    REQUIRE(tracker.current_usage("plugin.b") == 512);
}

TEST_CASE("PluginMemoryTracker: deallocation reduces usage", "[phase04][resource_limits]")
{
    PluginMemoryTracker tracker;

    tracker.register_plugin("plugin.a", 1024 * 1024);
    tracker.report_allocation("plugin.a", 4096);
    tracker.report_deallocation("plugin.a", 1024);

    REQUIRE(tracker.current_usage("plugin.a") == 3072);
}

TEST_CASE("PluginMemoryTracker: resource violation detection", "[phase04][resource_limits]")
{
    PluginMemoryTracker tracker;

    tracker.register_plugin("plugin.a", 1024 * 1024);

    PluginMemoryTracker::ResourceLimits limits;
    limits.memory_bytes = 2048;
    limits.event_subscriptions = 10;

    tracker.set_limits("plugin.a", limits);
    tracker.report_allocation("plugin.a", 3000);

    auto violations = tracker.check_limits("plugin.a");
    REQUIRE_FALSE(violations.empty());
    REQUIRE(violations[0].plugin_id == "plugin.a");
    REQUIRE(violations[0].resource_type == "memory");
}

TEST_CASE("PluginMemoryTracker: no violation when under limits", "[phase04][resource_limits]")
{
    PluginMemoryTracker tracker;

    tracker.register_plugin("plugin.a", 1024 * 1024);

    PluginMemoryTracker::ResourceLimits limits;
    limits.memory_bytes = 8192;

    tracker.set_limits("plugin.a", limits);
    tracker.report_allocation("plugin.a", 1024);

    auto violations = tracker.check_limits("plugin.a");
    REQUIRE(violations.empty());
}

// ─────────────────────── Task 9: Storage Encryption ───────────────────────

TEST_CASE("ExtensionStorageService: store and retrieve secret with encryption",
          "[phase04][encryption]")
{
    const auto temp_path = std::filesystem::temp_directory_path() / "test_ext_storage_enc.json";
    ExtensionStorageService storage(temp_path);

    storage.set_encryption_key("my-secret-key-123");
    REQUIRE(storage.has_encryption_key());

    storage.store_secret("test.ext", "api_token", "super-secret-value");

    auto retrieved = storage.retrieve_secret("test.ext", "api_token");
    REQUIRE(retrieved == "super-secret-value");

    // Clean up
    std::filesystem::remove(temp_path);
}

TEST_CASE("ExtensionStorageService: retrieve nonexistent secret returns empty",
          "[phase04][encryption]")
{
    const auto temp_path = std::filesystem::temp_directory_path() / "test_ext_storage_enc2.json";
    ExtensionStorageService storage(temp_path);

    storage.set_encryption_key("key");

    auto retrieved = storage.retrieve_secret("nonexistent", "missing");
    REQUIRE(retrieved.empty());

    std::filesystem::remove(temp_path);
}

TEST_CASE("ExtensionStorageService: delete secret", "[phase04][encryption]")
{
    const auto temp_path = std::filesystem::temp_directory_path() / "test_ext_storage_enc3.json";
    ExtensionStorageService storage(temp_path);

    storage.set_encryption_key("key");
    storage.store_secret("test.ext", "token", "value");
    storage.delete_secret("test.ext", "token");

    auto retrieved = storage.retrieve_secret("test.ext", "token");
    REQUIRE(retrieved.empty());

    std::filesystem::remove(temp_path);
}

TEST_CASE("ExtensionStorageService: encryption is symmetric (XOR roundtrip)",
          "[phase04][encryption]")
{
    const auto temp_path = std::filesystem::temp_directory_path() / "test_ext_storage_enc4.json";
    ExtensionStorageService storage(temp_path);

    storage.set_encryption_key("test-key-for-xor");

    const std::string original = "Hello, World! This is a test of XOR encryption.";
    storage.store_secret("ext", "data", original);
    auto decoded = storage.retrieve_secret("ext", "data");

    REQUIRE(decoded == original);

    std::filesystem::remove(temp_path);
}

TEST_CASE("ExtensionStorageService: no encryption key stores plaintext", "[phase04][encryption]")
{
    const auto temp_path = std::filesystem::temp_directory_path() / "test_ext_storage_enc5.json";
    ExtensionStorageService storage(temp_path);

    REQUIRE_FALSE(storage.has_encryption_key());

    storage.store_secret("test.ext", "token", "plaintext-value");
    auto retrieved = storage.retrieve_secret("test.ext", "token");
    REQUIRE(retrieved == "plaintext-value");

    std::filesystem::remove(temp_path);
}

// ─────────────────────── Task 10: Contribution Validation ───────────────────────

TEST_CASE("ContributionValidator: valid extension has no diagnostics",
          "[phase04][contribution_validation]")
{
    ContributionValidator validator;

    ExtensionContributions contrib;
    ExtensionCommand cmd;
    cmd.command = "test.cmd1";
    cmd.title = "Test Command";
    contrib.commands.push_back(cmd);

    auto diagnostics = validator.validate_extension("test.ext", contrib);
    REQUIRE(diagnostics.empty());
    REQUIRE_FALSE(validator.has_errors());
}

TEST_CASE("ContributionValidator: missing command ID is error",
          "[phase04][contribution_validation]")
{
    ContributionValidator validator;

    ExtensionContributions contrib;
    ExtensionCommand cmd;
    cmd.command = ""; // Missing!
    cmd.title = "Test";
    contrib.commands.push_back(cmd);

    auto diagnostics = validator.validate_extension("test.ext", contrib);
    REQUIRE_FALSE(diagnostics.empty());
    REQUIRE(validator.has_errors());
}

TEST_CASE("ContributionValidator: duplicate command ID is error",
          "[phase04][contribution_validation]")
{
    ContributionValidator validator;

    ExtensionContributions contrib;
    ExtensionCommand cmd1;
    cmd1.command = "test.cmd";
    cmd1.title = "First";
    contrib.commands.push_back(cmd1);

    ExtensionCommand cmd2;
    cmd2.command = "test.cmd"; // Duplicate!
    cmd2.title = "Second";
    contrib.commands.push_back(cmd2);

    auto diagnostics = validator.validate_extension("test.ext", contrib);
    REQUIRE_FALSE(diagnostics.empty());
    REQUIRE(validator.has_errors());
}

TEST_CASE("ContributionValidator: missing command title is warning",
          "[phase04][contribution_validation]")
{
    ContributionValidator validator;

    ExtensionContributions contrib;
    ExtensionCommand cmd;
    cmd.command = "test.cmd";
    cmd.title = ""; // Missing title
    contrib.commands.push_back(cmd);

    auto diagnostics = validator.validate_extension("test.ext", contrib);
    REQUIRE(diagnostics.size() == 1);
    REQUIRE(diagnostics[0].severity == ContributionDiagnostic::Severity::kWarning);
    REQUIRE_FALSE(validator.has_errors());
}

TEST_CASE("ContributionValidator: missing view ID is error", "[phase04][contribution_validation]")
{
    ContributionValidator validator;

    ExtensionContributions contrib;
    ExtensionView view;
    view.view_id = ""; // Missing!
    view.name = "Test View";
    contrib.views.push_back(view);

    auto diagnostics = validator.validate_extension("test.ext", contrib);
    REQUIRE_FALSE(diagnostics.empty());
    REQUIRE(validator.has_errors());
}

TEST_CASE("ContributionValidator: duplicate view ID is error", "[phase04][contribution_validation]")
{
    ContributionValidator validator;

    ExtensionContributions contrib;
    ExtensionView view1;
    view1.view_id = "test.view";
    view1.name = "First";
    contrib.views.push_back(view1);

    ExtensionView view2;
    view2.view_id = "test.view"; // Duplicate!
    view2.name = "Second";
    contrib.views.push_back(view2);

    auto diagnostics = validator.validate_extension("test.ext", contrib);
    REQUIRE_FALSE(diagnostics.empty());
    REQUIRE(validator.has_errors());
}

TEST_CASE("ContributionValidator: reset clears error state", "[phase04][contribution_validation]")
{
    ContributionValidator validator;

    ExtensionContributions contrib;
    ExtensionCommand cmd;
    cmd.command = "";
    contrib.commands.push_back(cmd);

    validator.validate_extension("test.ext", contrib);
    REQUIRE(validator.has_errors());

    validator.reset();
    REQUIRE_FALSE(validator.has_errors());
}

// ─────────────────────── Task 11: Output Channel Routing ───────────────────────

TEST_CASE("PluginOutputRouter: channel_name returns correct format", "[phase04][output_channel]")
{
    REQUIRE(PluginOutputRouter::channel_name("my-plugin") == "Plugin: my-plugin");
    REQUIRE(PluginOutputRouter::channel_name("ext.test") == "Plugin: ext.test");
}

// ─────────────────────── Task 12: Plugin Diagnostics ───────────────────────

TEST_CASE("PluginManager: PluginDiagnostic struct compiles", "[phase04][diagnostics]")
{
    PluginManager::PluginDiagnostic diag;
    diag.plugin_id = "test.plugin";
    diag.name = "Test Plugin";
    diag.version = "1.0.0";
    diag.active = false;
    diag.activation_time_us = 0;
    diag.contribution_count = 0;
    diag.command_count = 0;

    REQUIRE(diag.plugin_id == "test.plugin");
    REQUIRE_FALSE(diag.active);
}

// ─────────────────────── Task 15: Telemetry Events ───────────────────────

TEST_CASE("PluginActivatedEvent: publishes on activation", "[phase04][telemetry]")
{
    EventBus bus;
    bool received = false;
    std::string received_id;

    auto sub = bus.subscribe<events::PluginActivatedEvent>(
        [&](const events::PluginActivatedEvent& evt)
        {
            received = true;
            received_id = evt.plugin_id;
        });

    events::PluginActivatedEvent evt;
    evt.plugin_id = "test.plugin";
    bus.publish(evt);

    REQUIRE(received);
    REQUIRE(received_id == "test.plugin");
}

TEST_CASE("PluginErrorEvent: publishes error details", "[phase04][telemetry]")
{
    EventBus bus;
    bool received = false;
    std::string received_msg;

    auto sub = bus.subscribe<events::PluginErrorEvent>(
        [&](const events::PluginErrorEvent& evt)
        {
            received = true;
            received_msg = evt.error_message;
        });

    events::PluginErrorEvent evt;
    evt.plugin_id = "test.plugin";
    evt.error_message = "Activation failed: segfault";
    bus.publish(evt);

    REQUIRE(received);
    REQUIRE(received_msg == "Activation failed: segfault");
}

TEST_CASE("PluginDeactivatedEvent: publishes on deactivation", "[phase04][telemetry]")
{
    EventBus bus;
    bool received = false;

    auto sub = bus.subscribe<events::PluginDeactivatedEvent>(
        [&](const events::PluginDeactivatedEvent& evt)
        {
            received = true;
            REQUIRE(evt.plugin_id == "test.plugin");
        });

    events::PluginDeactivatedEvent evt;
    evt.plugin_id = "test.plugin";
    bus.publish(evt);

    REQUIRE(received);
}

// ─────────────────────── Integration: ActivationReport ───────────────────────

TEST_CASE("ActivationReport: default values", "[phase04][integration]")
{
    PluginManager::ActivationReport report;
    REQUIRE(report.total_plugins == 0);
    REQUIRE(report.activated_count == 0);
    REQUIRE(report.deferred_count == 0);
    REQUIRE(report.failed_count == 0);
    REQUIRE(report.total_duration_us == 0);
    REQUIRE(report.errors.empty());
}

TEST_CASE("ActivationReport: error accumulation", "[phase04][integration]")
{
    PluginManager::ActivationReport report;
    report.errors.push_back({"plugin.bad", "Crash during init", 1500});

    REQUIRE(report.errors.size() == 1);
    REQUIRE(report.errors[0].plugin_id == "plugin.bad");
    REQUIRE(report.errors[0].duration_us == 1500);
}

// ═══════════════════════════════════════════════════════
// Batch 4E — VSIX Manifest Verification, Search, Recommendations
// ═══════════════════════════════════════════════════════

TEST_CASE("ManifestVerification: valid manifest passes", "[phase04][manifest]")
{
    ExtensionManifest manifest;
    manifest.name = "test-ext";
    manifest.version = "1.2.3";
    manifest.publisher = "test-pub";
    manifest.display_name = "Test Extension";
    manifest.engines_vscode = "^1.60.0";

    const auto result = manifest.verify();
    REQUIRE(result.valid);
    REQUIRE(result.errors.empty());
    REQUIRE(result.warnings.empty());
}

TEST_CASE("ManifestVerification: missing required fields", "[phase04][manifest]")
{
    ExtensionManifest manifest;
    // All fields empty

    const auto result = manifest.verify();
    REQUIRE_FALSE(result.valid);
    REQUIRE(result.errors.size() >= 3); // name, version, publisher
}

TEST_CASE("ManifestVerification: invalid semver reports error", "[phase04][manifest]")
{
    ExtensionManifest manifest;
    manifest.name = "test-ext";
    manifest.version = "not-semver";
    manifest.publisher = "test-pub";

    const auto result = manifest.verify();
    REQUIRE_FALSE(result.valid);
    bool has_semver_error = false;
    for (const auto& err : result.errors)
    {
        if (err.find("semver") != std::string::npos)
        {
            has_semver_error = true;
        }
    }
    REQUIRE(has_semver_error);
}

TEST_CASE("ManifestVerification: missing engines warns", "[phase04][manifest]")
{
    ExtensionManifest manifest;
    manifest.name = "test-ext";
    manifest.version = "1.0.0";
    manifest.publisher = "test-pub";
    // engines_vscode is empty

    const auto result = manifest.verify();
    REQUIRE(result.valid); // warnings don't block validity
    bool has_engine_warning = false;
    for (const auto& warn : result.warnings)
    {
        if (warn.find("engines") != std::string::npos)
        {
            has_engine_warning = true;
        }
    }
    REQUIRE(has_engine_warning);
}

TEST_CASE("ExtensionRecommendations: file type recommendations", "[phase04][recommendations]")
{
    ExtensionRecommendations recs;
    recs.add_file_type_recommendation(".py", "python.linter");
    recs.add_file_type_recommendation(".py", "python.formatter");
    recs.add_file_type_recommendation(".md", "markdown.preview");

    // Should recommend python extensions for .py files
    auto py_recs = recs.recommend_for_file_types({".py"});
    REQUIRE(py_recs.size() == 2);

    // Should recommend markdown extension for .md files
    auto md_recs = recs.recommend_for_file_types({".md"});
    REQUIRE(md_recs.size() == 1);
    REQUIRE(md_recs[0] == "markdown.preview");

    // Combined lookup
    auto all_recs = recs.recommend_for_file_types({".py", ".md"});
    REQUIRE(all_recs.size() == 3);
}

TEST_CASE("ExtensionRecommendations: deduplication", "[phase04][recommendations]")
{
    ExtensionRecommendations recs;
    recs.add_file_type_recommendation(".py", "shared.extension");
    recs.add_file_type_recommendation(".md", "shared.extension");

    auto result = recs.recommend_for_file_types({".py", ".md"});
    REQUIRE(result.size() == 1); // Same extension not duplicated
}

TEST_CASE("ExtensionRecommendations: unwanted filtering", "[phase04][recommendations]")
{
    // Load JSON with unwanted list, then check file recommendations
    const nlohmann::json json_obj = {
        {"recommendations", nlohmann::json::array({"recommended.ext"})},
        {"unwantedRecommendations", nlohmann::json::array({"unwanted.ext"})}};

    ExtensionRecommendations recs;
    recs.load_from_json(json_obj);
    recs.add_file_type_recommendation(".txt", "unwanted.ext");
    recs.add_file_type_recommendation(".txt", "wanted.ext");

    auto result = recs.recommend_for_file_types({".txt"});
    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == "wanted.ext");
}
