#include "core/FileNode.h"
#include "core/SampleFiles.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ── FileNode creation ──

TEST_CASE("FileNode file creation", "[file_tree]")
{
    FileNode node;
    node.id = "test-file";
    node.name = "test.md";
    node.type = FileNodeType::File;
    node.content = "# Hello";

    REQUIRE(node.is_file());
    REQUIRE_FALSE(node.is_folder());
    REQUIRE(node.id == "test-file");
    REQUIRE(node.name == "test.md");
    REQUIRE(node.content.has_value());
    REQUIRE(node.content.value() == "# Hello");
}

TEST_CASE("FileNode folder creation", "[file_tree]")
{
    FileNode folder;
    folder.id = "test-folder";
    folder.name = "My Folder";
    folder.type = FileNodeType::Folder;

    REQUIRE(folder.is_folder());
    REQUIRE_FALSE(folder.is_file());
    REQUIRE(folder.children.empty());
    REQUIRE_FALSE(folder.is_open);
}

// ── Extension extraction ──

TEST_CASE("FileNode extension", "[file_tree]")
{
    FileNode md;
    md.name = "README.md";
    md.type = FileNodeType::File;
    REQUIRE(md.extension() == ".md");

    FileNode dots;
    dots.name = "archive.tar.gz";
    dots.type = FileNodeType::File;
    REQUIRE(dots.extension() == ".gz");

    FileNode no_ext;
    no_ext.name = "Makefile";
    no_ext.type = FileNodeType::File;
    REQUIRE(no_ext.extension().empty());

    FileNode hidden;
    hidden.name = ".gitignore";
    hidden.type = FileNodeType::File;
    REQUIRE(hidden.extension().empty()); // dot at position 0

    FileNode folder;
    folder.name = "src";
    folder.type = FileNodeType::Folder;
    REQUIRE(folder.extension().empty()); // folders have no extension
}

// ── Recursive find_by_id ──

TEST_CASE("FileNode find_by_id", "[file_tree]")
{
    FileNode root;
    root.id = "root";
    root.name = "Root";
    root.type = FileNodeType::Folder;

    FileNode child;
    child.id = "child-1";
    child.name = "child.md";
    child.type = FileNodeType::File;

    FileNode subfolder;
    subfolder.id = "sub";
    subfolder.name = "Sub";
    subfolder.type = FileNodeType::Folder;

    FileNode deep;
    deep.id = "deep-file";
    deep.name = "deep.md";
    deep.type = FileNodeType::File;

    subfolder.children.push_back(deep);
    root.children.push_back(child);
    root.children.push_back(subfolder);

    // Find self
    REQUIRE(root.find_by_id("root") == &root);

    // Find direct child
    auto* found_child = root.find_by_id("child-1");
    REQUIRE(found_child != nullptr);
    REQUIRE(found_child->name == "child.md");

    // Find deeply nested
    auto* found_deep = root.find_by_id("deep-file");
    REQUIRE(found_deep != nullptr);
    REQUIRE(found_deep->name == "deep.md");

    // Not found
    REQUIRE(root.find_by_id("nonexistent") == nullptr);
}

TEST_CASE("FileNode const find_by_id", "[file_tree]")
{
    FileNode root;
    root.id = "root";
    root.type = FileNodeType::Folder;

    FileNode child;
    child.id = "c";
    child.type = FileNodeType::File;
    root.children.push_back(child);

    const auto& const_root = root;
    const auto* found = const_root.find_by_id("c");
    REQUIRE(found != nullptr);
    REQUIRE(found->id == "c");
}

// ── Counts ──

TEST_CASE("FileNode file_count and folder_count", "[file_tree]")
{
    FileNode root;
    root.id = "root";
    root.type = FileNodeType::Folder;

    FileNode f1;
    f1.type = FileNodeType::File;

    FileNode f2;
    f2.type = FileNodeType::File;

    FileNode sub;
    sub.type = FileNodeType::Folder;

    FileNode f3;
    f3.type = FileNodeType::File;

    sub.children.push_back(f3);
    root.children.push_back(f1);
    root.children.push_back(f2);
    root.children.push_back(sub);

    REQUIRE(root.file_count() == 3);
    REQUIRE(root.folder_count() == 2); // root + sub

    // Single file node
    REQUIRE(f1.file_count() == 1);
    REQUIRE(f1.folder_count() == 0);
}

// ── Empty folder ──

TEST_CASE("FileNode empty folder", "[file_tree]")
{
    FileNode empty_folder;
    empty_folder.type = FileNodeType::Folder;

    REQUIRE(empty_folder.file_count() == 0);
    REQUIRE(empty_folder.folder_count() == 1); // counts itself
    REQUIRE(empty_folder.children.empty());
}

// ── Toggle state ──

TEST_CASE("FileNode toggle is_open", "[file_tree]")
{
    FileNode folder;
    folder.type = FileNodeType::Folder;

    REQUIRE_FALSE(folder.is_open);
    folder.is_open = true;
    REQUIRE(folder.is_open);
    folder.is_open = false;
    REQUIRE_FALSE(folder.is_open);
}

// ── Deep nesting ──

TEST_CASE("FileNode deep nesting (5 levels)", "[file_tree]")
{
    // Build 5-level tree: L0 > L1 > L2 > L3 > L4 > file
    FileNode file_at_bottom;
    file_at_bottom.id = "bottom-file";
    file_at_bottom.type = FileNodeType::File;

    FileNode current = file_at_bottom;
    for (int i = 4; i >= 0; --i)
    {
        FileNode parent;
        parent.id = "level-" + std::to_string(i);
        parent.type = FileNodeType::Folder;
        parent.children.push_back(std::move(current));
        current = std::move(parent);
    }

    // current is now the root (level-0)
    auto* found = current.find_by_id("bottom-file");
    REQUIRE(found != nullptr);
    REQUIRE(found->id == "bottom-file");

    REQUIRE(current.file_count() == 1);
    REQUIRE(current.folder_count() == 5); // 5 folder levels
}

// ── Sample file tree ──

TEST_CASE("Sample file tree structure", "[file_tree]")
{
    auto root = get_sample_file_tree();

    REQUIRE(root.name == "My Project");
    REQUIRE(root.is_folder());
    REQUIRE(root.is_open);
    REQUIRE(root.children.size() == 3);

    // README.md
    REQUIRE(root.children[0].name == "README.md");
    REQUIRE(root.children[0].is_file());
    REQUIRE(root.children[0].content.has_value());

    // Diagrams/
    const auto& diagrams = root.children[1];
    REQUIRE(diagrams.name == "Diagrams");
    REQUIRE(diagrams.is_folder());
    REQUIRE(diagrams.children.size() == 2);
    REQUIRE(diagrams.children[0].name == "architecture.md");
    REQUIRE(diagrams.children[1].name == "sequence.md");

    // TODO.md
    REQUIRE(root.children[2].name == "TODO.md");
    REQUIRE(root.children[2].is_file());
}

TEST_CASE("Sample file tree counts", "[file_tree]")
{
    auto root = get_sample_file_tree();

    REQUIRE(root.file_count() == 4);   // README, architecture, sequence, TODO
    REQUIRE(root.folder_count() == 2); // root + Diagrams
}

TEST_CASE("Sample files contain mermaid content", "[file_tree]")
{
    auto root = get_sample_file_tree();

    const auto* arch = root.find_by_id("architecture");
    REQUIRE(arch != nullptr);
    REQUIRE(arch->content.has_value());
    REQUIRE(arch->content->find("```mermaid") != std::string::npos);
    REQUIRE(arch->content->find("graph TD") != std::string::npos);

    const auto* seq = root.find_by_id("sequence");
    REQUIRE(seq != nullptr);
    REQUIRE(seq->content.has_value());
    REQUIRE(seq->content->find("sequenceDiagram") != std::string::npos);
}
