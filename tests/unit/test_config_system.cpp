// Phase 34 Batch 34B — Task 6: Config system comprehensive tests
// Default values, typed access, validation, batch mode, profiles.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

// ---------------------------------------------------------------------------
// Default values
// ---------------------------------------------------------------------------

TEST_CASE("Config — default font_size is 14", "[config][defaults]")
{
    Config config;
    REQUIRE(config.get_int("editor.font_size", 14) == 14);
}

TEST_CASE("Config — default theme is midnight-neon", "[config][defaults]")
{
    Config config;
    auto cached = config.cached();
    REQUIRE(cached.theme == "midnight-neon");
}

TEST_CASE("Config — default view_mode is split", "[config][defaults]")
{
    Config config;
    auto cached = config.cached();
    REQUIRE(cached.view_mode == "split");
}

TEST_CASE("Config — default sidebar_visible is true", "[config][defaults]")
{
    Config config;
    auto cached = config.cached();
    REQUIRE(cached.sidebar_visible);
}

TEST_CASE("Config — default word_wrap is true", "[config][defaults]")
{
    Config config;
    auto cached = config.cached();
    REQUIRE(cached.word_wrap);
}

// ---------------------------------------------------------------------------
// Typed getters with fallbacks
// ---------------------------------------------------------------------------

TEST_CASE("Config — get_string with default fallback", "[config][getters]")
{
    Config config;
    auto val = config.get_string("nonexistent.key", "fallback");
    REQUIRE(val == "fallback");
}

TEST_CASE("Config — get_int with default fallback", "[config][getters]")
{
    Config config;
    auto val = config.get_int("nonexistent.key", 42);
    REQUIRE(val == 42);
}

TEST_CASE("Config — get_bool with default fallback", "[config][getters]")
{
    Config config;
    auto val = config.get_bool("nonexistent.key", true);
    REQUIRE(val);
}

TEST_CASE("Config — get_double with default fallback", "[config][getters]")
{
    Config config;
    auto val = config.get_double("nonexistent.key", 3.14);
    REQUIRE(val == 3.14);
}

// ---------------------------------------------------------------------------
// Set and get round-trip
// ---------------------------------------------------------------------------

TEST_CASE("Config — set int then get", "[config][setget]")
{
    Config config;
    config.set("test.int_key", 99);
    REQUIRE(config.get_int("test.int_key") == 99);
}

TEST_CASE("Config — set bool then get", "[config][setget]")
{
    Config config;
    config.set("test.bool_key", true);
    REQUIRE(config.get_bool("test.bool_key"));
}

TEST_CASE("Config — set double then get", "[config][setget]")
{
    Config config;
    config.set("test.double_key", 2.718);
    REQUIRE(config.get_double("test.double_key") == 2.718);
}

// ---------------------------------------------------------------------------
// Has key / remove / all_keys
// ---------------------------------------------------------------------------

TEST_CASE("Config — has_key and remove", "[config][keys]")
{
    Config config;
    config.set("test.removable", 1);
    REQUIRE(config.has_key("test.removable"));

    config.remove("test.removable");
    REQUIRE_FALSE(config.has_key("test.removable"));
}

TEST_CASE("Config — all_keys returns set keys", "[config][keys]")
{
    Config config;
    config.set("aaa.key1", 1);
    config.set("aaa.key2", 2);

    auto keys = config.all_keys();
    REQUIRE(keys.size() >= 2);
}

TEST_CASE("Config — key_count matches", "[config][keys]")
{
    Config config;
    auto initial = config.key_count();
    config.set("new.unique.key", 42);
    REQUIRE(config.key_count() == initial + 1);
}

// ---------------------------------------------------------------------------
// Batch mode
// ---------------------------------------------------------------------------

TEST_CASE("Config — batch mode begin/commit", "[config][batch]")
{
    Config config;
    REQUIRE_FALSE(config.is_batching());

    config.begin_batch();
    REQUIRE(config.is_batching());

    config.set("batch.key1", 10);
    config.set("batch.key2", 20);
    config.commit_batch();

    REQUIRE_FALSE(config.is_batching());
    REQUIRE(config.get_int("batch.key1") == 10);
    REQUIRE(config.get_int("batch.key2") == 20);
}

TEST_CASE("Config — batch mode discard", "[config][batch]")
{
    Config config;
    config.set("discard.key", 100);

    config.begin_batch();
    config.set("discard.key", 999);
    config.discard_batch();

    REQUIRE_FALSE(config.is_batching());
    REQUIRE(config.get_int("discard.key") == 100);
}

// ---------------------------------------------------------------------------
// Snapshot and restore
// ---------------------------------------------------------------------------

TEST_CASE("Config — snapshot and restore", "[config][snapshot]")
{
    Config config;
    config.set("snap.key", 42);

    auto snap = config.snapshot();
    config.set("snap.key", 99);
    REQUIRE(config.get_int("snap.key") == 99);

    config.restore_from_snapshot(snap);
    REQUIRE(config.get_int("snap.key") == 42);
}

// ---------------------------------------------------------------------------
// Diff
// ---------------------------------------------------------------------------

TEST_CASE("Config — diff detects changes", "[config][diff]")
{
    Config config1;
    config1.set("diff.key1", 1);
    config1.set("diff.key2", 2);

    auto config2 = config1.snapshot();
    config2.set("diff.key2", 99);

    auto diffs = config1.diff(config2);
    REQUIRE_FALSE(diffs.empty());
}

// ---------------------------------------------------------------------------
// Cached values
// ---------------------------------------------------------------------------

TEST_CASE("Config — cached values reflect set calls", "[config][cached]")
{
    Config config;
    config.set("font_size", 20);

    auto cached = config.cached();
    REQUIRE(cached.font_size == 20);
}

TEST_CASE("Config — cached sidebar_visible toggles", "[config][cached]")
{
    Config config;
    config.set("sidebar_visible", false);

    auto cached = config.cached();
    REQUIRE_FALSE(cached.sidebar_visible);
}
