/// @file test_v20_notebook_storage.cpp
/// @brief V20 Phase 03 – NotebookStorageResolver unit tests.

#include "core/NotebookStorageResolver.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("NotebookStorageResolver: construction", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;
    REQUIRE(resolver.resolution_count() == 0);
}

// ============================================================================
// Resolve with workspace
// ============================================================================

TEST_CASE("NotebookStorageResolver: resolve with workspace", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    auto location = resolver.resolve("/workspace");

    REQUIRE(location.resolved_path == "/workspace/notebooks");
    REQUIRE(location.is_workspace);
    REQUIRE_FALSE(location.is_legacy);
}

// ============================================================================
// Resolve with explicit path
// ============================================================================

TEST_CASE("NotebookStorageResolver: explicit path wins", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    auto location = resolver.resolve("/workspace", "/explicit/dir");

    REQUIRE(location.resolved_path == "/explicit/dir");
}

// ============================================================================
// Resolve with knowledgebase (legacy)
// ============================================================================

TEST_CASE("NotebookStorageResolver: fallback to knowledgebase", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    auto location = resolver.resolve("", "", "/home/user/.markamp/kb");

    REQUIRE(location.resolved_path == "/home/user/.markamp/kb");
    REQUIRE(location.is_legacy);
    REQUIRE_FALSE(location.is_workspace);
}

// ============================================================================
// Resolve with nothing
// ============================================================================

TEST_CASE("NotebookStorageResolver: resolve with nothing", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    auto location = resolver.resolve();

    REQUIRE(location.resolved_path.empty());
    REQUIRE_FALSE(location.is_workspace);
    REQUIRE_FALSE(location.is_legacy);
}

// ============================================================================
// Workspace notebook dir
// ============================================================================

TEST_CASE("NotebookStorageResolver: workspace_notebook_dir", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    REQUIRE(resolver.workspace_notebook_dir("/project") == "/project/notebooks");
    REQUIRE(resolver.workspace_notebook_dir("").empty());
}

// ============================================================================
// Path classification
// ============================================================================

TEST_CASE("NotebookStorageResolver: is_workspace_path", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    REQUIRE(resolver.is_workspace_path("/workspace/notebooks/nb.ipynb", "/workspace"));
    REQUIRE_FALSE(resolver.is_workspace_path("/other/nb.ipynb", "/workspace"));
    REQUIRE_FALSE(resolver.is_workspace_path("", "/workspace"));
}

TEST_CASE("NotebookStorageResolver: is_legacy_path", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    REQUIRE(resolver.is_legacy_path("/home/.markamp/kb/nb.ipynb", "/home/.markamp/kb"));
    REQUIRE_FALSE(resolver.is_legacy_path("/workspace/nb.ipynb", "/home/.markamp/kb"));
}

// ============================================================================
// Resolution count
// ============================================================================

TEST_CASE("NotebookStorageResolver: resolution count", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    resolver.resolve("/ws");
    resolver.resolve("/ws2");

    REQUIRE(resolver.resolution_count() == 2);
}

// ============================================================================
// Priority chain
// ============================================================================

TEST_CASE("NotebookStorageResolver: priority explicit > workspace > kb", "[v20][notebook-storage]")
{
    NotebookStorageResolver resolver;

    // All three provided: explicit wins
    auto loc = resolver.resolve("/workspace", "/explicit", "/kb");
    REQUIRE(loc.resolved_path == "/explicit");

    // No explicit: workspace wins
    auto loc2 = resolver.resolve("/workspace", "", "/kb");
    REQUIRE(loc2.resolved_path == "/workspace/notebooks");

    // No workspace: kb wins
    auto loc3 = resolver.resolve("", "", "/kb");
    REQUIRE(loc3.resolved_path == "/kb");
}
