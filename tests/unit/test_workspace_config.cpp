/// @file test_workspace_config.cpp
/// @brief Phase 40 — Unit tests for WorkspaceConfig, WorkspaceTrustService,
///        RecentWorkspacesManager, WorkspaceEnvironment.

#include "core/RecentWorkspacesManager.h"
#include "core/WorkspaceConfig.h"
#include "core/WorkspaceEnvironment.h"
#include "core/WorkspaceTrustService.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using namespace markamp::core;

// ============================================================================
// WorkspaceConfig — data model
// ============================================================================

TEST_CASE("WorkspaceConfig: single_folder", "[workspace_config]")
{
    auto config = WorkspaceConfig::single_folder("/project");
    REQUIRE(config.folders.size() == 1);
    REQUIRE(config.folders[0].path == "/project");
    REQUIRE_FALSE(config.is_multi_root());
}

TEST_CASE("WorkspaceConfig: multi-root", "[workspace_config]")
{
    WorkspaceConfig config;
    config.add_folder("/project1", "Project 1");
    config.add_folder("/project2", "Project 2");
    REQUIRE(config.is_multi_root());
    REQUIRE(config.folders.size() == 2);
}

TEST_CASE("WorkspaceConfig: add_folder extracts basename", "[workspace_config]")
{
    WorkspaceConfig config;
    config.add_folder("/Users/dev/myproject");
    REQUIRE(config.folders[0].name == "myproject");
}

TEST_CASE("WorkspaceConfig: remove_folder", "[workspace_config]")
{
    WorkspaceConfig config;
    config.add_folder("/project1");
    config.add_folder("/project2");
    REQUIRE(config.remove_folder("/project1"));
    REQUIRE(config.folders.size() == 1);
    REQUIRE(config.folders[0].path == "/project2");
}

TEST_CASE("WorkspaceConfig: remove non-existent folder", "[workspace_config]")
{
    WorkspaceConfig config;
    config.add_folder("/project1");
    REQUIRE_FALSE(config.remove_folder("/nonexistent"));
}

TEST_CASE("WorkspaceConfig: reorder_folder", "[workspace_config]")
{
    WorkspaceConfig config;
    config.add_folder("/a");
    config.add_folder("/b");
    config.add_folder("/c");
    config.reorder_folder(2, 0);
    REQUIRE(config.folders[0].path == "/c");
    REQUIRE(config.folders[1].path == "/a");
    REQUIRE(config.folders[2].path == "/b");
}

TEST_CASE("WorkspaceConfig: YAML round-trip", "[workspace_config]")
{
    WorkspaceConfig config;
    config.add_folder("/project1", "Proj1");
    config.add_folder("/project2", "Proj2");
    config.exclude_patterns = {"**/node_modules", "**/.git"};

    auto yaml = config.to_yaml();
    REQUIRE_FALSE(yaml.empty());

    auto restored = WorkspaceConfig::from_yaml(yaml);
    REQUIRE(restored.folders.size() == 2);
    REQUIRE(restored.folders[0].path == "/project1");
    REQUIRE(restored.folders[1].name == "Proj2");
    REQUIRE(restored.exclude_patterns.size() == 2);
}

TEST_CASE("WorkspaceConfig: from_yaml handles empty", "[workspace_config]")
{
    auto config = WorkspaceConfig::from_yaml("");
    REQUIRE(config.folders.empty());
}

// ============================================================================
// WorkspaceTrustService
// ============================================================================

TEST_CASE("WorkspaceTrust: default is untrusted", "[workspace_trust]")
{
    WorkspaceTrustService trust;
    REQUIRE_FALSE(trust.is_trusted("/project"));
    REQUIRE(trust.trust_level("/project") == WorkspaceTrustLevel::kUntrusted);
}

TEST_CASE("WorkspaceTrust: grant and revoke", "[workspace_trust]")
{
    WorkspaceTrustService trust;
    trust.grant_trust("/project");
    REQUIRE(trust.is_trusted("/project"));
    REQUIRE(trust.trust_level("/project") == WorkspaceTrustLevel::kTrusted);

    trust.revoke_trust("/project");
    REQUIRE_FALSE(trust.is_trusted("/project"));
}

TEST_CASE("WorkspaceTrust: trusted_workspaces list", "[workspace_trust]")
{
    WorkspaceTrustService trust;
    trust.grant_trust("/a");
    trust.grant_trust("/b");
    auto all = trust.trusted_workspaces();
    REQUIRE(all.size() == 2);
}

TEST_CASE("WorkspaceTrust: feature restrictions", "[workspace_trust]")
{
    WorkspaceTrustService trust;
    // Trusted allows everything
    REQUIRE(trust.is_feature_allowed("task.execution", WorkspaceTrustLevel::kTrusted));
    REQUIRE(trust.is_feature_allowed("build.commands", WorkspaceTrustLevel::kTrusted));

    // Untrusted restricts dangerous features
    REQUIRE_FALSE(trust.is_feature_allowed("task.execution", WorkspaceTrustLevel::kUntrusted));
    REQUIRE_FALSE(trust.is_feature_allowed("terminal.commands", WorkspaceTrustLevel::kUntrusted));
    REQUIRE_FALSE(trust.is_feature_allowed("build.commands", WorkspaceTrustLevel::kUntrusted));
}

TEST_CASE("WorkspaceTrust: restricted_features list", "[workspace_trust]")
{
    WorkspaceTrustService trust;
    auto restricted = trust.restricted_features();
    REQUIRE(restricted.size() >= 3);
}

// ============================================================================
// RecentWorkspacesManager
// ============================================================================

TEST_CASE("RecentWorkspaces: add and retrieve", "[recent_workspaces]")
{
    RecentWorkspacesManager mgr;
    mgr.add("/project1", "Project 1");
    REQUIRE(mgr.count() == 1);
    auto* recent = mgr.most_recent();
    REQUIRE(recent != nullptr);
    REQUIRE(recent->path == "/project1");
    REQUIRE(recent->name == "Project 1");
}

TEST_CASE("RecentWorkspaces: MRU ordering", "[recent_workspaces]")
{
    RecentWorkspacesManager mgr;
    mgr.add("/a");
    mgr.add("/b");
    mgr.add("/c");
    REQUIRE(mgr.most_recent()->path == "/c");

    // Re-adding /a promotes it to front
    mgr.add("/a");
    REQUIRE(mgr.most_recent()->path == "/a");
    REQUIRE(mgr.count() == 3); // no duplicates
}

TEST_CASE("RecentWorkspaces: remove", "[recent_workspaces]")
{
    RecentWorkspacesManager mgr;
    mgr.add("/a");
    mgr.add("/b");
    REQUIRE(mgr.remove("/a"));
    REQUIRE(mgr.count() == 1);
    REQUIRE_FALSE(mgr.remove("/nonexistent"));
}

TEST_CASE("RecentWorkspaces: max limit", "[recent_workspaces]")
{
    RecentWorkspacesManager mgr;
    for (int i = 0; i < 25; ++i)
    {
        mgr.add("/project" + std::to_string(i));
    }
    REQUIRE(mgr.count() <= RecentWorkspacesManager::kMaxRecent);
}

TEST_CASE("RecentWorkspaces: clear", "[recent_workspaces]")
{
    RecentWorkspacesManager mgr;
    mgr.add("/a");
    mgr.add("/b");
    mgr.clear();
    REQUIRE(mgr.count() == 0);
}

TEST_CASE("RecentWorkspaces: auto-name from path", "[recent_workspaces]")
{
    RecentWorkspacesManager mgr;
    mgr.add("/Users/dev/my-project");
    REQUIRE(mgr.most_recent()->name == "my-project");
}

// ============================================================================
// WorkspaceEnvironment
// ============================================================================

TEST_CASE("WorkspaceEnvironment: set and get", "[workspace_env]")
{
    WorkspaceEnvironment env;
    env.set("API_KEY", "secret123");
    REQUIRE(env.has("API_KEY"));
    REQUIRE(env.get("API_KEY") == "secret123");
}

TEST_CASE("WorkspaceEnvironment: get missing key", "[workspace_env]")
{
    WorkspaceEnvironment env;
    REQUIRE(env.get("MISSING").empty());
    REQUIRE_FALSE(env.has("MISSING"));
}

TEST_CASE("WorkspaceEnvironment: remove", "[workspace_env]")
{
    WorkspaceEnvironment env;
    env.set("KEY", "val");
    REQUIRE(env.remove("KEY"));
    REQUIRE_FALSE(env.has("KEY"));
    REQUIRE_FALSE(env.remove("NONEXISTENT"));
}

TEST_CASE("WorkspaceEnvironment: count and clear", "[workspace_env]")
{
    WorkspaceEnvironment env;
    env.set("A", "1");
    env.set("B", "2");
    REQUIRE(env.count() == 2);
    env.clear();
    REQUIRE(env.count() == 0);
}

TEST_CASE("WorkspaceEnvironment: expand variables", "[workspace_env]")
{
    WorkspaceEnvironment env;
    env.set("PROJECT", "myapp");
    env.set("VERSION", "1.0");

    auto result = env.expand("${PROJECT}-${VERSION}");
    REQUIRE(result == "myapp-1.0");
}

TEST_CASE("WorkspaceEnvironment: expand unknown variable", "[workspace_env]")
{
    WorkspaceEnvironment env;
    auto result = env.expand("${UNKNOWN_VAR_12345}");
    // Unknown vars get replaced with empty string (or system env)
    REQUIRE(result.find("${") == std::string::npos);
}

TEST_CASE("WorkspaceEnvironment: expand no variables", "[workspace_env]")
{
    WorkspaceEnvironment env;
    auto result = env.expand("plain text");
    REQUIRE(result == "plain text");
}

TEST_CASE("WorkspaceEnvironment: all returns map", "[workspace_env]")
{
    WorkspaceEnvironment env;
    env.set("A", "1");
    env.set("B", "2");
    auto& all = env.all();
    REQUIRE(all.size() == 2);
    REQUIRE(all.at("A") == "1");
}
