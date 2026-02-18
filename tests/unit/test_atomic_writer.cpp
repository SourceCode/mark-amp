/// test_atomic_writer.cpp — V7 Phase 31: Atomic writer tests

#include "core/AtomicWriter.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

static auto temp_file(const std::string& name) -> std::filesystem::path
{
    return std::filesystem::temp_directory_path() / ("markamp_test_" + name);
}

static void cleanup(const std::filesystem::path& path)
{
    std::filesystem::remove(path);
    std::filesystem::remove(std::filesystem::path(path.string() + ".tmp"));
    std::filesystem::remove(std::filesystem::path(path.string() + ".bak"));
}

// ══════════════════════════════════════════════════════════════════════════════
// CRC32
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("CRC32: deterministic", "[atomic_writer]")
{
    auto crc1 = crc32_checksum("hello");
    auto crc2 = crc32_checksum("hello");
    REQUIRE(crc1 == crc2);
}

TEST_CASE("CRC32: different data gives different checksum", "[atomic_writer]")
{
    auto crc1 = crc32_checksum("hello");
    auto crc2 = crc32_checksum("world");
    REQUIRE(crc1 != crc2);
}

TEST_CASE("CRC32: empty string", "[atomic_writer]")
{
    auto crc = crc32_checksum("");
    REQUIRE(crc == 0); // CRC32 of empty string is 0
}

// ══════════════════════════════════════════════════════════════════════════════
// Atomic write
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("AtomicWriter: basic write", "[atomic_writer]")
{
    auto path = temp_file("aw_basic.txt");
    AtomicWriter writer;
    writer.set_backup_enabled(false);

    auto result = writer.write(path, "hello world");
    REQUIRE(result.has_value());

    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    REQUIRE(content == "hello world");

    cleanup(path);
}

TEST_CASE("AtomicWriter: write creates backup", "[atomic_writer]")
{
    auto path = temp_file("aw_backup.txt");
    AtomicWriter writer;

    // Write initial content
    writer.set_backup_enabled(false);
    auto r1 = writer.write(path, "original");
    REQUIRE(r1.has_value());

    // Write again with backup
    writer.set_backup_enabled(true);
    auto r2 = writer.write(path, "updated");
    REQUIRE(r2.has_value());

    // Check backup exists
    auto bak_path = std::filesystem::path(path.string() + ".bak");
    REQUIRE(std::filesystem::exists(bak_path));

    std::ifstream bak_file(bak_path);
    std::string bak_content((std::istreambuf_iterator<char>(bak_file)),
                            std::istreambuf_iterator<char>());
    REQUIRE(bak_content == "original");

    cleanup(path);
}

// ══════════════════════════════════════════════════════════════════════════════
// Checksum roundtrip
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("AtomicWriter: write and read with checksum", "[atomic_writer]")
{
    auto path = temp_file("aw_checksum.txt");
    AtomicWriter writer;
    writer.set_backup_enabled(false);

    auto w_result = writer.write_with_checksum(path, "important data");
    REQUIRE(w_result.has_value());

    auto r_result = writer.read_verified(path);
    REQUIRE(r_result.has_value());
    REQUIRE(r_result.value() == "important data");

    cleanup(path);
}

TEST_CASE("AtomicWriter: read_verified detects corruption", "[atomic_writer]")
{
    auto path = temp_file("aw_corrupt.txt");
    AtomicWriter writer;
    writer.set_backup_enabled(false);

    auto w_result = writer.write_with_checksum(path, "original data");
    REQUIRE(w_result.has_value());

    // Corrupt the file by modifying the payload
    {
        std::ofstream file(path);
        file << "corrupted data\n__CRC32__=00000000\n";
    }

    auto r_result = writer.read_verified(path);
    REQUIRE_FALSE(r_result.has_value());
    REQUIRE(r_result.error().code == ErrorCode::ChecksumMismatch);

    cleanup(path);
}

TEST_CASE("AtomicWriter: read_verified rejects missing trailer", "[atomic_writer]")
{
    auto path = temp_file("aw_no_trailer.txt");
    {
        std::ofstream file(path);
        file << "no checksum here";
    }

    AtomicWriter writer;
    auto result = writer.read_verified(path);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::CorruptedFile);

    cleanup(path);
}

// ══════════════════════════════════════════════════════════════════════════════
// Backup
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("AtomicWriter: backup of non-existent file fails", "[atomic_writer]")
{
    auto result = AtomicWriter::backup("/tmp/markamp_nonexistent_file.txt");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::FileNotFound);
}

TEST_CASE("AtomicWriter: backup_enabled default is true", "[atomic_writer]")
{
    AtomicWriter writer;
    REQUIRE(writer.backup_enabled());
}
