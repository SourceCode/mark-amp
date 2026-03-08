/// test_extension_sandbox.cpp — Comprehensive tests for ExtensionSandbox
#include "core/ExtensionSandbox.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("ExtensionSandbox: set and check permissions", "[extension_sandbox][positive]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("ext.a",
                            {ExtensionPermission::kFilesystem, ExtensionPermission::kNetwork});
    REQUIRE(sandbox.has_permission("ext.a", ExtensionPermission::kFilesystem));
    REQUIRE(sandbox.has_permission("ext.a", ExtensionPermission::kNetwork));
}

TEST_CASE("ExtensionSandbox: is_registered after set_permissions", "[extension_sandbox][positive]")
{
    ExtensionSandbox sandbox;
    REQUIRE_FALSE(sandbox.is_registered("ext.a"));
    sandbox.set_permissions("ext.a", {ExtensionPermission::kClipboard});
    REQUIRE(sandbox.is_registered("ext.a"));
}

TEST_CASE("ExtensionSandbox: get_permissions returns correct list", "[extension_sandbox][positive]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("ext.a",
                            {ExtensionPermission::kFilesystem, ExtensionPermission::kTerminal});
    auto perms = sandbox.get_permissions("ext.a");
    REQUIRE(perms.size() == 2);
}

TEST_CASE("ExtensionSandbox: remove_extension removes all perms", "[extension_sandbox][positive]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("ext.a", {ExtensionPermission::kDebug});
    sandbox.remove_extension("ext.a");
    REQUIRE_FALSE(sandbox.is_registered("ext.a"));
    REQUIRE_FALSE(sandbox.has_permission("ext.a", ExtensionPermission::kDebug));
}

TEST_CASE("ExtensionSandbox: to_string and from_string roundtrip", "[extension_sandbox][positive]")
{
    auto str = ExtensionSandbox::to_string(ExtensionPermission::kFilesystem);
    REQUIRE_FALSE(str.empty());
    auto perm = ExtensionSandbox::from_string(str);
    REQUIRE(perm == ExtensionPermission::kFilesystem);
}

// ── Negative Tests ──

TEST_CASE("ExtensionSandbox: has_permission for unregistered returns false",
          "[extension_sandbox][negative]")
{
    ExtensionSandbox sandbox;
    REQUIRE_FALSE(sandbox.has_permission("unknown", ExtensionPermission::kNetwork));
}

TEST_CASE("ExtensionSandbox: missing permission returns false", "[extension_sandbox][negative]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("ext.a", {ExtensionPermission::kFilesystem});
    REQUIRE_FALSE(sandbox.has_permission("ext.a", ExtensionPermission::kNetwork));
    REQUIRE_FALSE(sandbox.has_permission("ext.a", ExtensionPermission::kDebug));
}

TEST_CASE("ExtensionSandbox: get_permissions for unregistered returns empty",
          "[extension_sandbox][negative]")
{
    ExtensionSandbox sandbox;
    auto perms = sandbox.get_permissions("unknown");
    REQUIRE(perms.empty());
}

TEST_CASE("ExtensionSandbox: remove_extension on unregistered does not crash",
          "[extension_sandbox][negative]")
{
    ExtensionSandbox sandbox;
    sandbox.remove_extension("nonexistent"); // Should not crash
    REQUIRE_FALSE(sandbox.is_registered("nonexistent"));
}

// ── Edge Cases ──

TEST_CASE("ExtensionSandbox: set_permissions with empty list registers extension",
          "[extension_sandbox][edge]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("ext.a", {});
    // An extension with empty permissions is registered but has no rights
    REQUIRE(sandbox.has_permission("ext.a", ExtensionPermission::kFilesystem) == false);
}

TEST_CASE("ExtensionSandbox: overwrite permissions replaces previous", "[extension_sandbox][edge]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("ext.a", {ExtensionPermission::kFilesystem});
    sandbox.set_permissions("ext.a", {ExtensionPermission::kNetwork});
    REQUIRE_FALSE(sandbox.has_permission("ext.a", ExtensionPermission::kFilesystem));
    REQUIRE(sandbox.has_permission("ext.a", ExtensionPermission::kNetwork));
}

TEST_CASE("ExtensionSandbox: canvas permissions work", "[extension_sandbox][edge]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("canvas-ext",
                            {ExtensionPermission::kCanvasRead,
                             ExtensionPermission::kCanvasWrite,
                             ExtensionPermission::kCanvasSelection});
    REQUIRE(sandbox.has_permission("canvas-ext", ExtensionPermission::kCanvasRead));
    REQUIRE(sandbox.has_permission("canvas-ext", ExtensionPermission::kCanvasWrite));
    REQUIRE_FALSE(sandbox.has_permission("canvas-ext", ExtensionPermission::kCanvasComments));
}
