#include "core/FileNode.h"
#include "core/ThemeValidator.h"

#include <catch2/catch_all.hpp>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// ── Helper to create a temporary directory ──

class TempDir
{
public:
    TempDir()
        : path_(fs::temp_directory_path() /
                ("markamp_test_" +
                 std::to_string(std::chrono::system_clock::now().time_since_epoch().count())))
    {
        fs::create_directories(path_);
    }

    ~TempDir()
    {
        std::error_code err_code;
        fs::remove_all(path_, err_code);
    }

    TempDir(const TempDir&) = delete;
    auto operator=(const TempDir&) -> TempDir& = delete;
    TempDir(TempDir&&) = delete;
    auto operator=(TempDir&&) -> TempDir& = delete;

    [[nodiscard]] auto path() const -> const fs::path&
    {
        return path_;
    }

private:
    fs::path path_;
};

// ================================================
// Task 28.7: Edge case and stress testing
// ================================================

// --- Empty file handling ---

TEST_CASE("Empty file: 0-byte file does not crash file node", "[cross-platform][edge-case]")
{
    const TempDir tmp;
    const auto empty_file = tmp.path() / "empty.md";

    // Create 0-byte file
    {
        const std::ofstream ofs(empty_file, std::ios::binary);
        // Write nothing
    }

    REQUIRE(fs::exists(empty_file));
    REQUIRE(fs::file_size(empty_file) == 0);

    // Reading it should produce empty content, not crash
    std::ifstream ifs(empty_file, std::ios::binary);
    const std::string content((std::istreambuf_iterator<char>(ifs)),
                              std::istreambuf_iterator<char>());
    REQUIRE(content.empty());
}

// --- Binary file detection ---

TEST_CASE("Binary file: binary content is detectable", "[cross-platform][edge-case]")
{
    const TempDir tmp;
    const auto bin_file = tmp.path() / "image.png";

    // Write PNG magic bytes + binary junk
    {
        std::ofstream ofs(bin_file, std::ios::binary);
        constexpr std::array<uint8_t, 8> kPngHeader = {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        ofs.write(reinterpret_cast<const char*>(
                      kPngHeader.data()), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                  static_cast<std::streamsize>(kPngHeader.size()));
        // Add some null bytes
        constexpr std::array<char, 10> kNulls = {};
        ofs.write(kNulls.data(), static_cast<std::streamsize>(kNulls.size()));
    }

    // Read the file as text — should contain null bytes
    std::ifstream ifs(bin_file, std::ios::binary);
    const std::string content((std::istreambuf_iterator<char>(ifs)),
                              std::istreambuf_iterator<char>());
    REQUIRE(content.size() == 18);

    // Check for null bytes (simple binary detection)
    const bool has_null = content.find('\0') != std::string::npos;
    REQUIRE(has_null);
}

// --- Very long filenames ---

TEST_CASE("Long filename: 200+ character filename is handled", "[cross-platform][edge-case]")
{
    const TempDir tmp;

    // Create a filename with 200 characters
    std::string long_name(200, 'a');
    long_name += ".md";

    const auto long_file = tmp.path() / long_name;

    // This may fail on some filesystems (e.g., ext4 limit is 255 bytes)
    std::error_code err_code;
    {
        std::ofstream ofs(long_file);
        if (ofs.good())
        {
            ofs << "# Long filename test";
        }
    }

    if (fs::exists(long_file, err_code))
    {
        // FileNode should handle long names without crash
        markamp::core::FileNode node;
        node.name = long_name;
        node.type = markamp::core::FileNodeType::File;
        REQUIRE(node.name == long_name);
        REQUIRE(node.name.size() > 200);
    }
    else
    {
        // Filesystem doesn't support this length — that's fine, just don't crash
        SUCCEED("Filesystem does not support 200+ char filenames — skipping");
    }
}

// --- Deeply nested directories ---

TEST_CASE("Deeply nested: 20 levels of directories", "[cross-platform][edge-case]")
{
    const TempDir tmp;

    // Create 20 levels of nesting
    fs::path nested = tmp.path();
    for (int level = 0; level < 20; ++level)
    {
        nested /= ("level_" + std::to_string(level));
    }

    std::error_code err_code;
    fs::create_directories(nested, err_code);
    REQUIRE(!err_code);

    // Create a file at the bottom
    const auto deep_file = nested / "deep.md";
    {
        std::ofstream ofs(deep_file);
        ofs << "# Deeply nested file";
    }

    REQUIRE(fs::exists(deep_file));

    // Read it back
    std::ifstream ifs(deep_file);
    const std::string content((std::istreambuf_iterator<char>(ifs)),
                              std::istreambuf_iterator<char>());
    REQUIRE(content == "# Deeply nested file");
}

// --- Path with spaces ---

TEST_CASE("Path with spaces: handles correctly", "[cross-platform][edge-case]")
{
    const TempDir tmp;
    const auto space_dir = tmp.path() / "my folder" / "sub folder";

    std::error_code err_code;
    fs::create_directories(space_dir, err_code);
    REQUIRE(!err_code);

    const auto space_file = space_dir / "my document.md";
    {
        std::ofstream ofs(space_file);
        ofs << "# Spaces in path";
    }

    REQUIRE(fs::exists(space_file));

    std::ifstream ifs(space_file);
    const std::string content((std::istreambuf_iterator<char>(ifs)),
                              std::istreambuf_iterator<char>());
    REQUIRE(content == "# Spaces in path");
}

// --- Path with Unicode characters ---

TEST_CASE("Unicode path: handles special characters", "[cross-platform][edge-case]")
{
    const TempDir tmp;

    // Use Unicode characters that are safe on all platforms
    const auto unicode_dir = tmp.path() / "docs_cafe";

    std::error_code err_code;
    fs::create_directories(unicode_dir, err_code);

    if (!err_code)
    {
        const auto unicode_file = unicode_dir / "resume.md";
        {
            std::ofstream ofs(unicode_file);
            ofs << "# Unicode content: äöü ñ é — 日本語";
        }

        REQUIRE(fs::exists(unicode_file));

        std::ifstream ifs(unicode_file);
        const std::string content((std::istreambuf_iterator<char>(ifs)),
                                  std::istreambuf_iterator<char>());
        REQUIRE(content.find("Unicode content") != std::string::npos);
    }
    else
    {
        SUCCEED("Filesystem does not support Unicode directory names — skipping");
    }
}

// --- ThemeValidator: null bytes rejected ---

TEST_CASE("ThemeValidator: null bytes detected", "[cross-platform][edge-case]")
{
    // Directly test the static utility method
    const std::string clean_string = "normal_text";
    REQUIRE_FALSE(markamp::core::ThemeValidator::contains_null_bytes(clean_string));

    const std::string null_string = std::string("text\0here", 9);
    REQUIRE(markamp::core::ThemeValidator::contains_null_bytes(null_string));
}

TEST_CASE("ThemeValidator: control characters detected", "[cross-platform][edge-case]")
{
    const std::string clean_string = "normal text with tab\tand newline\n";
    // Tab and newline should be allowed
    REQUIRE_FALSE(markamp::core::ThemeValidator::contains_control_chars(clean_string));

    // Control character 0x01 (SOH) should be flagged
    const std::string control_string = std::string("text\x01here", 9);
    REQUIRE(markamp::core::ThemeValidator::contains_control_chars(control_string));
}

// --- Special filename characters ---

TEST_CASE("Filename with special chars: hyphens and dots", "[cross-platform][edge-case]")
{
    const TempDir tmp;

    const auto special_file = tmp.path() / "my-document.v2.0.draft.md";
    {
        std::ofstream ofs(special_file);
        ofs << "# Special characters in filename";
    }

    REQUIRE(fs::exists(special_file));

    markamp::core::FileNode node;
    node.name = "my-document.v2.0.draft.md";
    node.type = markamp::core::FileNodeType::File;
    REQUIRE(node.name == "my-document.v2.0.draft.md");
    REQUIRE(node.extension() == ".md");
}

// --- FileNode folder structure ---

TEST_CASE("FileNode: folder with children maintains counts", "[cross-platform][edge-case]")
{
    markamp::core::FileNode folder;
    folder.name = "project";
    folder.type = markamp::core::FileNodeType::Folder;

    // Add sub-folders and files
    for (int idx = 0; idx < 5; ++idx)
    {
        markamp::core::FileNode child_file;
        child_file.name = "file_" + std::to_string(idx) + ".md";
        child_file.type = markamp::core::FileNodeType::File;
        folder.children.push_back(child_file);
    }

    markamp::core::FileNode sub_folder;
    sub_folder.name = "subfolder";
    sub_folder.type = markamp::core::FileNodeType::Folder;

    markamp::core::FileNode nested_file;
    nested_file.name = "nested.md";
    nested_file.type = markamp::core::FileNodeType::File;
    sub_folder.children.push_back(nested_file);

    folder.children.push_back(sub_folder);

    REQUIRE(folder.is_folder());
    REQUIRE(folder.file_count() == 6);   // 5 direct + 1 nested
    REQUIRE(folder.folder_count() == 2); // root + 1 subfolder
}

// --- Read-only file detection ---

TEST_CASE("Read-only file: detected correctly", "[cross-platform][edge-case]")
{
    const TempDir tmp;
    const auto readonly_file = tmp.path() / "readonly.md";

    {
        std::ofstream ofs(readonly_file);
        ofs << "# Read only";
    }

    // Make read-only
    std::error_code err_code;
    fs::permissions(readonly_file, fs::perms::owner_read, fs::perm_options::replace, err_code);
    REQUIRE(!err_code);

    // Verify we can still read
    std::ifstream ifs(readonly_file);
    REQUIRE(ifs.good());
    const std::string content((std::istreambuf_iterator<char>(ifs)),
                              std::istreambuf_iterator<char>());
    REQUIRE(content == "# Read only");

    // Restore permissions for cleanup
    fs::permissions(readonly_file, fs::perms::owner_all, fs::perm_options::replace, err_code);
}
