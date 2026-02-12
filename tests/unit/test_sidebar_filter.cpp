#include "core/FileNode.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Helper: replicate the filter logic from FileTreeCtrl so we can unit-test it
// without requiring wxWidgets.
// ---------------------------------------------------------------------------

namespace
{

auto to_lower(const std::string& input) -> std::string
{
    std::string result = input;
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    return result;
}

auto matches_filter(const markamp::core::FileNode& node, const std::string& lower_filter) -> bool
{
    return to_lower(node.name).find(lower_filter) != std::string::npos;
}

void apply_filter_recursive(std::vector<markamp::core::FileNode>& nodes,
                            const std::string& lower_filter)
{
    for (auto& node : nodes)
    {
        if (node.is_folder())
        {
            apply_filter_recursive(node.children, lower_filter);
            bool has_visible_child = std::any_of(node.children.begin(),
                                                 node.children.end(),
                                                 [](const markamp::core::FileNode& child)
                                                 { return child.filter_visible; });
            node.filter_visible = matches_filter(node, lower_filter) || has_visible_child;
            if (has_visible_child)
            {
                node.is_open = true;
            }
        }
        else
        {
            node.filter_visible = matches_filter(node, lower_filter);
        }
    }
}

void clear_filter(std::vector<markamp::core::FileNode>& nodes)
{
    for (auto& node : nodes)
    {
        node.filter_visible = true;
        if (node.is_folder())
        {
            clear_filter(node.children);
        }
    }
}

// Count visible nodes (matching FileTreeCtrl::UpdateVirtualHeight logic)
auto count_visible(const std::vector<markamp::core::FileNode>& nodes) -> int
{
    int count = 0;
    for (const auto& node : nodes)
    {
        if (!node.filter_visible)
        {
            continue;
        }
        count += 1;
        if (node.is_folder() && node.is_open)
        {
            count += count_visible(node.children);
        }
    }
    return count;
}

// Build a simple test tree:
//   docs/
//     readme.md
//     guide.txt
//     api/
//       endpoints.md
//       schema.json
//   src/
//     main.cpp
//     utils.h
//   notes.md
auto build_test_tree() -> std::vector<markamp::core::FileNode>
{
    using markamp::core::FileNode;
    using markamp::core::FileNodeType;

    FileNode endpoints{"e1", "endpoints.md", FileNodeType::File, "# API", {}, false, true};
    FileNode schema{"e2", "schema.json", FileNodeType::File, "{}", {}, false, true};
    FileNode api_folder{
        "d3", "api", FileNodeType::Folder, std::nullopt, {endpoints, schema}, false, true};

    FileNode readme{"d1", "readme.md", FileNodeType::File, "# Readme", {}, false, true};
    FileNode guide{"d2", "guide.txt", FileNodeType::File, "Guide text", {}, false, true};
    FileNode docs_folder{
        "df", "docs", FileNodeType::Folder, std::nullopt, {readme, guide, api_folder}, true, true};

    FileNode main_cpp{"s1", "main.cpp", FileNodeType::File, "int main()", {}, false, true};
    FileNode utils_h{"s2", "utils.h", FileNodeType::File, "#pragma once", {}, false, true};
    FileNode src_folder{
        "sf", "src", FileNodeType::Folder, std::nullopt, {main_cpp, utils_h}, false, true};

    FileNode notes{"n1", "notes.md", FileNodeType::File, "Notes", {}, false, true};

    return {docs_folder, src_folder, notes};
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// FileNode: filter_visible defaults
// ---------------------------------------------------------------------------

TEST_CASE("FileNode: filter_visible defaults to true", "[sidebar_filter]")
{
    markamp::core::FileNode node;
    REQUIRE(node.filter_visible == true);
}

// ---------------------------------------------------------------------------
// Filter matching: case-insensitive substring
// ---------------------------------------------------------------------------

TEST_CASE("Filter: exact match", "[sidebar_filter]")
{
    markamp::core::FileNode node{
        "id", "readme.md", markamp::core::FileNodeType::File, std::nullopt, {}, false, true};
    REQUIRE(matches_filter(node, "readme.md") == true);
}

TEST_CASE("Filter: case-insensitive match", "[sidebar_filter]")
{
    markamp::core::FileNode node{
        "id", "README.md", markamp::core::FileNodeType::File, std::nullopt, {}, false, true};
    REQUIRE(matches_filter(node, "readme") == true);
}

TEST_CASE("Filter: partial substring match", "[sidebar_filter]")
{
    markamp::core::FileNode node{
        "id", "endpoints.md", markamp::core::FileNodeType::File, std::nullopt, {}, false, true};
    REQUIRE(matches_filter(node, "end") == true);
}

TEST_CASE("Filter: no match", "[sidebar_filter]")
{
    markamp::core::FileNode node{
        "id", "readme.md", markamp::core::FileNodeType::File, std::nullopt, {}, false, true};
    REQUIRE(matches_filter(node, "xyz") == false);
}

// ---------------------------------------------------------------------------
// Tree filtering: visibility propagation
// ---------------------------------------------------------------------------

TEST_CASE("Filter tree: all visible when no filter", "[sidebar_filter]")
{
    auto tree = build_test_tree();
    REQUIRE(count_visible(tree) > 0);

    // All nodes should be visible by default
    for (const auto& node : tree)
    {
        REQUIRE(node.filter_visible == true);
    }
}

TEST_CASE("Filter tree: filter 'md' shows only .md files and their parent folders",
          "[sidebar_filter]")
{
    auto tree = build_test_tree();
    apply_filter_recursive(tree, "md");

    // docs/ should be visible (has md children)
    REQUIRE(tree[0].filter_visible == true);
    // docs/readme.md
    REQUIRE(tree[0].children[0].filter_visible == true);
    // docs/guide.txt — does NOT contain "md"
    REQUIRE(tree[0].children[1].filter_visible == false);
    // docs/api/ should be visible (has endpoints.md)
    REQUIRE(tree[0].children[2].filter_visible == true);
    // docs/api/endpoints.md
    REQUIRE(tree[0].children[2].children[0].filter_visible == true);
    // docs/api/schema.json
    REQUIRE(tree[0].children[2].children[1].filter_visible == false);

    // src/ — no "md" files
    REQUIRE(tree[1].filter_visible == false);

    // notes.md — matches
    REQUIRE(tree[2].filter_visible == true);
}

TEST_CASE("Filter tree: filter 'main' shows only main.cpp and src folder", "[sidebar_filter]")
{
    auto tree = build_test_tree();
    apply_filter_recursive(tree, "main");

    REQUIRE(tree[0].filter_visible == false);             // docs — no match
    REQUIRE(tree[1].filter_visible == true);              // src — has main.cpp
    REQUIRE(tree[1].children[0].filter_visible == true);  // main.cpp
    REQUIRE(tree[1].children[1].filter_visible == false); // utils.h
    REQUIRE(tree[2].filter_visible == false);             // notes.md
}

TEST_CASE("Filter tree: filter auto-expands folders with matching children", "[sidebar_filter]")
{
    auto tree = build_test_tree();
    // src starts closed
    REQUIRE(tree[1].is_open == false);

    apply_filter_recursive(tree, "main");

    // src/ should be auto-expanded because main.cpp matches
    REQUIRE(tree[1].is_open == true);
}

TEST_CASE("Filter tree: filter folder name matches even without matching children",
          "[sidebar_filter]")
{
    auto tree = build_test_tree();
    apply_filter_recursive(tree, "api");

    // api/ folder matches by name
    REQUIRE(tree[0].children[2].filter_visible == true);
    // docs/ is visible because api/ child is visible
    REQUIRE(tree[0].filter_visible == true);
}

// ---------------------------------------------------------------------------
// Clear filter: restores all visibility
// ---------------------------------------------------------------------------

TEST_CASE("Clear filter: restores all nodes to visible", "[sidebar_filter]")
{
    auto tree = build_test_tree();

    // Apply restrictive filter
    apply_filter_recursive(tree, "xyz_no_match");

    // Verify some nodes are hidden
    bool all_visible =
        std::all_of(tree.begin(),
                    tree.end(),
                    [](const markamp::core::FileNode& node) { return node.filter_visible; });
    REQUIRE(all_visible == false);

    // Clear
    clear_filter(tree);

    // All should be visible again
    for (const auto& node : tree)
    {
        REQUIRE(node.filter_visible == true);
    }
}

// ---------------------------------------------------------------------------
// count_visible: respects filter_visible
// ---------------------------------------------------------------------------

TEST_CASE("count_visible: counts all expanded nodes when unfiltered", "[sidebar_filter]")
{
    auto tree = build_test_tree();
    // docs (open) has: readme, guide, api (closed) = 3 children
    // src (closed) = 0 children shown
    // notes = standalone
    // Total: docs + readme + guide + api + src + notes = 6
    int count = count_visible(tree);
    REQUIRE(count == 6);
}

TEST_CASE("count_visible: skips filtered-out nodes", "[sidebar_filter]")
{
    auto tree = build_test_tree();
    apply_filter_recursive(tree, "main");
    // src (open, expanded by filter) + main.cpp = 2
    int count = count_visible(tree);
    REQUIRE(count == 2);
}

// ---------------------------------------------------------------------------
// Empty filter: no-op
// ---------------------------------------------------------------------------

TEST_CASE("Filter: empty string does not hide anything", "[sidebar_filter]")
{
    auto tree = build_test_tree();
    // Simulate what ApplyFilter("") would do: call clear_filter
    clear_filter(tree);

    for (const auto& node : tree)
    {
        REQUIRE(node.filter_visible == true);
    }
}
