/// @file test_phase50_release_preparation.cpp
/// @brief Phase 50 tests — Release Preparation & Polish.

#include "core/ChangelogEngine.h"
#include "core/DeprecationTracker.h"
#include "core/Events.h"
#include "core/PolishCommandProvider.h"
#include "core/ReleaseCommandProvider.h"
#include "core/ReleaseNotesGenerator.h"
#include "core/VersionManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// ReleaseNotesGenerator
// ============================================================================

TEST_CASE("ReleaseNotesGenerator initial state", "[phase50][release]")
{
    ReleaseNotesGenerator generator;
    REQUIRE(generator.release_count() == 0);
}

TEST_CASE("ReleaseNotesGenerator create release", "[phase50][release]")
{
    ReleaseNotesGenerator generator;
    auto rid = generator.create_release("9.0.0", "2026-02-18");
    REQUIRE_FALSE(rid.empty());
    REQUIRE(generator.release_count() == 1);
    const auto* release = generator.get_release(rid);
    REQUIRE(release != nullptr);
    REQUIRE(release->version == "9.0.0");
}

TEST_CASE("ReleaseNotesGenerator add entries", "[phase50][release]")
{
    ReleaseNotesGenerator generator;
    auto rid = generator.create_release("9.0.0", "2026-02-18");
    ReleaseEntry entry;
    entry.title = "Data Processing";
    entry.description = "New computation engine";
    entry.category = ReleaseCategory::kFeature;
    entry.author = "markamp";
    REQUIRE(generator.add_entry(rid, entry));

    const auto* release = generator.get_release(rid);
    REQUIRE(release->entries.size() == 1);
}

TEST_CASE("ReleaseNotesGenerator generate markdown", "[phase50][release]")
{
    ReleaseNotesGenerator generator;
    auto rid = generator.create_release("9.0.0", "2026-02-18");
    generator.add_entry(rid, {"Feature A", "Desc", ReleaseCategory::kFeature, "dev"});
    generator.add_entry(rid, {"Bug fix", "Fixed X", ReleaseCategory::kFix, "dev"});

    auto markdown = generator.generate_markdown(rid);
    REQUIRE(markdown.find("9.0.0") != std::string::npos);
    REQUIRE(markdown.find("Features") != std::string::npos);
    REQUIRE(markdown.find("Bug Fixes") != std::string::npos);
}

TEST_CASE("ReleaseNotesGenerator list releases", "[phase50][release]")
{
    ReleaseNotesGenerator generator;
    generator.create_release("8.0.0", "2026-02-17");
    generator.create_release("9.0.0", "2026-02-18");
    auto list = generator.list_releases();
    REQUIRE(list.size() == 2);
}

TEST_CASE("ReleaseNotesGenerator clear", "[phase50][release]")
{
    ReleaseNotesGenerator generator;
    generator.create_release("1.0.0", "2026-01-01");
    generator.clear();
    REQUIRE(generator.release_count() == 0);
}

// ============================================================================
// VersionManager
// ============================================================================

TEST_CASE("VersionManager default version", "[phase50][version]")
{
    VersionManager mgr;
    auto ver = mgr.get_version();
    REQUIRE(ver.major == 1);
    REQUIRE(ver.minor == 0);
    REQUIRE(ver.patch == 0);
}

TEST_CASE("VersionManager bump major", "[phase50][version]")
{
    VersionManager mgr;
    mgr.bump_major();
    auto ver = mgr.get_version();
    REQUIRE(ver.major == 2);
    REQUIRE(ver.minor == 0);
    REQUIRE(ver.patch == 0);
}

TEST_CASE("VersionManager bump minor", "[phase50][version]")
{
    VersionManager mgr;
    mgr.bump_minor();
    auto ver = mgr.get_version();
    REQUIRE(ver.major == 1);
    REQUIRE(ver.minor == 1);
}

TEST_CASE("VersionManager bump patch", "[phase50][version]")
{
    VersionManager mgr;
    mgr.bump_patch();
    REQUIRE(mgr.get_version().patch == 1);
}

TEST_CASE("VersionManager parse version string", "[phase50][version]")
{
    auto ver = VersionManager::parse("v2.3.1-beta");
    REQUIRE(ver.major == 2);
    REQUIRE(ver.minor == 3);
    REQUIRE(ver.patch == 1);
    REQUIRE(ver.pre_release == "beta");
}

TEST_CASE("VersionManager compare", "[phase50][version]")
{
    auto ver_a = VersionManager::parse("1.0.0");
    auto ver_b = VersionManager::parse("2.0.0");
    REQUIRE(VersionManager::compare(ver_a, ver_b) < 0);
    REQUIRE(VersionManager::compare(ver_b, ver_a) > 0);
    REQUIRE(VersionManager::compare(ver_a, ver_a) == 0);
}

TEST_CASE("VersionManager is_compatible", "[phase50][version]")
{
    auto ver_a = VersionManager::parse("2.1.0");
    auto ver_b = VersionManager::parse("2.3.5");
    REQUIRE(VersionManager::is_compatible(ver_a, ver_b));

    auto ver_c = VersionManager::parse("3.0.0");
    REQUIRE_FALSE(VersionManager::is_compatible(ver_a, ver_c));
}

TEST_CASE("VersionManager version history", "[phase50][version]")
{
    VersionManager mgr;
    mgr.bump_patch();
    mgr.bump_minor();
    auto history = mgr.version_history();
    REQUIRE(history.size() == 2);
}

TEST_CASE("VersionManager to_string", "[phase50][version]")
{
    auto ver = VersionManager::parse("3.2.1-rc1");
    REQUIRE(ver.to_string() == "3.2.1-rc1");
}

// ============================================================================
// ChangelogEngine
// ============================================================================

TEST_CASE("ChangelogEngine initial state", "[phase50][changelog]")
{
    ChangelogEngine engine;
    REQUIRE(engine.entry_count() == 0);
}

TEST_CASE("ChangelogEngine load defaults", "[phase50][changelog]")
{
    ChangelogEngine engine;
    engine.load_defaults();
    REQUIRE(engine.entry_count() == 5);
}

TEST_CASE("ChangelogEngine add and filter entries", "[phase50][changelog]")
{
    ChangelogEngine engine;
    engine.add_entry({"2026-02-18", "9.0.0", "Added", "Feature A"});
    engine.add_entry({"2026-02-18", "9.0.0", "Fixed", "Bug B"});
    engine.add_entry({"2026-02-17", "8.0.0", "Added", "Feature C"});

    auto v9_entries = engine.get_entries("9.0.0");
    REQUIRE(v9_entries.size() == 2);
}

TEST_CASE("ChangelogEngine generate markdown", "[phase50][changelog]")
{
    ChangelogEngine engine;
    engine.add_entry({"2026-02-18", "9.0.0", "Added", "New feature"});
    auto markdown = engine.generate_markdown();
    REQUIRE(markdown.find("Changelog") != std::string::npos);
    REQUIRE(markdown.find("9.0.0") != std::string::npos);
}

TEST_CASE("ChangelogEngine clear", "[phase50][changelog]")
{
    ChangelogEngine engine;
    engine.load_defaults();
    engine.clear();
    REQUIRE(engine.entry_count() == 0);
}

// ============================================================================
// DeprecationTracker
// ============================================================================

TEST_CASE("DeprecationTracker initial state", "[phase50][deprecation]")
{
    DeprecationTracker tracker;
    REQUIRE(tracker.deprecation_count() == 0);
}

TEST_CASE("DeprecationTracker add and check", "[phase50][deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({"oldFeature", "8.0.0", "", "newFeature", "Superseded"});
    REQUIRE(tracker.is_deprecated("oldFeature"));
    REQUIRE_FALSE(tracker.is_deprecated("newFeature"));
}

TEST_CASE("DeprecationTracker get replacement", "[phase50][deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({"legacyAPI", "7.0.0", "", "modernAPI", "Better perf"});
    REQUIRE(tracker.get_replacement("legacyAPI") == "modernAPI");
    REQUIRE(tracker.get_replacement("unknown").empty());
}

TEST_CASE("DeprecationTracker active vs removed", "[phase50][deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({"active", "8.0.0", "", "replacement", "reason"});
    tracker.add_deprecation({"removed", "7.0.0", "9.0.0", "newImpl", "reason"});

    REQUIRE(tracker.active_deprecations().size() == 1);
    REQUIRE(tracker.removed_features().size() == 1);
}

TEST_CASE("DeprecationTracker clear", "[phase50][deprecation]")
{
    DeprecationTracker tracker;
    tracker.add_deprecation({"feat", "1.0.0", "", "alt", "reason"});
    tracker.clear();
    REQUIRE(tracker.deprecation_count() == 0);
}

// ============================================================================
// ReleaseCommandProvider
// ============================================================================

TEST_CASE("ReleaseCommandProvider provides 8 commands", "[phase50][commands]")
{
    REQUIRE(ReleaseCommandProvider::command_count() == 8);
    REQUIRE(ReleaseCommandProvider::command_ids().size() == 8);
}

TEST_CASE("ReleaseCommandProvider lookup", "[phase50][commands]")
{
    ReleaseCommandProvider provider;
    auto cmd = provider.get_command("release.create");
    REQUIRE(cmd.id == "release.create");
    REQUIRE(cmd.category == "Release");
}

// ============================================================================
// PolishCommandProvider
// ============================================================================

TEST_CASE("PolishCommandProvider provides 8 commands", "[phase50][commands]")
{
    REQUIRE(PolishCommandProvider::command_count() == 8);
    REQUIRE(PolishCommandProvider::command_ids().size() == 8);
}

TEST_CASE("PolishCommandProvider lookup", "[phase50][commands]")
{
    PolishCommandProvider provider;
    auto cmd = provider.get_command("polish.addChangelog");
    REQUIRE(cmd.id == "polish.addChangelog");
    REQUIRE(cmd.category == "Polish");
}

// ============================================================================
// Phase 50 Events
// ============================================================================

TEST_CASE("ReleaseCreatedEvent fields", "[phase50][events]")
{
    events::ReleaseCreatedEvent evt;
    evt.release_id = "rel_1";
    evt.version = "9.0.0";
    REQUIRE(evt.version == "9.0.0");
}

TEST_CASE("VersionBumpedEvent fields", "[phase50][events]")
{
    events::VersionBumpedEvent evt;
    evt.old_version = "8.0.0";
    evt.new_version = "9.0.0";
    REQUIRE(evt.new_version == "9.0.0");
}

TEST_CASE("ChangelogUpdatedEvent fields", "[phase50][events]")
{
    events::ChangelogUpdatedEvent evt;
    evt.version = "9.0.0";
    evt.entries_added = 5;
    REQUIRE(evt.entries_added == 5);
}

TEST_CASE("DeprecationAddedEvent fields", "[phase50][events]")
{
    events::DeprecationAddedEvent evt;
    evt.feature_name = "legacyAPI";
    evt.replacement = "modernAPI";
    REQUIRE(evt.replacement == "modernAPI");
}

TEST_CASE("ReleasePublishedEvent fields", "[phase50][events]")
{
    events::ReleasePublishedEvent evt;
    evt.version = "9.0.0";
    evt.entry_count = 12;
    REQUIRE(evt.entry_count == 12);
}

TEST_CASE("PolishCompleteEvent fields", "[phase50][events]")
{
    events::PolishCompleteEvent evt;
    evt.checks_passed = 10;
    evt.issues_found = 0;
    evt.release_ready = true;
    REQUIRE(evt.release_ready);
}
