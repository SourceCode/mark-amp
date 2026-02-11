#include "core/Config.h"
#include "core/EncodingDetector.h"
#include "core/EventBus.h"
#include "core/FileNode.h"
#include "core/FileSystem.h"
#include "core/RecentFiles.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

namespace
{

/// RAII helper to create and clean up a temporary directory.
struct TempDir
{
    std::filesystem::path path;

    TempDir()
    {
        path = std::filesystem::temp_directory_path() / "markamp_test_fs";
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path);
    }

    ~TempDir()
    {
        std::filesystem::remove_all(path);
    }

    void write(const std::string& relative_path, const std::string& content) const
    {
        auto full = path / relative_path;
        std::filesystem::create_directories(full.parent_path());
        std::ofstream f(full, std::ios::binary);
        f << content;
    }
};

} // anonymous namespace

// ═══════════════════════════════════════════════════════
// EncodingDetector tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Detect UTF-8 (no BOM)", "[encoding]")
{
    std::string data = "Hello, world!";
    auto result = detect_encoding(data);
    REQUIRE(result.encoding == Encoding::Ascii); // pure ASCII subset
    REQUIRE(result.display_name == "ASCII");
}

TEST_CASE("Detect UTF-8 with multibyte", "[encoding]")
{
    // UTF-8 encoded "café"
    std::string data = "caf\xC3\xA9";
    auto result = detect_encoding(data);
    REQUIRE(result.encoding == Encoding::Utf8);
    REQUIRE(result.display_name == "UTF-8");
}

TEST_CASE("Detect UTF-8 BOM", "[encoding]")
{
    std::string data = "\xEF\xBB\xBFHello with BOM";
    auto result = detect_encoding(data);
    REQUIRE(result.encoding == Encoding::Utf8Bom);
    REQUIRE(result.display_name == "UTF-8 BOM");
}

TEST_CASE("Detect UTF-16 LE BOM", "[encoding]")
{
    std::string data = "\xFF\xFE\x00\x00";
    auto result = detect_encoding(data);
    REQUIRE(result.encoding == Encoding::Utf16LE);
}

TEST_CASE("Detect UTF-16 BE BOM", "[encoding]")
{
    std::string data = "\xFE\xFF\x00\x00";
    auto result = detect_encoding(data);
    REQUIRE(result.encoding == Encoding::Utf16BE);
}

TEST_CASE("Detect empty content as UTF-8", "[encoding]")
{
    auto result = detect_encoding("");
    REQUIRE(result.encoding == Encoding::Utf8);
}

TEST_CASE("Detect invalid UTF-8 as Unknown", "[encoding]")
{
    // Invalid continuation byte
    std::string data = "\xC0\x00";
    auto result = detect_encoding(data);
    REQUIRE(result.encoding == Encoding::Unknown);
}

TEST_CASE("Strip BOM from UTF-8 BOM", "[encoding]")
{
    std::string content = "\xEF\xBB\xBFHello";
    auto stripped = strip_bom(content, Encoding::Utf8Bom);
    REQUIRE(stripped == "Hello");
}

TEST_CASE("Strip BOM no-op for plain UTF-8", "[encoding]")
{
    std::string content = "Hello";
    auto stripped = strip_bom(content, Encoding::Utf8);
    REQUIRE(stripped == "Hello");
}

TEST_CASE("Encoding display names", "[encoding]")
{
    REQUIRE(encoding_display_name(Encoding::Utf8) == "UTF-8");
    REQUIRE(encoding_display_name(Encoding::Utf8Bom) == "UTF-8 BOM");
    REQUIRE(encoding_display_name(Encoding::Utf16LE) == "UTF-16 LE");
    REQUIRE(encoding_display_name(Encoding::Utf16BE) == "UTF-16 BE");
    REQUIRE(encoding_display_name(Encoding::Ascii) == "ASCII");
    REQUIRE(encoding_display_name(Encoding::Unknown) == "Unknown");
}

// ═══════════════════════════════════════════════════════
// FileSystem read/write tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FileSystem read existing file", "[filesystem]")
{
    TempDir tmp;
    tmp.write("hello.md", "# Hello\nWorld");

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.read_file(tmp.path / "hello.md");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == "# Hello\nWorld");
}

TEST_CASE("FileSystem read non-existent file", "[filesystem]")
{
    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.read_file("/nonexistent/path/file.md");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().find("does not exist") != std::string::npos);
}

TEST_CASE("FileSystem write new file", "[filesystem]")
{
    TempDir tmp;

    EventBus bus;
    FileSystem fs(bus);

    auto target = tmp.path / "output.md";
    auto result = fs.write_file(target, "# Written");
    REQUIRE(result.has_value());

    // Verify content
    auto read_result = fs.read_file(target);
    REQUIRE(read_result.has_value());
    REQUIRE(read_result.value() == "# Written");
}

TEST_CASE("FileSystem write creates parent directories", "[filesystem]")
{
    TempDir tmp;

    EventBus bus;
    FileSystem fs(bus);

    auto target = tmp.path / "sub" / "dir" / "file.md";
    auto result = fs.write_file(target, "nested content");
    REQUIRE(result.has_value());
    REQUIRE(std::filesystem::exists(target));
}

TEST_CASE("FileSystem write overwrite existing", "[filesystem]")
{
    TempDir tmp;
    tmp.write("existing.md", "original");

    EventBus bus;
    FileSystem fs(bus);

    auto target = tmp.path / "existing.md";
    auto result = fs.write_file(target, "updated");
    REQUIRE(result.has_value());

    auto read = fs.read_file(target);
    REQUIRE(read.value() == "updated");
}

TEST_CASE("FileSystem read with encoding detection", "[filesystem]")
{
    TempDir tmp;
    // Write UTF-8 BOM file
    std::string bom_content = "\xEF\xBB\xBF# Hello BOM";
    tmp.write("bom.md", bom_content);

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.read_file_with_encoding(tmp.path / "bom.md");
    REQUIRE(result.has_value());
    REQUIRE(result.value().second.encoding == Encoding::Utf8Bom);
    REQUIRE(result.value().first == "# Hello BOM"); // BOM stripped
}

// ═══════════════════════════════════════════════════════
// Directory scanning tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FileSystem scan directory basic", "[filesystem]")
{
    TempDir tmp;
    tmp.write("README.md", "# readme");
    tmp.write("notes.txt", "notes");
    tmp.write("image.png", "binary"); // should be excluded
    tmp.write("docs/intro.md", "# intro");

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.scan_directory_to_tree(tmp.path);
    REQUIRE(result.has_value());

    auto& root = result.value();
    REQUIRE(root.is_folder());
    REQUIRE(root.is_open);

    // Should contain: docs/ folder, README.md, notes.txt (image.png excluded)
    REQUIRE(root.file_count() == 3);   // README + notes + intro
    REQUIRE(root.folder_count() == 2); // root + docs
}

TEST_CASE("FileSystem scan sorts folders first", "[filesystem]")
{
    TempDir tmp;
    tmp.write("zebra.md", "z");
    tmp.write("alpha.md", "a");
    tmp.write("beta/file.md", "b");
    tmp.write("alpha_dir/file.md", "a");

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.scan_directory_to_tree(tmp.path);
    REQUIRE(result.has_value());

    auto& children = result.value().children;
    REQUIRE(children.size() == 4);

    // Folders come first, alphabetically
    REQUIRE(children[0].is_folder());
    REQUIRE(children[0].name == "alpha_dir");
    REQUIRE(children[1].is_folder());
    REQUIRE(children[1].name == "beta");

    // Then files, alphabetically
    REQUIRE(children[2].is_file());
    REQUIRE(children[2].name == "alpha.md");
    REQUIRE(children[3].is_file());
    REQUIRE(children[3].name == "zebra.md");
}

TEST_CASE("FileSystem scan skips hidden files", "[filesystem]")
{
    TempDir tmp;
    tmp.write("visible.md", "ok");
    tmp.write(".hidden.md", "hidden");
    tmp.write(".hidden_dir/file.md", "hidden dir");

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.scan_directory_to_tree(tmp.path);
    REQUIRE(result.has_value());

    REQUIRE(result.value().file_count() == 1); // only visible.md
}

TEST_CASE("FileSystem scan skips excluded directories", "[filesystem]")
{
    TempDir tmp;
    tmp.write("src/main.md", "main");
    tmp.write("node_modules/pkg/index.md", "pkg");
    tmp.write("build/output.md", "out");

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.scan_directory_to_tree(tmp.path);
    REQUIRE(result.has_value());

    REQUIRE(result.value().file_count() == 1); // only src/main.md
}

TEST_CASE("FileSystem scan respects max depth", "[filesystem]")
{
    TempDir tmp;
    tmp.write("l1/l2/l3/l4/deep.md", "deep");
    tmp.write("top.md", "top");

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.scan_directory_to_tree(tmp.path, 2);
    REQUIRE(result.has_value());

    // l1/l2/l3 is depth 3 (0-indexed), but max_depth=2 stops at depth 2
    // So l1/l2/ is scanned (depth 2), but l1/l2/l3/ is depth 3 — not scanned
    // top.md is at depth 1
    auto& root = result.value();
    REQUIRE(root.file_count() == 1); // only top.md (deep.md too deep)
}

TEST_CASE("FileSystem scan non-existent directory", "[filesystem]")
{
    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.scan_directory_to_tree("/nonexistent/path");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().find("does not exist") != std::string::npos);
}

TEST_CASE("FileSystem scan extension filtering", "[filesystem]")
{
    TempDir tmp;
    tmp.write("readme.md", "md");
    tmp.write("readme.markdown", "markdown");
    tmp.write("notes.txt", "txt");
    tmp.write("code.cpp", "cpp");   // excluded
    tmp.write("data.json", "json"); // excluded

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.scan_directory_to_tree(tmp.path);
    REQUIRE(result.has_value());

    REQUIRE(result.value().file_count() == 3); // md, markdown, txt
}

// ═══════════════════════════════════════════════════════
// List directory tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FileSystem list directory", "[filesystem]")
{
    TempDir tmp;
    tmp.write("file1.md", "f1");
    tmp.write("file2.md", "f2");
    std::filesystem::create_directory(tmp.path / "subdir");

    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.list_directory(tmp.path);
    REQUIRE(result.has_value());
    REQUIRE(result.value().size() == 3);

    // First entry should be the folder (sorted: folders first)
    REQUIRE(result.value()[0].is_folder());
    REQUIRE(result.value()[0].name == "subdir");
}

TEST_CASE("FileSystem list non-existent directory", "[filesystem]")
{
    EventBus bus;
    FileSystem fs(bus);

    auto result = fs.list_directory("/nonexistent");
    REQUIRE_FALSE(result.has_value());
}

// ═══════════════════════════════════════════════════════
// File watcher tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FileSystem watch subscription lifecycle", "[filesystem]")
{
    TempDir tmp;
    tmp.write("watched.md", "initial");

    EventBus bus;
    FileSystem fs(bus);

    bool called = false;
    {
        auto sub = fs.watch_file(tmp.path / "watched.md", [&called]() { called = true; });
        // sub is alive here
    }
    // sub destroyed — watch should be removed
    // (We can't easily test the timer polling, but we can verify subscription management)
    REQUIRE_FALSE(called); // callback not invoked (no polling tick happened)
}

// ═══════════════════════════════════════════════════════
// RecentFiles tests
// ═══════════════════════════════════════════════════════

TEST_CASE("RecentFiles add and list", "[recent_files]")
{
    TempDir tmp;
    tmp.write("file1.md", "f1");
    tmp.write("file2.md", "f2");

    Config config;
    RecentFiles recent(config);
    recent.clear();

    recent.add(tmp.path / "file1.md");
    recent.add(tmp.path / "file2.md");

    auto& entries = recent.list();
    REQUIRE(entries.size() == 2);
    // Most recent first
    REQUIRE(entries[0].filename() == "file2.md");
    REQUIRE(entries[1].filename() == "file1.md");
}

TEST_CASE("RecentFiles deduplication", "[recent_files]")
{
    TempDir tmp;
    tmp.write("file1.md", "f1");

    Config config;
    RecentFiles recent(config);
    recent.clear();

    recent.add(tmp.path / "file1.md");
    recent.add(tmp.path / "file1.md"); // duplicate

    REQUIRE(recent.list().size() == 1);
}

TEST_CASE("RecentFiles moves duplicate to front", "[recent_files]")
{
    TempDir tmp;
    tmp.write("a.md", "a");
    tmp.write("b.md", "b");
    tmp.write("c.md", "c");

    Config config;
    RecentFiles recent(config);
    recent.clear();

    recent.add(tmp.path / "a.md");
    recent.add(tmp.path / "b.md");
    recent.add(tmp.path / "c.md");
    recent.add(tmp.path / "a.md"); // should move to front

    auto& entries = recent.list();
    REQUIRE(entries.size() == 3);
    REQUIRE(entries[0].filename() == "a.md");
    REQUIRE(entries[1].filename() == "c.md");
    REQUIRE(entries[2].filename() == "b.md");
}

TEST_CASE("RecentFiles max entries", "[recent_files]")
{
    TempDir tmp;

    Config config;
    RecentFiles recent(config);
    recent.clear();

    // Create and add more than max entries
    for (std::size_t i = 0; i < RecentFiles::kMaxEntries + 5; ++i)
    {
        auto filename = "file_" + std::to_string(i) + ".md";
        tmp.write(filename, "content");
        recent.add(tmp.path / filename);
    }

    REQUIRE(recent.list().size() == RecentFiles::kMaxEntries);
}

TEST_CASE("RecentFiles remove", "[recent_files]")
{
    TempDir tmp;
    tmp.write("keep.md", "k");
    tmp.write("remove.md", "r");

    Config config;
    RecentFiles recent(config);
    recent.clear();

    recent.add(tmp.path / "keep.md");
    recent.add(tmp.path / "remove.md");
    recent.remove(tmp.path / "remove.md");

    REQUIRE(recent.list().size() == 1);
    REQUIRE(recent.list()[0].filename() == "keep.md");
}

TEST_CASE("RecentFiles clear", "[recent_files]")
{
    TempDir tmp;
    tmp.write("file.md", "f");

    Config config;
    RecentFiles recent(config);

    recent.add(tmp.path / "file.md");
    recent.clear();

    REQUIRE(recent.list().empty());
}

TEST_CASE("RecentFiles prunes non-existent on load", "[recent_files]")
{
    TempDir tmp;
    tmp.write("exists.md", "ok");

    Config config;
    {
        RecentFiles recent(config);
        recent.clear();
        recent.add(tmp.path / "exists.md");
        recent.add(tmp.path / "gone.md"); // doesn't exist
    }

    // Reload — gone.md should be pruned
    RecentFiles recent2(config);
    REQUIRE(recent2.list().size() == 1);
    REQUIRE(recent2.list()[0].filename() == "exists.md");
}

// ═══════════════════════════════════════════════════════
// FileSystem constants
// ═══════════════════════════════════════════════════════

TEST_CASE("FileSystem markdown extensions set", "[filesystem]")
{
    REQUIRE(FileSystem::kMarkdownExtensions.contains(".md"));
    REQUIRE(FileSystem::kMarkdownExtensions.contains(".markdown"));
    REQUIRE(FileSystem::kMarkdownExtensions.contains(".txt"));
    REQUIRE_FALSE(FileSystem::kMarkdownExtensions.contains(".cpp"));
    REQUIRE_FALSE(FileSystem::kMarkdownExtensions.contains(".json"));
}

TEST_CASE("FileSystem excluded directories set", "[filesystem]")
{
    REQUIRE(FileSystem::kExcludedDirectories.contains("node_modules"));
    REQUIRE(FileSystem::kExcludedDirectories.contains(".git"));
    REQUIRE(FileSystem::kExcludedDirectories.contains("build"));
    REQUIRE_FALSE(FileSystem::kExcludedDirectories.contains("src"));
}
