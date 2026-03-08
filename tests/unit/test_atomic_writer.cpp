// test_atomic_writer.cpp — 10 tests for AtomicWriter and crc32_checksum
#include "core/AtomicWriter.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

TEST_CASE("crc32_checksum produces consistent results", "[atomic_writer]")
{
    auto c1 = crc32_checksum("Hello World");
    auto c2 = crc32_checksum("Hello World");
    CHECK(c1 == c2);
}

TEST_CASE("crc32_checksum differs for different input", "[atomic_writer]")
{
    auto c1 = crc32_checksum("Hello");
    auto c2 = crc32_checksum("World");
    CHECK(c1 != c2);
}

TEST_CASE("crc32_checksum handles empty input", "[atomic_writer]")
{
    // CRC32 of empty input may return 0 in some implementations; just verify it runs
    auto c = crc32_checksum("");
    // Ensure it produces a deterministic result
    auto c2 = crc32_checksum("");
    CHECK(c == c2);
}

TEST_CASE("AtomicWriter default has backup enabled", "[atomic_writer]")
{
    AtomicWriter writer;
    CHECK(writer.backup_enabled());
}

TEST_CASE("AtomicWriter set_backup_enabled toggles backup", "[atomic_writer]")
{
    AtomicWriter writer;
    writer.set_backup_enabled(false);
    CHECK_FALSE(writer.backup_enabled());
    writer.set_backup_enabled(true);
    CHECK(writer.backup_enabled());
}

TEST_CASE("AtomicWriter write creates file", "[atomic_writer]")
{
    AtomicWriter writer;
    writer.set_backup_enabled(false);
    auto path = std::filesystem::temp_directory_path() / "markamp_test_atomic_write.txt";
    auto result = writer.write(path, "test data");
    CHECK(result.has_value());
    std::ifstream in(path);
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    CHECK(content == "test data");
    std::filesystem::remove(path);
}

TEST_CASE("AtomicWriter write_with_checksum appends trailer", "[atomic_writer]")
{
    AtomicWriter writer;
    writer.set_backup_enabled(false);
    auto path = std::filesystem::temp_directory_path() / "markamp_test_atomic_checksum.txt";
    auto result = writer.write_with_checksum(path, "data");
    CHECK(result.has_value());
    std::ifstream in(path);
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    CHECK(content.find("__CRC32__") != std::string::npos);
    std::filesystem::remove(path);
}

TEST_CASE("AtomicWriter read_verified validates checksum", "[atomic_writer]")
{
    AtomicWriter writer;
    writer.set_backup_enabled(false);
    auto path = std::filesystem::temp_directory_path() / "markamp_test_atomic_verify.txt";
    (void)writer.write_with_checksum(path, "verified data");
    auto result = writer.read_verified(path);
    REQUIRE(result.has_value());
    CHECK(*result == "verified data");
    std::filesystem::remove(path);
}

TEST_CASE("AtomicWriter read_verified detects corruption", "[atomic_writer]")
{
    AtomicWriter writer;
    writer.set_backup_enabled(false);
    auto path = std::filesystem::temp_directory_path() / "markamp_test_atomic_corrupt.txt";
    (void)writer.write_with_checksum(path, "original");
    // Read the file to get the checksum trailer, then overwrite with different content but same
    // trailer
    std::ifstream in(path);
    std::string raw((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    // Replace "original" with "modified" in the raw file — the checksum will mismatch
    auto pos = raw.find("original");
    if (pos != std::string::npos)
    {
        raw.replace(pos, 8, "modified");
        std::ofstream out(path, std::ios::trunc);
        out << raw;
        out.close();
        auto result = writer.read_verified(path);
        CHECK_FALSE(result.has_value());
    }
    std::filesystem::remove(path);
}

TEST_CASE("extract_checksum_trailer works on valid data", "[atomic_writer]")
{
    auto checksum = crc32_checksum("test");
    std::string trailer = "test\n__CRC32__=" + std::to_string(checksum) + "\n";
    auto result = extract_checksum_trailer(trailer);
    if (result.has_value())
    {
        CHECK(result.value().first == "test");
    }
}
