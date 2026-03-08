// test_key_manager.cpp — 10 tests for KeyManager
#include "core/KeyManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("KeyManager starts empty", "[encryption][keys]")
{
    KeyManager manager;
    CHECK(manager.key_count() == 0);
    CHECK(manager.revoked_count() == 0);
}

TEST_CASE("KeyManager generate_key creates a key", "[encryption][keys]")
{
    KeyManager manager;
    auto key_id = manager.generate_key();
    CHECK_FALSE(key_id.empty());
    CHECK(manager.key_count() == 1);
}

TEST_CASE("KeyManager find_key returns entry", "[encryption][keys]")
{
    KeyManager manager;
    auto key_id = manager.generate_key();
    auto found = manager.find_key(key_id);
    REQUIRE(found != nullptr);
    CHECK(found->algorithm == "AES-256-GCM");
    CHECK(found->is_active);
}

TEST_CASE("KeyManager find_key missing returns null", "[encryption][keys]")
{
    KeyManager manager;
    CHECK(manager.find_key("no-such-key") == nullptr);
}

TEST_CASE("KeyManager revoke_key", "[encryption][keys]")
{
    KeyManager manager;
    auto key_id = manager.generate_key();
    CHECK(manager.revoke_key(key_id));
    CHECK(manager.revoked_count() == 1);
}

TEST_CASE("KeyManager rotate_key produces new key", "[encryption][keys]")
{
    KeyManager manager;
    auto old_id = manager.generate_key();
    auto new_id = manager.rotate_key(old_id);
    CHECK_FALSE(new_id.empty());
    CHECK(new_id != old_id);
    CHECK(manager.key_count() == 2);
}

TEST_CASE("KeyManager active_keys excludes revoked", "[encryption][keys]")
{
    KeyManager manager;
    auto k1 = manager.generate_key();
    auto k2 = manager.generate_key();
    manager.revoke_key(k1);
    auto active = manager.active_keys();
    CHECK(active.size() == 1);
}

TEST_CASE("KeyManager set and get rotation policy", "[encryption][keys]")
{
    KeyManager manager;
    KeyRotationPolicy policy;
    policy.rotation_interval_days = 30;
    policy.auto_rotate = true;
    manager.set_rotation_policy(policy);
    auto retrieved = manager.get_rotation_policy();
    CHECK(retrieved.rotation_interval_days == 30);
    CHECK(retrieved.auto_rotate);
}

TEST_CASE("KeyManager clear_all", "[encryption][keys]")
{
    KeyManager manager;
    manager.generate_key();
    manager.generate_key();
    manager.clear_all();
    CHECK(manager.key_count() == 0);
}

TEST_CASE("KeyRotationPolicy defaults", "[encryption][keys]")
{
    KeyRotationPolicy policy;
    CHECK(policy.rotation_interval_days == 90);
    CHECK(policy.max_key_age_days == 365);
    CHECK_FALSE(policy.auto_rotate);
}
